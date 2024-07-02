/**************************************************************************
 *
 * Copyright (C) 1999 Mats Eirik Hansen (mats.hansen@triumph.no)
 *
 * $Id: config.c,v 1.1 1999/04/28 18:51:46 meh Exp meh $
 *
 * $Log: config.c,v $
 * Revision 1.1  1999/04/28 18:51:46  meh
 * Initial revision
 *
 *
 *************************************************************************/

#include <proto/dos.h>

extern "C" {
#include <exec/types.h>
//just for INVALID_ID
#include <graphics/modeid.h>
}

#include <moo/db.h>

#include "main.h"
#include "version.h"
#include <stdio.h>
#include <strings.h>
#include <string.h>



LONG Config[CFG_ITEMS];

static char       **SectionNames;
static int        NumConfigs;
static dbfile_t   *DBFile;
static dbarg_t    *DBArg;
#ifdef MESS
static dbstring_t *DBImage;
#else
static dbbool_t   *DBShow;
static dbbool_t   *DBUseDefaults;
#endif
static dbbool_t   *DBFound;
static dbbool_t   *DBAntialiasing;
static dbbool_t   *DBTranslucency;
static dbint_t    *DBBeamWidth;
static dbint_t    *DBVectorFlicker;
static dbenum_t   *DBScreenType;
static dbuint_t   *DBScreenMode;
static dbenum_t   *DBDirectMode;
static dbbool_t   *DBDirtyLines;
static dbint_t    *DBDepth;
static dbbool_t   *DBAllow16Bit;
static dbbool_t   *DBFlipX;
static dbbool_t   *DBFlipY;
static dbbool_t   *DBAutoFrameSkip;
static dbint_t    *DBFrameSkip;
static dbint_t    *DBWidth;
static dbint_t    *DBHeight;
static dbenum_t   *DBBuffering;
static dbenum_t   *DBRotation;
static dbenum_t   *DBSound;
static dbint_t    *DBAudioChannel0;
static dbint_t    *DBAudioChannel1;
static dbint_t    *DBAudioChannel2;
static dbint_t    *DBAudioChannel3;
static dbint_t    *DBMinFreeChip;
static dbenum_t   *DBJoy1Type;
static dbint_t    *DBJoy1ButtonBTime;
static dbint_t    *DBJoy1AutoFireRate;
static dbenum_t   *DBJoy2Type;
static dbint_t    *DBJoy2ButtonBTime;
static dbint_t    *DBJoy2AutoFireRate;
static dbstring_t *DBRomPath;
static dbstring_t *DBSamplePath;
#ifdef POWERUP
static dbbool_t   *DBAsyncPPC;
#endif

void MakeDirs(void);

