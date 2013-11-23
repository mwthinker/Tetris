# - Try to find SDL2_image
# Once done this will define
#  SDL2_TTF_FOUND - System has SDL2_ttf
#  SDL2_TTF_INCLUDE_DIRS - The SDL2_ttf include directory
#  SDL2_TTF_LIBRARIES - The libraries needed to use SDL2_ttf
#  SDL2_TTF_DEFINITIONS - Compiler switches required for using SDL2_ttf

SET(SDL2_SEARCH_PATHS 
	/usr/local
	/usr
	/opt
)

FIND_PATH(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES include/SDL2 include
    PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_TTF_LIBRARY
    NAMES SDL2_ttf
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib lib/x86
    PATHS ${SDL2_SEARCH_PATHS}
)

SET(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
SET(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set SDL2_TTF_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SDL2_ttf DEFAULT_MSG SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)

mark_as_advanced(SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)
