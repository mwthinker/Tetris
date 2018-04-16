# - Try to find Catch2.
# Once done this will define:
#  CATCH2_FOUND - The system found Catch2.
#  CATCH_INCLUDE_DIRS - The Catch2 include directory

set(CATCH2_SEARCH_PATHS
	/usr/local
	/usr
	/opt
)

find_path(CATCH2_INCLUDE_DIR catch.hpp
	HINTS $ENV{CATCH2}
	PATH_SUFFIXES include/catch Catch2/include/catch
    PATHS ${CATCH2_SEARCH_PATHS}
)

set(CATCH2_INCLUDE_DIRS ${CATCH2_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(CATCH2 DEFAULT_MSG CATCH2_INCLUDE_DIR)

mark_as_advanced(CATCH2_INCLUDE_DIR)
