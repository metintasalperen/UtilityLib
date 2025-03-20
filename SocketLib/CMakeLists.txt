# CMakeList.txt : CMake project for SocketLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(SocketLib)

add_library(SocketLib STATIC
    "src/UdpClientCls.cpp"
    "src/UdpServerCls.cpp"
    "src/TcpClientCls.cpp"
    "src/TcpServerCls.cpp"
    "src/TcpSessionCls.cpp"
)

target_include_directories(SocketLib PRIVATE
    ${CMAKE_SOURCE_DIR}/StringLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(SocketLib PRIVATE
    ${CMAKE_BINARY_DIR}/StringLib/StringLib.lib)

set_target_properties(SocketLib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

# TODO: Add tests and install targets if needed.
