# CMakeList.txt : CMake project for TftpServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TftpServer)

add_executable(TftpServer
    src/main.cpp
)

target_include_directories(TftpServer PRIVATE
    ${CMAKE_SOURCE_DIR}/UtilityLib/include/error
    ${CMAKE_SOURCE_DIR}/UtilityLib/include/file
    ${CMAKE_SOURCE_DIR}/UtilityLib/include/string
    ${CMAKE_SOURCE_DIR}/UtilityLib/include/winsock
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(TftpServer PRIVATE
    ${CMAKE_SOURCE_DIR}/out/build/x64-Debug/UtilityLib/UtilityLib.lib)

set_target_properties(TftpServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
