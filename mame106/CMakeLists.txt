#  done in 2024 to adapt Mame4Allv0.37 to old triumph Amiga Mame.

cmake_minimum_required (VERSION 3.1)
if(AMIGA)
enable_language( VASM )
endif()

set(C_STANDARD 11)
set(CXX_STANDARD 11)
project(MameLib LANGUAGES C CXX VERSION 1.0)

macro(replace_list_item LIST OLD_VALUE NEW_VALUE)
    list(FIND ${LIST} ${OLD_VALUE} OLD_VALUE_INDEX)
    if(OLD_VALUE_INDEX GREATER_EQUAL 0)
        list(REMOVE_AT ${LIST} ${OLD_VALUE_INDEX})
        list(INSERT ${LIST} ${OLD_VALUE_INDEX} ${NEW_VALUE})
    endif()
endmacro()

macro(move_list_item LIST NLIST OLD_VALUE )
    list(FIND ${LIST} ${OLD_VALUE} OLD_VALUE_INDEX)
    if(OLD_VALUE_INDEX GREATER_EQUAL 0)
        list(REMOVE_AT ${LIST} ${OLD_VALUE_INDEX})
		 list(APPEND ${NLIST} ${OLD_VALUE} )
    endif()
endmacro()

include(gamedrivers.cmake)
set(COMMONLIBS "../../amigacommonlibs")

set(ZLIBPATH "${COMMONLIBS}/zlib")
set(EXPATPATH "${COMMONLIBS}/libexpat")
add_subdirectory(${ZLIBPATH} zlib)
add_subdirectory(${EXPATPATH} libexpat)

set(MAME_CORE_SRC
# chdman.c is an external tool !
# romcmp.c  is an external tool !
 mamelog.cpp
 mamelog.h
 bootlog.h
 bootlog.c
 drivertuning.cpp
 drivertuning.h
 fileio.cpp
 artwork.c
   cpuint.c    inptport.c  palette.c   streams_krb.c   video.c
	cpuintrf.c  input.c     png.c
	cdrom.c      jedparse.c  timer.c     xml2info.c
	chd.c      driver.c    jedutil.c   xmlfile.c
	chdcd.c    mame.c      romload.c   ui_text.c
	  harddisk.c  mamecore.c  sha1.c      unzip.c
	cheat.c    hash.c      mamedriv.c  sndintrf.c  usrintrf.c
	config.c   hiscore.c   md5.c            validity.c
	cpuexec.cpp  info.c      memory.c memoryn.cpp   state.c     version.c

	cpustats.cpp

 debugger.h   jedparse.h            png.h        ui_text.h
 audit.h

 # drawgfx.h drawgfxn.h drawgfx.c  drawgfxn.cpp tilemapn.cpp
# sound_krb.c

 mame.h                profiler.h   unzip.h
 cdrom.h      driver.h     mamecore.h            romload.h    usrintrf.h
 chd.h        fileio.h     md5.h                 sha1.h       video.h
 chdcd.h      harddisk.h   memconv.h             sndintrf.h   x86drc.h
 cheat.h      hash.h       memory.h              sound.h      xmlfile.h
 config.h     hiscore.h    osd_tool.h            state.h
 cpuexec.h    info.h       streams.h
 cpuint.h     inptport.h   osdepend.h            tilemap.h
 cpuintrf.h   input.h      palette.h             timer.h
	cpustats.h

  cpu/m68000/m68kkrbopt.h
 sound_krb.h

  drawCtrl.cpp
  drawCtrl.h
	# minimal common driver needs:
	vidhrdw/generic.c vidhrdw/vector.c vidhrdw/vector.cpp

	machine/generic.c
	machine/generic.h
	machine/eeprom.c
	machine/eeprom.h

	sound/flt_vol.c
	sound/flt_vol.h
	sound/flt_rc.c
	sound/flt_rc.h


	sndhrdw/generic.c
	sndhrdw/generic.h


	)

