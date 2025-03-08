# CMakeList.txt : CMake project for UtilityLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_library(UtilityLib STATIC
    src/error/ErrorPkg.cpp
    src/string/StringPkg.cpp
    src/file/FilePkg.cpp
    src/winsock/SocketPkg.cpp)

target_include_directories(UtilityLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/string
    ${CMAKE_CURRENT_SOURCE_DIR}/include/file
    ${CMAKE_CURRENT_SOURCE_DIR}/include/winsock
    ${CMAKE_CURRENT_SOURCE_DIR}/include/error)

set_target_properties(UtilityLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
