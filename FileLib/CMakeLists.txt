# CMakeList.txt : CMake project for FileLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(FileLib)

add_library(${PROJECT_NAME} STATIC
    src/FilePkg.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/StringLib/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/StringLib/StringLib.lib
)

# TODO: Add tests and install targets if needed.
