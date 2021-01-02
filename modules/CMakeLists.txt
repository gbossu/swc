set(SOURCES
    Graph.cpp
    ModuleGrid.cpp
    ModuleInfos.cpp
    ModuleInfosDeserialize.cpp
    ModuleSize.cpp
    ModuleUtils.cpp)

add_library(modules ${SOURCES})
target_link_libraries(modules utils Qt5::Core Qt5::Charts)
target_include_directories(modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})