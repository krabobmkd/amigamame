# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CXX_STANDARD 11)
set(C_STANDARD 11)
project(AmigaMame LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(MAME_USE_HARDFLOAT "compile using direct 68882 FPU, will not work on economic 060" ON)
# options can only be bool:
option(AMIGA_USE_68030 " use 68030" OFF)
option(AMIGA_USE_68040 " use 68040" OFF)
option(AMIGA_USE_68060 " use 68060" ON)

option(COMPILE_TESTS " compile extra dev tools/ test units" ON)

if(AMIGA)

#  -fno-rtti -fno-exceptions -fno-strict-aliasing
	# add your generic options here:
	#add_compile_options( -save-temps )
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
		list(APPEND AMIGA_COMPILER_OPTIONS -mhard-float -ffast-math)
	else()
		list(APPEND AMIGA_COMPILER_OPTIONS -msoft-float)
	endif()

endif()
#add_compile_options( -mhard-float -ffast-math )
#add_link_options( -mhard-float -ffast-math)


add_subdirectory(../mame106 MameLib)

set(AMIGA_SRC
	main.cpp

	gui_mui.cpp
	gui_gadtools.cpp

	amiga106.cpp


	amiga106_audiostream.cpp
	amiga106_filesystem.cpp
	amiga106_timer.cpp
	amiga106_config.cpp
	amiga106_config.h
	amiga106_video.h
	amiga106_video.cpp
#	amiga106_video_p96.cpp

#	amiga106_video_aga.h
#	amiga106_video_aga.cpp
	amiga_locale.cpp


	audio.h

	amiga_locale.h
	gui_mui.h
	gui_gadtools.h
	mame_ver.h
	mame_msg.h

	directdraw.s
#	parallelporthandler.s
	devnotes.txt
#	c2p.s
#	gfx.s
#	../../lib/c2p.s.o ../../lib/gfx.s.o
#	cgxhooks.c
#	cgxhooks.h
#	cgxhooks_re.s
	)
# - - -have some Amiga code using O2
	add_library(Mame106AmigaO2 STATIC
		amiga106_video_cgx.h
		amiga106_video_cgx.cpp

		amiga106_inputs.h
		amiga106_inputs.cpp

		amiga_parallelpads.h
		amiga_parallelpads.c
	)
	set_property(TARGET Mame106AmigaO2 PROPERTY C_STANDARD 11)
	set_property(TARGET Mame106AmigaO2 PROPERTY CXX_STANDARD 11)
	if(MAME_USE_HARDFLOAT)
		target_compile_definitions(Mame106AmigaO2 PRIVATE MAME_USE_HARD_FLOAT=1 )
	endif()
	target_link_directories(Mame106AmigaO2 PUBLIC "../../lib")
	target_include_directories(Mame106AmigaO2 PUBLIC "../../include" )
	target_include_directories(Mame106AmigaO2 PUBLIC "../mame106")
	target_compile_options(Mame106AmigaO2 PRIVATE -O2 ${AMIGA_COMPILER_OPTIONS} )
# - - - -

set(PROGNAME Mame106)
add_executable(${PROGNAME} ${AMIGA_SRC} )
set_property(TARGET ${PROGNAME} PROPERTY C_STANDARD 11)
set_property(TARGET ${PROGNAME} PROPERTY CXX_STANDARD 11)
target_compile_definitions(${PROGNAME} PUBLIC MAMEVERSION=106 ZIP_CACHE=1 )
if(MAME_USE_HARDFLOAT)
	target_compile_definitions(${PROGNAME} PRIVATE MAME_USE_HARD_FLOAT=1 )
endif()
# the static libs that are part eof this repo:
target_link_libraries(${PROGNAME} PUBLIC Mame106AmigaO2 mamecoreO2 z expat)

target_link_directories(${PROGNAME} PUBLIC "../../lib")
target_include_directories(${PROGNAME} PUBLIC "../../include" )

#add_library(${PROGNAME} PUBLIC OBJECT IMPORTED  ../../c2p.s.o ../../gfx.s.o)

# target_include_directories(${PROGNAME} PUBLIC "../mame037b/src")
target_include_directories(${PROGNAME} PUBLIC "../mame106")

# MUI is not provided by gccbebbo6.5 ndk by default, we have it there:
target_include_directories(${PROGNAME} PUBLIC "../../../extrasdk/MUI/include")
target_include_directories(${PROGNAME} PUBLIC "../../../extrasdk/P96/include")

target_compile_options(${PROGNAME} PRIVATE -O0 -Wno-narrowing ${AMIGA_COMPILER_OPTIONS} )

target_link_options(${PROGNAME} PRIVATE -O0 -s ${AMIGA_COMPILER_OPTIONS})

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

