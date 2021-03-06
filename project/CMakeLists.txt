# Set the minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.6)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# Needed for not globally unique names in executables
CMAKE_POLICY(SET CMP0002 OLD)

# Needed for strings in defines
CMAKE_POLICY(SET CMP0005 OLD)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

PROJECT(medusa)

# Debug build
SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS("-DDEBUG")

# Release build
#SET(CMAKE_BUILD_TYPE Release)
#ADD_DEFINITIONS("-DNDEBUG")

SET(CMAKE_SOURCE_DIR ./../../library/)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

SET(LIBRARY_INCLUDE "${CMAKE_SOURCE_DIR}/include/")
SET(LIBRARY_SRC "${CMAKE_SOURCE_DIR}/src/")

# Add definitions, compiler switches, etc.
ADD_DEFINITIONS("-Wall -W -Wextra -Werror -Wformat -Wformat-y2k -Winit-self -Wunknown-pragmas -Wundef -Wwrite-strings -Wno-unused-parameter -Wpointer-arith -Wno-switch -Woverloaded-virtual -Wmissing-include-dirs -Wuninitialized -Wc++0x-compat -std=c++0x -ffor-scope -ffast-math -O2")
IF(WIN32)
ADD_DEFINITIONS("-DUNICODE -D_UNICODE")
ENDIF()
ADD_DEFINITIONS("-D__GTK__")

ADD_DEFINITIONS("-DSPITFIRE_APPLICATION_NAME=\"\\\"Medusa\\\"\"")
ADD_DEFINITIONS("-DSPITFIRE_APPLICATION_NAME_LWR=\"\\\"medusa\\\"\"")

ADD_DEFINITIONS("-DBUILD_APPLICATION_NAME=\"\\\"Medusa\\\"\"")
ADD_DEFINITIONS("-DBUILD_APPLICATION_NAME_LWR=\"\\\"medusa\\\"\"")
ADD_DEFINITIONS("-DBUILD_APPLICATION_VERSION_STRING=\"\\\"0.9\\\"\"")
ADD_DEFINITIONS("-DBUILD_APPLICATION_COPYRIGHT_OWNER=\"\\\"Chris Pilkington\\\"\"")
ADD_DEFINITIONS("-DBUILD_APPLICATION_WEBSITE=\"\\\"http://chris.iluo.net/projects/medusa/\\\"\"")

SET(LIBRARY_SRC "${CMAKE_SOURCE_DIR}/src/")


#IF(APPLE)
IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  ADD_DEFINITIONS("-D__LINUX__")
ENDIF()
IF(WIN32)
  ADD_DEFINITIONS("-DNOMINMAX")
ENDIF()

INCLUDE_DIRECTORIES(${LIBRARY_INCLUDE})


#Files from library directory

MACRO(PREFIX_PATHS SOURCE_DIRECTORY SOURCE_FILES)
MESSAGE(STATUS "PREFIX_PATHS dir=${SOURCE_DIRECTORY}, files=${SOURCE_FILES}, ${ARGN}")
MESSAGE(a=${SOURCE_FILES})
  SET(OUTPUT_FILES "")
  SET(a ${OUTPUT_FILES})
  SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILES}")
  FOREACH(SOURCE_FILE ${ARGN})
    MESSAGE(b=${SOURCE_FILE})
    SET(a ${OUTPUT_FILES})
    SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILE}")
  ENDFOREACH()
MESSAGE(d=${OUTPUT_FILES})
ENDMACRO(PREFIX_PATHS)

#Add all source files from our project src directory
AUX_SOURCE_DIRECTORY(./../src/ PROJECT_SOURCE_FILES)



SET(LIBRARY_LIBGTKMM_SOURCE_DIRECTORY libgtkmm/)
SET(LIBRARY_LIBGTKMM_SOURCE_FILES
alertdialog.cpp dispatcher.cpp filebrowse.cpp icontheme.cpp slider.cpp)

