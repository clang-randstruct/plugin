//===- Randstruct.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "clang/AST/ExternalASTSource.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;

namespace {

class Randstruct : public ExternalASTSource {
  virtual bool layoutRecordType(const RecordDecl *Record, 
                                uint64_t &Size, 
                                uint64_t &Alignment, 
                                llvm::DenseMap<const FieldDecl *, uint64_t> &FieldOffsets, 
                                llvm::DenseMap<const CXXRecordDecl *, CharUnits> &BaseOffsets, 
                                llvm::DenseMap<const CXXRecordDecl *, CharUnits> &VirtualBaseOffsets) override {
                                  llvm::errs() << "randstruct is responsible for this structure's layout!\n";
                                  return false;
                                }
};

class RandstructConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  RandstructConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {
        Instance.getASTContext().setExternalSource(llvm::IntrusiveRefCntPtr<Randstruct>(new Randstruct));
        // auto& ctx = Instance.getASTContext();
        // ctx.setExternalSource( ... )
        //                         ^ an instance of our implementation of ExternalASTSource goes here!
      }

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    /*
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const RecordDecl *RD = dyn_cast<RecordDecl>(D)) {
        llvm::errs() << "record-decl: \"" << RD->getNameAsString() << "\"\n";
        for (auto field : RD->fields())
          llvm::errs() << "\tfield: \"" << field->getNameAsString() << "\" " << "(" << field->getType().getAsString() << ")" << "\n";
      }
    }
    */

    return true;
  }

};

class RandstructAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<RandstructConsumer>(CI, ParsedTemplates);
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
        ParsedTemplates.insert(args[i]);
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for Randstruct plugin goes here\n";
  }

};

}

static FrontendPluginRegistry::Add<RandstructAction>
X("randstruct", "randomizes the layout of structures");
