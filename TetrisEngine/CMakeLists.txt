cmake_minimum_required(VERSION 2.8)
project(TetrisEngine)

if (MSVC)
	# Nothing.
else ()
	set(CMAKE_CXX_FLAGS "-std=c++1y")
	set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(TetrisEngineTest "TetrisEngineTest project is added" OFF)

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

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
	../Calculator/src
)

set(SOURCES_TEST
	srcTest/main.cpp)

add_library(TetrisEngine ${SOURCES})

if (TetrisEngineTest)
	include_directories(src)
	
	add_executable(TetrisEngineTest ${SOURCES_TEST})
	
	target_link_libraries(TetrisEngineTest
		Calculator
		TetrisEngine
	)
endif ()
