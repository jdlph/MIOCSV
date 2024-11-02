cmake_minimum_required (VERSION 3.10)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.297.tar.gz"
    SHA1 "3319fe6a3b08090df7df98dee75134d68e2ef5a3"
)

project ("miocsv")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DATA_DIR ${PROJECT_SOURCE_DIR}/data)

include_directories(include)

# hunter_add_package(GTest)
hunter_add_package(benchmark)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

enable_testing()
# add_subdirectory(tests)

add_subdirectory(benchmarks)