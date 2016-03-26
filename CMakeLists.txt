cmake_minimum_required(VERSION 3.0)
project(kabufuda)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-multichar -fno-exceptions")
include_directories(include)

add_library(kabufuda STATIC
    include/Card.hpp src/Card.cpp
    include/Util.hpp src/Util.cpp
    include/SRAM.hpp src/SRAM.cpp
    include/WideStringConvert.hpp src/WideStringConvert.cpp)

add_subdirectory(test)
