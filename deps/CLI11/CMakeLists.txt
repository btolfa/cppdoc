cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(CLI11 CXX)

add_library(CLI11 INTERFACE)
target_include_directories(CLI11 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(CLI11 INTERFACE cxx_std_17)