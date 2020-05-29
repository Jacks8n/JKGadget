cmake_minimum_required (VERSION 3.8)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_SILENCE_CLANG_CONCEPTS_MESSAGE)

project (JKGadgets)

add_subdirectory(third_party/googletest)
add_subdirectory(third_party/rapidjson)
add_subdirectory(Demo)
add_subdirectory(IGI)
add_subdirectory(PNGParvus)
add_subdirectory(rflite)