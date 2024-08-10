# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CXX_STANDARD 11)
set(C_STANDARD 11)
project(AmigaMame LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# this option system makes sure each cpu options are the same for each libs.
# options can only be bool:
option(MAME_USE_HARDFLOAT "compile using direct 68882 FPU, will not work on economic 060" ON)
option(AMIGA_USE_68030 " use 68030" OFF)
option(AMIGA_USE_68040 " use 68040" OFF)
option(AMIGA_USE_68060 " use 68060" ON)

option(COMPILE_TESTS " compile extra dev tools/ test units" OFF)

set(MAME_INLINE "static inline")

# get current git branch id
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
#string(REPLACE "/" "." GIT_BRANCH ${GIT_BRANCH} )
# message(WARNING "git branch detected:${GIT_BRANCH}")

if(AMIGA)

#  -fno-rtti -fno-exceptions -fno-strict-aliasing
	# add your generic options here:
	#add_compile_options( -save-temps )
	# this is to have assember .s files in the build.
	list(APPEND AMIGA_COMPILER_OPTIONS -save-temps)

	if(AMIGA_USE_68030)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68030)
	elseif(AMIGA_USE_68040)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68040)
	elseif(AMIGA_USE_68060)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68060)
	else()
		list(APPEND AMIGA_COMPILER_OPTIONS -m68020)
	endif()

	if(MAME_USE_HARDFLOAT)
		list(APPEND AMIGA_COMPILER_OPTIONS -mhard-float -ffast-math -fsingle-precision-constant)
	else()
		list(APPEND AMIGA_COMPILER_OPTIONS -msoft-float)
	endif()

endif()

# test -fsingle-precision-constant
# test -mpc32
#list(APPEND AMIGA_COMPILER_OPTIONS -fsingle-precision-constant )

set(COMMONLIBS "../../amigacommonlibs")

add_subdirectory(../mame106 MameLib)

set(AMIGA_SRC
	main.cpp
	amiga106.cpp
	amiga106_audiostream.h
	amiga106_audiostream.cpp
	amiga106_filesystem.cpp
	amiga106_timer.cpp
	amiga106_config.cpp
	amiga106_config.h
	amiga106_video.h
	amiga106_video.cpp
	amiga_video_remap.h
	amiga_video_remap.cpp
	amiga106_video_intuition.h
	amiga106_video_intuition.cpp
	amiga106_video_os3.h
	amiga106_video_os3.cpp
	amiga106_video_cgx.h
	amiga106_video_cgx.cpp
	amiga106_inputs.h
	amiga106_inputs.cpp
	amiga_locale.h
	amiga_locale.cpp
#	inptport_krb.c
	amiga_parallelpads.h
	amiga_parallelpads.c
		amiga_audiomixer.cpp

	serializer_mamexml.cpp
	serializer_mamexml.h
	audio.h
		${COMMONLIBS}/libserializer/serializer.h
		${COMMONLIBS}/libserializer/serializer_mui.h
		${COMMONLIBS}/libserializer/serializer_mui.cpp
		gui_gadtools.h
		gui_gadtools.cpp


	mame_ver.h
	mame_msg.h

#	directdraw.s
#	parallelporthandler.s
	devnotes.txt
#	c2p.s
#	gfx.s
#	../../lib/c2p.s.o ../../lib/gfx.s.o
#	cgxhooks.c
#	cgxhooks.h
#	cgxhooks_re.s
	)
# - - -have some Amiga code using O0
	add_library(Mame106AmigaO0 STATIC

		gui_mui.h
		gui_mui.cpp

	)
	set_property(TARGET Mame106AmigaO0 PROPERTY C_STANDARD 11)
	set_property(TARGET Mame106AmigaO0 PROPERTY CXX_STANDARD 11)
	if(MAME_USE_HARDFLOAT)
		target_compile_definitions(Mame106AmigaO0 PRIVATE MAME_USE_HARD_FLOAT=1 )
	endif()
	target_include_directories(Mame106AmigaO0 PUBLIC "../mame106" "${COMMONLIBS}/libserializer")
	target_compile_options(Mame106AmigaO0 PRIVATE -O0 ${AMIGA_COMPILER_OPTIONS} )
	target_include_directories(Mame106AmigaO0 PUBLIC "${COMMONLIBS}/extrasdk/MUI/include")
	# those for mame includes
	target_compile_definitions(Mame106AmigaO0 PUBLIC INLINE=${MAME_INLINE} ZIP_CACHE=1 CRLF=2 MUISERIALIZER_USES_FLOAT=1  ${CPU_DEFS} )
