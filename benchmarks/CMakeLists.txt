project(benchmark_miocsv)

add_executable(${PROJECT_NAME} benchmark.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)
target_compile_definitions(${PROJECT_NAME} PRIVATE DATA_PATH="${DATA_DIR}")