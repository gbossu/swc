cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(swc)

set(CMAKE_AUTOMOC ON)

# find Qt5 packages
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Charts REQUIRED)

# find libxdo (from xdotool)
find_library(xdo_LIBRARY NAMES xdo libxdo PATHS /usr/lib /usr/local/lib)
if (xdo_LIBRARY)
  message(STATUS "found xdo " ${xdo_LIBRARY})
else (xdo_LIBRARY)
  message(FATAL_ERROR "can't find xdotool/libxdo")
endif (xdo_LIBRARY)

# check if help2man is installed
find_program(h2m_PROGRAM help2man)
if (h2m_PROGRAM)
  message(STATUS "found help2man")
else (h2m_PROGRAM)
  message(FATAL_ERROR "help2man is not installed")
endif (h2m_PROGRAM)

# build libraries
add_subdirectory(utils)
add_subdirectory(modules)

# build swc
add_subdirectory(swc)