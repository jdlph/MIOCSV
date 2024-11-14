project(test_miocsv)

add_executable(${PROJECT_NAME} test_miocsv.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_FILE="${DATA_DIR}/test.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_FILE_CRLF="${DATA_DIR}/test_CRLF.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE ILLFORMED_FILE="${DATA_DIR}/illformed.csv")

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})