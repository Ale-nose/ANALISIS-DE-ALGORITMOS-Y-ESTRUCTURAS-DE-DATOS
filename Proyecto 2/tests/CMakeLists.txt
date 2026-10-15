# Test executable: links maze_core only, never Qt.
file(GLOB MAZE_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(MazeCrazeTests ${MAZE_TEST_SOURCES})
target_link_libraries(MazeCrazeTests PRIVATE maze_core)

add_test(NAME MazeCrazeTests COMMAND MazeCrazeTests)