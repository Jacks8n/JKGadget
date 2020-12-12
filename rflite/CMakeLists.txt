project(rflite)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ./)

# Testing
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(rflite_test sample.cpp "rflite/internal/attribute.h"  "rflite/internal/utilities.h" "rflite/internal/refl_dynamic.h" "rflite/internal/refl_static.h")

target_link_libraries(rflite_test PRIVATE
    rflite
    gtest
    gtest_main
)

add_test(rflite_test rflite_test)