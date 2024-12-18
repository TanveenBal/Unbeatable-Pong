cmake_minimum_required(VERSION 3.28)
project(Unbeatable_Pong)

set(CMAKE_CXX_STANDARD 17)
set(RAYLIB_PATH "C:/raylib/raylib/src")

# Include directory for raylib headers
include_directories(${RAYLIB_PATH})

# Library directory - assuming the library file is here
link_directories(${RAYLIB_PATH})

add_executable(Unbeatable_Pong Pong.cpp Pong.h)

# Link the raylib library and additional system libraries needed by raylib
target_link_libraries(Unbeatable_Pong raylib winmm)
