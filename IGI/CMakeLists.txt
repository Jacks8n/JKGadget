# Immersive Graphic Illusion
project(IGI)

aux_source_directory(src/geometry SOURCES)
aux_source_directory(src/integrator SOURCES)
aux_source_directory(src/scene SOURCES)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} INTERFACE PNGParvus)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(IGI PUBLIC "-std=c++17")
	target_compile_definitions(IGI PUBLIC "_HAS_CXX17" "_HAS_CXX20")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(IGI PUBLIC "/std:c++latest")
endif()
