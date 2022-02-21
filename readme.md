# Headers Checker

This tool checks include header file one by one for correctness.

# How to build Headers Checker

## Prerequisites

* Visual Studio - https://visualstudio.microsoft.com/downloads/
* CMake - https://cmake.org/download/
* Ninja build system - https://ninja-build.org/

### Clang
Headers checker depends on [LibTooling](https://clang.llvm.org/docs/LibASTMatchersTutorial.html), so you have to build Clang from source.

Steps for building Clang on Windows using Visual Studio and Ninja:

1) Add CMake and Ninja directories to the `PATH` environment variable.

2) Run MSVC 2019 X64 Command Promt
```
Start Menu -> Visual Studio 2019 -> x64 Native Tools Command Prompt for VS 2019
```
or from command line:
```
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
```

3) Clone LLVM/Clang repository
```
git clone --branch release/12.x https://github.com/llvm/llvm-project.git X:\llvm-project
```

4) Create build directory
```
cd /d X:\llvm-project
mkdir build
cd build
```

5) Use CMake to generate build files for Ninja
```
cmake -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DMSVC_TOOLSET_VERSION=142 -DCMAKE_CXX_STANDARD=17 -DLLVM_ENABLE_EH:BOOL=ON -DLLVM_ENABLE_RTTI:BOOL=ON -DLLVM_ENABLE_PDB:BOOL=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_UCRT_LIBRARIES=ON -DCMAKE_INSTALL_PREFIX="X:/LLVM" -DLLVM_ENABLE_ASSERTIONS=OFF -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt;lld" ../llvm
```

6) Build LLVM/Clang
```
ninja all
```

7) Install LLVM/Clang to `X:/LLVM`

```
ninja install
```

## Build

```
git clone 
cd /d X:\aspose.cppdoc
mkdir build
cmake -G "Visual Studio 16 2019" -Thost=x64 -Ax64 -DLLVM_ROOT=X:\LLVM -S X:\aspose.cppdoc -B X:\aspose.cppdoc\build
cmake --open X:\aspose.cppdoc\build
```

Use the following command for installation
```
cmake --install build --config Release --prefix=<path-to-install>
```