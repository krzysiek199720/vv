# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.19)
project(vv)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# O3 seems to break somethings on save
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(vv archdef.h win32.cpp saveManager.cpp libraries/stb_libraries_init.cpp palette/palette.cpp palette/image.cpp main.cpp)
# static link standard libs
target_link_libraries(vv Msimg32.lib -static-libgcc -static-libstdc++ -mwindows)

# for debug functions -- like debug print to standard output
add_compile_definitions(DEBUG=0)