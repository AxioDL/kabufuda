cmake_minimum_required(VERSION 3.0)
project(kabufuda)

if(NOT TARGET hecl)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    endif()
endif()

include_directories(include)
set(KABUFUDA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "kabufuda include path" FORCE)

unset(PLAT_SRCS)
if(WIN32)
list(APPEND PLAT_SRCS lib/kabufuda/winsupport.cpp include/kabufuda/winsupport.hpp)
endif()

add_library(kabufuda STATIC
    include/kabufuda/Constants.hpp
    include/kabufuda/BlockAllocationTable.hpp lib/kabufuda/BlockAllocationTable.cpp
    include/kabufuda/Card.hpp lib/kabufuda/Card.cpp
    include/kabufuda/Directory.hpp lib/kabufuda/Directory.cpp
    include/kabufuda/File.hpp lib/kabufuda/File.cpp
    include/kabufuda/Util.hpp lib/kabufuda/Util.cpp
    include/kabufuda/SRAM.hpp lib/kabufuda/SRAM.cpp
    include/kabufuda/WideStringConvert.hpp lib/kabufuda/WideStringConvert.cpp
    ${PLAT_SRCS})

add_subdirectory(test)
