cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(swc)

set(CMAKE_AUTOMOC ON)

# specify source files
set(SOURCES
    swc/main.cpp
    swc/AnimatedContainer.cpp
    swc/CommandLineParser.cpp
    swc/Settings.cpp)
include_directories(swc)

# find Qt5 packages
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)

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

# create the executable
add_executable(swc ${SOURCES})
target_link_libraries(swc Qt5::Widgets Qt5::DBus xdo)

# grenarate the manpage
add_custom_command(TARGET swc POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/man/genman.sh ${CMAKE_CURRENT_SOURCE_DIR}/man/*.h2m
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating swc's manpage")

# add an install rule for both the executable and the manpage
INSTALL(TARGETS swc
    RUNTIME DESTINATION bin)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/swc.1.gz
    DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)