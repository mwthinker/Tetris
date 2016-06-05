cmake_minimum_required(VERSION 2.8)
project(TetrisEngine)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/ ${CMAKE_CURRENT_SOURCE_DIR}/Tetris/TetrisEngine/cmake/Modules/)

if (MSVC)
	# Nothing.
else ()
	SET(CMAKE_CXX_FLAGS "-std=c++11")
	set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(TetrisEngineTest "TetrisEngineTest project is added" OFF)

set(SOURCES
	src/ai.h
	src/block.cpp
	src/block.h
	src/computer.cpp
	src/computer.h
	src/device.h
	src/player.cpp
	src/player.h
	src/rawtetrisboard.cpp
	src/rawtetrisboard.h
	src/square.h
	src/tetrisboard.cpp
	src/tetrisboard.h)

find_package(SDL2 REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIRS}
	../SdlGui/Signal/src
	../Calculator/src)

set(SOURCES_TEST
	srcTest/main.cpp)

add_library(TetrisEngine ${SOURCES})

if (TetrisEngineTest)
	include_directories(src)
	
	add_executable(TetrisEngineTest ${SOURCES_TEST})
	
	target_link_libraries(TetrisEngineTest
		Signal
		TetrisEngine
		Calculator
		${SDL2_LIBRARIES})
endif ()
