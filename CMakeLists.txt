cmake_minimum_required(VERSION 3.5)

enable_testing()
project(sat-solver)

FIND_PACKAGE(BISON 3.0.4 REQUIRED)
FIND_PACKAGE(FLEX 2.5.35 REQUIRED)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(./src)





