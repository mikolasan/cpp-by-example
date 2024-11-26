#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/Host.h>
#include "clang/Tooling/Tooling.h"
#include <llvm/MC/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>

#include <iostream>
#include <string>
#include <memory>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Function to generate assembly code from LLVM IR code.
std::string compileToAssembly(const std::string &code) {
    // Initialize LLVM components.
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    llvm::LLVMContext context;
    llvm::SMDiagnostic err;

    // Create a memory buffer from the provided code string.
    auto codeBuffer = llvm::MemoryBuffer::getMemBuffer(code);

    // Parse the IR code into an LLVM Module.
    auto module = llvm::parseIR(*codeBuffer, err, context);
    if (!module) {
        std::string errorMessage;
        llvm::raw_string_ostream errorStream(errorMessage);
        err.print("IR Parsing Error", errorStream);
        throw std::runtime_error(errorStream.str());
    }

    // Set up the target machine for the host system.
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        throw std::runtime_error("Failed to lookup target: " + error);
    }

    llvm::TargetOptions opt;
    auto targetMachine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(
        targetTriple, "generic", "", opt, llvm::Reloc::Static));

    // Set up the module with the target information.
    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);

    // // Create an output buffer to store the assembly code.
    // std::string assembly;
    // llvm::raw_string_ostream assemblyStream(assembly);
    // llvm::raw_pwrite_stream outputStream;

    // // Emit the assembly code.
    // llvm::legacy::PassManager passManager;
    // if (targetMachine->addPassesToEmitFile(passManager, outputStream, nullptr, llvm::CGFT_AssemblyFile)) {
    //     throw std::runtime_error("Target machine cannot emit assembly code.");
    // }

    // passManager.run(*module);
    // assemblyStream.flush();

    // return assembly;

    // Create a temporary file to capture the assembly output.
    llvm::SmallString<64> tempFileName;
    int fd;
    llvm::sys::fs::createTemporaryFile("output", "s", fd, tempFileName);

    llvm::raw_fd_ostream assemblyStream(fd, /*shouldClose=*/true, llvm::sys::fs::OF_None);

    // Emit the assembly code.
    llvm::legacy::PassManager passManager;
    if (targetMachine->addPassesToEmitFile(passManager, assemblyStream, nullptr, llvm::CGFT_AssemblyFile)) {
        throw std::runtime_error("Target machine cannot emit assembly code.");
    }

    passManager.run(*module);

    // Read the generated assembly from the file.
    assemblyStream.flush();

    // Convert file descriptor to FILE* using fdopen
    FILE* file = fdopen(fd, "r");
    if (!file) {
        throw std::runtime_error("Failed to open file descriptor.");
    }

    // Use the file stream in an ifstream for C++-style reading
    // std::ifstream tempFile(file);
    // if (!tempFile) {
    //     throw std::runtime_error("Failed to create ifstream from FILE*.");
    // }

    // // Read the entire file into a string
    // std::string content((std::istreambuf_iterator<char>(tempFile)),
    //                     std::istreambuf_iterator<char>());

    // // No need to close FILE* manually; it will be handled by std::ifstream
    
    // tempFile.close();

    // Clean up the temporary file.
    // llvm::sys::fs::remove(tempFileName);

    return "";
}

int main() {
    // Example LLVM IR code as a string.
    std::string llvmCode = R"(
        define i32 @main() {
        entry:
          ret i32 42
        }
    )";

    try {
        std::string assemblyCode = compileToAssembly(llvmCode);
        std::cout << "Generated Assembly:\n" << assemblyCode << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
