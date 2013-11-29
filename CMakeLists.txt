CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MWetris)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if (MSVC)
	# Exception handler model.
	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc -D_SCL_SECURE_NO_WARNINGS")
	SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
	SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm") # ENET
	
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /nodefaultlib:libcmt.lib /nodefaultlib:msvcrt")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /nodefaultlib:libcmt.lib /nodefaultlib:msvcrt.lib")
else (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
SET(SOURCES
	src/actionhandler.cpp
	src/actionhandler.h
	src/ai.cpp
	src/ai.h
	src/background.h
	src/barcolor.cpp
	src/barcolor.h
	src/bar.h
	src/block.cpp
	src/block.h
	src/button.cpp
	src/button.h
	src/component.h
	src/computer.cpp
	src/computer.h
	src/device.h
	src/frame.cpp
	src/frame.h
	src/gamefont.cpp
	src/gamefont.h
	src/gamesound.cpp
	src/gamesound.h
	src/gamesprite.cpp
	src/gamesprite.h
	src/graphicboard.cpp
	src/graphicboard.h
	src/group.h
	src/guitypedefs.h
	src/guiwindow.cpp
	src/guiwindow.h
	src/highscore.cpp
	src/highscore.h
	src/imagebackground.h
	src/inputformatter.h
	src/inputnumberformatter.h
	src/joystick.cpp
	src/joystick.h
	src/keyboard.cpp
	src/keyboard.h
	src/localplayer.cpp
	src/localplayer.h
	src/main.cpp
	src/manbutton.cpp
	src/manbutton.h
	src/multiframe.cpp
	src/multiframe.h
	src/networkevent.h
	src/networklooby.cpp
	src/networklooby.h
	src/panel.h
	src/panel.cpp
	src/player.cpp
	src/player.h
	src/playerinfo.cpp
	src/playerinfo.h
	src/protocol.cpp
	src/protocol.h
	src/rawtetrisboard.cpp
	src/rawtetrisboard.h
	src/remoteplayer.h
	src/square.h
	src/tetrisboard.cpp
	src/tetrisboard.h
	src/tetrisgame.cpp
	src/tetrisgame.h
	src/tetrisparameters.h
	src/tetriswindow.cpp
	src/tetriswindow.h
	src/textbox.cpp
	src/textboxdraw.cpp
	src/textboxdraw.h
	src/textbox.h
	src/textbutton.cpp
	src/textbutton.h
	src/textitem.cpp
	src/textitem.h
	src/userconnection.h
	src/widget.h
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

FILE(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} 
	${SDL2_IMAGE_INCLUDE_DIRS} ${OpenGL_INCLUDE_DIR} "MwEnet/MwExtra/src" 
	"MwEnet" "MwEnet/enet-1.3.9" "MwLibrary2/src"
)

ADD_EXECUTABLE(MWetris ${SOURCES})

TARGET_LINK_LIBRARIES(MWetris MwExtra MwEnet MwLibrary2 ${WINDOWS_ENET_DEPENDENCIES} 
	${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} 
	${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${OPENGL_LIBRARIES}
)
