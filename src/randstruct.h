#pragma once

#include "clang/AST/ExternalASTSource.h"
#include "clang/Frontend/CompilerInstance.h"
#include <random>

using namespace clang;

/// Randstruct is an ExternalASTSource that provides structure layout
/// functionality.
class Randstruct : public ExternalASTSource {
private:
  std::seed_seq Seq;
  std::default_random_engine rng;
  CompilerInstance &Instance;

  /// Performs basic randomization.
  std::vector<FieldDecl *> randomize(std::vector<FieldDecl *> fields);
  /// Performs performance-sensitive randomization and makes a best effort
  /// to group fields into cache lines.
  std::vector<FieldDecl *> perfrandomize(std::vector<FieldDecl *> fields);
  /// Entry point for reorganizing fields of a structure.
  std::vector<FieldDecl *> rearrange(std::vector<FieldDecl *> fields);
  /// Performs layout and alignment of a structure.
  bool layout(const RecordDecl *Record, std::vector<FieldDecl *> &fields,
              uint64_t &Size, uint64_t &Alignment,
              llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
              ASTContext &ctx);

public:
  Randstruct(CompilerInstance &I, std::string &seed)
      : Instance(I), Seq(seed.begin(), seed.end()), rng(Seq) {}

  /// Entry point for Randstruct, called by the Clang compiler.
  virtual bool layoutRecordType(
      const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
      llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets)
      override;
};
