cmake_minimum_required(VERSION 3.16)
project(MazeCraze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt moc/uic/rcc are only needed by the GUI target.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# maze_core: game logic, no Qt dependency (core, game, logging).
file(GLOB_RECURSE MAZE_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/core/*.cpp
    ${CMAKE_SOURCE_DIR}/src/game/*.cpp
    ${CMAKE_SOURCE_DIR}/src/logging/*.cpp
)

# Until the first .cpp files exist, a static library cannot be empty.
if(NOT MAZE_CORE_SOURCES)
    set(MAZE_CORE_PLACEHOLDER ${CMAKE_BINARY_DIR}/maze_core_placeholder.cpp)
    file(WRITE ${MAZE_CORE_PLACEHOLDER} "// Placeholder until maze_core has real sources.\n")
    set(MAZE_CORE_SOURCES ${MAZE_CORE_PLACEHOLDER})
endif()

add_library(maze_core STATIC ${MAZE_CORE_SOURCES})
target_include_directories(maze_core PUBLIC ${CMAKE_SOURCE_DIR}/include)

# MazeCraze: Qt Widgets application (the only target that links Qt).
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE MAZE_UI_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/ui/*.cpp
)

add_executable(MazeCraze src/main.cpp ${MAZE_UI_SOURCES})
target_link_libraries(MazeCraze PRIVATE maze_core Qt${QT_VERSION_MAJOR}::Widgets)

# MazeCrazeTests: independent test executable (no Qt, no window).
enable_testing()
add_subdirectory(tests)