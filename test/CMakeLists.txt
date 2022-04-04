cmake_minimum_required (VERSION 3.8)

project ("miocsv")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(miocsv.demo demo.cpp)

include_directories(../include)