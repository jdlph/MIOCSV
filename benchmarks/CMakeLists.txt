add_executable(benchmark benchmark.cpp)

target_link_libraries(benchmark PRIVATE benchmark::benmark)
target_compile_definitions(benchmark PRIVATE DATA_PATH=${DATA_DIR})