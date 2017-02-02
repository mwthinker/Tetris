cmake_minimum_required(VERSION 2.8)
project(TetrisEngine)

if (MSVC)
	# Nothing.
else ()
	SET(CMAKE_CXX_FLAGS "-std=c++11")
	set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(TetrisEngineTest "TetrisEngineTest project is added" OFF)

set(SOURCES
	src/ai.cpp
	src/ai.h
	src/block.cpp
	src/block.h	
	src/random.h
	src/rawtetrisboard.cpp
	src/rawtetrisboard.h
	src/square.h
	src/tetrisboard.cpp
	src/tetrisboard.h
)

include_directories(
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
		Calculator)
endif ()
