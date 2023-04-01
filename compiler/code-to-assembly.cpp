#include <iostream>
#include <string>
#include <vector>

#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace llvm;

std::string compileToAssembly(std::string code) {
  // Initialize LLVM components
  // SMDiagnostic error;
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  // Create a new compiler instance
  CompilerInstance compiler;
  compiler.createDiagnostics();
  // Create a new tool
  auto tool = new tooling::ClangTool(compiler.getCompilations(), compiler.getSourceManager());

  // Define the code to be compiled
  std::vector<std::string> sources;
  sources.push_back(code);

  tooling::ToolAction* a = tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>().get();
  // Run the compilation
  // tool->run(a, tooling::Arguments(sources, ""));
  tool->run(a);

  LLVMContext context;
  // Create a new code generation action
  std::unique_ptr<CodeGenAction> action(new EmitAssemblyAction(&context));
  // Get the module and verify it
  Module *module = action->takeModule();
  verifyModule(*module, &errs());

  // Create a new pass manager and add optimization passes
  std::unique_ptr<legacy::FunctionPassManager> fpm(new legacy::FunctionPassManager(module));
  fpm->add(createPromoteMemoryToRegisterPass());
  fpm->add(createInstructionCombiningPass());
  fpm->add(createReassociatePass());
  fpm->add(createGVNPass());
  fpm->add(createCFGSimplificationPass());
  fpm->doInitialization();

  // Run the optimization passes
  for (auto &f : *module) {
      fpm->run(f);
  }

  // Create a new output stream and emit the assembly code
  std::string assembly;
  raw_string_ostream stream(assembly);
  module->print(stream, nullptr);
  stream.flush();

  // Return the assembly code
  return assembly;
}

int main(int argc, char const *argv[])
{
  std::string code = "int main() { return 0; }";
  std::string assembly = compileToAssembly(code);

  return 0;
}
