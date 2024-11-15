project(demo_miocsv)

add_executable(${PROJECT_NAME} demo.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE INPUT_FILE="${DATA_DIR}/test.csv")