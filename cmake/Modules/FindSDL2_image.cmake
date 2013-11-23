# - Try to find SDL2_image
# Once done this will define
#  SDL2_IMAGE_FOUND - System has SDL2_image
#  SDL2_IMAGE_INCLUDE_DIRS - The SDL2_image include directory
#  SDL2_IMAGE_LIBRARIES - The libraries needed to use SDL2_image
#  SDL2_IMAGE_DEFINITIONS - Compiler switches required for using SDL2_image2'

SET(SDL2_SEARCH_PATHS 
	/usr/local
	/usr
	/opt
) 

FIND_PATH(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES include/SDL2 include
    PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib lib/x86
    PATHS ${SDL2_SEARCH_PATHS}
)

SET(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
SET(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set SDL2_IMAGE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SDL2_image DEFAULT_MSG SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
