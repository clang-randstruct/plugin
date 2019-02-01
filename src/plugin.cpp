#include <stack>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include "randstruct.h"
#include <stdlib.h>

namespace {
class RandstructConsumer : public ASTConsumer {
  CompilerInstance &Instance;

public:
  RandstructConsumer(CompilerInstance &Instance, std::string &seed)
      : Instance(Instance) {
    Instance.getASTContext().setExternalSource(
        llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct(Instance, seed)));
  }
};

class RandstructAction : public PluginASTAction {
protected:
  std::string seed;

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<RandstructConsumer>(CI, seed);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-rand-seed") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing randomization seed"));
          return false;
        }
        seed = args[i + 1];
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
