cmake_minimum_required (VERSION 3.8)

project ("miocsv")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

# add_executable(miocsv.demo demo.cpp)
add_executable(miocsv.test test.cpp)

include_directories(../include)