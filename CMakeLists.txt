cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # because of c++17
project(kabufuda)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

unset(KABUFUDA_INCLUDE_DIR CACHE)
#set(KABUFUDA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "kabufuda include path" FORCE)

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
)

if(WIN32)
  if (MSVC)
    target_compile_options(kabufuda PRIVATE
      # Enforce various standards compliant behavior.
      /permissive-

      # Enable standard volatile semantics.
      /volatile:iso

      # Reports the proper value for the __cplusplus preprocessor macro.
      /Zc:__cplusplus

      # Allow constexpr variables to have explicit external linkage.
      /Zc:externConstexpr

      # Assume that new throws exceptions, allowing better code generation.
      /Zc:throwingNew
    )
  endif()

  target_sources(kabufuda PRIVATE
    include/kabufuda/winsupport.hpp
    lib/kabufuda/AsyncIOWin32.cpp
  )
else()
  target_sources(kabufuda PRIVATE
    lib/kabufuda/AsyncIOPosix.cpp
  )
endif()

target_include_directories(kabufuda PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_subdirectory(test)
