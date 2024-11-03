cmake_minimum_required (VERSION 3.18)

project ("miocsv")

# find_package(LLVM REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DATA_DIR ${PROJECT_SOURCE_DIR}/data)

include_directories(include)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)

FetchContent_Declare(
        benchmark
        GIT_REPOSITORY "https://github.com/google/benchmark.git"
        GIT_TAG v1.5.0
)

FetchContent_MakeAvailable(googletest benchmark)

# enable_testing()
# add_subdirectory(tests)

add_subdirectory(benchmarks)