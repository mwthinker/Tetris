cmake_minimum_required(VERSION 2.8)
project(MWetris)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/ ${CMAKE_CURRENT_SOURCE_DIR}/SdlGui/MwLibrary2/cmake/Modules/)
if (MSVC)
	# Exception handler model.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32 -D_WINSOCKAPI_)
	# D_WINSOCKAPI_ makes redefinition warnings disappear.
else ()
	set(CMAKE_CXX_FLAGS "-std=c++1y")
	set(CMAKE_BUILD_TYPE "Debug")
endif ()

# Source files.
set(SOURCES
	src/actionhandler.cpp
	src/actionhandler.h
	src/boardshader.cpp
	src/boardshader.h
	src/boardsizebutton.cpp
	src/boardsizebutton.h
	src/boardvertexdata.cpp
	src/boardvertexdata.h
	src/computer.cpp
	src/computer.h
	src/definitions.h
	src/device.h
	src/drawblock.cpp
	src/drawblock.h
	src/drawrow.cpp
	src/drawrow.h
	src/drawtext.cpp
	src/drawtext.h
	src/gamecomponent.cpp
	src/gamecomponent.h
	src/gamegraphic.cpp
	src/gamegraphic.h
	src/guiclasses.h
	src/highscore.cpp
	src/highscore.h
	src/gamecontroller.cpp
	src/gamecontroller.h
	src/keyboard.cpp
	src/keyboard.h
	src/lightningbolt.cpp
	src/lightningbolt.h
	src/lightningboltcluster.cpp
	src/lightningboltcluster.h
	src/lightningshader.cpp
	src/lightningshader.h
	src/lightningvertexdata.cpp
	src/lightningvertexdata.h
	src/localconnection.h
	src/localplayer.cpp
	src/localplayer.h
	src/main.cpp
	src/manbutton.cpp
	src/manbutton.h
	src/mat44.h
	src/player.cpp
	src/player.h
	src/protocol.cpp
	src/protocol.h
	src/remoteconnection.h
	src/remoteplayer.cpp
	src/remoteplayer.h
	src/sdldevice.h
	src/tetrisdata.cpp
	src/tetrisdata.h
	src/tetrisgame.cpp
	src/tetrisgame.h
	src/tetrisgameevent.h
	src/tetrisparameters.h
	src/tetriswindow.cpp
	src/tetriswindow.h 
)

set(SOURCES_CONSOLE
	src/consolegraphic.cpp
	src/consolegraphic.h
	src/consolekeyboard.cpp
	src/consolekeyboard.h
	src/consoletetris.cpp
	src/consoletetris.h
)

# End of source files.

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

if (CMAKE_COMPILER_IS_GNUCXX)
	find_package(Threads REQUIRED) # Due to a bug, which forces the user to link to Threads when using #include <thread>.
endif (CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(SdlGui)
add_subdirectory(Calculator)
add_subdirectory(SimpleNetwork)
add_subdirectory(TetrisEngine)

option(ConsoleTetris "Console tetris is added" ON)

if (ConsoleTetris)
	add_definitions(-DCONSOLE_TETRIS)
	add_subdirectory(CppConsole)
endif ()

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

include_directories(
	${SDL2_INCLUDE_DIRS} 
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS} 
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDL2_NET_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	SimpleNetwork/src
	SdlGui/src
	SdlGui/MwLibrary2/src
	SdlGui/Signal/src
	Calculator/src
	TetrisEngine/src
	nlohmann-json/src
)

if (ConsoleTetris)
	if (MSVC)
		set(CONSOLE_LIB Console)
	else ()
		find_package(Curses REQUIRED)
		set(CONSOLE_LIB Console ${CURSES_LIBRARIES})
		include_directories(${CURSES_INCLUDE_DIR})
	endif ()

	include_directories(
		CppConsole/src
	)
	add_executable(MWetris ${SOURCES} ${SOURCES_CONSOLE})
else ()
	set(ConsoleTest OFF)
	add_executable(MWetris ${SOURCES})
endif ()

if (MSVC)
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt")
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
endif ()

target_link_libraries(MWetris
	Calculator
	${CONSOLE_LIB}
	SimpleNetwork
	MwLibrary2
	Signal
	SdlGui
	TetrisEngine
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_NET_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)
