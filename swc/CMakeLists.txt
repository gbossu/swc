# specify source files
set(SOURCES
    main.cpp
    AnimatedContainer.cpp
    AnimatedWindowContainer.cpp
    CommandLineParser.cpp
    Settings.cpp)

# create the executable
add_executable(swc ${SOURCES})
target_link_libraries(swc Qt5::Widgets Qt5::DBus xdo)

# grenarate the manpage
add_custom_command(TARGET swc POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../man/genman.sh ${CMAKE_CURRENT_SOURCE_DIR}/../man/*.h2m
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating swc's manpage")

# add an install rule for both the executable and the manpage
INSTALL(TARGETS swc
    RUNTIME DESTINATION bin)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/swc.1.gz
    DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)