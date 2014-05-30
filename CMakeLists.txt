cmake_minimum_required(VERSION 2.8)
project(MWetris)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if (MSVC)
	# Exception handler model.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	set(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm") # ENET
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
else (MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
set(SOURCES
	src/actionhandler.cpp
	src/actionhandler.h
	src/ai.cpp
	src/ai.h
	src/block.cpp
	src/block.h
	src/computer.cpp
	src/computer.h
	src/device.h
	src/gamecomponent.cpp
	src/gamecomponent.h
	src/gamedata.cpp
	src/gamedata.h
	src/gamefont.cpp
	src/gamefont.h
	src/gamehandler.h
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
	src/userconnection.h
)
# End of source files.

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(TinyXML2)
add_subdirectory(MwEnet)
add_subdirectory(SdlGui)

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

include_directories(
	${SDL2_INCLUDE_DIRS} 
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS} 
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIR}
	MwEnet/src
	MwEnet/Enet/src
	SdlGui/src
	SdlGui/MwLibrary2/src
	SdlGui/Signal/src
	TinyXML2/src
)

add_executable(MWetris ${SOURCES})

if (MSVC)
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt")
	set_target_properties(MWetris PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
endif (MSVC)

target_link_libraries(MWetris
	TinyXML2
	MwEnet
	Enet
	MwLibrary2
	Signal
	SdlGui
	${WINDOWS_ENET_DEPENDENCIES}
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${OPENGL_LIBRARIES}
)
