cmake_minimum_required(VERSION 3.0)
project(MWetris)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/ ${CMAKE_CURRENT_SOURCE_DIR}/SimpleNetwork/cmake/Modules/ ${CMAKE_CURRENT_SOURCE_DIR}/SdlGui/MwLibrary2/cmake/Modules/)
if (MSVC)
	# Exception handler model.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32 -D_WINSOCKAPI_)
	# D_WINSOCKAPI_ makes redefinition warnings disappear.

	set(WIN_RESOURCE_FILE ${CMAKE_SOURCE_DIR}/data/package/mwetris.rc)
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

set(MWETRIS_VERSION_MAJOR "2")
set(MWETRIS_VERSION_MINOR "1")
set(MWETRIS_VERSION_PATCH "1")

# Source files.
set(SOURCES
	src/actionhandler.cpp
	src/actionhandler.h
	src/boardbatch.h
	src/boardshader.cpp
	src/boardshader.h
	src/computer.cpp
	src/computer.h
	src/connection.h
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
	src/localconnection.cpp
	src/localconnection.h
	src/localplayer.cpp
	src/localplayer.h
	src/main.cpp
	src/manbutton.cpp
	src/manbutton.h
	src/player.cpp
	src/player.h
	src/playerdata.h
	src/protocol.cpp
	src/protocol.h
	src/remoteconnection.cpp
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

set(SOURCES_TEST
	src/testmain.cpp
	src/testspeed.cpp
	src/testtetrisengine.cpp
)

# End of source files.

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CATCH2)

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
	add_executable(MWetris ${SOURCES} ${SOURCES_CONSOLE} ${WIN_RESOURCE_FILE})
else ()
	set(ConsoleTest OFF)
	add_executable(MWetris ${SOURCES})
endif ()

if (MSVC)
	set_target_properties(MWetris PROPERTIES
		LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt"
	)
	set_target_properties(MWetris PROPERTIES
		LINK_FLAGS_RELEASE "/SUBSYSTEM:windows"
	)
endif ()

set(LINK_LIBS
	${CONSOLE_LIB}
	SimpleNetwork
	SdlGui
	MwLibrary2
	TetrisEngine
	Calculator
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_NET_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(MWetris ${LINK_LIBS})

if (CATCH2_FOUND)
	add_executable(MWetrisTest ${SOURCES_TEST})	
	
	target_include_directories(MWetrisTest
		PUBLIC ${CATCH2_INCLUDE_DIRS}
		PUBLIC src
	)

	add_library(MWetrisLib ${SOURCES})
	target_link_libraries(MWetrisTest MWetrisLib ${LINK_LIBS})
endif ()

set(DLL_FOLDER $ENV{DLL_FOLDER})

if (MSVC AND NOT DLL_FOLDER)
	message(STATUS "Enviroment variable DLL_FOLDER not defined, installer can not be created")
endif ()

install(TARGETS MWetris
	DESTINATION .
	COMPONENT application
)

if (MSVC AND DLL_FOLDER)
	set(TMP_DLL_FOLDER ${DLL_FOLDER})
	string(REPLACE "\\" "/" DLL_FOLDER ${TMP_DLL_FOLDER})
	message(STATUS ${DLL_FOLDER} "")
	if (DLL_FOLDER)
		set(DLL_FILES
			"${DLL_FOLDER}/glew32.dll"
			"${DLL_FOLDER}/libFLAC-8.dll"
			"${DLL_FOLDER}/libfreetype-6.dll"
			"${DLL_FOLDER}/libjpeg-9.dll"
			"${DLL_FOLDER}/libmodplug-1.dll"
			"${DLL_FOLDER}/libogg-0.dll"
			"${DLL_FOLDER}/libpng16-16.dll"
			"${DLL_FOLDER}/libtiff-5.dll"
			"${DLL_FOLDER}/libvorbis-0.dll"
			"${DLL_FOLDER}/libvorbisfile-3.dll"
			"${DLL_FOLDER}/libwebp-4.dll"
			"${DLL_FOLDER}/SDL2.dll"
			"${DLL_FOLDER}/SDL2_image.dll"
			"${DLL_FOLDER}/SDL2_mixer.dll"
			"${DLL_FOLDER}/SDL2_net.dll"
			"${DLL_FOLDER}/SDL2_ttf.dll"
			"${DLL_FOLDER}/smpeg2.dll"
			"${DLL_FOLDER}/zlib1.dll"
		)
		
		install(FILES ${DLL_FILES}
			DESTINATION .
			COMPONENT runtime
		)
	else ()
		message(STATUS "Enviroment variable DLL_FOLDER not defined")
	endif ()
endif ()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/
	DESTINATION .
	COMPONENT data
)

