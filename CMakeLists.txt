cmake_minimum_required (VERSION 3.8)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

project (JKGadgets)

add_subdirectory(third_party/googletest)
add_subdirectory(third_party/rapidjson)
add_subdirectory(Demo)
add_subdirectory(IGI)
add_subdirectory(PNGParvus)
add_subdirectory(Reflitest)