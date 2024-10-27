# C++ by example

[![Netlify Status](https://api.netlify.com/api/v1/badges/73450bc7-9e25-4544-a0e8-79a2dbed6054/deploy-status)](https://app.netlify.com/sites/cpp-by-example/deploys)

Develop, train and test your skills in C++ language

## Dependencies

### LLVM

```
git submodule add https://github.com/llvm/llvm-project.git thirdparty/llvm
git checkout llvmorg-16.0.0
cd thirdparty
cmake -S llvm\llvm -B llvm-build --install-prefix="C:\Users\neupo\develop\cpp-skill\thirdparty\windows\LLVM" -DLLVM_ENABLE_PROJECTS=clang -DLLVM_TARGETS_TO_BUILD=X86 -Thost=x64
cmake --build llvm-build --config Release
cmake --install llvm-build
```

### Lua

Add to user environment variables

```
Lua51_ROOT = "C:\<path_to_cpp-skill>\windows\lua"
```


## Build

```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build --target all
```
