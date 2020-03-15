set(SOURCES
    CPUStatsReader.cpp)

add_library(utils ${SOURCES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})