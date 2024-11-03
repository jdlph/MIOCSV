project(test_miocsv)

add_executable(${PROJECT_NAME} test_miocsv.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)