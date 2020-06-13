# Immersive Graphic Illusion
project(IGI CXX)

aux_source_directory(src/geometry SOURCES)
aux_source_directory(src/integrator SOURCES)
aux_source_directory(src/scene SOURCES)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} PUBLIC PNGParvus rflite RapidJSON)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	target_compile_options(${PROJECT_NAME} PUBLIC ${COMPILE_OPTIONS} "-mavx" "/clang:-ffast-math")
endif()