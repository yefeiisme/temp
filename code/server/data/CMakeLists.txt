cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O2 -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -g")
#SET(CMAKE_C_COMPILER g++)

add_compile_options(-std=c++11)

project (centerserver)
if (NOT SET_OUTPUT_DIRECTORY)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../bin/server")
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../../share/llib")
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "../../share/llib")
	set (SET_OUTPUT_DIRECTORY ON)
endif (NOT SET_OUTPUT_DIRECTORY)

SET(SRC_LIST
Main.cpp
CenterServer.cpp
Config/CenterServerConfig.cpp
Connection/AppConnection.cpp
Connection/ClientConnection.cpp
Connection/DataConnection.cpp
Connection/WebConnection.cpp
)

include_directories (
	"./"
	"../../share/header"
	"../../share/header/Base"
	"../../share/header/CenterServer"
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../share/llib)

ADD_EXECUTABLE(centerserver ${SRC_LIST})

add_subdirectory ("CenterServerLogic" "CenterServerLogic")

target_link_libraries (centerserver
	daemon
	filelog
	filereader
	network
	database
	ringbuffer
	centerserverlogic
	protobuf
	pthread
	mysqlclient
)

