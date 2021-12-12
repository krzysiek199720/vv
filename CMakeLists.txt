# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.19)
project(vv)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(includes)

add_executable(vv main.cpp win32.cpp archdef.h libraries/stb_libraries_init.cpp palette/palette.cpp palette/image.cpp SaveManager.cpp)
# static link standard libs
target_link_libraries(vv Msimg32.lib -static-libgcc -static-libstdc++ -mwindows)

# for debug functions -- like debug print to standard output
add_compile_definitions(DEBUG=0)