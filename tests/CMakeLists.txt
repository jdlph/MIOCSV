project(test_miocsv)

add_executable(${PROJECT_NAME} test_miocsv.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_FILE="${DATA_DIR}/regular.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_FILE_CRLF="${DATA_DIR}/regular_CRLF.csv")
target_compile_definitions(${PROJECT_NAME} PRIVATE ILL_FILE="${DATA_DIR}/ill_format.csv")

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})