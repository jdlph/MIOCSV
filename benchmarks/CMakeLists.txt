find_package(benchmark REQUIRED)

add_executable(mytest benchmark.cpp)

target_link_libraries(mytest PRIVATE benchmark::benchmark)
target_compile_definitions(mytest PRIVATE DATA_PATH="${DATA_DIR}")