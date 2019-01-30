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
    // Required for finding out size of field.
    auto& ctx = Instance.getASTContext();

    // All of the buckets produced by best-effort cache-line algorithm.
    std::vector<std::unique_ptr<Bucket>> buckets;

    // The current bucket of fields that we are trying to fill to a cache-line.
    std::unique_ptr<Bucket> currentBucket = nullptr;
    // The current bucket containing the run of adjacent  bitfields to ensure 
    // they remain adjacent.
    std::unique_ptr<Bucket> currentBitfieldRun = nullptr;

    // Tracks the number of fields that we failed to fit to the current bucket,
    // and thus still need to be added later.
    auto skipped = 0;

    while (!fields.empty()) {
        // If we've skipped more fields than we have remaining to place,
        // that means that they can't fit in our current bucket, and we
        // need to start a new one.
        if (skipped >= fields.size()) {
            skipped = 0;
            buckets.push_back(std::move(currentBucket));
        }

        // Take the first field that needs to be put in a bucket.
        auto field = fields.begin();

        if ((*field)->isBitField()) {
            // Start a bitfield run if this is the first bitfield
            // we have found.
            if (!currentBitfieldRun) {
                currentBitfieldRun = std::make_unique<BitfieldRun>();
            }

            // We've placed the field, and can remove it from the
            // "awaiting buckets" vector called "fields"
            currentBitfieldRun->add(*field, 1);
            fields.erase(field);
        } else {
            // Else, current field is not a bitfield
            // If we were previously in a bitfield run, end it.
            if (currentBitfieldRun) {
                buckets.push_back(std::move(currentBitfieldRun));
            }
            // If we don't have a bucket, make one.
            if (!currentBucket) {
                currentBucket = std::make_unique<Bucket>();
            }

            auto width = ctx.getTypeInfo((*field)->getType()).Width;

            // If we can fit, add it.
            if (currentBucket->canFit(width)) {
                currentBucket->add(*field, width);
                fields.erase(field);

                // If it's now full, tie off the bucket.
                if (currentBucket->full()) {
                    skipped = 0;
                    buckets.push_back(std::move(currentBucket));
                }
            } else {
                // We can't fit it in our current bucket.
                // Move to the end for processing later.
                ++skipped; // Mark it skipped.
                fields.push_back(*field);
                fields.erase(field);
            }
        }
    }

    // Done processing the fields awaiting a bucket.

    // If we were filling a bucket, tie it off.
    if (currentBucket) {
        buckets.push_back(std::move(currentBucket));
    }

    // If we were processing a bitfield run bucket, tie it off.
    if (currentBitfieldRun) {
        buckets.push_back(std::move(currentBitfieldRun));
    }

    // TODO use seed
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(buckets), std::end(buckets), rng);

    // Produce the new ordering of the elements from our buckets.
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
