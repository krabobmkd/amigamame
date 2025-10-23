

#ifndef _GENREDEFS_H_
#define _GENREDEFS_H_


/** krb 2025: add enum for game genre */
typedef enum {
    egg_Unknown,

    egg_Platform,
    egg_Climbing,

    egg_ShootEmUp,
    egg_Shooter, // actually pang or some joystick gun games..(?)

    egg_BeatNUp, // cooperative
    egg_Fighter, // versus
    egg_Driving, // would have tag 3D
    egg_Motorcycle,
    egg_Flying,
    egg_LightGuns,
    egg_BallNPaddles,
    egg_Pinballs,
    egg_Maze,

    egg_Tabletop,
    egg_Puzzle,
    egg_CardBattle,
    egg_Mahjong,
    egg_Quizz,
    egg_ChiFouMi, // added for scud hammer

    egg_Casino,
    egg_HorseRacing,
    egg_PoolNDart,

    egg_sport_,
    egg_sport_Baseball,
    egg_sport_Basketball,
    egg_sport_Volleyball,
    egg_sport_Football,
    egg_sport_Soccer,
    egg_sport_Golf,
    egg_sport_Hockey,
    egg_sport_Rugby,
    egg_sport_Tennis,
    egg_sport_TrackNField,
    egg_sport_Boxing,
    egg_sport_Wrestling,

    egg_sport_Bowling,
    egg_sport_Skiing,
    egg_sport_Skate,
    egg_sport_Rythm,
    egg_sport_end = egg_sport_Rythm, // sport is ((>=egg_sport_)&& (<= egg_sport_end))
    egg_Fishing,

    egg_Compilation,
    egg_Miscellaneous,
    egg_Mature,
    egg_Demoscene,
    egg_NumberOfGenre // keep it last, do not use as a genre.
} eGGenre;

// krb 2025 also genre flags...

//  geometry styles ...
#define EGF_HOR 1
#define EGF_VER 2  // just to seek vertical egg_ShootEmUp.
#define EGF_P3D 4 // perspective 3D
#define EGF_I3D 8 // isometric 3D

#define EGF_Girly 16 // basically twinkle star and rodland.
#define EGF_Childish 32
#define EGF_Sexy 64 // would match egg_Mature ? -> not only.
#define EGF_Funny 128
#define EGF_UViolent 256 // all are violent, this is for blood and gore games.
#define EGF_Hentai 512 // because "monster maulers"

#define EGFB_Numberof 10 // not a value


#endif

