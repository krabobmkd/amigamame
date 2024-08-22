# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

project(MamePortTools LANGUAGES C CXX )
set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable( mamemakToCmake mameMakToCmake.cpp )

add_executable( apichanger apichanger.cpp )
#set_property(TARGET apichanger PROPERTY CXX_STANDARD 14)
#add_executable( m68kmake ../src/mame106/cpu/m68000/m68kmake.c )
