cmake_minimum_required(VERSION 2.8)
project(MWetris)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/ ${CMAKE_CURRENT_SOURCE_DIR}/SdlGui/MwLibrary2/cmake/Modules/)

if (MSVC)
	# Exception handler model.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
else ()
	set(CMAKE_CXX_FLAGS "-std=c++11")
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
	src/color.h
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
	src/dynamicbuffer.cpp
	src/dynamicbuffer.h
	src/gamecomponent.cpp
	src/gamecomponent.h
	src/gamegraphic.cpp
	src/gamegraphic.h
	src/guiclasses.h
	src/highscore.cpp
	src/highscore.h
	src/joystick.cpp
	src/joystick.h
	src/keyboard.cpp
	src/keyboard.h
	src/lightningbolt.cpp
	src/lightningbolt.h
	src/lightningboltcluster.cpp
	src/lightningboltcluster.h
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
	src/staticbuffer.cpp
	src/staticbuffer.h
	src/tetrisentry.cpp
	src/tetrisentry.h
	src/tetrisgame.cpp
	src/tetrisgame.h
	src/tetrisgameevent.h
	src/tetrisparameters.h
	src/tetriswindow.cpp
	src/tetriswindow.h
	src/vertexdata.cpp
	src/vertexdata.h
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

add_subdirectory(DataEntry)
add_subdirectory(SdlGui)
add_subdirectory(Calculator)
add_subdirectory(SimpleNetwork)
add_subdirectory(TetrisEngine)

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
	DataEntry/src
	DataEntry/TinyXML2/src
	Calculator/src
	TetrisEngine/src
)

add_executable(MWetris ${SOURCES})

if (MSVC)
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt")
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
endif ()

target_link_libraries(MWetris
	Calculator
	TinyXML2
	DataEntry
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
