#include "clang/AST/AST.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <memory>
#include <random>
#include <stack>
#include <vector>

#include "bucket.h"
#include "randstruct.h"

std::vector<FieldDecl *> Randstruct::randomize(std::vector<FieldDecl *> fields) {
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(fields), std::end(fields), rng);
  return fields;
}

std::vector<FieldDecl *> Randstruct::perfrandomize(std::vector<FieldDecl *> fields) {
    std::vector<std::unique_ptr<Bucket>> buckets;

    std::unique_ptr<Bucket> currentBucket = nullptr;
    std::unique_ptr<Bucket> currentBitfieldRun = nullptr;
    auto& ctx = Instance.getASTContext();

    auto skipped = 0;

    while (!fields.empty()) {
        if (skipped > fields.size()) {
            skipped = 0;
            buckets.push_back(std::move(currentBucket));
        }
        auto field = fields.begin();

        if ((*field)->isBitField()) {
            if (!currentBitfieldRun) {
                currentBitfieldRun = std::make_unique<BitfieldRun>();
            }

            currentBitfieldRun->add(*field, 1);
            fields.erase(field);
        } else {
            if (currentBitfieldRun) {
                buckets.push_back(std::move(currentBitfieldRun));
            }
            if (!currentBucket) {
                currentBucket = std::make_unique<Bucket>();
            }

            auto width = ctx.getTypeInfo((*field)->getType()).Width;
            if (currentBucket->canFit(width)) {
                currentBucket->add(*field, width);
                fields.erase(field);

                if (currentBucket->full()) {
                    skipped = 0;
                    buckets.push_back(std::move(currentBucket));
                }
            } else {
                // Move to the end for processing later
                ++skipped;
                fields.push_back(*field);
                fields.erase(field);
            }
        }
    }

    if (currentBucket) {
        buckets.push_back(std::move(currentBucket));
    }

    if (currentBitfieldRun) {
        buckets.push_back(std::move(currentBitfieldRun));
    }

    // TODO use seed
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(buckets), std::end(buckets), rng);

    std::vector<FieldDecl *> finalOrder;
    for (auto& bucket : buckets) {
        auto randomized = bucket->randomize();
        finalOrder.insert(finalOrder.end(), randomized.begin(), randomized.end());
    }

    return finalOrder;
}

std::vector<FieldDecl *> Randstruct::rearrange(std::vector<FieldDecl *> fields) {
    return perfrandomize(fields);
}

bool Randstruct::layout(const RecordDecl *Record, std::vector<FieldDecl *> &fields,
                   uint64_t &Size, uint64_t &Alignment,
                   llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
                   ASTContext &ctx) {
  Alignment = 0;
  Size = 0;

#ifndef NDEBUG
  llvm::errs() << "Name\tType\tSize\tAlign\tOffset\tAligned?\n"
               << "----\t----\t-----\t------\t--------\n";
#endif

  for (auto f : fields) {
    auto width = ctx.getTypeInfo(f->getType()).Width;
    auto align = ctx.getTypeInfo(f->getType()).Align;

    Alignment = Alignment > align ? Alignment : align;

    // TODO: pad bitfields?
    // https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding
    auto padding = (-Size & (align - 1));

    FieldOffsets[f] = Size + padding;

#ifndef NDEBUG
    llvm::errs() << f->getNameAsString() << "\t" << f->getType().getAsString()
                 << "\t" << width << "\t" << align << "\t" << Size + padding
                 << "\t"
                 << ((Size + width + padding) % align == 0 ? "Yes" : "No")
                 << "\n";
#endif

    Size += width + padding;
  }

  // https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding
  auto tailpadded = (Size + (Alignment - 1)) & -Alignment;

  Size = tailpadded;

  // Respect the programmer's requested alignment from the structure
  // by overriding what we've calculated so far.
  if (auto alignAttr = Record->getAttr<AlignedAttr>()) {
    Alignment = alignAttr->getAlignment(ctx);
  }

#ifndef NDEBUG
  llvm::errs() << "\n"
               << "Structure Size: " << Size << " bits ("
               << "Should be "
               << Size + (Alignment - (Size % Alignment)) % Alignment
               << " bits)\n"
               << "Alignment: " << Alignment << " bits\n"
               << "\n";
#endif

  return true;
}

bool Randstruct::layoutRecordType(
    const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
    llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
    llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
    llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets) {
  auto &ctx = Instance.getASTContext();
  Alignment = 0;
  Size = 0;
  if (Record->isUnion()) {
    return false;
  }
  std::vector<FieldDecl *> fields;
  for (auto field : Record->fields()) {
    fields.push_back(field);
  }

  fields = rearrange(fields);

  return layout(Record, fields, Size, Alignment, FieldOffsets,
                Instance.getASTContext());
}
