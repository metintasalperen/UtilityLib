# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("UtilityLib")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory ("FileLib")
add_subdirectory ("StringLib")
add_subdirectory ("SocketLib")
add_subdirectory ("TftpLib")
add_subdirectory ("BitManipulationLib")
add_subdirectory ("AlgorithmLib")

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/UtilityLib/${CMAKE_BUILD_TYPE}")

install(
	TARGETS FileLib StringLib SocketLib TftpLib BitManipulationLib
	DESTINATION lib
)

install(
	DIRECTORY FileLib/include StringLib/include SocketLib/include TftpLib/include BitManipulationLib/include
	DESTINATION "./"
)