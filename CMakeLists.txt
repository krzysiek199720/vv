# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.19)
project(vv)

set(CMAKE_CXX_STANDARD 14)

add_executable(vv win32.cpp archdef.h libraries/stb_libraries_init.cpp palette/palette.cpp palette/image.cpp)
target_link_libraries(vv Msimg32.lib)
# for debug functions -- like debug print to standard output
add_compile_definitions(DEBUG=1)