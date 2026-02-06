cmake_minimum_required(VERSION 3.16)
project(game_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

add_executable(game_server ${SRC_FILES})