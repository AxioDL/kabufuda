cmake_minimum_required(VERSION 3.0)
project(Card)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-multichar -fno-exceptions")
include_directories(include)

add_library(card STATIC
    include/Card.hpp src/Card.cpp)

add_subdirectory(test)
