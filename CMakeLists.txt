# FetchContent_Declare requires 3.14 or higher
cmake_minimum_required (VERSION 3.14)

project ("miocsv")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ensure consistent visibility flags
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(DATA_DIR ${PROJECT_SOURCE_DIR}/data)

include(FetchContent)

# place it before benchmark as benchmark has dependency on googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)

# v1.5.0 does not work with googletest 1.10.0
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.0
)

FetchContent_MakeAvailable(googletest benchmark)

include_directories(include)

option(ENABLE_TESTING "Enable testing" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

option(ENABLE_BENCHMARKS "Enable benchmarks" ON)
if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()