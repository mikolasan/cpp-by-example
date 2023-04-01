# C++ training snippets

Develop, train and test your skills in C++ language with *cpp-skill*

## Dependencies

LLVM

```
git submodule add https://github.com/llvm/llvm-project.git thirdparty/llvm
git checkout llvmorg-16.0.0
cd thirdparty
cmake -S llvm\llvm -B llvm-build --install-prefix="C:\Users\neupo\develop\cpp-skill\thirdparty\windows\LLVM" -DLLVM_ENABLE_PROJECTS=clang -DLLVM_TARGETS_TO_BUILD=X86 -Thost=x64
cmake --build llvm-build --config Release
cmake --install llvm-build
```