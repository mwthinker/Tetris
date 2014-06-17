CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Calculator)

set(SOURCES
	src/calc/calculator.cpp
	src/calc/calculator.h
	src/calc/cache.cpp
	src/calc/cache.h
	src/calc/symbol.cpp
	src/calc/symbol.h
)

ADD_LIBRARY(Calculator ${SOURCES})
