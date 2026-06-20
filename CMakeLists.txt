cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

project(OL2SharedInventoryMod VERSION 0.1.0 LANGUAGES C CXX)

# requires the use of at least C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# tells CMake what build type we should use.
set(CMAKE_BUILD_TYPE Debug)

# sets CMake compiler flags.
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# finds all .h and .cpp files. also compiles "glad.c".
file(GLOB_RECURSE SOURCES
    "src/*.hpp"
    "src/*.cpp"
)

# tells CMake to create a .exe file using the specified source files.
add_executable(OL2SharedInventoryMod main.cpp ${SOURCES})

# handles include directories and libraries.
target_include_directories(OL2SharedInventoryMod PRIVATE "src/")
