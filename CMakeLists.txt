cmake_minimum_required(VERSION 3.13)
project(Soccer_AI VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SDL2 REQUIRED)

# .cc files here.
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# .cc files here.
set (
    COMMON_SOURCES
    src/game/steering_behavs.cc
    src/game/soccer_pitch.cc
    src/game/entities/soccer_ball.cc
    src/game/soccer_team.cc
    src/game/entities/player.cc
    src/game/entities/field_player.cc
    src/game/entities/goal_keeper.cc
)

# Add tests
file(GLOB TEST_SOURCES "tests/*.cc")
list(FILTER TEST_SOURCES EXCLUDE REGEX "tests/main.cc") # Exclude the old manual main

add_executable(soccer_ai
    src/main.cc
    ${COMMON_SOURCES}
)

add_executable(run_tests ${TEST_SOURCES} ${COMMON_SOURCES})

target_include_directories(soccer_ai PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(run_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(soccer_ai PRIVATE SDL2::SDL2)
target_link_libraries(run_tests PRIVATE
    SDL2::SDL2
    GTest::gtest_main
)
