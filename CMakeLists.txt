# R. A. Bertens, 2018
# move project to CMAKE

project(MixingManager CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(ROOT 5.34 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# we need ROOT dictionaries and maps
include(ROOTHelper)

# using ROOT as system library
include_directories(SYSTEM ${ROOT_INCLUDE_DIR})
link_directories(SYSTEM ${ROOT_LIBDIR})

include_directories("${CMAKE_SOURCE_DIR}/src")

add_subdirectory(src)
