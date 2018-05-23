cmake_minimum_required(VERSION 2.8)
project(TetrisEngine)

if (MSVC)
	# Nothing.
else ()
	set(CMAKE_CXX_FLAGS "-std=c++1y")
	option(Debug "Set projects to debug mode.")
	if (Debug)
		set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Build type.")
		add_definitions(-D_DEBUG)
	else ()
		set(CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "Build type.")
	endif ()
endif ()

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

set(SOURCES_LIB
	srcLib/ai.cpp
	srcLib/ai.h
	srcLib/block.cpp
	srcLib/block.h
	srcLib/block2.cpp
	srcLib/block2.h
	srcLib/random.h
	srcLib/rawtetrisboard.cpp
	srcLib/rawtetrisboard.h
	srcLib/rawtetrisboard2.cpp
	srcLib/rawtetrisboard2.h
	srcLib/square.h
	srcLib/tetrisboard.cpp
	srcLib/tetrisboard.h
)

include_directories(
	../SdlGui/Signal/src
	../Calculator/src
)

set(SOURCES_MAIN
	src/flags.cpp
	src/flags.h
	src/flagsexception.h
	src/main.cpp
)

add_library(TetrisEngineLib ${SOURCES_LIB})

include_directories(srcLib)

add_executable(TetrisEngine ${SOURCES_MAIN})
	
target_link_libraries(TetrisEngine
	Calculator
	TetrisEngineLib
)
