# CMakeList.txt : CMake project for TftpLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TftpLib)

add_library(TftpLib STATIC
    src/TftpClientCls.cpp
    src/TftpServerCls.cpp)

target_include_directories(TftpLib PRIVATE
    ${CMAKE_SOURCE_DIR}/FileLib/include
    ${CMAKE_SOURCE_DIR}/StringLib/include
    ${CMAKE_SOURCE_DIR}/SocketLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(TftpLib PRIVATE
    ${CMAKE_BINARY_DIR}/StringLib/StringLib.lib
    ${CMAKE_BINARY_DIR}/FileLib/FileLib.lib
    ${CMAKE_BINARY_DIR}/SocketLib/SocketLib.lib)

set_target_properties(TftpLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