# - - -have some Amiga code using O3
	add_library(Mame106AmigaO3 STATIC
		amiga_video_tracers.h
		amiga_video_tracers_clut16.h
		amiga_video_tracers_clut16.cpp
		amiga_video_tracers_argb32.h
		amiga_video_tracers_argb32.cpp
		amiga_audiomixer.cpp
	)
	set_property(TARGET Mame106AmigaO3 PROPERTY C_STANDARD 11)
	set_property(TARGET Mame106AmigaO3 PROPERTY CXX_STANDARD 11)
	if(MAME_USE_HARDFLOAT)
		target_compile_definitions(Mame106AmigaO3 PRIVATE MAME_USE_HARD_FLOAT=1 )
	endif()
	target_include_directories(Mame106AmigaO3 PUBLIC "../mame106" "${COMMONLIBS}/libserializer")
	# -funroll-loops -ftree-loop-ivcanon -ftree-loop-im
	target_compile_options(Mame106AmigaO3 PRIVATE -O3 -funroll-loops -ftree-loop-ivcanon -DNDEBUG  ${AMIGA_COMPILER_OPTIONS} )
#	 -funroll-loops -ftree-loop-ivcanon -ftree-loop-im
#			 $<$<CONFIG:RELEASE>:-O3 -funroll-loops -ftree-loop-ivcanon -ftree-loop-im -DNDEBUG>
#	  ${AMIGA_COMPILER_OPTIONS} )
	target_include_directories(Mame106AmigaO3 PUBLIC "${COMMONLIBS}/extrasdk/MUI/include")
	# those for mame includes
	target_compile_definitions(Mame106AmigaO3 PUBLIC INLINE=${MAME_INLINE} ZIP_CACHE=1 CRLF=2 MUISERIALIZER_USES_FLOAT=1 ${CPU_DEFS} )

# - - - -

set(PROGNAME Mame106)
add_executable(${PROGNAME} ${AMIGA_SRC} )
set_property(TARGET ${PROGNAME} PROPERTY C_STANDARD 11)
set_property(TARGET ${PROGNAME} PROPERTY CXX_STANDARD 11)
target_compile_definitions(${PROGNAME} PUBLIC MAMEVERSION=106 ZIP_CACHE=1 MUISERIALIZER_USES_FLOAT=1 GIT_BRANCH="${GIT_BRANCH}" )
if(MAME_USE_HARDFLOAT)
	target_compile_definitions(${PROGNAME} PRIVATE MAME_USE_HARD_FLOAT=1 )
endif()
# the static libs that are part eof this repo:
target_link_libraries(${PROGNAME} PUBLIC Mame106AmigaO0 Mame106AmigaO3 mamecore z expat)

# target_include_directories(${PROGNAME} PUBLIC "../mame037b/src")
target_include_directories(${PROGNAME} PUBLIC "../mame106" "${COMMONLIBS}/libserializer")

# MUI is not provided by gccbebbo6.5 ndk by default, we have it there:
target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/MUI/include")
# target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/P96/include")

# sources that doesn't support -O2 are moved to MameXAmigaO0 lib
target_compile_options(${PROGNAME} PRIVATE
		$<$<CONFIG:RELEASE>:-O2 -Wno-narrowing -DNDEBUG>
		 ${AMIGA_COMPILER_OPTIONS} )

target_link_options(${PROGNAME} PRIVATE -s ${AMIGA_COMPILER_OPTIONS})

# add_executable(cgxhooktest cgxhookstest.c cgxhooks.c cgxhooks.h )
# set_property(TARGET cgxhooktest PROPERTY C_STANDARD 11)

if(COMPILE_TESTS)
	add_executable(testparpads
		testparpads.cpp
		amiga_parallelpads.h
		amiga_parallelpads.c
#		parallelporthandler.s
		)
	set_property(TARGET testparpads PROPERTY C_STANDARD 11)
	set_property(TARGET testparpads PROPERTY CXX_STANDARD 11)

endif()