# Replace the USE_APPLICATION_JSON file, use default saving place.
install(FILES ${CMAKE_SOURCE_DIR}/data/package/USE_APPLICATION_JSON
	DESTINATION .
	COMPONENT data
)

if (MSVC AND DLL_FOLDER)
	########################################################################
	# Tell CMake to install the windows runtime libraries to the programs
	# directory and tell CPack that they belong to the applications component
	include(InstallRequiredSystemLibraries)
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
		DESTINATION .
		COMPONENT application
	)
endif ()

set(CPACK_PACKAGE_CONTACT "mwthinker@yahoo.com")

# CPack configuration
set(CPACK_PACKAGE_NAME "MWetris")

set(CPACK_PACKAGE_VENDOR "MW")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installs MWetris")
set(CPACK_PACKAGE_VERSION_MAJOR ${MWETRIS_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MWETRIS_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${MWETRIS_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "MWetris")	

set(CPACK_GENERATOR ZIP)

# Tell CPack about the components and group the data components together.
if (MSVC)
	set(CPACK_COMPONENTS_ALL application data runtime vcredist)
else ()
	set(CPACK_COMPONENTS_ALL application data)
endif ()

# More descriptive names for each of the components, and component groups.
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Application")
set(CPACK_COMPONENT_APPLICATION_REQUIRED true)

set(CPACK_COMPONENT_DATA_DISPLAY_NAME "Data")
set(CPACK_COMPONENT_DATA_REQUIRED true)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
set(CPACK_COMPONENT_RUNTIME_REQUIRED true)

# Text from "LICENSE.txt" is displayed in the installer's license tab.
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\data\\\\package\\\\mw.bmp")

set(CPACK_PACKAGE_EXECUTABLES "MWetris" "MWetris")
set(CPACK_CREATE_DESKTOP_LINKS "MWetris")
	
if (MSVC AND DLL_FOLDER)
	option(USE_NSIS_GENERATOR "Use NSIS generator" OFF)
	if (USE_NSIS_GENERATOR)
		set(CPACK_GENERATOR ${CPACK_GENERATOR} NSIS)
		set(CPACK_NSIS_MODIFY_PATH "ON") # Ask of adding path and desktop icon.
		set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")

		set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")	
		set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
		
		# Set the icon used inside the installer wizard and the icons for the installer and uninstaller.	
		set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/data/package/mwetris.ico")
		set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/data/package/mwetris.ico")
		set(CPACK_NSIS_INSTALLED_ICON_NAME "MWetris.exe")
	 
		# Set the programs displayed under the Start menu shortcut.
		set(CPACK_NSIS_MENU_LINKS
			"MWetris" "MWetris"
			"https://github.com/mwthinker/MWetris" "Github MWetris"
		)
		
		set(CPACK_NSIS_MUI_FINISHPAGE_RUN "MWetris.exe")
	endif ()
endif ()

if (UNIX)
	option(USE_DEB_GENERATOR "Use DEB generator" OFF)
	if (USE_DEB_GENERATOR)
		set(CPACK_GENERATOR ${CPACK_GENERATOR} DEB)

		set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
		#set(CPACK_DEB_COMPONENT_INSTALL OFF)		
		#set(CPACK_DEBIAN_PACKAGE_PREDEPENDS ON)		# Hmm
		#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
		set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
		
		#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2 (>=2.0-0), libsdl2-image (>=2.0-0), libsdl2-mixer(>=2.0-0), libsdl2-net (>= 2.0-0), libsdl2-ttf (>= 2.0-0), libglew (>= 1.13)")
	endif ()
endif ()

include(CPack)
