cmake_minimum_required(VERSION 2.8)
project(rlutil)
if (MSVC)
	# Exception handler model.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
else (MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

option(rlutilTest "LoadXmlTest project is added" OFF)
option(rlutilTestGame "LoadXmlTest project is added" OFF)

# Source files.
set(SOURCES
	src/rlutil.h
	src/rlutil.cpp
)

set(SOURCES_TEST
	srcTest/test.cpp
)

set(SOURCES_TEST_GAME
	srcTestGame/example.cpp
)

# End of source files.

add_library(rlutil ${SOURCES})

if (rlutilTest)
	include_directories(src)
	
	add_executable(rlutilTest ${SOURCES_TEST})
	
	target_link_libraries(rlutilTest rlutil)
endif (rlutilTest)

if (rlutilTestGame)
	include_directories(src)
	
	add_executable(rlutilTestGame ${SOURCES_TEST_GAME})
	
	target_link_libraries(rlutilTestGame rlutil)
endif (rlutilTestGame)

