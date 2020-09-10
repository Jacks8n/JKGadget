project(Demo)

add_executable(Demo main.cpp)

target_link_libraries(Demo PRIVATE IGI)

set(DEMO_CONFIG_JSON ${PROJECT_SOURCE_DIR}/demo.json)

add_custom_target(ConfigFiles ALL
    ${CMAKE_COMMAND} -E copy_if_different ${DEMO_CONFIG_JSON} ${Demo_BINARY_DIR}/demo.json
    DEPENDS ${DEMO_CONFIG_JSON}
    VERBATIM USES_TERMINAL)

add_dependencies(Demo ConfigFiles)