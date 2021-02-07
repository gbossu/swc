set(SOURCES
    CPUStatsReader.cpp
    DataUsage.cpp
    DiskStatsReader.cpp
    MemStatsReader.cpp
    NetworkStatsReader.cpp)

add_library(utils ${SOURCES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})