int AllocConfig(int argc, char **argv)
{
#ifndef MESS
  static dbenumdef_t dbshow_defs[] = {{"All",0},{"Found",1},{NULL,0}};
#endif
  static dbenumdef_t dbscreentype_defs[] = {{"Best",0},{"WB",1},{"Custom",2},{"UserSelect",3},{NULL,0}};
  static dbenumdef_t dbdirectmode_defs[] = {{"Off",0},{"Draw",1},{"Copy",2},{NULL,0}};
  static dbenumdef_t dbbuffering_defs[]  = {{"Single",0},{"Double",1},{"Triple",2},{NULL,0}};
  static dbenumdef_t dbrotation_defs[]   = {{"No",0},{"Left",1},{"Right",2},{NULL,0}};
  static dbenumdef_t dbsound_defs[]      = {{"No",0},{"Paula",1},{"AHI",2},{NULL,0}};
  static dbenumdef_t dbjoy1type_defs[]   = {{"No",0},{"JoyStickPort2",1},{"JoyPadPort2",2},{"MousePort1",3},{NULL,0}};
  static dbenumdef_t dbjoy2type_defs[]   = {{"No",0},{"JoyStickPort1",1},{"JoyPadPort1",2},{NULL,0}};
  int i;

  DBFile        = NULL;
  SectionNames    = NULL;
  Config[CFG_DRIVER]  = -2;

  for(NumConfigs = 0; Drivers[NumConfigs]; NumConfigs++);

#ifdef MESS
  NumConfigs++;
#else
  NumConfigs += 3;
#endif

  DBArg = dbargNew(
#ifdef MESS
        DBImage            = dbstringNew("Image", 1, 0, ""),
#else
        DBShow             = dbenumNew("Show", 1, 0, 1, dbshow_defs),
        DBUseDefaults      = dbboolNew("UseDefaults", NumConfigs, 0, 1),
#endif
        DBFound            = dbboolNew("Found", NumConfigs, 0, 0),
        DBScreenType       = dbenumNew("ScreenType", NumConfigs, 0, 0, dbscreentype_defs),
        DBScreenMode       = dbuintNew("ScreenMode", NumConfigs, 0, INVALID_ID, 0xffffffff, 0),
        DBDirectMode       = dbenumNew("DirectMode", NumConfigs, 0, 0, dbdirectmode_defs),
        DBDirtyLines       = dbboolNew("DirtyLines", NumConfigs, 0, 1),
        DBDepth            = dbintNew("Depth", NumConfigs, 0, 8, 32, 1),
        DBAllow16Bit       = dbboolNew("Allow16Bit", NumConfigs, 0, 1),
        DBFlipX            = dbboolNew("FlipX", NumConfigs, 0, 0),
        DBFlipY            = dbboolNew("FlipY", NumConfigs, 0, 0),
        DBAntialiasing     = dbboolNew("Antialiasing", NumConfigs, 0, 0),
        DBTranslucency     = dbboolNew("Translucency", NumConfigs, 0, 0),
        DBBeamWidth        = dbintNew("BeamWidth", NumConfigs, 0, 1, 16, 1),
        DBVectorFlicker    = dbintNew("VectorFlicker", NumConfigs, 0, 0, 100, 0),
        DBAutoFrameSkip    = dbboolNew("AutoFrameSkip", NumConfigs, 0, 0),
        DBFrameSkip        = dbintNew("FrameSkip", NumConfigs, 0, 0, 3, 0),
        DBWidth            = dbintNew("Width", NumConfigs, 0, 0, 4096, 0),
        DBHeight           = dbintNew("Height", NumConfigs, 0, 0, 4096, 0),
        DBBuffering        = dbenumNew("Buffering", NumConfigs, 0, 0, dbbuffering_defs),
        DBRotation         = dbenumNew("Rotation", NumConfigs, 0, 0, dbrotation_defs),
        DBSound            = dbenumNew("Sound", NumConfigs, 0, 1, dbsound_defs),
        DBAudioChannel0    = dbintNew("AudioChannel0", NumConfigs, 0, 0, 15, 0),
        DBAudioChannel1    = dbintNew("AudioChannel1", NumConfigs, 0, 1, 15, 0),
        DBAudioChannel2    = dbintNew("AudioChannel2", NumConfigs, 0, 2, 15, 0),
        DBAudioChannel3    = dbintNew("AudioChannel3", NumConfigs, 0, 3, 15, 0),
        DBMinFreeChip      = dbintNew("MinFreeChip", NumConfigs, 0, 64, 2048, 0),
        DBJoy1Type         = dbenumNew("Joy1Type", NumConfigs, 0, 1, dbjoy1type_defs),
        DBJoy1ButtonBTime  = dbintNew("Joy1ButtonBTime", NumConfigs, 0, 0, 9, 0),
        DBJoy1AutoFireRate = dbintNew("Joy1AutoFireRate", NumConfigs, 0, 0, 5, 0),
        DBJoy2Type         = dbenumNew("Joy2Type", NumConfigs, 0, 0, dbjoy2type_defs),
        DBJoy2ButtonBTime  = dbintNew("Joy2ButtonBTime", NumConfigs, 0, 0, 9, 0),
        DBJoy2AutoFireRate = dbintNew("Joy2AutoFireRate", NumConfigs, 0, 0, 5, 0),
        DBRomPath          = dbstringNew("RomPath", NumConfigs, 0, ""),
        DBSamplePath       = dbstringNew("SamplePath", NumConfigs, 0, ""),
#ifdef POWERUP
        DBAsyncPPC         = dbboolNew("AsyncPPC", NumConfigs, 0, 1),
#endif
      DBEND);

  NumConfigs--;

  if(argc > 1)
  {
    if(argv[1][0] != '-')
    {
      for(i = 0; Drivers[i]; i++)
      {
        if(!stricmp(Drivers[i]->name, argv[1]))
        {
          Config[CFG_DRIVER] = i;
          break;
        }
      }
    }
  
    if(Config[CFG_DRIVER] < 0)
    {
      if(argv[1][0] == '-')
      {
        if(argv[1][1] == '-')
          i = 2;
        else
          i = 1;
      }
      else
        i = 0;
      
      if(!stricmp(&argv[1][i], "list"))
      {
        printf("\n"APPNAME" includes these drivers:\n\n");
        for(i = 0;  Drivers[i]; i++)
        {
          if(strlen(Drivers[i]->name) < 8)
            printf("%s\t\t%s\n", Drivers[i]->name, Drivers[i]->description);
          else
            printf("%s\t%s\n", Drivers[i]->name, Drivers[i]->description);
        }
        printf("\nTotal drivers included: %4d\n", i);
        return(TRUE);
      }

      if((argv[1][0] == '?') || !stricmp(&argv[1][i], "help"))
      {
        printf("Usage: %s <driver> [options]\nOptions:\n", argv[0]);
        dbargPrintHelp(DBArg);
        return(TRUE);
      }

      if(DBArg && (argc > 2) && (Config[CFG_DRIVER] < 0))
        dbargParse(DBArg, argc-2, &argv[2], NumConfigs);

      if(!stricmp(&argv[1][i], "makedirs"))
      {
        MakeDirs();
        return(TRUE);
      }
    }
  }
  
  return(FALSE);
}

