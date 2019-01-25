#include "clang/AST/AST.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <random>
#include <stack>
#include <vector>

#include "randstruct.h"

static std::vector<FieldDecl *> rearrange(std::vector<FieldDecl *> fields) {
  auto rng = std::default_random_engine{};
  std::shuffle(std::begin(fields), std::end(fields), rng);
  return fields;
}

static bool layout(const RecordDecl *Record, std::vector<FieldDecl *> &fields,
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
