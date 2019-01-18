#include <stack>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;

namespace {

class Randstruct : public ExternalASTSource {
public:
  Randstruct(CompilerInstance &I) : Instance(I) {}
  CompilerInstance &Instance;

  virtual bool layoutRecordType(
      const RecordDecl *Record, uint64_t &Size, uint64_t &Alignment,
      llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets,
      llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets)
      override {
    auto &ctx = Instance.getASTContext();
    Alignment = 0;
    Size = 0;

    std::stack<FieldDecl *> fields;
    for (auto field : Record->fields()) {
      fields.push(field);
    }

    #ifndef NDEBUG
    llvm::errs() << "Type\tSize\tAlign\tOffset\tAligned?\n"
                 << "----\t----\t-----\t------\t--------\n";
    #endif

    while (!fields.empty()) {
      auto f = fields.top();
      fields.pop();

      auto width = ctx.getTypeInfo(f->getType()).Width;
      auto align = ctx.getTypeInfo(f->getType()).Align;
      Alignment = Alignment > align ? Alignment : align;

      // https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding
      auto padding = (-Size & (align - 1));

      FieldOffsets[f] = Size + padding;

      #ifndef NDEBUG
      llvm::errs() << f->getType().getAsString() << "\t"
                   << width << "\t" << align << "\t" << Size + padding << "\t"
                   << ((Size + width + padding) % align == 0 ? "Yes" : "No") << "\n";
      #endif

      Size += width + padding;
    }

    Size += (Alignment - (Size % Alignment)) % Alignment;

    #ifndef NDEBUG
    llvm::errs() << "\n"
                 << "Structure Size: " << Size << " bits ("
                 << "Should be " << Size + (Alignment - (Size % Alignment)) % Alignment << " bits)\n"
                 << "Alignment: " << Alignment << " bits\n"
                 << "\n";
    #endif

    return true;
  }
};

class RandstructConsumer : public ASTConsumer {
  CompilerInstance &Instance;

public:
  RandstructConsumer(CompilerInstance &Instance) : Instance(Instance) {
    Instance.getASTContext().setExternalSource(
        llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct(Instance)));
  }
};

class RandstructAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<RandstructConsumer>(CI);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "Randstruct arg = " << args[i] << "\n";

      // Example error handling.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-an-error") {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      } else if (args[i] == "-parse-template") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -parse-template argument"));
          return false;
        }
        ++i;
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Help for Randstruct plugin goes here\n";
  }
};

} // namespace

static FrontendPluginRegistry::Add<RandstructAction>
    X("randstruct", "randomizes the layout of structures");
