#ifndef CONFIG_H
#define CONFIG_H
/**************************************************************************
 *
 * Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
 *
 * $Id: config.h,v 1.1 1999/04/28 18:51:46 meh Exp meh $
 *
 * $Log: config.h,v $
 * Revision 1.1  1999/04/28 18:51:46  meh
 * Initial revision
 *
 *
 *************************************************************************/

#include "amiga106_inputs.h"

int  AllocConfig(int argc, char **argv);
void FreeConfig(void);
void LoadConfig(int argc, char **argv);
void GetConfig(int driver, LONG *cfg);
void SetConfig(int driver, LONG *cfg);
int  GetUseDefaults(int driver);

BOOL GetFound(int driver);
void SetFound(int driver, BOOL found);

const char *GetRomPath(int driver, int path_num);
const char *GetSamplePath(int driver, int path_num);

#ifdef MESS
#define CFG_DRIVER           0
#define CFG_IMAGE            1
#define CFG_SCREENTYPE       2
#define CFG_SCREENMODE       3
#define CFG_DIRECTMODE       4
#define CFG_DIRTYLINES       5
#define CFG_DEPTH            6
#define CFG_ALLOW16BIT       7
#define CFG_FLIPX            8
#define CFG_FLIPY            9
#define CFG_ANTIALIASING     10
#define CFG_BEAMWIDTH        11
#define CFG_VECTORFLICKER    12
#define CFG_AUTOFRAMESKIP    13
#define CFG_FRAMESKIP        14
#define CFG_WIDTH            15
#define CFG_HEIGHT           16
#define CFG_BUFFERING        17
#define CFG_ROTATION         18
#define CFG_SOUND            19
#define CFG_AUDIOCHANNEL0    20
#define CFG_AUDIOCHANNEL1    21
#define CFG_AUDIOCHANNEL2    22
#define CFG_AUDIOCHANNEL3    23
#define CFG_MINFREECHIP      24
#define CFG_JOY1TYPE         25
#define CFG_JOY1BUTTONBTIME  26
#define CFG_JOY1AUTOFIRERATE 27
#define CFG_JOY2TYPE         28
#define CFG_JOY2BUTTONBTIME  29
#define CFG_JOY2AUTOFIRERATE 30
#define CFG_ROMPATH          31
#define CFG_SAMPLEPATH       32
#define CFG_ITEMS            33
#else
#define CFG_DRIVER           0
#define CFG_SHOW             1
#define CFG_USEDEFAULTS      2
#define CFG_SCREENTYPE       3
#define CFG_SCREENMODE       4
#define CFG_DIRECTMODE       5
#define CFG_DIRTYLINES       6
#define CFG_DEPTH            7
#define CFG_ALLOW16BIT       8
#define CFG_FLIPX            9
#define CFG_FLIPY            10
#define CFG_ANTIALIASING     11
#define CFG_TRANSLUCENCY     12
#define CFG_BEAMWIDTH        13
#define CFG_VECTORFLICKER    14
#define CFG_AUTOFRAMESKIP    15
#define CFG_FRAMESKIP        16
#define CFG_WIDTH            17
#define CFG_HEIGHT           18
#define CFG_BUFFERING        19
#define CFG_ROTATION         20
#define CFG_SOUND            21
#define CFG_AUDIOCHANNEL0    22
#define CFG_AUDIOCHANNEL1    23
#define CFG_AUDIOCHANNEL2    24
#define CFG_AUDIOCHANNEL3    25
#define CFG_MINFREECHIP      26
#define CFG_JOY1TYPE         27
#define CFG_JOY1BUTTONBTIME  28
#define CFG_JOY1AUTOFIRERATE 29
#define CFG_JOY2TYPE         30
#define CFG_JOY2BUTTONBTIME  31
#define CFG_JOY2AUTOFIRERATE 32
#define CFG_ROMPATH          33
#define CFG_SAMPLEPATH       34
#ifdef POWERUP
#define CFG_ASYNCPPC         35
#define CFG_ITEMS            36
#else
#define CFG_ITEMS            35
#endif
#endif

/* CFG_SHOW values: */

#define CFGS_ALL   0
#define CFGS_FOUND 1

/* CFG_SCREENTYPE values: */

#define CFGST_BEST       0
#define CFGST_WB         1
#define CFGST_CUSTOM     2
#define CFGST_USERSELECT 3

/* CFG_DIRECTMODE values: */

#define CFGDM_OFF  0
#define CFGDM_DRAW 1
#define CFGDM_COPY 2

/* CFG_BUFFERING values: */

#define CFGB_SINGLE 0
#define CFGB_DOUBLE 1
#define CFGB_TRIPLE 2

/* CFG_ROTATION values: */

#define CFGR_NO    0
#define CFGR_LEFT  1
#define CFGR_RIGHT 2

/* CFG_SOUND values: */

#define CFGS_NO    0
#define CFGS_PAULA 1
#define CFGS_AHI   2

/* CFG_JOY1TYPE values: */

#define CFGJ1_NO        IPT_NONE
#define CFGJ1_JOYSTICK2 IPT_JOYSTICK
#define CFGJ1_JOYPAD2   IPT_JOYPAD
#define CFGJ1_MOUSE1    IPT_MOUSE

/* CFG_JOY2TYPE values: */

#define CFGJ2_NO        IPT_NONE
#define CFGJ2_JOYSTICK1 IPT_JOYSTICK
#define CFGJ2_JOYPAD1   IPT_JOYPAD

extern LONG Config[];

#endif
