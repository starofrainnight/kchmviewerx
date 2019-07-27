# Project name
project( kchmviewer )

cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Not work as expected, failed to compile libebook
# set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Init cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Necessary packages
find_package(Qt5 COMPONENTS	Widgets WebEngine WebEngineWidgets WebView PrintSupport Xml DBus Network REQUIRED)
find_package(Libchm REQUIRED)

include_directories(
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5WebEngine_INCLUDE_DIRS}
	${Qt5WebView_INCLUDE_DIRS}
	${Qt5WebEngineWidgets_INCLUDE_DIRS}
	${Qt5PrintSupport_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
	${Qt5DBus_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
)

add_definitions(
	${Qt5Widgets_DEFINITIONS}
	${Qt5WebEngine_DEFINITIONS}
	${Qt5WebView_DEFINITIONS}
	${Qt5WebEngineWidgets_DEFINITIONS}
	${Qt5PrintSupport_DEFINITIONS}
	${Qt5Xml_DEFINITIONS}
	${Qt5DBus_DEFINITIONS}
	${Qt5Network_DEFINITIONS}
	-DUSE_WEBENGINE=1
)

include("${CMAKE_SOURCE_DIR}/cmake/QtHelper.cmake")

# Qt5 required
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC")

# libzip
find_path(LIBZIP_INCLUDE_DIR zip.h)
find_library(LIBZIP_LIBRARY NAMES zip)

if (LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARY)
	message(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
else ()
	message(FATAL_ERROR "Could not find libzip. Please install libzip and libzip-devel packages")
endif ()

# Set compilation params
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/libebook)

# Run in those subdirectories
add_subdirectory( lib/libebook )
add_subdirectory( src )
add_subdirectory( po )
# TODO: What about this directory ?
# add_subdirectory( packages )
