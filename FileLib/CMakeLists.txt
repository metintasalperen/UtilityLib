# CMakeList.txt : CMake project for FileLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_library(FileLib STATIC
    src/FilePkg.cpp)

target_include_directories(FileLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(FileLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
