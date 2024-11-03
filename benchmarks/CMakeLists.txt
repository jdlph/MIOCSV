project(benchmark_miocsv)

add_executable(${PROJECT_NAME} benchmark_miocsv.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)
target_compile_definitions(${PROJECT_NAME} PRIVATE INPUT_FILE="${DATA_DIR}/csvreader.csv")