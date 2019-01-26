#pragma once

#include "clang/AST/ExternalASTSource.h"
#include "clang/Frontend/CompilerInstance.h"

using namespace clang;

class Randstruct : public ExternalASTSource {
private:
  CompilerInstance &Instance;
  char * seed;

protected:
  int ParseArgs(
      const CompilerInstance &CI, 
      const std::vector<std::string> &args);

public:
  Randstruct(CompilerInstance &I) : Instance(I) {}

  virtual bool layoutRecordType(
      const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
      llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets)
      override; 
};
