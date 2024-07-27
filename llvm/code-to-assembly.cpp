// https://clang.llvm.org/docs/RAVFrontendAction.html
// https://clang.llvm.org/doxygen/namespaceclang_1_1tooling.html#ae416ffe2c36c8c9f57181044b4c72252

#include <iostream>
#include <string>
// #include <vector>

// #include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LLVMContext.h"
// #include "llvm/IR/Module.h"
// #include "llvm/IR/Verifier.h"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/ExecutionEngine/MCJIT.h"
// #include "llvm/ExecutionEngine/ExecutionEngine.h"
// #include "llvm/ExecutionEngine/GenericValue.h"
// #include "llvm/IR/LegacyPassManager.h"
// #include "llvm/Transforms/Scalar.h"
// #include "llvm/Transforms/Utils.h"
// #include "llvm/Support/raw_ostream.h"
#include "clang/CodeGen/CodeGenAction.h" // EmitAssemblyAction
#include "clang/Basic/CodeGenOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

// using namespace clang;
// using namespace clang::tooling;
// using namespace llvm;

// static cl::OptionCategory MyToolCategory("My tool options");

class MyASTConsumer : public clang::ASTConsumer {
public:
  explicit MyASTConsumer(clang::ASTContext *) {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) {
    // Process the AST
    std::cout << "Processing AST for: " 
      << context.getTranslationUnitDecl()
      << std::endl;
  }
};

class MyAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &compiler, llvm::StringRef code) {
    
    llvm::LLVMContext context;
    // std::unique_ptr<clang::CodeGenAction> assemblyAction(new clang::EmitAssemblyAction(&context));
    // std::unique_ptr<llvm::Module> module = assemblyAction->takeModule();
    compiler.ExecuteAction(new clang::EmitAssemblyAction(&context));
    return std::make_unique<MyASTConsumer>(&compiler.getASTContext());
  }
};

std::string compileToAssembly(std::string code) {
  llvm::StringRef testCode(code);
  clang::tooling::runToolOnCode(
    std::make_unique<MyAction>(),
    testCode
  );

  return "TODO";
}

int main(int argc, char const *argv[]) {
  std::string code = R"CODE(
#include <iostream>
int main() {
  std::cout << "hello wroRLDL" << std::endl;
  return 0;
}
)CODE";

  std::string assembly = compileToAssembly(code);
  std::cout << "assembly: " << assembly << std::endl;
  std::cout << std::endl;
  return 0;
}
