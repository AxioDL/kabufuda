cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # because of c++17
project(kabufuda)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

unset(KABUFUDA_INCLUDE_DIR CACHE)
#set(KABUFUDA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "kabufuda include path" FORCE)

unset(PLAT_SRCS)
if(WIN32)
list(APPEND PLAT_SRCS include/kabufuda/winsupport.hpp lib/kabufuda/AsyncIOWin32.cpp)
else()
list(APPEND PLAT_SRCS lib/kabufuda/AsyncIOPosix.cpp)
endif()

add_library(kabufuda STATIC
    include/kabufuda/Constants.hpp
    include/kabufuda/AsyncIO.hpp
    include/kabufuda/BlockAllocationTable.hpp lib/kabufuda/BlockAllocationTable.cpp
    include/kabufuda/Card.hpp lib/kabufuda/Card.cpp
    include/kabufuda/Directory.hpp lib/kabufuda/Directory.cpp
    include/kabufuda/File.hpp lib/kabufuda/File.cpp
    include/kabufuda/Util.hpp lib/kabufuda/Util.cpp
    include/kabufuda/SRAM.hpp lib/kabufuda/SRAM.cpp
    include/kabufuda/WideStringConvert.hpp lib/kabufuda/WideStringConvert.cpp
    ${PLAT_SRCS})
target_include_directories(kabufuda PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_subdirectory(test)
