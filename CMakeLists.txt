cmake_minimum_required (VERSION 3.15)

project ("miocsv")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

set(DATA_DIR ${PROJECT_SOURCE_DIR}/data)

include_directories(include)
add_subdirectory(examples)

option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_BENCHMARKS "Enable benchmarks" ON)

if (ENABLE_TESTING OR ENABLE_BENCHMARKS)
    # ensure consistent visibility flags
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

    include(FetchContent)

    if (ENABLE_TESTING)
        message("-- testing enabled")
        # GoogleTest requires to be multi-threaded statically-linked on Windows
        if (CMAKE_SYSTEM MATCHES Windows)
            # it requires CMake 3.15 or higher
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )

        FetchContent_MakeAvailable(googletest)

        enable_testing()
        add_subdirectory(tests)
    endif()

    if (ENABLE_BENCHMARKS)
        message("-- benchmark enabled")

        # v1.5.0 does not work with GoogleTest 1.10.0
        FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.0
        )

        # disable GoogleBenchmark's all built-in tests including these depending on GoogleTest
        # thus remove this dependency.
        set(BENCHMARK_ENABLE_TESTING OFF)
        FetchContent_MakeAvailable(benchmark)

        add_subdirectory(benchmarks)
    endif()
endif()
