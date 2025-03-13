# CMakeList.txt : CMake project for StringLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_library(StringLib STATIC
    src/StringPkg.cpp)

target_include_directories(StringLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(StringLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