PREFIX_PATHS(${LIBRARY_LIBGTKMM_SOURCE_DIRECTORY} ${LIBRARY_LIBGTKMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBGTKMM_SOURCE_FILES ${OUTPUT_FILES})


SET(LIBRARY_SPITFIRE_SOURCE_DIRECTORY spitfire/)
SET(LIBRARY_SPITFIRE_SOURCE_FILES
spitfire.cpp
audio/lastfm.cpp audio/metadata.cpp audio/playlist.cpp
communication/http.cpp communication/network.cpp
platform/notifications.cpp platform/operatingsystem.cpp platform/pipe.cpp
storage/csv.cpp storage/document.cpp storage/file.cpp storage/filesystem.cpp storage/settings.cpp storage/xml.cpp
util/cConsoleApplication.cpp util/datetime.cpp util/lang.cpp util/string.cpp util/thread.cpp util/updatechecker.cpp)

PREFIX_PATHS(${LIBRARY_SPITFIRE_SOURCE_DIRECTORY} ${LIBRARY_SPITFIRE_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES ${OUTPUT_FILES})


SET(LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY libtrashmm/)
SET(LIBRARY_LIBTRASHMM_SOURCE_FILES
libtrashmm.cpp)

PREFIX_PATHS(${LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY} ${LIBRARY_LIBTRASHMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES ${OUTPUT_FILES})


SET(LIBRARY_LIBXDGMM_SOURCE_DIRECTORY libxdgmm/)
SET(LIBRARY_LIBXDGMM_SOURCE_FILES
libxdgmm.cpp)

PREFIX_PATHS(${LIBRARY_LIBXDGMM_SOURCE_DIRECTORY} ${LIBRARY_LIBXDGMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES ${OUTPUT_FILES})


SET(LIBRARY_SOURCE_FILES ${OUTPUT_LIBRARY_LIBGTKMM_SOURCE_FILES} ${OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES})
PREFIX_PATHS(${LIBRARY_SRC} ${LIBRARY_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SOURCE_FILES ${OUTPUT_FILES})
#MESSAGE(final=${OUTPUT_LIBRARY_SOURCE_FILES})

#list all source files here
ADD_EXECUTABLE(${PROJECT_NAME} ${OUTPUT_LIBRARY_SOURCE_FILES} ${PROJECT_SOURCE_FILES})

#SET(RUNTIME_OUTPUT_DIRECTORY bin)
SET(PROJECT_BINARY_DIR .)
SET(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/..)


# libid3tag and libmad
INCLUDE_DIRECTORIES("/usr/include")


FIND_PACKAGE(PkgConfig)

# This is ugly and fragile but PKG_CHECK_MODULES doesn't seem to work for Gstreamermm
#
# From the output of:
# pkg-config gstreamermm-0.10 glibmm-2.4 --libs --cflags
PKG_CHECK_MODULES(GSTREAMERMM gstreamer-0.10)
INCLUDE_DIRECTORIES("/usr/include/libxml++-2.6")
INCLUDE_DIRECTORIES("/usr/lib/libxml++-2.6/include/")
INCLUDE_DIRECTORIES("/usr/include/gstreamer-0.10")
INCLUDE_DIRECTORIES("/usr/include/gstreamermm-0.10")
INCLUDE_DIRECTORIES("/usr/lib/gstreamermm-0.10/include")
INCLUDE_DIRECTORIES("/usr/include/sigc++-2.0")
INCLUDE_DIRECTORIES("/usr/include/libxml2")
SET(CMAKE_CXX__FLAGS ${CMAKE_CXX__FLAGS} ${GSTREAMERMM_CFLAGS})

PKG_CHECK_MODULES(GTKMM gtkmm-3.0)
IF(GTKMM_FOUND)
    LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIRS})
ENDIF(GTKMM_FOUND)


PKG_CHECK_MODULES(GNOME_KEY_RING gnome-keyring-1)
IF(GNOME_KEY_RING_FOUND)
    LINK_DIRECTORIES(${GNOME_KEY_RING_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${GNOME_KEY_RING_INCLUDE_DIRS})
ENDIF(GNOME_KEY_RING_FOUND)


#FIND_PACKAGE(Libnotifymm REQUIRED)
#INCLUDE_DIRECTORIES(${LIBNOTIFYMM_INCLUDE_DIR})


FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})


SET(LIBRARIES
  XDG-basedir
)
# Some of the libraries have different names than their Find*.cmake name
SET(LIBRARIES_LINKED
  boost_iostreams
  boost_filesystem
  boost_locale
  boost_system
  glibmm-2.4
  giomm-2.4
  gstreamermm-0.10
  sigc-2.0
  xdg-basedir
  xml++-2.6
  gstcontroller-0.10
  gstdataprotocol-0.10
  gstnet-0.10
  gstaudio-0.10
  gstcdda-0.10
  gstnetbuffer-0.10
  gstrtp-0.10
  gstvideo-0.10
  gio-2.0
  gstinterfaces-0.10
  gsttag-0.10
  gstbase-0.10
  gstreamer-0.10
  gmodule-2.0
  gobject-2.0
  gthread-2.0
  rt
  glib-2.0
  gnome-keyring
  xml2
  id3tag
  mad
  clastfm
  notify
  pthread
)

FOREACH(LIBRARY_FILE ${LIBRARIES})
  Find_Package(${LIBRARY_FILE} REQUIRED)
ENDFOREACH(LIBRARY_FILE)

# Need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_LINKED} ${Boost_LIBRARIES} ${GSTREAMERMM_LIBS} ${GTKMM_LIBRARIES})
