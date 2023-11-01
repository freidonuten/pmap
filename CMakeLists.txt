cmake_minimum_required(VERSION 3.14)
project(frozen_trie)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

add_subdirectory(test)
add_subdirectory(sample)
add_subdirectory(benchmark)
