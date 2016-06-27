cmake_minimum_required(VERSION 3.0)
project(kabufuda)

if(NOT TARGET urde)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
endif()

include_directories(include)
set(KABUFUDA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "kabufuda include path" FORCE)

add_library(kabufuda STATIC
    include/kabufuda/Card.hpp lib/Card.cpp
    include/kabufuda/Util.hpp lib/Util.cpp
    include/kabufuda/SRAM.hpp lib/SRAM.cpp
    include/kabufuda/WideStringConvert.hpp lib/WideStringConvert.cpp)

if (NOT TARGET urde)
    add_subdirectory(test)
endif()
