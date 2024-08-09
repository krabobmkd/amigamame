#ifndef MAME_MSG_H
#define MAME_MSG_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_NOMEM_SAMPLE 1
#define MSG_LOG_ERRORS 2
#define MSG_AUDIO_CH_0 3
#define MSG_AUDIO_CH_1 4
#define MSG_AUDIO_CH_2 5
#define MSG_AUDIO_CH_3 6
#define MSG_FRAMESKIP 7
#define MSG_NO 8
#define MSG_LEFT 9
#define MSG_RIGHT 10
#define MSG_ROTATION 11
#define MSG_START 12
#define MSG_QUIT 13
#define MSG_MENU_GAME 14
#define MSG_MENU_NEW 15
#define MSG_MENU_SAVE_ILBM 16
#define MSG_MENU_ABOUT 17
#define MSG_MENU_QUIT 18
#define MSG_SCREENMODE 19
#define MSG_WIDTH 20
#define MSG_HEIGHT 21
#define MSG_FLIPX 22
#define MSG_FLIPY 23
#define MSG_DISPLAY 24
#define MSG_SOUND 25
#define MSG_INPUT 26
#define MSG_PATHS 27
#define MSG_MIN_CHIP 28
#define MSG_PRIMARY_CONTROLLER 29
#define MSG_SECONDARY_CONTROLLER 30
#define MSG_NONE 31
#define MSG_JOYSTICK_PORT_1 32
#define MSG_JOYPAD_PORT_1 33
#define MSG_JOYSTICK_PORT_2 34
#define MSG_JOYPAD_PORT_2 35
#define MSG_MOUSE_PORT_1 36
#define MSG_BUTTON_B_HOLD_TIME 37
#define MSG_AUTO_FIRE_RATE 38
#define MSG_TYPE 39
#define MSG_ROM_PATH 40
#define MSG_SAMPLE_PATH 41
#define MSG_SCAN 42
#define MSG_BUFFERING 43
#define MSG_SINGLE 44
#define MSG_DOUBLE 45
#define MSG_TRIPLE 46
#define MSG_ANTIALIAS 47
#define MSG_BEAM 48
#define MSG_FLICKER 49
#define MSG_PAULA 50
#define MSG_AHI 51
#define MSG_DRIVERS 52
#define MSG_DRIVER 53
#define MSG_COMMENT 54
#define MSG_BITMAP_GAME_DEFAULTS 55
#define MSG_VECTOR_GAME_DEFAULTS 56
#define MSG_BITMAP 57
#define MSG_VECTOR 58
#define MSG_NOT_WORKING 59
#define MSG_SCREEN_TYPE 60
#define MSG_BEST 61
#define MSG_WORKBENCH 62
#define MSG_CUSTOM 63
#define MSG_USER_SELECT 64
#define MSG_USE_DEFAULTS 65
#define MSG_DIRECT_MODE 66
#define MSG_OFF 67
#define MSG_DRAW 68
#define MSG_COPY 69
#define MSG_DIRTY_LINES 70
#define MSG_SHOW 71
#define MSG_ALL 72
#define MSG_FOUND 73
#define MSG_COLORS 74
#define MSG_INVALID 75
#define MSG_REQUIRES_LIB 76
#define MSG_FAILED_TO_LOAD_ELF 77
#define MSG_FAILED_TO_CREATE_PPC_TASK 78
#define MSG_NOT_ENOUGH_MEMORY 79
#define MSG_FAILED_TO_OPEN_SCREEN 80
#define MSG_FAILED_TO_OPEN_WINDOW 81
#define MSG_FAILED_TO_ALLOCATE_INPUTS 82
#define MSG_OK 83
#define MSG_WRONG_COLORS 84
#define MSG_IMPERFECT_COLORS 85
#define MSG_DIRECTORY 86
#define MSG_AUTO_FRAMESKIP 87
#define MSG_TRANSLUCENCY 88
#define MSG_ALLOW16BIT 89
#define MSG_ASYNCPPC 90

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_NOMEM_SAMPLE_STR "Not enough memory to load sample \"%s\".\n"
#define MSG_LOG_ERRORS_STR "Log errors"
#define MSG_AUDIO_CH_0_STR "Audio channel 0"
#define MSG_AUDIO_CH_1_STR "Audio channel 1"
#define MSG_AUDIO_CH_2_STR "Audio channel 2"
#define MSG_AUDIO_CH_3_STR "Audio channel 3"
#define MSG_FRAMESKIP_STR "Frames to skip"
#define MSG_NO_STR "No"
#define MSG_LEFT_STR "Left"
#define MSG_RIGHT_STR "Right"
#define MSG_ROTATION_STR "Rotation"
#define MSG_START_STR "_Start"
#define MSG_QUIT_STR "_Quit"
#define MSG_MENU_GAME_STR "Game"
#define MSG_MENU_NEW_STR "New..."
#define MSG_MENU_SAVE_ILBM_STR "Save ILBM..."
#define MSG_MENU_ABOUT_STR "About..."
#define MSG_MENU_QUIT_STR "Quit"
#define MSG_SCREENMODE_STR "Screenmode"
#define MSG_WIDTH_STR "Width"
#define MSG_HEIGHT_STR "Height"
#define MSG_FLIPX_STR "Flip X"
#define MSG_FLIPY_STR "Flip Y"
#define MSG_DISPLAY_STR "Display"
#define MSG_SOUND_STR "Sound"
#define MSG_INPUT_STR "Input"
#define MSG_PATHS_STR "Paths"
#define MSG_MIN_CHIP_STR "Minimum free chip memory"
#define MSG_PRIMARY_CONTROLLER_STR "Primary controller"
#define MSG_SECONDARY_CONTROLLER_STR "Secondary controller"
#define MSG_NONE_STR "None"
#define MSG_JOYSTICK_PORT_1_STR "Joystick in port 1"
#define MSG_JOYPAD_PORT_1_STR "Joypad in port 1"
#define MSG_JOYSTICK_PORT_2_STR "Joystick in port 2"
#define MSG_JOYPAD_PORT_2_STR "Joypad in port 2"
#define MSG_MOUSE_PORT_1_STR "Mouse in port 1"
#define MSG_BUTTON_B_HOLD_TIME_STR "Button B hold time"
#define MSG_AUTO_FIRE_RATE_STR "Auto fire rate"
#define MSG_TYPE_STR "Type"
#define MSG_ROM_PATH_STR "Rom path"
#define MSG_SAMPLE_PATH_STR "Sample path"
#define MSG_SCAN_STR "Scan"
#define MSG_BUFFERING_STR "Buffering"
#define MSG_SINGLE_STR "Single"
#define MSG_DOUBLE_STR "Double"
#define MSG_TRIPLE_STR "Triple"
#define MSG_ANTIALIAS_STR "Vector antialiasing"
#define MSG_BEAM_STR "Vector beam width"
#define MSG_FLICKER_STR "Vector flicker"
#define MSG_PAULA_STR "Paula"
#define MSG_AHI_STR "AHI"
#define MSG_DRIVERS_STR "Drivers"
#define MSG_DRIVER_STR "Driver"
#define MSG_COMMENT_STR "Comment"
#define MSG_BITMAP_GAME_DEFAULTS_STR "Bitmap game defaults"
#define MSG_VECTOR_GAME_DEFAULTS_STR "Vector game defaults"
#define MSG_BITMAP_STR "Bitmap"
#define MSG_VECTOR_STR "Vector"
#define MSG_NOT_WORKING_STR "Not working"
#define MSG_SCREEN_TYPE_STR "Screen type"
#define MSG_BEST_STR "Best"
#define MSG_WORKBENCH_STR "Workbench"
#define MSG_CUSTOM_STR "Custom"
#define MSG_USER_SELECT_STR "User select"
#define MSG_USE_DEFAULTS_STR "Use defaults"
#define MSG_DIRECT_MODE_STR "Direct mode"
#define MSG_OFF_STR "Off"
#define MSG_DRAW_STR "Draw"
#define MSG_COPY_STR "Copy"
#define MSG_DIRTY_LINES_STR "Dirty lines"
#define MSG_SHOW_STR "Show"
#define MSG_ALL_STR "All"
#define MSG_FOUND_STR "Found"
#define MSG_COLORS_STR "Colors"
#define MSG_INVALID_STR "Invalid"
#define MSG_REQUIRES_LIB_STR "Requires %s version %ld"
#define MSG_FAILED_TO_LOAD_ELF_STR "Failed to load mameppc.elf"
#define MSG_FAILED_TO_CREATE_PPC_TASK_STR "Failed to create PPC task"
#define MSG_NOT_ENOUGH_MEMORY_STR "Not enough memory"
#define MSG_FAILED_TO_OPEN_SCREEN_STR "Failed to open screen"
#define MSG_FAILED_TO_OPEN_WINDOW_STR "Failed to open window"
#define MSG_FAILED_TO_ALLOCATE_INPUTS_STR "Failed to allocate inputs"
#define MSG_OK_STR "OK"
#define MSG_WRONG_COLORS_STR "Wrong colors"
#define MSG_IMPERFECT_COLORS_STR "Imperfect colors"
#define MSG_DIRECTORY_STR "Directory"
#define MSG_AUTO_FRAMESKIP_STR "Auto frameskip"
#define MSG_TRANSLUCENCY_STR "Vector translucency"
#define MSG_ALLOW16BIT_STR "Allow 16 bit"
#define MSG_ASYNCPPC_STR "Asynchronus PPC"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_NOMEM_SAMPLE,(STRPTR)MSG_NOMEM_SAMPLE_STR},
    {MSG_LOG_ERRORS,(STRPTR)MSG_LOG_ERRORS_STR},
    {MSG_AUDIO_CH_0,(STRPTR)MSG_AUDIO_CH_0_STR},
    {MSG_AUDIO_CH_1,(STRPTR)MSG_AUDIO_CH_1_STR},
    {MSG_AUDIO_CH_2,(STRPTR)MSG_AUDIO_CH_2_STR},
    {MSG_AUDIO_CH_3,(STRPTR)MSG_AUDIO_CH_3_STR},
    {MSG_FRAMESKIP,(STRPTR)MSG_FRAMESKIP_STR},
    {MSG_NO,(STRPTR)MSG_NO_STR},
    {MSG_LEFT,(STRPTR)MSG_LEFT_STR},
    {MSG_RIGHT,(STRPTR)MSG_RIGHT_STR},
    {MSG_ROTATION,(STRPTR)MSG_ROTATION_STR},
    {MSG_START,(STRPTR)MSG_START_STR},
    {MSG_QUIT,(STRPTR)MSG_QUIT_STR},
    {MSG_MENU_GAME,(STRPTR)MSG_MENU_GAME_STR},
    {MSG_MENU_NEW,(STRPTR)MSG_MENU_NEW_STR},
    {MSG_MENU_SAVE_ILBM,(STRPTR)MSG_MENU_SAVE_ILBM_STR},
    {MSG_MENU_ABOUT,(STRPTR)MSG_MENU_ABOUT_STR},
    {MSG_MENU_QUIT,(STRPTR)MSG_MENU_QUIT_STR},
    {MSG_SCREENMODE,(STRPTR)MSG_SCREENMODE_STR},
    {MSG_WIDTH,(STRPTR)MSG_WIDTH_STR},
    {MSG_HEIGHT,(STRPTR)MSG_HEIGHT_STR},
    {MSG_FLIPX,(STRPTR)MSG_FLIPX_STR},
    {MSG_FLIPY,(STRPTR)MSG_FLIPY_STR},
    {MSG_DISPLAY,(STRPTR)MSG_DISPLAY_STR},
    {MSG_SOUND,(STRPTR)MSG_SOUND_STR},
    {MSG_INPUT,(STRPTR)MSG_INPUT_STR},
    {MSG_PATHS,(STRPTR)MSG_PATHS_STR},
    {MSG_MIN_CHIP,(STRPTR)MSG_MIN_CHIP_STR},
    {MSG_PRIMARY_CONTROLLER,(STRPTR)MSG_PRIMARY_CONTROLLER_STR},
    {MSG_SECONDARY_CONTROLLER,(STRPTR)MSG_SECONDARY_CONTROLLER_STR},
    {MSG_NONE,(STRPTR)MSG_NONE_STR},
    {MSG_JOYSTICK_PORT_1,(STRPTR)MSG_JOYSTICK_PORT_1_STR},
    {MSG_JOYPAD_PORT_1,(STRPTR)MSG_JOYPAD_PORT_1_STR},
    {MSG_JOYSTICK_PORT_2,(STRPTR)MSG_JOYSTICK_PORT_2_STR},
    {MSG_JOYPAD_PORT_2,(STRPTR)MSG_JOYPAD_PORT_2_STR},
    {MSG_MOUSE_PORT_1,(STRPTR)MSG_MOUSE_PORT_1_STR},
    {MSG_BUTTON_B_HOLD_TIME,(STRPTR)MSG_BUTTON_B_HOLD_TIME_STR},
    {MSG_AUTO_FIRE_RATE,(STRPTR)MSG_AUTO_FIRE_RATE_STR},
    {MSG_TYPE,(STRPTR)MSG_TYPE_STR},
    {MSG_ROM_PATH,(STRPTR)MSG_ROM_PATH_STR},
    {MSG_SAMPLE_PATH,(STRPTR)MSG_SAMPLE_PATH_STR},
    {MSG_SCAN,(STRPTR)MSG_SCAN_STR},
    {MSG_BUFFERING,(STRPTR)MSG_BUFFERING_STR},
    {MSG_SINGLE,(STRPTR)MSG_SINGLE_STR},
    {MSG_DOUBLE,(STRPTR)MSG_DOUBLE_STR},
    {MSG_TRIPLE,(STRPTR)MSG_TRIPLE_STR},
    {MSG_ANTIALIAS,(STRPTR)MSG_ANTIALIAS_STR},
    {MSG_BEAM,(STRPTR)MSG_BEAM_STR},
    {MSG_FLICKER,(STRPTR)MSG_FLICKER_STR},
    {MSG_PAULA,(STRPTR)MSG_PAULA_STR},
    {MSG_AHI,(STRPTR)MSG_AHI_STR},
    {MSG_DRIVERS,(STRPTR)MSG_DRIVERS_STR},
    {MSG_DRIVER,(STRPTR)MSG_DRIVER_STR},
    {MSG_COMMENT,(STRPTR)MSG_COMMENT_STR},
    {MSG_BITMAP_GAME_DEFAULTS,(STRPTR)MSG_BITMAP_GAME_DEFAULTS_STR},
    {MSG_VECTOR_GAME_DEFAULTS,(STRPTR)MSG_VECTOR_GAME_DEFAULTS_STR},
    {MSG_BITMAP,(STRPTR)MSG_BITMAP_STR},
    {MSG_VECTOR,(STRPTR)MSG_VECTOR_STR},
    {MSG_NOT_WORKING,(STRPTR)MSG_NOT_WORKING_STR},
    {MSG_SCREEN_TYPE,(STRPTR)MSG_SCREEN_TYPE_STR},
    {MSG_BEST,(STRPTR)MSG_BEST_STR},
    {MSG_WORKBENCH,(STRPTR)MSG_WORKBENCH_STR},
    {MSG_CUSTOM,(STRPTR)MSG_CUSTOM_STR},
    {MSG_USER_SELECT,(STRPTR)MSG_USER_SELECT_STR},
    {MSG_USE_DEFAULTS,(STRPTR)MSG_USE_DEFAULTS_STR},
    {MSG_DIRECT_MODE,(STRPTR)MSG_DIRECT_MODE_STR},
    {MSG_OFF,(STRPTR)MSG_OFF_STR},
    {MSG_DRAW,(STRPTR)MSG_DRAW_STR},
    {MSG_COPY,(STRPTR)MSG_COPY_STR},
    {MSG_DIRTY_LINES,(STRPTR)MSG_DIRTY_LINES_STR},
    {MSG_SHOW,(STRPTR)MSG_SHOW_STR},
    {MSG_ALL,(STRPTR)MSG_ALL_STR},
    {MSG_FOUND,(STRPTR)MSG_FOUND_STR},
    {MSG_COLORS,(STRPTR)MSG_COLORS_STR},
    {MSG_INVALID,(STRPTR)MSG_INVALID_STR},
    {MSG_REQUIRES_LIB,(STRPTR)MSG_REQUIRES_LIB_STR},
    {MSG_FAILED_TO_LOAD_ELF,(STRPTR)MSG_FAILED_TO_LOAD_ELF_STR},
    {MSG_FAILED_TO_CREATE_PPC_TASK,(STRPTR)MSG_FAILED_TO_CREATE_PPC_TASK_STR},
    {MSG_NOT_ENOUGH_MEMORY,(STRPTR)MSG_NOT_ENOUGH_MEMORY_STR},
    {MSG_FAILED_TO_OPEN_SCREEN,(STRPTR)MSG_FAILED_TO_OPEN_SCREEN_STR},
    {MSG_FAILED_TO_OPEN_WINDOW,(STRPTR)MSG_FAILED_TO_OPEN_WINDOW_STR},
    {MSG_FAILED_TO_ALLOCATE_INPUTS,(STRPTR)MSG_FAILED_TO_ALLOCATE_INPUTS_STR},
    {MSG_OK,(STRPTR)MSG_OK_STR},
    {MSG_WRONG_COLORS,(STRPTR)MSG_WRONG_COLORS_STR},
    {MSG_IMPERFECT_COLORS,(STRPTR)MSG_IMPERFECT_COLORS_STR},
    {MSG_DIRECTORY,(STRPTR)MSG_DIRECTORY_STR},
    {MSG_AUTO_FRAMESKIP,(STRPTR)MSG_AUTO_FRAMESKIP_STR},
    {MSG_TRANSLUCENCY,(STRPTR)MSG_TRANSLUCENCY_STR},
    {MSG_ALLOW16BIT,(STRPTR)MSG_ALLOW16BIT_STR},
    {MSG_ASYNCPPC,(STRPTR)MSG_ASYNCPPC_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x01\x00\x28"
    MSG_NOMEM_SAMPLE_STR "\x00"
    "\x00\x00\x00\x02\x00\x0C"
    MSG_LOG_ERRORS_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x10"
    MSG_AUDIO_CH_0_STR "\x00"
    "\x00\x00\x00\x04\x00\x10"
    MSG_AUDIO_CH_1_STR "\x00"
    "\x00\x00\x00\x05\x00\x10"
    MSG_AUDIO_CH_2_STR "\x00"
    "\x00\x00\x00\x06\x00\x10"
    MSG_AUDIO_CH_3_STR "\x00"
    "\x00\x00\x00\x07\x00\x10"
    MSG_FRAMESKIP_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x04"
    MSG_NO_STR "\x00\x00"
    "\x00\x00\x00\x09\x00\x06"
    MSG_LEFT_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x06"
    MSG_RIGHT_STR "\x00"
    "\x00\x00\x00\x0B\x00\x0A"
    MSG_ROTATION_STR "\x00\x00"
    "\x00\x00\x00\x0C\x00\x08"
    MSG_START_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x06"
    MSG_QUIT_STR "\x00"
    "\x00\x00\x00\x0E\x00\x06"
    MSG_MENU_GAME_STR "\x00\x00"
    "\x00\x00\x00\x0F\x00\x08"
    MSG_MENU_NEW_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x0E"
    MSG_MENU_SAVE_ILBM_STR "\x00\x00"
    "\x00\x00\x00\x11\x00\x0A"
    MSG_MENU_ABOUT_STR "\x00\x00"
    "\x00\x00\x00\x12\x00\x06"
    MSG_MENU_QUIT_STR "\x00\x00"
    "\x00\x00\x00\x13\x00\x0C"
    MSG_SCREENMODE_STR "\x00\x00"
    "\x00\x00\x00\x14\x00\x06"
    MSG_WIDTH_STR "\x00"
    "\x00\x00\x00\x15\x00\x08"
    MSG_HEIGHT_STR "\x00\x00"
    "\x00\x00\x00\x16\x00\x08"
    MSG_FLIPX_STR "\x00\x00"
    "\x00\x00\x00\x17\x00\x08"
    MSG_FLIPY_STR "\x00\x00"
    "\x00\x00\x00\x18\x00\x08"
    MSG_DISPLAY_STR "\x00"
    "\x00\x00\x00\x19\x00\x06"
    MSG_SOUND_STR "\x00"
    "\x00\x00\x00\x1A\x00\x06"
    MSG_INPUT_STR "\x00"
    "\x00\x00\x00\x1B\x00\x06"
    MSG_PATHS_STR "\x00"
    "\x00\x00\x00\x1C\x00\x1A"
    MSG_MIN_CHIP_STR "\x00\x00"
    "\x00\x00\x00\x1D\x00\x14"
    MSG_PRIMARY_CONTROLLER_STR "\x00\x00"
    "\x00\x00\x00\x1E\x00\x16"
    MSG_SECONDARY_CONTROLLER_STR "\x00\x00"
    "\x00\x00\x00\x1F\x00\x06"
    MSG_NONE_STR "\x00\x00"
    "\x00\x00\x00\x20\x00\x14"
    MSG_JOYSTICK_PORT_1_STR "\x00\x00"
    "\x00\x00\x00\x21\x00\x12"
    MSG_JOYPAD_PORT_1_STR "\x00\x00"
    "\x00\x00\x00\x22\x00\x14"
    MSG_JOYSTICK_PORT_2_STR "\x00\x00"
    "\x00\x00\x00\x23\x00\x12"
    MSG_JOYPAD_PORT_2_STR "\x00\x00"
    "\x00\x00\x00\x24\x00\x10"
    MSG_MOUSE_PORT_1_STR "\x00"
    "\x00\x00\x00\x25\x00\x14"
    MSG_BUTTON_B_HOLD_TIME_STR "\x00\x00"
    "\x00\x00\x00\x26\x00\x10"
    MSG_AUTO_FIRE_RATE_STR "\x00\x00"
    "\x00\x00\x00\x27\x00\x06"
    MSG_TYPE_STR "\x00\x00"
    "\x00\x00\x00\x28\x00\x0A"
    MSG_ROM_PATH_STR "\x00\x00"
    "\x00\x00\x00\x29\x00\x0C"
    MSG_SAMPLE_PATH_STR "\x00"
    "\x00\x00\x00\x2A\x00\x06"
    MSG_SCAN_STR "\x00\x00"
    "\x00\x00\x00\x2B\x00\x0A"
    MSG_BUFFERING_STR "\x00"
    "\x00\x00\x00\x2C\x00\x08"
    MSG_SINGLE_STR "\x00\x00"
    "\x00\x00\x00\x2D\x00\x08"
    MSG_DOUBLE_STR "\x00\x00"
    "\x00\x00\x00\x2E\x00\x08"
    MSG_TRIPLE_STR "\x00\x00"
    "\x00\x00\x00\x2F\x00\x14"
    MSG_ANTIALIAS_STR "\x00"
    "\x00\x00\x00\x30\x00\x12"
    MSG_BEAM_STR "\x00"
    "\x00\x00\x00\x31\x00\x10"
    MSG_FLICKER_STR "\x00\x00"
    "\x00\x00\x00\x32\x00\x06"
    MSG_PAULA_STR "\x00"
    "\x00\x00\x00\x33\x00\x04"
    MSG_AHI_STR "\x00"
    "\x00\x00\x00\x34\x00\x08"
    MSG_DRIVERS_STR "\x00"
    "\x00\x00\x00\x35\x00\x08"
    MSG_DRIVER_STR "\x00\x00"
    "\x00\x00\x00\x36\x00\x08"
    MSG_COMMENT_STR "\x00"
    "\x00\x00\x00\x37\x00\x16"
    MSG_BITMAP_GAME_DEFAULTS_STR "\x00\x00"
    "\x00\x00\x00\x38\x00\x16"
    MSG_VECTOR_GAME_DEFAULTS_STR "\x00\x00"
    "\x00\x00\x00\x39\x00\x08"
    MSG_BITMAP_STR "\x00\x00"
    "\x00\x00\x00\x3A\x00\x08"
    MSG_VECTOR_STR "\x00\x00"
    "\x00\x00\x00\x3B\x00\x0C"
    MSG_NOT_WORKING_STR "\x00"
    "\x00\x00\x00\x3C\x00\x0C"
    MSG_SCREEN_TYPE_STR "\x00"
    "\x00\x00\x00\x3D\x00\x06"
    MSG_BEST_STR "\x00\x00"
    "\x00\x00\x00\x3E\x00\x0A"
    MSG_WORKBENCH_STR "\x00"
    "\x00\x00\x00\x3F\x00\x08"
    MSG_CUSTOM_STR "\x00\x00"
    "\x00\x00\x00\x40\x00\x0C"
    MSG_USER_SELECT_STR "\x00"
    "\x00\x00\x00\x41\x00\x0E"
    MSG_USE_DEFAULTS_STR "\x00\x00"
    "\x00\x00\x00\x42\x00\x0C"
    MSG_DIRECT_MODE_STR "\x00"
    "\x00\x00\x00\x43\x00\x04"
    MSG_OFF_STR "\x00"
    "\x00\x00\x00\x44\x00\x06"
    MSG_DRAW_STR "\x00\x00"
    "\x00\x00\x00\x45\x00\x06"
    MSG_COPY_STR "\x00\x00"
    "\x00\x00\x00\x46\x00\x0C"
    MSG_DIRTY_LINES_STR "\x00"
    "\x00\x00\x00\x47\x00\x06"
    MSG_SHOW_STR "\x00\x00"
    "\x00\x00\x00\x48\x00\x04"
    MSG_ALL_STR "\x00"
    "\x00\x00\x00\x49\x00\x06"
    MSG_FOUND_STR "\x00"
    "\x00\x00\x00\x4A\x00\x08"
    MSG_COLORS_STR "\x00\x00"
    "\x00\x00\x00\x4B\x00\x08"
    MSG_INVALID_STR "\x00"
    "\x00\x00\x00\x4C\x00\x18"
    MSG_REQUIRES_LIB_STR "\x00"
    "\x00\x00\x00\x4D\x00\x1C"
    MSG_FAILED_TO_LOAD_ELF_STR "\x00\x00"
    "\x00\x00\x00\x4E\x00\x1A"
    MSG_FAILED_TO_CREATE_PPC_TASK_STR "\x00"
    "\x00\x00\x00\x4F\x00\x12"
    MSG_NOT_ENOUGH_MEMORY_STR "\x00"
    "\x00\x00\x00\x50\x00\x16"
    MSG_FAILED_TO_OPEN_SCREEN_STR "\x00"
    "\x00\x00\x00\x51\x00\x16"
    MSG_FAILED_TO_OPEN_WINDOW_STR "\x00"
    "\x00\x00\x00\x52\x00\x1A"
    MSG_FAILED_TO_ALLOCATE_INPUTS_STR "\x00"
    "\x00\x00\x00\x53\x00\x04"
    MSG_OK_STR "\x00\x00"
    "\x00\x00\x00\x54\x00\x0E"
    MSG_WRONG_COLORS_STR "\x00\x00"
    "\x00\x00\x00\x55\x00\x12"
    MSG_IMPERFECT_COLORS_STR "\x00\x00"
    "\x00\x00\x00\x56\x00\x0A"
    MSG_DIRECTORY_STR "\x00"
    "\x00\x00\x00\x57\x00\x10"
    MSG_AUTO_FRAMESKIP_STR "\x00\x00"
    "\x00\x00\x00\x58\x00\x14"
    MSG_TRANSLUCENCY_STR "\x00"
    "\x00\x00\x00\x59\x00\x0E"
    MSG_ALLOW16BIT_STR "\x00\x00"
    "\x00\x00\x00\x5A\x00\x10"
    MSG_ASYNCPPC_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


STRPTR GetMessage(LONG);


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#define XLocaleBase LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#define LocaleBase XLocaleBase
#undef XLocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* MAME_MSG_H */
