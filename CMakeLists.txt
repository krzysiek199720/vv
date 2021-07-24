# cmake_minimum_required(VERSION <specify CMake version here>)
project(vv)

set(CMAKE_CXX_STANDARD 14)

add_executable(vv main.cpp archdef.h libraries/stb_libraries_init.cpp palette/palette.cpp palette/image.cpp)

# for debug functions -- like debug print to standard output
add_compile_definitions(DEBUG=1)