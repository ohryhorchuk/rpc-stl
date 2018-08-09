cmake_minimum_required (VERSION 2.6)
project (rpc-stl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wunused-variable -Wall -m64 -O3 -Wc++11-extensions -Wc++17-extensions")

add_library(rpc-stl main.cpp)
add_executable(rpc-stl-app main.cpp)