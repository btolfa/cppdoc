cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(nlohmann_json CXX)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(nlohmann_json INTERFACE cxx_std_17)

add_library(nlohmann::json ALIAS nlohmann_json)