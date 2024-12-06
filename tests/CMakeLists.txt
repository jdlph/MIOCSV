project(test_miocsv)

add_executable(${PROJECT_NAME} miocsv_gtest.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_FILE="${DATA_DIR}/test.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_CRLF_FILE="${DATA_DIR}/test_CRLF.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE ILLFORMED_FILE="${DATA_DIR}/illformed.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE ILLFORMED_CRLF_FILE="${DATA_DIR}/illformed_CRLF.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE BENCHMARK_FILE="${DATA_DIR}/benchmark.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE BENCHMARK_CRLF_FILE="${DATA_DIR}/benchmark_CRLF.csv")

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})