CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MWetris)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
IF (MSVC)
	# Exception handler model.
	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc -D_SCL_SECURE_NO_WARNINGS")
	SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
	SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm") # ENET
ELSE (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
ENDIF (MSVC)

# Source files.
SET(SOURCES
	src/actionhandler.cpp
	src/actionhandler.h
	src/ai.cpp
	src/ai.h
	src/block.cpp
	src/block.h
	src/computer.cpp
	src/computer.h
	src/device.h
	src/gamecomponent.h
	src/gamefont.cpp
	src/gamefont.h
	src/gamesound.cpp
	src/gamesound.h
	src/gamesprite.cpp
	src/gamesprite.h
	src/graphicboard.cpp
	src/graphicboard.h
	src/highscore.cpp
	src/highscore.h
	src/joystick.cpp
	src/joystick.h
	src/keyboard.cpp
	src/keyboard.h
	src/localplayer.cpp
	src/localplayer.h
	src/main.cpp
	src/manbutton.cpp
	src/manbutton.h
	src/networkevent.h
	src/networklooby.cpp
	src/networklooby.h
	src/player.cpp
	src/player.h
	src/playerinfo.cpp
	src/playerinfo.h
	src/rawtetrisboard.cpp
	src/rawtetrisboard.h
	src/remoteplayer.h
	src/square.h
	src/tetrisboard.cpp
	src/tetrisboard.h
	src/tetrisgame.cpp
	src/tetrisgame.h
	src/tetrisrules.cpp
	src/tetrisrules.h
	src/tetrisparameters.h
	src/tetriswindow.cpp
	src/tetriswindow.h
	src/userconnection.h
)
# End of source files.

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)
FIND_PACKAGE(SDL2_ttf REQUIRED)

ADD_SUBDIRECTORY(TinyXML2)
ADD_SUBDIRECTORY(MwEnet)
ADD_SUBDIRECTORY(MwLibrary2)
ADD_SUBDIRECTORY(Gui)

FILE(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

INCLUDE_DIRECTORIES(
	${SDL2_INCLUDE_DIRS} 
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS} 
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIR}
	MwEnet/MwExtra/src
	MwEnet/
	MwEnet/enet-1.3.9/
	MwLibrary2/src
	Gui/src
)

ADD_EXECUTABLE(MWetris ${SOURCES})
IF (WIN32)
	SET_TARGET_PROPERTIES (MWetris PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(MWetris MwExtra MwEnet MwLibrary2 Gui ${WINDOWS_ENET_DEPENDENCIES} 
	${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} 
	${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${OPENGL_LIBRARIES}
)
