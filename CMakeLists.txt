cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

cmake_policy(SET CMP0074 OLD) # Use LLVM_ROOT as a regular variable

project(headers-checker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LLVM_ROOT "LLVM root directory")

if (NOT LLVM_ROOT)
    message(FATAL_ERROR "LLVM_ROOT option is not set, invoke cmake with -DLLVM_ROOT=<path_to_llvm>")
endif()

if (NOT EXISTS ${LLVM_ROOT})
    message(FATAL_ERROR "'${LLVM_ROOT}' directory is not found, invoke cmake with valid LLVM_ROOT option")
endif()

add_executable(headers-checker)

# Find ClangConfig.cmake which will find_package(LLVM)
find_package(Clang REQUIRED CONFIG PATHS ${LLVM_ROOT} PATH_SUFFIXES "lib/cmake/clang" NO_DEFAULT_PATH)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Build Type: ${LLVM_BUILD_TYPE}")
message(STATUS "Using LLVMConfig.cmake in ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in ${CLANG_CMAKE_DIR}")

# Check LLVM build options

if (DEFINED CMAKE_BUILD_TYPE AND NOT (CMAKE_BUILD_TYPE STREQUAL LLVM_BUILD_TYPE))
    message(FATAL_ERROR
        "Variables CMAKE_BUILD_TYPE (${CMAKE_BUILD_TYPE}) and LLVM_BUILD_TYPE (${LLVM_BUILD_TYPE}) must be equal, "
        " invoke cmake with -DCMAKE_BUILD_TYPE=${LLVM_BUILD_TYPE}")
endif()

if (NOT LLVM_ENABLE_EH OR NOT LLVM_ENABLE_RTTI)
    message(FATAL_ERROR
        "LLVM must be built with exception-handling support, use options "
        "-DLLVM_ENABLE_EH=ON and -DLLVM_ENABLE_RTTI=ON")
endif()

target_include_directories(headers-checker PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(headers-checker PRIVATE clangTooling)

add_subdirectory(deps/json)
add_subdirectory(deps/CLI11)
add_subdirectory(deps/fmt)

target_sources(headers-checker PRIVATE 
    src/main.cpp
)
target_include_directories(headers-checker PRIVATE src)
target_link_libraries(headers-checker PRIVATE nlohmann::json CLI11 fmt::fmt)

install(TARGETS headers-checker DESTINATION .)