void FreeConfig(void)
{
  if(DBArg)
  {
    if(SectionNames)
    {
      if(DBFile)
      {
        dbfileSave(DBFile);
        Dispose(DBFile);
      }
    
      free(SectionNames);
    }

    Dispose(DBArg);
  }
}

void LoadConfig(int argc, char **argv)
{
  uint32_t i;

  if(DBArg)
  {
    SectionNames = (char **)malloc((NumConfigs + 1) * sizeof(char *));

    if(SectionNames)
    {
#ifdef MESS
      for(i = 0; i < NumConfigs; i++)
        SectionNames[i] = (char *) Drivers[i]->description;
    
      SectionNames[i] = NULL;

      DBFile  = dbfileNew("mess.cfg",
#else
      SectionNames[0] = "Bitmap game defaults";
      SectionNames[1] = "Vector game defaults";
    
      for(i = 2; i < NumConfigs; i++)
        SectionNames[i] = (char *) Drivers[i-2]->description;
    
      SectionNames[i] = NULL;
    
      DBFile  = dbfileNew("mame.cfg",
            dbsectionNew("MAME",
              DBShow,
            DBEND),
#endif
            dbsectionsNew(SectionNames,
#ifndef MESS
              DBUseDefaults,
#endif
              DBFound,
              DBScreenType,
              DBScreenMode,
              DBDirectMode,
              DBDirtyLines,
              DBDepth,
              DBAllow16Bit,
              DBFlipX,
              DBFlipY,
              DBAntialiasing,
              DBTranslucency,
              DBBeamWidth,
              DBVectorFlicker,
              DBAutoFrameSkip,
              DBFrameSkip,
              DBWidth,
              DBHeight,
              DBBuffering,
              DBRotation,
              DBSound,
              DBAudioChannel0,
              DBAudioChannel1,
              DBAudioChannel2,
              DBAudioChannel3,
              DBMinFreeChip,
              DBJoy1Type,
              DBJoy1ButtonBTime,
              DBJoy1AutoFireRate,
              DBJoy2Type,
              DBJoy2ButtonBTime,
              DBJoy2AutoFireRate,
              DBRomPath,
              DBSamplePath,
#ifdef POWERUP
              DBAsyncPPC,
#endif
            DBEND),
          DBEND);

      if((argc > 2) && (Config[CFG_DRIVER] >= 0))
      {
#ifdef MESS
        dbargParse(DBArg, argc-2, &argv[2], Config[CFG_DRIVER]);
#else
        dbargParse(DBArg, argc-2, &argv[2], Config[CFG_DRIVER]+2);
#endif
      }
    }
  }
}

void SetConfig(int index, LONG *cfg)
{
  if(DBArg)
  {
#ifdef MESS
    dbstringSet( DBImage,            0,      (char *) cfg[CFG_IMAGE]);
#else
    dbenumSet(   DBShow,             0,      cfg[CFG_SHOW]);
    dbboolSet(   DBUseDefaults,      index,  cfg[CFG_USEDEFAULTS]);
#endif
    dbenumSet(   DBScreenType,       index,  cfg[CFG_SCREENTYPE]);
    dbuintSet(   DBScreenMode,       index,  cfg[CFG_SCREENMODE]);
    dbenumSet(   DBDirectMode,       index,  cfg[CFG_DIRECTMODE]);
    dbboolSet(   DBDirtyLines,       index,  cfg[CFG_DIRTYLINES]);
    dbintSet(    DBDepth,            index,  cfg[CFG_DEPTH]);
    dbboolSet(   DBAllow16Bit,       index,  cfg[CFG_ALLOW16BIT]);
    dbboolSet(   DBFlipX,            index,  cfg[CFG_FLIPX]);
    dbboolSet(   DBFlipY,            index,  cfg[CFG_FLIPY]);
    dbboolSet(   DBAntialiasing,     index,  cfg[CFG_ANTIALIASING]);
    dbboolSet(   DBTranslucency,     index,  cfg[CFG_TRANSLUCENCY]);
    dbintSet(    DBBeamWidth,        index,  cfg[CFG_BEAMWIDTH]);
    dbintSet(    DBVectorFlicker,    index,  cfg[CFG_VECTORFLICKER]);
    dbboolSet(   DBAutoFrameSkip,    index,  cfg[CFG_AUTOFRAMESKIP]);
    dbintSet(    DBFrameSkip,        index,  cfg[CFG_FRAMESKIP]);
    dbintSet(    DBWidth,            index,  cfg[CFG_WIDTH]);
    dbintSet(    DBHeight,           index,  cfg[CFG_HEIGHT]);
    dbenumSet(   DBBuffering,        index,  cfg[CFG_BUFFERING]);
    dbenumSet(   DBRotation,         index,  cfg[CFG_ROTATION]);
    dbenumSet(   DBSound,            index,  cfg[CFG_SOUND]);
    dbintSet(    DBAudioChannel0,    index,  cfg[CFG_AUDIOCHANNEL0]);
    dbintSet(    DBAudioChannel1,    index,  cfg[CFG_AUDIOCHANNEL1]);
    dbintSet(    DBAudioChannel2,    index,  cfg[CFG_AUDIOCHANNEL2]);
    dbintSet(    DBAudioChannel3,    index,  cfg[CFG_AUDIOCHANNEL3]);
    dbintSet(    DBMinFreeChip,      index,  cfg[CFG_MINFREECHIP]);
    dbenumSet(   DBJoy1Type,         index,  cfg[CFG_JOY1TYPE]);
    dbintSet(    DBJoy1ButtonBTime,  index,  cfg[CFG_JOY1BUTTONBTIME]);
    dbintSet(    DBJoy1AutoFireRate, index,  cfg[CFG_JOY1AUTOFIRERATE]);
    dbenumSet(   DBJoy2Type,         index,  cfg[CFG_JOY2TYPE]);
    dbintSet(    DBJoy2ButtonBTime,  index,  cfg[CFG_JOY2BUTTONBTIME]);
    dbintSet(    DBJoy2AutoFireRate, index,  cfg[CFG_JOY2AUTOFIRERATE]);
    dbstringSet( DBRomPath,          index,  (char *) cfg[CFG_ROMPATH]);
    dbstringSet( DBSamplePath,       index,  (char *) cfg[CFG_SAMPLEPATH]);
#ifdef POWERUP
    dbboolSet(   DBAsyncPPC,         index,  cfg[CFG_ASYNCPPC]);
#endif
  }
}

void GetConfig(int index, LONG *cfg)
{
  if(DBArg)
  {
#ifdef MESS
    cfg[CFG_IMAGE]            = (LONG) dbstringGet( DBImage,      0);
#else
    cfg[CFG_SHOW]             = dbenumGet( DBShow,       0);
    cfg[CFG_USEDEFAULTS]      = dbboolGet( DBUseDefaults,      index);
#endif
    cfg[CFG_SCREENTYPE]       = dbenumGet( DBScreenType,       index);
    cfg[CFG_SCREENMODE]       = dbuintGet( DBScreenMode,       index);
    cfg[CFG_DIRECTMODE]       = dbenumGet( DBDirectMode,       index);
    cfg[CFG_DIRTYLINES]       = dbboolGet( DBDirtyLines,       index);
    cfg[CFG_DEPTH]            = dbintGet(  DBDepth,            index);
    cfg[CFG_ALLOW16BIT]       = dbboolGet( DBAllow16Bit,       index);
    cfg[CFG_FLIPX]            = dbboolGet( DBFlipX,            index);
    cfg[CFG_FLIPY]            = dbboolGet( DBFlipY,            index);
    cfg[CFG_ANTIALIASING]     = dbboolGet( DBAntialiasing,     index);
    cfg[CFG_TRANSLUCENCY]     = dbboolGet( DBTranslucency,     index);
    cfg[CFG_BEAMWIDTH]        = dbintGet(  DBBeamWidth,        index);
    cfg[CFG_VECTORFLICKER]    = dbintGet(  DBVectorFlicker,    index);
    cfg[CFG_AUTOFRAMESKIP]    = dbboolGet( DBAutoFrameSkip,    index);
    cfg[CFG_FRAMESKIP]        = dbintGet(  DBFrameSkip,        index);
    cfg[CFG_WIDTH]            = dbintGet(  DBWidth,            index);
    cfg[CFG_HEIGHT]           = dbintGet(  DBHeight,           index);
    cfg[CFG_BUFFERING]        = dbenumGet( DBBuffering,        index);
    cfg[CFG_ROTATION]         = dbenumGet( DBRotation,         index);
    cfg[CFG_SOUND]            = dbenumGet( DBSound,            index);
    cfg[CFG_AUDIOCHANNEL0]    = dbintGet(  DBAudioChannel0,    index);
    cfg[CFG_AUDIOCHANNEL1]    = dbintGet(  DBAudioChannel1,    index);
    cfg[CFG_AUDIOCHANNEL2]    = dbintGet(  DBAudioChannel2,    index);
    cfg[CFG_AUDIOCHANNEL3]    = dbintGet(  DBAudioChannel3,    index);
    cfg[CFG_MINFREECHIP]      = dbintGet(  DBMinFreeChip,      index);
    cfg[CFG_JOY1TYPE]         = dbenumGet( DBJoy1Type,         index);
    cfg[CFG_JOY1BUTTONBTIME]  = dbintGet(  DBJoy1ButtonBTime,  index);
    cfg[CFG_JOY1AUTOFIRERATE] = dbintGet(  DBJoy1AutoFireRate, index);
    cfg[CFG_JOY2TYPE]         = dbenumGet( DBJoy2Type,         index);
    cfg[CFG_JOY2BUTTONBTIME]  = dbintGet(  DBJoy2ButtonBTime,  index);
    cfg[CFG_JOY2AUTOFIRERATE] = dbintGet(  DBJoy2AutoFireRate, index);
#ifdef POWERUP
    cfg[CFG_ASYNCPPC]         = dbboolGet(DBAsyncPPC,index);
#endif
    cfg[CFG_ROMPATH]          = (LONG) dbstringGet( DBRomPath,      index);
    cfg[CFG_SAMPLEPATH]       = (LONG) dbstringGet( DBSamplePath,   index);
  }
  else
  {
#ifdef MESS
    cfg[CFG_IMAGE]            = (LONG) "";
#else
    cfg[CFG_SHOW]             = 1;
    cfg[CFG_USEDEFAULTS]      = FALSE;
#endif
    cfg[CFG_SCREENTYPE]       = CFGST_BEST;
    cfg[CFG_SCREENMODE]       = INVALID_ID;
    cfg[CFG_DIRECTMODE]       = CFGDM_OFF;
    cfg[CFG_DIRTYLINES]       = TRUE;
    cfg[CFG_DEPTH]            = 8;
    cfg[CFG_ALLOW16BIT]       = TRUE;
    cfg[CFG_FLIPX]            = FALSE;
    cfg[CFG_FLIPY]            = FALSE;
    cfg[CFG_ANTIALIASING]     = FALSE;
    cfg[CFG_TRANSLUCENCY]     = FALSE;
    cfg[CFG_BEAMWIDTH]        = 1;
    cfg[CFG_VECTORFLICKER]    = 0;
    cfg[CFG_AUTOFRAMESKIP]    = FALSE;
    cfg[CFG_FRAMESKIP]        = 0;
    cfg[CFG_WIDTH]            = 0;
    cfg[CFG_HEIGHT]           = 0;
    cfg[CFG_BUFFERING]        = CFGB_SINGLE;
    cfg[CFG_ROTATION]         = CFGR_NO;
    cfg[CFG_SOUND]            = CFGS_PAULA;
    cfg[CFG_AUDIOCHANNEL0]    = 0;
    cfg[CFG_AUDIOCHANNEL1]    = 1;
    cfg[CFG_AUDIOCHANNEL2]    = 2;
    cfg[CFG_AUDIOCHANNEL3]    = 3;
    cfg[CFG_MINFREECHIP]      = 64;
    cfg[CFG_JOY1TYPE]         = CFGJ1_JOYSTICK2;
    cfg[CFG_JOY1BUTTONBTIME]  = 0;
    cfg[CFG_JOY1AUTOFIRERATE] = 0;
    cfg[CFG_JOY2TYPE]         = CFGJ2_NO;
    cfg[CFG_JOY2BUTTONBTIME]  = 0;
    cfg[CFG_JOY2AUTOFIRERATE] = 0;
    cfg[CFG_ROMPATH]          = NULL;
    cfg[CFG_SAMPLEPATH]       = NULL;
#ifdef POWERUP
    cfg[CFG_ASYNCPPC]         = TRUE;
#endif
  }
}

#ifndef MESS
int GetUseDefaults(int index)
{
  if(DBArg)
    return(dbboolGet(DBUseDefaults, index));
  else
    return(TRUE);
}
#endif

void MakeDirs(void)
{
  const char *rom_path;
  BPTR  dir, old_dir, new_dir;
  LONG  i;

  dir     = NULL;
  old_dir = NULL;

  rom_path = GetRomPath(NumConfigs, 0);

  if(rom_path && strlen(rom_path))
  {
    dir = Lock((STRPTR) rom_path, ACCESS_READ);
    
    if(!dir)
      dir = CreateDir((STRPTR) rom_path);

    if(dir)
      old_dir = CurrentDir(dir);
  }

  for(i = 0;  Drivers[i]; i++)
  {
    new_dir = CreateDir((STRPTR)Drivers[i]->name);
    
    if(new_dir)
      UnLock(new_dir);
  }

  if(dir)
  {
    CurrentDir(old_dir);
    UnLock(dir);
  }
}

const char *GetRomPath(int index, int path_num)
{
  const char *rom_path;

  switch(path_num)
  {
    case 0:
      rom_path = dbstringGet(DBRomPath, index);
      
      break;

#ifndef MESS
    case 1:
//      if(Drivers[index]->drv->video_attributes & VIDEO_TYPE_VECTOR)
//        rom_path = dbstringGet(DBRomPath, 1);
//      else
        rom_path = dbstringGet(DBRomPath, 0);
      
      break;

    case 2:
//      if(Drivers[index]->drv->video_attributes & VIDEO_TYPE_VECTOR)
//        rom_path = dbstringGet(DBRomPath, 0);
//      else
        rom_path = dbstringGet(DBRomPath, 1);

      break;
#endif

    default:
      rom_path = NULL;
      
      break;
  }
  
  return(rom_path);
}

const char *GetSamplePath(int index, int path_num)
{
  const char *sample_path;

  switch(path_num)
  {
    case 0:
      sample_path = dbstringGet(DBSamplePath, index);
      
      break;

#ifndef MESS
    case 1:
//      if(Drivers[index]->drv->video_attributes & VIDEO_TYPE_VECTOR)
//        sample_path = dbstringGet(DBSamplePath, 1);
//      else
        sample_path = dbstringGet(DBSamplePath, 0);
      
      break;

    case 2:
//      if(Drivers[index]->drv->video_attributes & VIDEO_TYPE_VECTOR)
//        sample_path = dbstringGet(DBSamplePath, 0);
//      else
        sample_path = dbstringGet(DBSamplePath, 1);

      break;
#endif

    default:
      sample_path = NULL;
      
      break;
  }
  
  return(sample_path);
}

BOOL GetFound(int driver)
{
  if(dbboolGet(DBFound, driver))
    return(TRUE);

  return(FALSE);
}

void SetFound(int driver, BOOL found)
{
  dbboolSet(DBFound, driver, found);
}
