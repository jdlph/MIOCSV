cmake_minimum_required (VERSION 3.8)

project ("miocsv")

option(BUILD_TEST OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (BUILD_TEST)
    message("build miocsv.test for benchmarking")
    set(CMAKE_BUILD_TYPE Release)
    add_executable(miocsv.test test.cpp)
else()
    message("build miocsv.demo for demonstration")
    add_executable(miocsv.demo demo.cpp)
endif()

include_directories(../include)