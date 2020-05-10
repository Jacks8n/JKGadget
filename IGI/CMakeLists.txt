# Immersive Graphic Illusion
project(IGI)

aux_source_directory(src/geometry SOURCES)
aux_source_directory(src/integrator SOURCES)
aux_source_directory(src/scene SOURCES)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} PNGParvus Reflitest)

target_compile_options(IGI PUBLIC ${COMPILE_OPTIONS} "-mavx")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
target_compile_options(IGI PUBLIC ${COMPILE_OPTIONS} "/clang:-ffast-math")
endif()