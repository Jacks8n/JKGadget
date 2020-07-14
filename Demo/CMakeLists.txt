project(Demo)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE IGI)

add_custom_command(TARGET Demo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
        copy_if_different
        "${PROJECT_SOURCE_DIR}/demo.json"
        "${PROJECT_BINARY_DIR}")