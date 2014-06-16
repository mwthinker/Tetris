CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Calculator)

set(SOURCES
	src/main.cpp
	src/calc/calculator.cpp
	src/calc/calculator.h
)

add_executable(Calculator ${SOURCES})
