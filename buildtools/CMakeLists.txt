# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)

set(CXX_STANDARD 11)
set(C_STANDARD 11)
project(MamePortTools LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable( mamemakToCmake mameMakToCmake.cpp )
#add_executable( m68kmake ../src/mame106/cpu/m68000/m68kmake.c )
