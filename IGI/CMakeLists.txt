# Immersive Graphic Illusion
project(IGI)

set(${PROJECT_NAME} CXX_STANDARD 17)

aux_source_directory(src/geometry SOURCES)
aux_source_directory(src/integrator SOURCES)
aux_source_directory(src/scene SOURCES)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} INTERFACE PNGParvus)