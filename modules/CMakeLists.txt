set(SOURCES
    CPUGraph.cpp)

add_library(modules ${SOURCES})
target_link_libraries(modules utils Qt5::Charts)
target_include_directories(modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})