set(MAME_UNROLL_SRC  drawgfx.c  drawgfxn.cpp
					drawgfx.h  drawgfxn.h
				 tilemap.cpp
				 sound_krb.c
				 sound/wavwrite.c
				 sound/wavwrite.h
					)

if(OPT_TAITO)
	list(APPEND MAME_UNROLL_SRC vidhrdw/taitoicn.cpp  vidhrdw/taitoicn.h)
endif()
if(OPT_SEGA)
	list(APPEND MAME_UNROLL_SRC vidhrdw/segaic16n.cpp )
endif()
# move_list_item(MAME_DRIVERS_SRC MAME_UNROLL_SRC "vidhrdw/neogeo.c")
move_list_item(MAME_DRIVERS_SRC MAME_UNROLL_SRC "vidhrdw/segaic16.c")
# ${MAME_CORE_SRC} ${MAME_DRIVERS_SRC} ${MAME_SOUND_SRC} ${MAME_CPU_SRC}

if(AMIGA)
#todo     list(APPEND MAME_CORE_SRC memorynasm.s)
#todo
#	list(APPEND MAME_CORE_SRC vidhrdw/neogeodrawasm.s)
endif()

set(MAME_INCLUDE_DIRS "src" "../amigamame106" "." "./includes" ${EXPATPATH})

# force specific implementation of drivers like this:
replace_list_item(MAME_CPU_SRC "cpu/tms32010/tms32010.c" "cpu/tms32010/tms32010.cpp")

#replace_list_item(MAME_CORE_SRC "usrintrf.c" "usrintrf_krb.c")
#replace_list_item(MAME_CORE_SRC "palette.c" "palette_krb.c")
#replace_list_item(MAME_CORE_SRC "mame.c" "mame_krb.c")

add_library( mamecore STATIC  ${MAME_CORE_SRC} ${MAME_DRIVERS_SRC} ${MAME_CPU_SRC} )
set_property(TARGET mamecore PROPERTY C_STANDARD 11)
set_property(TARGET mamecore PROPERTY CXX_STANDARD 11)
target_include_directories(mamecore PUBLIC ${MAME_INCLUDE_DIRS} ${ZLIBPATH} )
set(MAME_INLINE "static inline")
target_compile_definitions(mamecore PUBLIC INLINE=${MAME_INLINE} ZIP_CACHE=1 CRLF=2 ${CPU_DEFS} )
# -fpermissive -Wno-narrowing

 target_compile_options(mamecore PRIVATE ${AMIGA_COMPILER_OPTIONS} )
#it's all O3 for release only
if(AMIGA)
 target_compile_options(mamecore PRIVATE
	 $<$<CONFIG:RELEASE>:-O3 -fno-unroll-loops -ftree-loop-ivcanon -ftree-loop-im -fno-tree-vectorize -DNDEBUG>
		  )
endif()



add_library( mamecoreUNROLL STATIC  ${MAME_UNROLL_SRC} ${MAME_SOUND_SRC} )
set_property(TARGET mamecoreUNROLL PROPERTY C_STANDARD 11)
set_property(TARGET mamecoreUNROLL PROPERTY CXX_STANDARD 11)
target_include_directories(mamecoreUNROLL PUBLIC ${MAME_INCLUDE_DIRS} ${ZLIBPATH} )
set(MAME_INLINE "static inline")
target_compile_definitions(mamecoreUNROLL PUBLIC INLINE=${MAME_INLINE} ZIP_CACHE=1 CRLF=2 ${CPU_DEFS} )
# -fpermissive -Wno-narrowing

 target_compile_options(mamecoreUNROLL PRIVATE ${AMIGA_COMPILER_OPTIONS} )
#it's all O3 for release only
if(AMIGA)
 target_compile_options(mamecoreUNROLL PRIVATE
		 $<$<CONFIG:RELEASE>:-O3 -funroll-loops -ftree-loop-ivcanon -ftree-loop-im -ftree-vectorize -DNDEBUG>
		  )
endif()
