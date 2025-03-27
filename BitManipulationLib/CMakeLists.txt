# CMakeList.txt : CMake project for BitManipulationLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(BitManipulationLib)

add_library(${PROJECT_NAME} STATIC
    src/BitManipulationPkg.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
