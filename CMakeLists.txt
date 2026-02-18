cmake_minimum_required(VERSION 3.13)
project(Soccer_AI VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

find_package(SDL2 REQUIRED)

# .cc files here.
set (
    COMMON_SOURCES

)


add_executable(soccer_ai
    src/main.cc
)

target_include_directories(soccer_ai PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(soccer_ai PRIVATE SDL2::SDL2)
