# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)


set(CXX_STANDARD 11)
set(C_STANDARD 11)
project(LinuxMame LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

		list(APPEND AMIGA_COMPILER_OPTIONS -DLSB_FIRST=1 -D__LP64__=1)

set(COMMONLIBS "../../amigacommonlibs")

set(MAME_INLINE "static inline")

# get current git branch id
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)


add_subdirectory(../mame106 MameLib)

set(LINUX_SRC
	main.cpp
	filesystem.cpp
	lconfig.cpp
	lconfig.h
	serializer_mamexml.cpp
	serializer_mamexml.h
	)

set(PROGNAME lmame)
add_executable(${PROGNAME} ${LINUX_SRC} )
set_property(TARGET ${PROGNAME} PROPERTY C_STANDARD 11)
set_property(TARGET ${PROGNAME} PROPERTY CXX_STANDARD 11)
target_compile_definitions(${PROGNAME} PUBLIC MAMEVERSION=106 ZIP_CACHE=1 MUISERIALIZER_USES_FLOAT=1 GIT_BRANCH="${GIT_BRANCH}" LSB_FIRST=1 __LP64__=1 )
if(MAME_USE_HARDFLOAT)
	target_compile_definitions(${PROGNAME} PRIVATE MAME_USE_HARD_FLOAT=1 )
endif()
# the static libs that are part eof this repo:
target_link_libraries(${PROGNAME} PUBLIC mamecore z expat)

# target_include_directories(${PROGNAME} PUBLIC "../mame037b/src")
target_include_directories(${PROGNAME} PUBLIC "../linux" "${COMMONLIBS}/libserializer")

# MUI is not provided by gccbebbo6.5 ndk by default, we have it there:
#target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/MUI/include")
# target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/P96/include")

# sources that doesn't support -O2 are moved to MameXAmigaO0 lib
if(AMIGA)
target_compile_options(${PROGNAME} PRIVATE
		$<$<CONFIG:RELEASE>:-O2 -Wno-narrowing -DNDEBUG>
		 ${AMIGA_COMPILER_OPTIONS} )
endif()
