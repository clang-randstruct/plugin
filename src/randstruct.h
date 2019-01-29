#pragma once

#include "clang/AST/ExternalASTSource.h"
#include "clang/Frontend/CompilerInstance.h"

using namespace clang;

class Randstruct : public ExternalASTSource {
private:
  CompilerInstance &Instance;

  std::vector<FieldDecl *> randomize(std::vector<FieldDecl *> fields);
  std::vector<FieldDecl *> perfrandomize(std::vector<FieldDecl *> fields);
  std::vector<FieldDecl *> rearrange(std::vector<FieldDecl *> fields);
  bool layout(const RecordDecl *Record, std::vector<FieldDecl *> &fields,
                   uint64_t &Size, uint64_t &Alignment,
                   llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
                   ASTContext &ctx);
public:
  Randstruct(CompilerInstance &I) : Instance(I) {}

  virtual bool layoutRecordType(
      const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
      llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets)
      override;
};
