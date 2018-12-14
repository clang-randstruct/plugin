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
  Randstruct() { llvm::errs() << "randstruct ctor\n"; }
  virtual bool layoutRecordType(const RecordDecl *Record, 
                                uint64_t &Size, 
                                uint64_t &Alignment, 
                                llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets, 
                                llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets, 
                                llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets) override {
                                  llvm::errs() << "randstruct is responsible for this structure's layout!\n";
                                  return true;
                                }
};

class SwapFieldsConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  SwapFieldsConsumer(CompilerInstance &Instance,
                     std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {
          Instance.getASTContext().setExternalSource(llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct));
      }

  /*
  void HandleTranslationUnit(ASTContext &Ctx) override {
      Ctx.setExternalSource(llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct));
      Ctx.getTranslationUnitDecl()->setHasExternalVisibleStorage();
      llvm::errs() << "HandleTranslationUnit\n";
  }
  */

  /*
  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const RecordDecl *RD = dyn_cast<RecordDecl>(D)) {
        llvm::errs() << "record-decl: \"" << RD->getNameAsString() << "\"\n";
        for (auto field : RD->fields())
          llvm::errs() << "\tfield: \"" << field->getNameAsString() << "\" " << "(" << field->getType().getAsString() << ")" << "\n";
      }
    }

    return true;
  }
  */
};

class SwapFieldsAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
      //CI.getASTContext().setExternalSource(llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct));
    return llvm::make_unique<SwapFieldsConsumer>(CI, ParsedTemplates);
  }

  virtual ActionType getActionType() override {
    return ActionType::AddBeforeMainAction;
  }

  // Not needed yet
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "SwapFields arg = " << args[i] << "\n";

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
        ParsedTemplates.insert(args[i]);
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Help for SwapFields plugin goes here\n";
  }
};

} // namespace

static FrontendPluginRegistry::Add<SwapFieldsAction>
    X("swap-fields", "swaps the first two fields of a structure");
