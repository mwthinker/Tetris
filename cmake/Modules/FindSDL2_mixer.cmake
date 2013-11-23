# - Try to find SDL2_image
# Once done this will define
#  SDL2_MIXER_FOUND - System has SDL2_mixer
#  SDL2_MIXER_INCLUDE_DIRS - The SDL2_mixer include directory
#  SDL2_MIXER_LIBRARIES - The libraries needed to use SDL2_mixer
#  SDL2_MIXER_DEFINITIONS - Compiler switches required for using SDL2_mixer

SET(SDL2_SEARCH_PATHS 
	/usr/local
	/usr
	/opt
)

FIND_PATH(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES include/SDL2 include
    PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib lib/x86
    PATHS ${SDL2_SEARCH_PATHS}
)

SET(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
SET(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set SDL2_MIXER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SDL2_mixer DEFAULT_MSG SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)

mark_as_advanced(SDL2_MIXER_INCLUDE_DIR SDL2_MIXER_LIBRARY)
