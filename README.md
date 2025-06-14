# C++ by example

![Static Badge](https://img.shields.io/badge/cpp-11-6BBAEC)
![Static Badge](https://img.shields.io/badge/cpp-17-6987C9)
![Static Badge](https://img.shields.io/badge/cpp-20-B388EB)


A collection of code snippets in C++ language

## Optional dependencies

## bgfx

Open MSVC terminal

```
cd graphics\bgfx
..\bx\tools\bin\windows\genie --with-shared-lib --with-tools --with-sdl vs2022
start .build/projects/vs2022/bgfx.sln
```

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

## The book

Run locally:

```
mdbook serve
```