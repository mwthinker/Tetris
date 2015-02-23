# - Try to find SDL2_net
# Once done this will define
#  SDL2_NET_FOUND - System has SDL2
#  SDL2_NET_INCLUDE_DIRS - The SDL2 include directory
#  SDL2_NET_LIBRARIES - The libraries needed to use SDL2_net
#  SDL2_NET_DEFINITIONS - Compiler switches required for using SDL2_net

SET(SDL2_SEARCH_PATHS
	/usr/local
	/usr
	/opt
)

FIND_PATH(SDL2_NET_INCLUDE_DIR SDL_net.h
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES include/SDL2 include
    PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_NET_LIBRARY
    NAMES SDL2_net
	HINTS $ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib lib/x86
    PATHS ${SDL2_SEARCH_PATHS}
)

SET(SDL2_NET_LIBRARIES ${SDL2_NET_LIBRARY})
SET(SDL2_NET_INCLUDE_DIRS ${SDL2_NET_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set SDL2_NET_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SDL2_net DEFAULT_MSG SDL2_NET_LIBRARY SDL2_NET_INCLUDE_DIR)

mark_as_advanced(SDL2_NET_INCLUDE_DIR SDL2_NET_LIBRARY)
