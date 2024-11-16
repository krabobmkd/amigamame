# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)


option(USE_QT_APP "" ON)

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

#set(PROGNAME lmame)
#add_executable(${PROGNAME} ${LINUX_SRC} )
#set_property(TARGET ${PROGNAME} PROPERTY C_STANDARD 11)
#set_property(TARGET ${PROGNAME} PROPERTY CXX_STANDARD 11)
#target_compile_definitions(${PROGNAME} PUBLIC MAMEVERSION=106 ZIP_CACHE=1 MUISERIALIZER_USES_FLOAT=1 GIT_BRANCH="${GIT_BRANCH}" LSB_FIRST=1 __LP64__=1 )
#if(MAME_USE_HARDFLOAT)
#	target_compile_definitions(${PROGNAME} PRIVATE MAME_USE_HARD_FLOAT=1 )
#endif()
## the static libs that are part eof this repo:
#target_link_libraries(${PROGNAME} PUBLIC mamecore z expat)

## target_include_directories(${PROGNAME} PUBLIC "../mame037b/src")
#target_include_directories(${PROGNAME} PUBLIC "../linux" "${COMMONLIBS}/libserializer")

## MUI is not provided by gccbebbo6.5 ndk by default, we have it there:
##target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/MUI/include")
## target_include_directories(${PROGNAME} PUBLIC "${COMMONLIBS}/extrasdk/P96/include")

## sources that doesn't support -O2 are moved to MameXAmigaO0 lib
#if(AMIGA)
#target_compile_options(${PROGNAME} PRIVATE
#		$<$<CONFIG:RELEASE>:-O2 -Wno-narrowing -DNDEBUG>
#		 ${AMIGA_COMPILER_OPTIONS} )
#endif()

if(USE_QT_APP)


	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	set(GUI_APPNAME "QMame")

	 # Find the QtWidgets library
	 find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)

	 if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	 message( STATUS " **** ITS QT6 *****")
			find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets )
	 else()
	 message( STATUS " **** ITS QT5 *****")
			# Find the QtWidgets library
				find_package(Qt5Core REQUIRED)
				find_package(Qt5Widgets REQUIRED)
	 endif()

    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        include_directories(${qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS} ${qt${QT_VERSION_MAJOR}Quick_INCLUDE_DIRS} ${qt${QT_VERSION_MAJOR}Svg_INCLUDE_DIRS} )
        add_definitions(${qt${QT_VERSION_MAJOR}Widgets_DEFINITIONS} ${qt${QT_VERSION_MAJOR}Qml_DEFINITIONS})
    else()
        # Qt5:
        include_directories(${Qt5Widgets_INCLUDE_DIRS} )
        add_definitions(${qt5Widgets_DEFINITIONS} )
    endif()

    set(QMAME_FILES_MAIN
           qtmain.cpp
           qtmain.h
        filesystem.cpp
        lconfig.cpp
        lconfig.h
        serializer_mamexml.cpp
        serializer_mamexml.h
    )


    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

        qt_add_executable(${GUI_APPNAME}
            MANUAL_FINALIZATION
            ${QMAME_FILES_MAIN}
        )
    else()
            add_executable(${GUI_APPNAME}  ${QMAME_FILES_MAIN} )
    endif()

     if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
         # note: need PRIVATE to work with new qt6 system.
        target_link_libraries(${GUI_APPNAME} PRIVATE Qt6::Widgets )
     else()
         # qt5
        target_link_libraries(${GUI_APPNAME} PRIVATE Qt5::Widgets  )
     endif()

    target_link_libraries(${GUI_APPNAME} PUBLIC mamecore mamecoreUNROLL z expat)
    target_include_directories(${GUI_APPNAME} PUBLIC "../linux" "${COMMONLIBS}/libserializer")

    set_property(TARGET ${GUI_APPNAME} PROPERTY C_STANDARD 11)
    set_property(TARGET ${GUI_APPNAME} PROPERTY CXX_STANDARD 11)
    target_compile_definitions(${GUI_APPNAME} PUBLIC MAMEVERSION=106 ZIP_CACHE=1 MUISERIALIZER_USES_FLOAT=1 GIT_BRANCH="${GIT_BRANCH}" LSB_FIRST=1 __LP64__=1 )
    if(MAME_USE_HARDFLOAT)
        target_compile_definitions(${GUI_APPNAME} PRIVATE MAME_USE_HARD_FLOAT=1 )
    endif()

# endif qt app
endif()
