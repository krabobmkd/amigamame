/******************************************************************************

    mamedriv.c

    Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

    The list of all available drivers. Drivers have to be included here to be
    recognized by the executable.

    To save some typing, we use a hack here. This file is recursively #included
    twice, with different definitions of the DRIVER() macro. The first one
    declares external references to the drivers; the second one builds an array
    storing all the drivers.

******************************************************************************/

#include "driver.h"

#ifndef DRIVER_RECURSIVE

#define DRIVER_RECURSIVE

/* step 1: declare all external references */
#define DRIVER(NAME) extern game_driver driver_##NAME;
#include "mamedriv.c"

/* step 2: define the drivers[] array */
#undef DRIVER
#define DRIVER(NAME) &driver_##NAME,
const game_driver * const drivers[] =
{
#include "mamedriv.c"
	0	/* end of array */
};

#else	/* DRIVER_RECURSIVE */
#ifdef LINK_ALBA
	DRIVER( hanaroku ) /* 1988 "Alba" "Hanaroku" */
	DRIVER( rmhaihai ) /* 1985 "Alba" "Real Mahjong Haihai (Japan)" */
	DRIVER( rmhaihib ) /* 1985 "Alba" "Real Mahjong Haihai [BET] (Japan)" */
	DRIVER( rmhaijin ) /* 1986 "Alba" "Real Mahjong Haihai Jinji Idou Hen (Japan)" */
	DRIVER( rmhaisei ) /* 1986 "Visco" "Real Mahjong Haihai Seichouhen (Japan)" */
	DRIVER( themj ) /* 1987 "Visco" "The Mah-jong (Japan)" */
#endif
#ifdef LINK_ALLIEDL
	DRIVER( ace ) /* 1976 "Allied Leisure" "Ace" */
	DRIVER( clayshoo ) /* 1979 "Allied Leisure" "Clay Shoot" */
#endif
#ifdef LINK_ALPHA
	DRIVER( btlfield ) /* 1987 "[Alpha Denshi Co.] (SNK license)" "Battle Field (Japan)" */
	DRIVER( btlfildb ) /* 1987 "bootleg" "Battle Field (bootleg)" */
	DRIVER( bullfgtr ) /* 1984 "Alpha Denshi Co." "Bull Fighter" */
	DRIVER( bullfgts ) /* 1984 "Alpha Denshi Co. (Sega license)" "Bull Fighter (Sega)" */
	DRIVER( champbas ) /* 1983 "Sega" "Champion Baseball" */
	DRIVER( champbb2 ) /* 1983 "Sega" "Champion Baseball II (set 1)" */
	DRIVER( champbbj ) /* 1983 "Alpha Denshi Co." "Champion Baseball (Japan set 1)" */
	DRIVER( champbja ) /* 1983 "Alpha Denshi Co." "Champion Baseball (Japan set 2)" */
	DRIVER( equites ) /* 1984 "Alpha Denshi Co." "Equites" */
	DRIVER( equitess ) /* 1984 "Alpha Denshi Co. (Sega license)" "Equites (Sega)" */
	DRIVER( exctscc2 ) /* 1984 "Alpha Denshi Co." "Exciting Soccer II" */
	DRIVER( exctscca ) /* 1983 "Alpha Denshi Co." "Exciting Soccer (alternate music)" */
	DRIVER( exctsccb ) /* 1983 "bootleg" "Exciting Soccer (bootleg)" */
	DRIVER( exctsccr ) /* 1983 "Alpha Denshi Co." "Exciting Soccer" */
	DRIVER( gangwarb ) /* 1989 "bootleg" "Gang Wars (bootleg)" */
	DRIVER( gangwars ) /* 1989 "Alpha Denshi Co." "Gang Wars (US)" */
	DRIVER( goldmeda ) /* 1988 "SNK" "Gold Medalist (alt)" */
	DRIVER( goldmedl ) /* 1988 "SNK" "Gold Medalist" */
	DRIVER( hvoltage ) /* 1985 "Alpha Denshi Co." "High Voltage" */
	DRIVER( jongbou ) /* 1987 "SNK" "Mahjong Block Jongbou (Japan)" */
	DRIVER( kouyakyu ) /* 1985 "Alpha Denshi Co." "The Koukouyakyuh" */
	DRIVER( kyros ) /* 1987 "World Games Inc" "Kyros" */
	DRIVER( kyrosj ) /* 1986 "Alpha Denshi Co." "Kyros No Yakata (Japan)" */
	DRIVER( meijinsn ) /* 1986 "SNK Electronics corp." "Meijinsen" */
	DRIVER( paddlema ) /* 1988 "SNK" "Paddle Mania" */
	DRIVER( sbasebal ) /* 1989 "Alpha Denshi Co. (SNK of America license)" "Super Champion Baseball (US)" */
	DRIVER( shougi ) /* 198? "Alpha Denshi" "Shougi" */
	DRIVER( shougi2 ) /* 198? "Alpha Denshi" "Shougi 2" */
	DRIVER( skyadvnj ) /* 1989 "Alpha Denshi Co." "Sky Adventure (Japan)" */
	DRIVER( skyadvnt ) /* 1989 "Alpha Denshi Co." "Sky Adventure (World)" */
	DRIVER( skyadvnu ) /* 1989 "Alpha Denshi Co. (SNK of America license)" "Sky Adventure (US)" */
	DRIVER( skysoldr ) /* 1988 "[Alpha Denshi Co.] (SNK of America/Romstar license)" "Sky Soldiers (US)" */
	DRIVER( splndrbt ) /* 1985 "Alpha Denshi Co." "Splendor Blast" */
	DRIVER( sstingry ) /* 1986 "Alpha Denshi Co." "Super Stingray" */
	DRIVER( talbot ) /* 1982 "Volt Electronics (Alpha license)" "Talbot" */
	DRIVER( timesol1 ) /* 1987 "[Alpha Denshi Co.] (SNK/Romstar license)" "Time Soldiers (US Rev 1)" */
	DRIVER( timesold ) /* 1987 "[Alpha Denshi Co.] (SNK/Romstar license)" "Time Soldiers (US Rev 3)" */
	DRIVER( tnexspce ) /* 1989 "SNK" "The Next Space" */
#endif
#ifdef LINK_AMIGA
	DRIVER( ar_airh ) /* 1988 "Arcadia Systems" "SportTime Table Hockey (Arcadia */
	DRIVER( ar_bios ) /* 1988 "Arcadia Systems" "Arcadia System BIOS" */
	DRIVER( ar_bowl ) /* 1988 "Arcadia Systems" "SportTime Bowling (Arcadia */
	DRIVER( ar_dart ) /* 1987 "Arcadia Systems" "World Darts (Arcadia */
	DRIVER( ar_fast ) /* 1988 "Arcadia Systems" "Magic Johnson's Fast Break (Arcadia */
	DRIVER( ar_ldrb ) /* 1988 "Arcadia Systems" "Leader Board (Arcadia */
	DRIVER( ar_ldrba ) /* 1988 "Arcadia Systems" "Leader Board (Arcadia */
	DRIVER( ar_ninj ) /* 1987 "Arcadia Systems" "Ninja Mission (Arcadia */
	DRIVER( ar_rdwr ) /* 1988 "Arcadia Systems" "RoadWars (Arcadia */
	DRIVER( ar_sdwr ) /* 1988 "Arcadia Systems" "Sidewinder (Arcadia */
	DRIVER( ar_socc ) /* 1989 "Arcadia Systems" "World Trophy Soccer (Arcadia */
	DRIVER( ar_spot ) /* 1990 "Arcadia Systems" "Spot (Arcadia)" */
	DRIVER( ar_sprg ) /* 1987 "Arcadia Systems" "Space Ranger (Arcadia */
	DRIVER( ar_xeon ) /* 1988 "Arcadia Systems" "Xenon (Arcadia */
	DRIVER( upscope ) /* 1986 "Grand Products" "Up Scope" */
#endif
#ifdef LINK_ATARI
	DRIVER( 720 ) /* 1986 "Atari Games" "720 Degrees (rev 4)" */
	DRIVER( 720g ) /* 1986 "Atari Games" "720 Degrees (German */
	DRIVER( 720gr1 ) /* 1986 "Atari Games" "720 Degrees (German */
	DRIVER( 720r1 ) /* 1986 "Atari Games" "720 Degrees (rev 1)" */
	DRIVER( 720r2 ) /* 1986 "Atari Games" "720 Degrees (rev 2)" */
	DRIVER( 720r3 ) /* 1986 "Atari Games" "720 Degrees (rev 3)" */
	DRIVER( a51mxr3k ) /* 1998 "Atari Games" "Area 51 / Maximum Force Duo (R3000)" */
	DRIVER( abaseb ) /* 1979 "Atari" "Atari Baseball (set 1)" */
	DRIVER( abaseb2 ) /* 1979 "Atari" "Atari Baseball (set 2)" */
	DRIVER( alphaona ) /* 1983 "Atari" "Alpha One (prototype */
	DRIVER( alphaone ) /* 1983 "Atari" "Alpha One (prototype */
	DRIVER( apb ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 7)" */
	DRIVER( apb1 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 1)" */
	DRIVER( apb2 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 2)" */
	DRIVER( apb3 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 3)" */
	DRIVER( apb4 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 4)" */
	DRIVER( apb5 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 5)" */
	DRIVER( apb6 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 6)" */
	DRIVER( apbf ) /* 1987 "Atari Games" "APB - All Points Bulletin (French)" */
	DRIVER( apbg ) /* 1987 "Atari Games" "APB - All Points Bulletin (German)" */
	DRIVER( arcadecl ) /* 1992 "Atari Games" "Arcade Classics (prototype)" */
	DRIVER( area51 ) /* 1996 "Atari Games" "Area 51 (R3000)" */
	DRIVER( area51a ) /* 1995 "Atari Games" "Area 51 (Atari Games License)" */
	DRIVER( area51mx ) /* 1998 "Atari Games" "Area 51 / Maximum Force Duo v2.0" */
	DRIVER( area51t ) /* 1995 "Time Warner" "Area 51 (Time Warner License)" */
	DRIVER( astdelu1 ) /* 1980 "Atari" "Asteroids Deluxe (rev 1)" */
	DRIVER( astdelux ) /* 1980 "Atari" "Asteroids Deluxe (rev 2)" */
	DRIVER( asterock ) /* 1979 "Sidam" "Asterock" */
	DRIVER( asteroi1 ) /* 1979 "Atari" "Asteroids (rev 1)" */
	DRIVER( asteroib ) /* 1979 "bootleg" "Asteroids (bootleg on Lunar Lander hardware)" */
	DRIVER( asteroid ) /* 1979 "Atari" "Asteroids (rev 2)" */
	DRIVER( atarifb ) /* 1978 "Atari" "Atari Football (revision 2)" */
	DRIVER( atarifb1 ) /* 1978 "Atari" "Atari Football (revision 1)" */
	DRIVER( atarifb4 ) /* 1979 "Atari" "Atari Football (4 players)" */
	DRIVER( atetckt2 ) /* 1989 "Atari Games" "Tetris (Cocktail set 2)" */
	DRIVER( atetcktl ) /* 1989 "Atari Games" "Tetris (Cocktail set 1)" */
	DRIVER( atetris ) /* 1988 "Atari Games" "Tetris (set 1)" */
	DRIVER( atetrisa ) /* 1988 "Atari Games" "Tetris (set 2)" */
	DRIVER( atetrisb ) /* 1988 "bootleg" "Tetris (bootleg set 1)" */
	DRIVER( atetrsb2 ) /* 1988 "bootleg" "Tetris (bootleg set 2)" */
	DRIVER( avalnche ) /* 1978 "Atari" "Avalanche" */
	DRIVER( badlands ) /* 1989 "Atari Games" "Bad Lands" */
	DRIVER( batman ) /* 1991 "Atari Games" "Batman" */
	DRIVER( beathead ) /* 1993 "Atari Games" "BeatHead (prototype)" */
	DRIVER( blsthead ) /* 1987 "Atari Games" "Blasteroids (with heads)" */
	DRIVER( blstroi2 ) /* 1987 "Atari Games" "Blasteroids (rev 2)" */
	DRIVER( blstroi3 ) /* 1987 "Atari Games" "Blasteroids (rev 3)" */
	DRIVER( blstroid ) /* 1987 "Atari Games" "Blasteroids (rev 4)" */
	DRIVER( blstroig ) /* 1987 "Atari Games" "Blasteroids (German */
	DRIVER( boxer ) /* 1978 "Atari" "Boxer (prototype)" */
	DRIVER( bradley ) /* 1980 "Atari" "Bradley Trainer" */
	DRIVER( bsktball ) /* 1979 "Atari" "Basketball" */
	DRIVER( bullsdrt ) /* 1985 "Shinkai Inc. (Magic Eletronics Inc. licence)" "Bulls Eye Darts" */
	DRIVER( bwidow ) /* 1982 "Atari" "Black Widow" */
	DRIVER( bzone ) /* 1980 "Atari" "Battle Zone (set 1)" */
	DRIVER( bzone2 ) /* 1980 "Atari" "Battle Zone (set 2)" */
	DRIVER( bzonec ) /* 1980 "Atari" "Battle Zone (cocktail)" */
	DRIVER( canyon ) /* 1977 "Atari" "Canyon Bomber" */
	DRIVER( canyonp ) /* 1977 "Atari" "Canyon Bomber (prototype)" */
	DRIVER( cascade ) /* 1978 "Sidam" "Cascade" */
	DRIVER( caterplr ) /* 1980 "bootleg" "Caterpillar" */
	DRIVER( cball ) /* 1976 "Atari" "Cannonball (Atari */
	DRIVER( ccastle2 ) /* 1983 "Atari" "Crystal Castles (version 2)" */
	DRIVER( ccastle3 ) /* 1983 "Atari" "Crystal Castles (version 3)" */
	DRIVER( ccastles ) /* 1983 "Atari" "Crystal Castles (version 4)" */
	DRIVER( centipd2 ) /* 1980 "Atari" "Centipede (revision 2)" */
	DRIVER( centipdb ) /* 1980 "bootleg" "Centipede (bootleg)" */
	DRIVER( centiped ) /* 1980 "Atari" "Centipede (revision 3)" */
	DRIVER( centtime ) /* 1980 "Atari" "Centipede (1 player */
	DRIVER( cloak ) /* 1983 "Atari" "Cloak & Dagger (rev 5)" */
	DRIVER( cloakfr ) /* 1983 "Atari" "Cloak & Dagger (French)" */
	DRIVER( cloakgr ) /* 1983 "Atari" "Cloak & Dagger (German)" */
	DRIVER( cloaksp ) /* 1983 "Atari" "Cloak & Dagger (Spanish)" */
	DRIVER( cloud9 ) /* 1983 "Atari" "Cloud 9 (prototype)" */
	DRIVER( copsnrob ) /* 1976 "Atari" "Cops'n Robbers" */
	DRIVER( cspring1 ) /* 1986 "Atari Games" "Championship Sprint (German */
	DRIVER( csprins1 ) /* 1986 "Atari Games" "Championship Sprint (Spanish */
	DRIVER( csprint ) /* 1986 "Atari Games" "Championship Sprint (rev 3)" */
	DRIVER( csprint1 ) /* 1986 "Atari Games" "Championship Sprint (rev 1)" */
	DRIVER( csprint2 ) /* 1986 "Atari Games" "Championship Sprint (rev 2)" */
	DRIVER( csprintf ) /* 1986 "Atari Games" "Championship Sprint (French)" */
	DRIVER( csprintg ) /* 1986 "Atari Games" "Championship Sprint (German */
	DRIVER( csprints ) /* 1986 "Atari Games" "Championship Sprint (Spanish */
	DRIVER( cyberb21 ) /* 1989 "Atari Games" "Cyberball 2072 (2 player */
	DRIVER( cyberb22 ) /* 1989 "Atari Games" "Cyberball 2072 (2 player */
	DRIVER( cyberb23 ) /* 1989 "Atari Games" "Cyberball 2072 (2 player */
	DRIVER( cyberb2p ) /* 1989 "Atari Games" "Cyberball 2072 (2 player */
	DRIVER( cyberba2 ) /* 1988 "Atari Games" "Cyberball (rev 2)" */
	DRIVER( cyberbal ) /* 1988 "Atari Games" "Cyberball (rev 4)" */
	DRIVER( cyberbap ) /* 1988 "Atari Games" "Cyberball (prototype)" */
	DRIVER( cyberbt ) /* 1989 "Atari Games" "Tournament Cyberball 2072 (rev 2)" */
	DRIVER( cyberbt1 ) /* 1989 "Atari Games" "Tournament Cyberball 2072 (rev 1)" */
	DRIVER( destroyr ) /* 1977 "Atari" "Destroyer" */
	DRIVER( dominos ) /* 1977 "Atari" "Dominos" */
	DRIVER( dragrace ) /* 1977 "Atari" "Drag Race" */
	DRIVER( eprom ) /* 1989 "Atari Games" "Escape from the Planet of the Robot Monsters (set 1)" */
	DRIVER( eprom2 ) /* 1989 "Atari Games" "Escape from the Planet of the Robot Monsters (set 2)" */
	DRIVER( esb ) /* 1985 "Atari Games" "The Empire Strikes Back" */
	DRIVER( firetrk ) /* 1978 "Atari" "Fire Truck" */
	DRIVER( flyball ) /* 1976 "Atari" "Flyball" */
	DRIVER( foodf ) /* 1982 "Atari" "Food Fight (rev 3)" */
	DRIVER( foodf2 ) /* 1982 "Atari" "Food Fight (rev 2)" */
	DRIVER( foodfc ) /* 1982 "Atari" "Food Fight (cocktail)" */
	DRIVER( gaun22p1 ) /* 1986 "Atari Games" "Gauntlet II (2 Players */
	DRIVER( gaun22pg ) /* 1986 "Atari Games" "Gauntlet II (2 Players */
	DRIVER( gaun2pg1 ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gaun2pj2 ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gaun2pr3 ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gaunt2 ) /* 1986 "Atari Games" "Gauntlet II" */
	DRIVER( gaunt22p ) /* 1986 "Atari Games" "Gauntlet II (2 Players */
	DRIVER( gaunt2g ) /* 1986 "Atari Games" "Gauntlet II (German)" */
	DRIVER( gaunt2p ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gaunt2pg ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gaunt2pj ) /* 1985 "Atari Games" "Gauntlet (2 Players */
	DRIVER( gauntg ) /* 1985 "Atari Games" "Gauntlet (German */
	DRIVER( gauntgr3 ) /* 1985 "Atari Games" "Gauntlet (German */
	DRIVER( gauntgr6 ) /* 1985 "Atari Games" "Gauntlet (German */
	DRIVER( gauntgr8 ) /* 1985 "Atari Games" "Gauntlet (German */
	DRIVER( gauntj ) /* 1985 "Atari Games" "Gauntlet (Japanese */
	DRIVER( gauntj12 ) /* 1985 "Atari Games" "Gauntlet (Japanese */
	DRIVER( gauntlet ) /* 1985 "Atari Games" "Gauntlet (rev 14)" */
	DRIVER( gauntr1 ) /* 1985 "Atari Games" "Gauntlet (rev 1)" */
	DRIVER( gauntr2 ) /* 1985 "Atari Games" "Gauntlet (rev 2)" */
	DRIVER( gauntr4 ) /* 1985 "Atari Games" "Gauntlet (rev 4)" */
	DRIVER( gauntr5 ) /* 1985 "Atari Games" "Gauntlet (rev 5)" */
	DRIVER( gauntr7 ) /* 1985 "Atari Games" "Gauntlet (rev 7)" */
	DRIVER( gauntr9 ) /* 1985 "Atari Games" "Gauntlet (rev 9)" */
	DRIVER( gaunts ) /* 1985 "Atari Games" "Gauntlet (Spanish */
	DRIVER( gravitar ) /* 1982 "Atari" "Gravitar (version 3)" */
	DRIVER( gravitr2 ) /* 1982 "Atari" "Gravitar (version 2)" */
	DRIVER( gravp ) /* 1982 "Atari" "Gravitar (prototype)" */
	DRIVER( guardian ) /* 1992 "Atari Games" "Guardians of the 'Hood" */
	DRIVER( harddrb5 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrb6 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrc1 ) /* 1990 "Atari Games" "Hard Drivin' (compact */
	DRIVER( harddrcb ) /* 1990 "Atari Games" "Hard Drivin' (compact */
	DRIVER( harddrcg ) /* 1990 "Atari Games" "Hard Drivin' (compact */
	DRIVER( harddrg4 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddriv ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrj6 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrv1 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrv2 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrv3 ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrvb ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrvc ) /* 1990 "Atari Games" "Hard Drivin' (compact */
	DRIVER( harddrvg ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( harddrvj ) /* 1988 "Atari Games" "Hard Drivin' (cockpit */
	DRIVER( hdrivaip ) /* 1993 "Atari Games" "Hard Drivin's Airborne (prototype */
	DRIVER( hdrivair ) /* 1993 "Atari Games" "Hard Drivin's Airborne (prototype)" */
	DRIVER( hydra ) /* 1990 "Atari Games" "Hydra" */
	DRIVER( hydrap ) /* 1990 "Atari Games" "Hydra (prototype 5/14/90)" */
	DRIVER( hydrap2 ) /* 1990 "Atari Games" "Hydra (prototype 5/25/90)" */
	DRIVER( indytem2 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 2)" */
	DRIVER( indytem3 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 3)" */
	DRIVER( indytem4 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 4)" */
	DRIVER( indytemd ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (German)" */
	DRIVER( indytemp ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 1)" */
	DRIVER( irobot ) /* 1983 "Atari" "I */
	DRIVER( jedi ) /* 1984 "Atari" "Return of the Jedi" */
	DRIVER( klax ) /* 1989 "Atari Games" "Klax (set 1)" */
	DRIVER( klax2 ) /* 1989 "Atari Games" "Klax (set 2)" */
	DRIVER( klax3 ) /* 1989 "Atari Games" "Klax (set 3)" */
	DRIVER( klaxd ) /* 1989 "Atari Games" "Klax (Germany)" */
	DRIVER( klaxj ) /* 1989 "Atari Games" "Klax (Japan)" */
	DRIVER( klaxp1 ) /* 1989 "Atari Games" "Klax (prototype set 1)" */
	DRIVER( klaxp2 ) /* 1989 "Atari Games" "Klax (prototype set 2)" */
	DRIVER( liberat2 ) /* 1982 "Atari" "Liberator (set 2)" */
	DRIVER( liberatr ) /* 1982 "Atari" "Liberator (set 1)" */
	DRIVER( llander ) /* 1979 "Atari" "Lunar Lander (rev 2)" */
	DRIVER( llander1 ) /* 1979 "Atari" "Lunar Lander (rev 1)" */
	DRIVER( lunarba1 ) /* 1982 "Atari" "Lunar Battle (prototype */
	DRIVER( lunarbat ) /* 1982 "Atari" "Lunar Battle (prototype */
	DRIVER( magworm ) /* 1980 "bootleg" "Magic Worm (bootleg)" */
	DRIVER( marble ) /* 1984 "Atari Games" "Marble Madness (set 1)" */
	DRIVER( marble2 ) /* 1984 "Atari Games" "Marble Madness (set 2)" */
	DRIVER( marble3 ) /* 1984 "Atari Games" "Marble Madness (set 3)" */
	DRIVER( marble4 ) /* 1984 "Atari Games" "Marble Madness (set 4)" */
	DRIVER( maxf_102 ) /* 1996 "Atari Games" "Maximum Force v1.02" */
	DRIVER( maxforce ) /* 1996 "Atari Games" "Maximum Force v1.05" */
	DRIVER( meteorts ) /* 1979 "VGG" "Meteorites" */
	DRIVER( mgolf ) /* 1978 "Atari" "Atari Mini Golf (prototype)" */
	DRIVER( mhavoc ) /* 1983 "Atari" "Major Havoc (rev 3)" */
	DRIVER( mhavoc2 ) /* 1983 "Atari" "Major Havoc (rev 2)" */
	DRIVER( mhavocp ) /* 1983 "Atari" "Major Havoc (prototype)" */
	DRIVER( mhavocrv ) /* 1983 "hack" "Major Havoc (Return to Vax)" */
	DRIVER( milliped ) /* 1982 "Atari" "Millipede" */
	DRIVER( millpac ) /* 1980 "Valadon Automation" "Millpac" */
	DRIVER( missile ) /* 1980 "Atari" "Missile Command (set 1)" */
	DRIVER( missile2 ) /* 1980 "Atari" "Missile Command (set 2)" */
	DRIVER( montecar ) /* 1979 "Atari" "Monte Carlo" */
	DRIVER( nitedrvr ) /* 1976 "Atari" "Night Driver" */
	DRIVER( offtwalc ) /* 1991 "Atari Games" "Off the Wall (2-player cocktail)" */
	DRIVER( offtwall ) /* 1991 "Atari Games" "Off the Wall (2/3-player upright)" */
	DRIVER( orbit ) /* 1978 "Atari" "Orbit" */
	DRIVER( paperboy ) /* 1984 "Atari Games" "Paperboy (rev 3)" */
	DRIVER( paperbr1 ) /* 1984 "Atari Games" "Paperboy (rev 1)" */
	DRIVER( paperbr2 ) /* 1984 "Atari Games" "Paperboy (rev 2)" */
	DRIVER( peterpak ) /* 1984 "Atari Games" "Peter Pack-Rat" */
	DRIVER( pitfigh3 ) /* 1990 "Atari Games" "Pit Fighter (rev 3)" */
	DRIVER( pitfigh4 ) /* 1990 "Atari Games" "Pit Fighter (rev 4)" */
	DRIVER( pitfighb ) /* 1990 "Atari Games" "Pit Fighter (bootleg)" */
	DRIVER( pitfighj ) /* 1990 "Atari Games" "Pit Fighter (Japan */
	DRIVER( pitfight ) /* 1990 "Atari Games" "Pit Fighter (rev 5)" */
	DRIVER( poolshrk ) /* 1977 "Atari" "Poolshark" */
	DRIVER( primraga ) /* 1994 "Atari Games" "Primal Rage (version 2.0)" */
	DRIVER( primrage ) /* 1994 "Atari Games" "Primal Rage (version 2.3)" */
	DRIVER( quantum ) /* 1982 "Atari" "Quantum (rev 2)" */
	DRIVER( quantum1 ) /* 1982 "Atari" "Quantum (rev 1)" */
	DRIVER( quantump ) /* 1982 "Atari" "Quantum (prototype)" */
	DRIVER( qwak ) /* 1982 "Atari" "Qwak (prototype)" */
	DRIVER( racedcb4 ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedcg4 ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrb1 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrb4 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrc1 ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrc2 ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrc4 ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrcb ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrcg ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrg1 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrg4 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedriv ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrv1 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrv2 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrv3 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrv4 ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrvb ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( racedrvc ) /* 1990 "Atari Games" "Race Drivin' (compact */
	DRIVER( racedrvg ) /* 1990 "Atari Games" "Race Drivin' (cockpit */
	DRIVER( rampart ) /* 1990 "Atari Games" "Rampart (Trackball)" */
	DRIVER( rampartj ) /* 1990 "Atari Games" "Rampart (Japan */
	DRIVER( ramprt2p ) /* 1990 "Atari Games" "Rampart (Joystick)" */
	DRIVER( redbaron ) /* 1980 "Atari" "Red Baron" */
	DRIVER( relief ) /* 1992 "Atari Games" "Relief Pitcher (set 1)" */
	DRIVER( relief2 ) /* 1992 "Atari Games" "Relief Pitcher (set 2)" */
	DRIVER( roadblc1 ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblcg ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblg1 ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadblg2 ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadbls1 ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadbls2 ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadbls3 ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadblsc ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblsg ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadblst ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadriot ) /* 1991 "Atari Games" "Road Riot 4WD" */
	DRIVER( roadrun1 ) /* 1985 "Atari Games" "Road Runner (rev 1)" */
	DRIVER( roadrun2 ) /* 1985 "Atari Games" "Road Runner (rev 1+)" */
	DRIVER( roadrunn ) /* 1985 "Atari Games" "Road Runner (rev 2)" */
	DRIVER( runaway ) /* 1982 "Atari" "Runaway (prototype)" */
	DRIVER( sbrkout ) /* 1978 "Atari" "Super Breakout" */
	DRIVER( shuuz ) /* 1990 "Atari Games" "Shuuz (version 8.0)" */
	DRIVER( shuuz2 ) /* 1990 "Atari Games" "Shuuz (version 7.1)" */
	DRIVER( skullxb1 ) /* 1989 "Atari Games" "Skull & Crossbones (rev 1)" */
	DRIVER( skullxb2 ) /* 1989 "Atari Games" "Skull & Crossbones (rev 2)" */
	DRIVER( skullxb3 ) /* 1989 "Atari Games" "Skull & Crossbones (rev 3)" */
	DRIVER( skullxb4 ) /* 1989 "Atari Games" "Skull & Crossbones (rev 4)" */
	DRIVER( skullxbo ) /* 1989 "Atari Games" "Skull & Crossbones (rev 5)" */
	DRIVER( skydiver ) /* 1978 "Atari" "Sky Diver" */
	DRIVER( skyraid ) /* 1978 "Atari" "Sky Raider" */
	DRIVER( soccer ) /* 1980 "Atari" "Atari Soccer" */
	DRIVER( spacduel ) /* 1980 "Atari" "Space Duel" */
	DRIVER( sparkz ) /* 1992 "Atari Games" "Sparkz (prototype)" */
	DRIVER( spclordg ) /* 1992 "Atari Games" "Space Lords (rev A */
	DRIVER( spclords ) /* 1992 "Atari Games" "Space Lords (rev C)" */
	DRIVER( sprint1 ) /* 1978 "Atari" "Sprint 1" */
	DRIVER( sprint2 ) /* 1976 "Atari" "Sprint 2 (set 1)" */
	DRIVER( sprint2a ) /* 1976 "Atari" "Sprint 2 (set 2)" */
	DRIVER( sprint4 ) /* 1977 "Atari" "Sprint 4 (set 1)" */
	DRIVER( sprint4a ) /* 1977 "Atari" "Sprint 4 (set 2)" */
	DRIVER( sprint8 ) /* 1977 "Atari" "Sprint 8" */
	DRIVER( sprint8a ) /* 1977 "Atari" "Sprint 8 (play tag & chase)" */
	DRIVER( sprmatkd ) /* 1981 "Atari + Gencomp" "Super Missile Attack (not encrypted)" */
	DRIVER( sspring1 ) /* 1986 "Atari Games" "Super Sprint (German */
	DRIVER( ssprint ) /* 1986 "Atari Games" "Super Sprint (rev 4)" */
	DRIVER( ssprint1 ) /* 1986 "Atari Games" "Super Sprint (rev 1)" */
	DRIVER( ssprint3 ) /* 1986 "Atari Games" "Super Sprint (rev 3)" */
	DRIVER( ssprintf ) /* 1986 "Atari Games" "Super Sprint (French)" */
	DRIVER( ssprintg ) /* 1986 "Atari Games" "Super Sprint (German */
	DRIVER( ssprints ) /* 1986 "Atari Games" "Super Sprint (Spanish)" */
	DRIVER( starshp1 ) /* 1977 "Atari" "Starship 1" */
	DRIVER( starshpp ) /* 1977 "Atari" "Starship 1 (prototype?)" */
	DRIVER( starwar1 ) /* 1983 "Atari" "Star Wars (rev 1)" */
	DRIVER( starwars ) /* 1983 "Atari" "Star Wars (rev 2)" */
	DRIVER( steelta1 ) /* 1991 "Atari Games" "Steel Talons (rev 1)" */
	DRIVER( steeltag ) /* 1991 "Atari Games" "Steel Talons (German */
	DRIVER( steeltal ) /* 1991 "Atari Games" "Steel Talons (rev 2)" */
	DRIVER( strtdriv ) /* 1993 "Atari Games" "Street Drivin' (prototype)" */
	DRIVER( stunrn2e ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 2 */
	DRIVER( stunrn3e ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 3 */
	DRIVER( stunrun ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 6)" */
	DRIVER( stunrun0 ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 0)" */
	DRIVER( stunrun2 ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 2)" */
	DRIVER( stunrun3 ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 3)" */
	DRIVER( stunrun4 ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 4)" */
	DRIVER( stunrun5 ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 5)" */
	DRIVER( stunrune ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 5 */
	DRIVER( stunrunj ) /* 1989 "Atari Games" "S.T.U.N. Runner (rev 7 */
	DRIVER( stunrunp ) /* 1989 "Atari Games" "S.T.U.N. Runner (upright prototype)" */
	DRIVER( subs ) /* 1977 "Atari" "Subs" */
	DRIVER( superbug ) /* 1977 "Atari" "Super Bug" */
	DRIVER( suprmatk ) /* 1981 "Atari + Gencomp" "Super Missile Attack (for set 2)" */
	DRIVER( tank8 ) /* 1976 "Atari" "Tank 8 (set 1)" */
	DRIVER( tempest ) /* 1980 "Atari" "Tempest (rev 3)" */
	DRIVER( tempest1 ) /* 1980 "Atari" "Tempest (rev 1)" */
	DRIVER( tempest2 ) /* 1980 "Atari" "Tempest (rev 2)" */
	DRIVER( tempest3 ) /* 1980 "Atari" "Tempest (rev ?)" */
	DRIVER( temptube ) /* 1980 "hack" "Tempest Tubes" */
	DRIVER( thunderj ) /* 1990 "Atari Games" "ThunderJaws" */
	DRIVER( tmek ) /* 1994 "Atari Games" "T-MEK" */
	DRIVER( tmekprot ) /* 1994 "Atari Games" "T-MEK (prototype)" */
	DRIVER( toobin ) /* 1988 "Atari Games" "Toobin' (rev 3)" */
	DRIVER( toobin1 ) /* 1988 "Atari Games" "Toobin' (rev 1)" */
	DRIVER( toobin2 ) /* 1988 "Atari Games" "Toobin' (rev 2)" */
	DRIVER( toobin2e ) /* 1988 "Atari Games" "Toobin' (Europe */
	DRIVER( toobine ) /* 1988 "Atari Games" "Toobin' (Europe */
	DRIVER( toobing ) /* 1988 "Atari Games" "Toobin' (German */
	DRIVER( tourtab2 ) /* 1978 "Atari" "Tournament Table (set 2)" */
	DRIVER( tourtabl ) /* 1978 "Atari" "Tournament Table (set 1)" */
	DRIVER( triplhnt ) /* 1977 "Atari" "Triple Hunt" */
	DRIVER( tunhunt ) /* 1979 "Atari" "Tunnel Hunt" */
	DRIVER( tunhuntc ) /* 1981 "Atari (Centuri license)" "Tunnel Hunt (Centuri)" */
	DRIVER( ultratnk ) /* 1978 "Atari" "Ultra Tank" */
	DRIVER( vcircle ) /* 1996 "Atari Games" "Vicious Circle (prototype)" */
	DRIVER( videopin ) /* 1979 "Atari" "Video Pinball" */
	DRIVER( vindc2r1 ) /* 1988 "Atari Games" "Vindicators Part II (rev 1)" */
	DRIVER( vindc2r2 ) /* 1988 "Atari Games" "Vindicators Part II (rev 2)" */
	DRIVER( vindctr2 ) /* 1988 "Atari Games" "Vindicators Part II (rev 3)" */
	DRIVER( vindice3 ) /* 1988 "Atari Games" "Vindicators (Europe */
	DRIVER( vindice4 ) /* 1988 "Atari Games" "Vindicators (Europe */
	DRIVER( vindict1 ) /* 1988 "Atari Games" "Vindicators (rev 1)" */
	DRIVER( vindict2 ) /* 1988 "Atari Games" "Vindicators (rev 2)" */
	DRIVER( vindict4 ) /* 1988 "Atari Games" "Vindicators (rev 4)" */
	DRIVER( vindicte ) /* 1988 "Atari Games" "Vindicators (Europe */
	DRIVER( vindictg ) /* 1988 "Atari Games" "Vindicators (German */
	DRIVER( vindictr ) /* 1988 "Atari Games" "Vindicators (rev 5)" */
	DRIVER( warlords ) /* 1980 "Atari" "Warlords" */
	DRIVER( wolfpack ) /* 1978 "Atari" "Wolf Pack (prototype)" */
	DRIVER( xybots ) /* 1987 "Atari Games" "Xybots (rev 2)" */
	DRIVER( xybots0 ) /* 1987 "Atari Games" "Xybots (rev 0)" */
	DRIVER( xybots1 ) /* 1987 "Atari Games" "Xybots (rev 1)" */
	DRIVER( xybotsf ) /* 1987 "Atari Games" "Xybots (French */
	DRIVER( xybotsg ) /* 1987 "Atari Games" "Xybots (German */
#endif
#ifdef LINK_ATLUS
	DRIVER( blmbycar ) /* 1994 "ABM & Gecas" "Blomby Car" */
	DRIVER( blmbycau ) /* 1994 "ABM & Gecas" "Blomby Car (not encrypted)" */
	DRIVER( naname ) /* 1994 "Atlus" "Naname de Magic! (Japan)" */
	DRIVER( ohmygod ) /* 1993 "Atlus" "Oh My God! (Japan)" */
	DRIVER( powerina ) /* 1993 "Atlus" "Power Instinct (USA */
	DRIVER( powerinb ) /* 1993 "Atlus" "Power Instinct (USA */
	DRIVER( powerins ) /* 1993 "Atlus" "Gouketsuji Ichizoku (Japan)" */
	DRIVER( powernjb ) /* 1993 "Atlus" "Gouketsuji Ichizoku (Japan */
	DRIVER( watrball ) /* 1996 "ABM" "Water Balls" */
#endif
#ifdef LINK_BFM
	DRIVER( bctvidbs ) /* 199? "Barcrest" "MPU4 Video Firmware" */
	DRIVER( quintoon ) /* 1993 "BFM" "Quintoon (UK */
#endif
#ifdef LINK_CAPCOM
	DRIVER( 1941 ) /* 1990 "Capcom" "1941 - Counter Attack (World)" */
	DRIVER( 1941j ) /* 1990 "Capcom" "1941 - Counter Attack (Japan)" */
	DRIVER( 1942 ) /* 1984 "Capcom" "1942 (set 1)" */
	DRIVER( 1942a ) /* 1984 "Capcom" "1942 (set 2)" */
	DRIVER( 1942b ) /* 1984 "Capcom" "1942 (set 3)" */
	DRIVER( 1943 ) /* 1987 "Capcom" "1943: The Battle of Midway (US)" */
	DRIVER( 1943j ) /* 1987 "Capcom" "1943: Midway Kaisen (Japan)" */
	DRIVER( 1943kai ) /* 1987 "Capcom" "1943 Kai: Midway Kaisen (Japan)" */
	DRIVER( 1944 ) /* 2000 "Capcom supported by Eighting/Raizing" */
	DRIVER( 1944j ) /* 2000 "Capcom supported by Eighting/Raizing" */
	DRIVER( 19xx ) /* 1996 "Capcom" "19XX: The War Against Destiny (US 951207)" */
	DRIVER( 19xxa ) /* 1996 "Capcom" "19XX: The War Against Destiny (Asia 951207)" */
	DRIVER( 19xxh ) /* 1996 "Capcom" "19XX: The War Against Destiny (Hispanic 951218)" */
	DRIVER( 19xxjr1 ) /* 1996 "Capcom" "19XX: The War Against Destiny (Japan 951207)" */
	DRIVER( 3wonders ) /* 1991 "Capcom" "Three Wonders (World 910520)" */
	DRIVER( 3wonderu ) /* 1991 "Capcom" "Three Wonders (US 910520)" */
	DRIVER( 7toitsu ) /* 1988 "Yuga" "Chi-Toitsu" */
	DRIVER( acpsx ) /* 1995 "Acclaim" "Acclaim PSX" */
	DRIVER( area88 ) /* 1989 "Capcom" "Area 88 (Japan)" */
	DRIVER( armwar ) /* 1994 "Capcom" "Armored Warriors (Euro 941024)" */
	DRIVER( armwarr1 ) /* 1994 "Capcom" "Armored Warriors (Euro 941011)" */
	DRIVER( armwaru ) /* 1994 "Capcom" "Armored Warriors (US 941024)" */
	DRIVER( atluspsx ) /* 1996 "Sony/Atlus" "Atlus PSX" */
	DRIVER( atpsx ) /* 1996 "Atari" "Atari PSX" */
	DRIVER( avenger2 ) /* 1987 "Capcom" "Avengers (US set 2)" */
	DRIVER( avengers ) /* 1987 "Capcom" "Avengers (US set 1)" */
	DRIVER( avsp ) /* 1994 "Capcom" "Alien vs. Predator (Euro 940520)" */
	DRIVER( avspa ) /* 1994 "Capcom" "Alien vs. Predator (Asia 940520)" */
	DRIVER( avspj ) /* 1994 "Capcom" "Alien vs. Predator (Japan 940520)" */
	DRIVER( avspu ) /* 1994 "Capcom" "Alien vs. Predator (US 940520)" */
	DRIVER( batcir ) /* 1997 "Capcom" "Battle Circuit (Euro 970319)" */
	DRIVER( batcirj ) /* 1997 "Capcom" "Battle Circuit (Japan 970319)" */
	DRIVER( bbros ) /* 1989 "Capcom" "Buster Bros. (US)" */
	DRIVER( beastrzb ) /* 1997 "Eighting/Raizing" "Beastorizer (USA Bootleg)" */
	DRIVER( beastrzr ) /* 1997 "Eighting/Raizing" "Beastorizer (USA)" */
	DRIVER( bionicc ) /* 1987 "Capcom" "Bionic Commando (US set 1)" */
	DRIVER( bionicc2 ) /* 1987 "Capcom" "Bionic Commando (US set 2)" */
	DRIVER( bktigerb ) /* 1987 "bootleg" "Black Tiger (bootleg)" */
	DRIVER( bldyror2 ) /* 1998 "Eighting/Raizing" "Bloody Roar 2 (JAPAN)" */
	DRIVER( blkdrgnb ) /* 1987 "bootleg" "Black Dragon (bootleg)" */
	DRIVER( blkdrgon ) /* 1987 "Capcom" "Black Dragon" */
	DRIVER( blktiger ) /* 1987 "Capcom" "Black Tiger" */
	DRIVER( block ) /* 1991 "Capcom" "Block Block (World 910910)" */
	DRIVER( blockbl ) /* 1991 "bootleg" "Block Block (bootleg)" */
	DRIVER( blockj ) /* 1991 "Capcom" "Block Block (Japan 910910)" */
	DRIVER( blockjoy ) /* 1991 "Capcom" "Block Block (World 911106 Joystick)" */
	DRIVER( brvblade ) /* 2000 "Eighting/Raizing" "Brave Blade (JAPAN)" */
	DRIVER( buraiken ) /* 1987 "Capcom" "Hissatsu Buraiken (Japan)" */
	DRIVER( captcomj ) /* 1991 "Capcom" "Captain Commando (Japan 911202)" */
	DRIVER( captcomm ) /* 1991 "Capcom" "Captain Commando (World 911014)" */
	DRIVER( captcomu ) /* 1991 "Capcom" "Captain Commando (US 910928)" */
	DRIVER( cawing ) /* 1990 "Capcom" "Carrier Air Wing (World 901012)" */
	DRIVER( cawingj ) /* 1990 "Capcom" "U.S. Navy (Japan 901012)" */
	DRIVER( cawingr1 ) /* 1990 "Capcom" "Carrier Air Wing (World 901009)" */
	DRIVER( cawingu ) /* 1990 "Capcom" "Carrier Air Wing (US 901012)" */
	DRIVER( cbaj ) /* 1998 "Tecmo" "Cool Boarders Arcade Jam" */
	DRIVER( cbasebal ) /* 1989 "Capcom" "Capcom Baseball (Japan)" */
	DRIVER( chikij ) /* 1990 "Capcom" "Chiki Chiki Boys (Japan 900619)" */
	DRIVER( choko ) /* 2001 "Mitchell distributed by Capcom" */
	DRIVER( commandj ) /* 1985 "Capcom" "Senjou no Ookami" */
	DRIVER( commando ) /* 1985 "Capcom" "Commando (World)" */
	DRIVER( commandu ) /* 1985 "Capcom (Data East USA license)" "Commando (US)" */
	DRIVER( cpzn1 ) /* 1995 "Sony/Capcom" "ZN1" */
	DRIVER( cpzn2 ) /* 1997 "Sony/Capcom" "ZN2" */
	DRIVER( csclub ) /* 1997 "Capcom" "Capcom Sports Club (Euro 970722)" */
	DRIVER( cscluba ) /* 1997 "Capcom" "Capcom Sports Club (Asia 970722)" */
	DRIVER( csclubj ) /* 1997 "Capcom" "Capcom Sports Club (Japan 970722)" */
	DRIVER( cworld ) /* 1989 "Capcom" "Capcom World (Japan)" */
	DRIVER( cworld2j ) /* 1992 "Capcom" "Capcom World 2 (Japan 920611)" */
	DRIVER( cybots ) /* 1995 "Capcom" "Cyberbots: Fullmetal Madness (Euro 950424)" */
	DRIVER( cybotsj ) /* 1995 "Capcom" "Cyberbots: Fullmetal Madness (Japan 950420)" */
	DRIVER( cybotsu ) /* 1995 "Capcom" "Cyberbots: Fullmetal Madness (US 950424)" */
	DRIVER( daimakai ) /* 1988 "Capcom" "Dai Makai-Mura (Japan)" */
	DRIVER( ddsom ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (Euro 960619)" */
	DRIVER( ddsoma ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (Asia 960619)" */
	DRIVER( ddsomj ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (Japan 960619)" */
	DRIVER( ddsomjr1 ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (Japan 960206)" */
	DRIVER( ddsomr2 ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (Euro 960209)" */
	DRIVER( ddsomu ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (US 960619)" */
	DRIVER( ddsomur1 ) /* 1996 "Capcom" "Dungeons & Dragons: Shadow over Mystara (US 960209)" */
	DRIVER( ddtod ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (Euro 940412)" */
	DRIVER( ddtodh ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (Hispanic 940125)" */
	DRIVER( ddtodj ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (Japan 940125)" */
	DRIVER( ddtodjr1 ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (Japan 940113)" */
	DRIVER( ddtodu ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (US 940125)" */
	DRIVER( ddtodur1 ) /* 1993 "Capcom" "Dungeons & Dragons: Tower of Doom (US 940113)" */
	DRIVER( diamond ) /* 1989 "KH Video" "Diamond Run" */
	DRIVER( dimahoo ) /* 2000 "Eighting/Raizing distributed by Capcom" */
	DRIVER( dino ) /* 1993 "Capcom" "Cadillacs and Dinosaurs (World 930201)" */
	DRIVER( dinoj ) /* 1993 "Capcom" "Cadillacs Kyouryuu-Shinseiki (Japan 930201)" */
	DRIVER( dinou ) /* 1993 "Capcom" "Cadillacs and Dinosaurs (US 930201)" */
	DRIVER( doapp ) /* 1998 "Tecmo" "Dead Or Alive ++ (JAPAN)" */
	DRIVER( dokaben ) /* 1989 "Capcom" "Dokaben (Japan)" */
	DRIVER( dstlk ) /* 1994 "Capcom" "Darkstalkers: The Night Warriors (Euro 940705)" */
	DRIVER( dstlka ) /* 1994 "Capcom" "Darkstalkers: The Night Warriors (Asia 940705)" */
	DRIVER( dstlku ) /* 1994 "Capcom" "Darkstalkers: The Night Warriors (US 940818)" */
	DRIVER( dstlkur1 ) /* 1994 "Capcom" "Darkstalkers: The Night Warriors (US 940705)" */
	DRIVER( dyger ) /* 1989 "Philko" "Dyger (Korea set 1)" */
	DRIVER( dygera ) /* 1989 "Philko" "Dyger (Korea set 2)" */
	DRIVER( dynwar ) /* 1989 "Capcom" "Dynasty Wars (World)" */
	DRIVER( dynwarj ) /* 1989 "Capcom" "Tenchi wo Kurau (Japan)" */
	DRIVER( ecofghtr ) /* 1993 "Capcom" "Eco Fighters (World 931203)" */
	DRIVER( egghunt ) /* 1995 "Invi Image" "Egg Hunt" */
	DRIVER( exedexes ) /* 1985 "Capcom" "Exed Exes" */
	DRIVER( f1dream ) /* 1988 "Capcom (Romstar license)" "F-1 Dream" */
	DRIVER( f1dreamb ) /* 1988 "bootleg" "F-1 Dream (bootleg)" */
	DRIVER( fcrash ) /* 1990 "Playmark bootleg [Capcom]" */
	DRIVER( ffight ) /* 1989 "Capcom" "Final Fight (World)" */
	DRIVER( ffightj ) /* 1989 "Capcom" "Final Fight (Japan)" */
	DRIVER( ffightj1 ) /* 1989 "Capcom" "Final Fight (Japan 900305)" */
	DRIVER( ffightu ) /* 1989 "Capcom" "Final Fight (US 900112)" */
	DRIVER( ffightua ) /* 1989 "Capcom" "Final Fight (US 900613)" */
	DRIVER( flameguj ) /* 1999 "GAPS Inc." "Flame Gunner (Japan)" */
	DRIVER( flamegun ) /* 1999 "GAPS Inc." "Flame Gunner" */
	DRIVER( forgottn ) /* 1988 "Capcom" "Forgotten Worlds (US)" */
	DRIVER( gdarius ) /* 1997 "Taito" "G-Darius (Ver 2.01J)" */
	DRIVER( gdarius2 ) /* 1997 "Taito" "G-Darius Ver.2 (Ver 2.03J)" */
	DRIVER( gdariusb ) /* 1997 "Taito" "G-Darius (Ver 2.02A)" */
	DRIVER( ghouls ) /* 1988 "Capcom" "Ghouls'n Ghosts (World)" */
	DRIVER( ghoulsu ) /* 1988 "Capcom" "Ghouls'n Ghosts (US)" */
	DRIVER( gigawing ) /* 1999 "Capcom supported by Takumi" */
	DRIVER( glpracr ) /* 1996 "Tecmo" "Gallop Racer (JAPAN Ver 9.01.12)" */
	DRIVER( glpracr3 ) /* 1999 "Tecmo" "Gallop Racer 3 (JAPAN)" */
	DRIVER( gmahou ) /* 2000 "Eighting/Raizing distributed by Capcom" */
	DRIVER( gng ) /* 1985 "Capcom" "Ghosts'n Goblins (World? set 1)" */
	DRIVER( gnga ) /* 1985 "Capcom" "Ghosts'n Goblins (World? set 2)" */
	DRIVER( gngt ) /* 1985 "Capcom (Taito America license)" "Ghosts'n Goblins (US)" */
	DRIVER( gunsmoka ) /* 1986 "Capcom (Romstar license)" "Gun.Smoke (US set 2)" */
	DRIVER( gunsmoke ) /* 1985 "Capcom" "Gun.Smoke (World)" */
	DRIVER( gunsmokj ) /* 1985 "Capcom" "Gun.Smoke (Japan)" */
	DRIVER( gunsmoku ) /* 1985 "Capcom (Romstar license)" "Gun.Smoke (US set 1)" */
	DRIVER( gwinga ) /* 1999 "Capcom supported by Takumi" */
	DRIVER( gwingj ) /* 1999 "Capcom supported by Takumi" */
	DRIVER( hatena ) /* 1990 "Capcom" "Adventure Quiz 2 Hatena Hatena no Dai-Bouken (Japan 900228)" */
	DRIVER( higemaru ) /* 1984 "Capcom" "Pirate Ship Higemaru" */
	DRIVER( hvnsgate ) /* 1996 "Atlus/RACDYM" "Heaven's Gate" */
	DRIVER( jgakuen ) /* 1997 "Capcom" "Justice Gakuen (JAPAN 971117)" */
	DRIVER( kikaioh ) /* 1998 "Capcom" "Kikaioh (JAPAN 980914)" */
	DRIVER( knights ) /* 1991 "Capcom" "Knights of the Round (World 911127)" */
	DRIVER( knightsj ) /* 1991 "Capcom" "Knights of the Round (Japan 911127)" */
	DRIVER( knightsu ) /* 1991 "Capcom" "Knights of the Round (US 911127)" */
	DRIVER( kod ) /* 1991 "Capcom" "The King of Dragons (World 910711)" */
	DRIVER( kodj ) /* 1991 "Capcom" "The King of Dragons (Japan 910805)" */
	DRIVER( kodu ) /* 1991 "Capcom" "The King of Dragons (US 910910)" */
	DRIVER( lastduel ) /* 1988 "Capcom" "Last Duel (US set 1)" */
	DRIVER( ledstorm ) /* 1988 "Capcom" "Led Storm (US)" */
	DRIVER( lostwrld ) /* 1988 "Capcom" "Lost Worlds (Japan)" */
	DRIVER( lstduela ) /* 1988 "Capcom" "Last Duel (US set 2)" */
	DRIVER( lstduelb ) /* 1988 "bootleg" "Last Duel (bootleg)" */
	DRIVER( lwings ) /* 1986 "Capcom" "Legendary Wings (US set 1)" */
	DRIVER( lwings2 ) /* 1986 "Capcom" "Legendary Wings (US set 2)" */
	DRIVER( lwingsjp ) /* 1986 "Capcom" "Ares no Tsubasa (Japan)" */
	DRIVER( madgear ) /* 1989 "Capcom" "Mad Gear (US)" */
	DRIVER( madgearj ) /* 1989 "Capcom" "Mad Gear (Japan)" */
	DRIVER( makaimuc ) /* 1985 "Capcom" "Makai-Mura (Japan Revision C)" */
	DRIVER( makaimug ) /* 1985 "Capcom" "Makai-Mura (Japan Revision G)" */
	DRIVER( makaimur ) /* 1985 "Capcom" "Makai-Mura (Japan)" */
	DRIVER( marukin ) /* 1990 "Yuga" "Super Marukin-Ban (Japan 901017)" */
	DRIVER( mbomberj ) /* 1993 "Capcom" "Muscle Bomber - The Body Explosion (Japan 930713)" */
	DRIVER( mbombrd ) /* 1993 "Capcom" "Muscle Bomber Duo - Ultimate Team Battle (World 931206)" */
	DRIVER( mbombrdj ) /* 1993 "Capcom" "Muscle Bomber Duo - Heat Up Warriors (Japan 931206)" */
	DRIVER( megaman ) /* 1995 "Capcom" "Mega Man - The Power Battle (CPS1 Asia 951006)" */
	DRIVER( megaman2 ) /* 1996 "Capcom" "Mega Man 2: The Power Fighters (US 960708)" */
	DRIVER( mercs ) /* 1990 "Capcom" "Mercs (World 900302)" */
	DRIVER( mercsj ) /* 1990 "Capcom" "Senjou no Ookami II (Japan 900302)" */
	DRIVER( mercsu ) /* 1990 "Capcom" "Mercs (US 900302)" */
	DRIVER( mercsua ) /* 1990 "Capcom" "Mercs (US 900608)" */
	DRIVER( mfjump ) /* 2001 "Tecmo" "Monster Farm Jump (JAPAN)" */
	DRIVER( mgakuen ) /* 1988 "Yuga" "Mahjong Gakuen" */
	DRIVER( mgakuen2 ) /* 1989 "Face" "Mahjong Gakuen 2 Gakuen-chou no Fukushuu" */
	DRIVER( mgcldtex ) /* 1997 "Taito" "Magical Date EX / Magical Date - sotsugyou kokuhaku daisakusen (Ver 2.01J)" */
	DRIVER( mmatrix ) /* 2000 "Capcom supported by Takumi" */
	DRIVER( mmatrixj ) /* 2000 "Capcom supported by Takumi" */
	DRIVER( mpangj ) /* 2000 "Mitchell distributed by Capcom" */
	DRIVER( msh ) /* 1995 "Capcom" "Marvel Super Heroes (Euro 951024)" */
	DRIVER( msha ) /* 1995 "Capcom" "Marvel Super Heroes (Asia 951024)" */
	DRIVER( mshh ) /* 1995 "Capcom" "Marvel Super Heroes (Hispanic 951117)" */
	DRIVER( mshjr1 ) /* 1995 "Capcom" "Marvel Super Heroes (Japan 951024)" */
	DRIVER( mshu ) /* 1995 "Capcom" "Marvel Super Heroes (US 951024)" */
	DRIVER( mshvsf ) /* 1997 "Capcom" "Marvel Super Heroes Vs. Street Fighter (Euro 970625)" */
	DRIVER( mshvsfj ) /* 1997 "Capcom" "Marvel Super Heroes Vs. Street Fighter (Japan 970707)" */
	DRIVER( mshvsfj1 ) /* 1997 "Capcom" "Marvel Super Heroes Vs. Street Fighter (Japan 970702)" */
	DRIVER( mshvsfu ) /* 1997 "Capcom" "Marvel Super Heroes Vs. Street Fighter (US 970827)" */
	DRIVER( mshvsfu1 ) /* 1997 "Capcom" "Marvel Super Heroes Vs. Street Fighter (US 970625)" */
	DRIVER( mstworld ) /* 1994 "TCH" "Monsters World" */
	DRIVER( msword ) /* 1990 "Capcom" "Magic Sword - Heroic Fantasy (World 900725)" */
	DRIVER( mswordj ) /* 1990 "Capcom" "Magic Sword (Japan 900623)" */
	DRIVER( mswordr1 ) /* 1990 "Capcom" "Magic Sword - Heroic Fantasy (World 900623)" */
	DRIVER( mswordu ) /* 1990 "Capcom" "Magic Sword - Heroic Fantasy (US 900725)" */
	DRIVER( mtwins ) /* 1990 "Capcom" "Mega Twins (World 900619)" */
	DRIVER( mvsc ) /* 1998 "Capcom" "Marvel Vs. Capcom: Clash of Super Heroes (Euro 980112)" */
	DRIVER( mvscar1 ) /* 1998 "Capcom" "Marvel Vs. Capcom: Clash of Super Heroes (Asia 980112)" */
	DRIVER( mvscj ) /* 1998 "Capcom" "Marvel Vs. Capcom: Clash of Super Heroes (Japan 980123)" */
	DRIVER( mvscjr1 ) /* 1998 "Capcom" "Marvel Vs. Capcom: Clash of Super Heroes (Japan 980112)" */
	DRIVER( mvscu ) /* 1998 "Capcom" "Marvel Vs. Capcom: Clash of Super Heroes (US 980123)" */
	DRIVER( nemo ) /* 1990 "Capcom" "Nemo (World 901130)" */
	DRIVER( nemoj ) /* 1990 "Capcom" "Nemo (Japan 901120)" */
	DRIVER( nwarr ) /* 1995 "Capcom" "Night Warriors: Darkstalkers' Revenge (US 950406)" */
	DRIVER( pang ) /* 1989 "Mitchell" "Pang (World)" */
	DRIVER( pang3 ) /* 1995 "Mitchell" "Pang! 3 (Euro 950511)" */
	DRIVER( pang3j ) /* 1995 "Mitchell" "Pang! 3 (Japan 950511)" */
	DRIVER( pangb ) /* 1989 "bootleg" "Pang (bootleg)" */
	DRIVER( pfghtj ) /* 1997 "Capcom" "Pocket Fighter (Japan 970904)" */
	DRIVER( pgear ) /* 1994 "Capcom" "Powered Gear: Strategic Variant Armor Equipment (Japan 941024)" */
	DRIVER( pgearr1 ) /* 1994 "Capcom" "Powered Gear: Strategic Variant Armor Equipment (Japan 940916)" */
	DRIVER( pkladiel ) /* 1989 "Leprechaun" "Poker Ladies (Leprechaun ver. 510)" */
	DRIVER( pkladies ) /* 1989 "Mitchell" "Poker Ladies" */
	DRIVER( pkladila ) /* 1989 "Leprechaun" "Poker Ladies (Leprechaun ver. 401)" */
	DRIVER( plsmaswd ) /* 1998 "Capcom" "Plasma Sword (USA 980316)" */
	DRIVER( plsmswda ) /* 1998 "Capcom" "Plasma Sword (ASIA 980316)" */
	DRIVER( pnickj ) /* 1994 "Compile (Capcom license)" "Pnickies (Japan 940608)" */
	DRIVER( pompingw ) /* 1989 "Mitchell" "Pomping World (Japan)" */
	DRIVER( progear ) /* 2001 "Capcom supported by Cave" */
	DRIVER( progeara ) /* 2001 "Capcom supported by Cave" */
	DRIVER( progearj ) /* 2001 "Capcom supported by Cave" */
	DRIVER( psarc95 ) /* 1997 "Sony/Eighting/Raizing" "PS Arcade 95" */
	DRIVER( psyforce ) /* 1995 "Taito" "Psychic Force (Ver 2.4O)" */
	DRIVER( psyforcj ) /* 1995 "Taito" "Psychic Force (Ver 2.4J)" */
	DRIVER( psyfrcex ) /* 1995 "Taito" "Psychic Force EX (Ver 2.0J)" */
	DRIVER( punisher ) /* 1993 "Capcom" "The Punisher (World 930422)" */
	DRIVER( punishrj ) /* 1993 "Capcom" "The Punisher (Japan 930422)" */
	DRIVER( punishru ) /* 1993 "Capcom" "The Punisher (US 930422)" */
	DRIVER( pzloop2j ) /* 2001 "Mitchell distributed by Capcom" */
	DRIVER( qad ) /* 1992 "Capcom" "Quiz & Dragons (US 920701)" */
	DRIVER( qadj ) /* 1994 "Capcom" "Quiz & Dragons (Japan 940921)" */
	DRIVER( qndream ) /* 1996 "Capcom" "Quiz Nanairo Dreams: Nijiirochou no Kiseki (Japan 960826)" */
	DRIVER( qsangoku ) /* 1991 "Capcom" "Quiz Sangokushi (Japan)" */
	DRIVER( qtono1 ) /* 1991 "Capcom" "Quiz Tonosama no Yabou (Japan)" */
	DRIVER( qtono2 ) /* 1995 "Capcom" "Quiz Tonosama no Yabou 2 Zenkoku-ban (Japan 950123)" */
	DRIVER( raystorj ) /* 1996 "Taito" "Ray Storm (Ver 2.05J)" */
	DRIVER( raystorm ) /* 1996 "Taito" "Ray Storm (Ver 2.06A)" */
	DRIVER( rckman2j ) /* 1996 "Capcom" "Rockman 2: The Power Fighters (Japan 960708)" */
	DRIVER( ringdest ) /* 1994 "Capcom" "Ring of Destruction: Slammasters II (Euro 940902)" */
	DRIVER( rockmanj ) /* 1995 "Capcom" "Rockman - The Power Battle (CPS1 Japan 950922)" */
	DRIVER( rushcrsh ) /* 1986 "Capcom" "Rush & Crash (Japan)" */
	DRIVER( rvschola ) /* 1997 "Capcom" "Rival Schools (ASIA 971117)" */
	DRIVER( rvschool ) /* 1997 "Capcom" "Rival Schools (USA 971117)" */
	DRIVER( savgbees ) /* 1985 "Capcom (Memetron license)" "Savage Bees" */
	DRIVER( sbbros ) /* 1990 "Mitchell + Capcom" "Super Buster Bros. (US 901001)" */
	DRIVER( sctionza ) /* 1985 "Capcom" "Section Z (set 2)" */
	DRIVER( sectionz ) /* 1985 "Capcom" "Section Z (set 1)" */
	DRIVER( sf ) /* 1987 "Capcom" "Street Fighter (World)" */
	DRIVER( sf2 ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (World 910522)" */
	DRIVER( sf2accp2 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (Accelerator Pt.II)" */
	DRIVER( sf2ce ) /* 1992 "Capcom" "Street Fighter II' - Champion Edition (World 920313)" */
	DRIVER( sf2cej ) /* 1992 "Capcom" "Street Fighter II' - Champion Edition (Japan 920513)" */
	DRIVER( sf2ceua ) /* 1992 "Capcom" "Street Fighter II' - Champion Edition (US 920313)" */
	DRIVER( sf2ceub ) /* 1992 "Capcom" "Street Fighter II' - Champion Edition (US 920513)" */
	DRIVER( sf2ceuc ) /* 1992 "Capcom" "Street Fighter II' - Champion Edition (US 920803)" */
	DRIVER( sf2eb ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (World 910214)" */
	DRIVER( sf2hf ) /* 1992 "Capcom" "Street Fighter II' - Hyper Fighting (World 921209)" */
	DRIVER( sf2j ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (Japan 911210)" */
	DRIVER( sf2ja ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (Japan 910214)" */
	DRIVER( sf2jc ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (Japan 910306)" */
	DRIVER( sf2koryu ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (Kouryu)" */
	DRIVER( sf2m4 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (M4)" */
	DRIVER( sf2m5 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (M5)" */
	DRIVER( sf2m6 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (M6)" */
	DRIVER( sf2m7 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (M7)" */
	DRIVER( sf2rb ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (Rainbow set 1)" */
	DRIVER( sf2rb2 ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (Rainbow set 2)" */
	DRIVER( sf2red ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (Red Wave)" */
	DRIVER( sf2t ) /* 1992 "Capcom" "Street Fighter II' - Hyper Fighting (US 921209)" */
	DRIVER( sf2tj ) /* 1992 "Capcom" "Street Fighter II' Turbo - Hyper Fighting (Japan 921209)" */
	DRIVER( sf2ua ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910206)" */
	DRIVER( sf2ub ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910214)" */
	DRIVER( sf2ud ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910318)" */
	DRIVER( sf2ue ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910228)" */
	DRIVER( sf2uf ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910411)" */
	DRIVER( sf2ui ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 910522)" */
	DRIVER( sf2uk ) /* 1991 "Capcom" "Street Fighter II - The World Warrior (US 911101)" */
	DRIVER( sf2v004 ) /* 1992 "bootleg" "Street Fighter II! - Champion Edition (V004)" */
	DRIVER( sf2yyc ) /* 1992 "bootleg" "Street Fighter II' - Champion Edition (YYC)" */
	DRIVER( sfa ) /* 1995 "Capcom" "Street Fighter Alpha: Warriors' Dreams (Euro 950727)" */
	DRIVER( sfa2 ) /* 1996 "Capcom" "Street Fighter Alpha 2 (US 960306)" */
	DRIVER( sfa3 ) /* 1998 "Capcom" "Street Fighter Alpha 3 (US 980904)" */
	DRIVER( sfa3r1 ) /* 1998 "Capcom" "Street Fighter Alpha 3 (US 980629)" */
	DRIVER( sfar1 ) /* 1995 "Capcom" "Street Fighter Alpha: Warriors' Dreams (Euro 950718)" */
	DRIVER( sfar3 ) /* 1995 "Capcom" "Street Fighter Alpha: Warriors' Dreams (Euro 950605)" */
	DRIVER( sfau ) /* 1995 "Capcom" "Street Fighter Alpha: Warriors' Dreams (US 950627)" */
	DRIVER( sfex ) /* 1996 "Capcom/Arika" "Street Fighter EX (USA 961219)" */
	DRIVER( sfex2 ) /* 1998 "Capcom/Arika" "Street Fighter EX 2 (USA 980526)" */
	DRIVER( sfex2a ) /* 1998 "Capcom/Arika" "Street Fighter EX 2 (ASIA 980312)" */
	DRIVER( sfex2j ) /* 1998 "Capcom/Arika" "Street Fighter EX 2 (JAPAN 980312)" */
	DRIVER( sfex2p ) /* 1999 "Capcom/Arika" "Street Fighter EX 2 Plus (USA 990611)" */
	DRIVER( sfex2pa ) /* 1999 "Capcom/Arika" "Street Fighter EX 2 Plus (ASIA 990611)" */
	DRIVER( sfex2pj ) /* 1999 "Capcom/Arika" "Street Fighter EX 2 Plus (JAPAN 990611)" */
	DRIVER( sfexa ) /* 1996 "Capcom/Arika" "Street Fighter EX (ASIA 961219)" */
	DRIVER( sfexj ) /* 1996 "Capcom/Arika" "Street Fighter EX (JAPAN 961130)" */
	DRIVER( sfexp ) /* 1997 "Capcom/Arika" "Street Fighter EX Plus (USA 970407)" */
	DRIVER( sfexpj ) /* 1997 "Capcom/Arika" "Street Fighter EX Plus (JAPAN 970311)" */
	DRIVER( sfexpu1 ) /* 1997 "Capcom/Arika" "Street Fighter EX Plus (USA 970311)" */
	DRIVER( sfjp ) /* 1987 "Capcom" "Street Fighter (Japan)" */
	DRIVER( sfp ) /* 1987 "Capcom" "Street Fighter (prototype)" */
	DRIVER( sfus ) /* 1987 "Capcom" "Street Fighter (US)" */
	DRIVER( sfz2a ) /* 1996 "Capcom" "Street Fighter Zero 2 (Asia 960227)" */
	DRIVER( sfz2aa ) /* 1996 "Capcom" "Street Fighter Zero 2 Alpha (Asia 960826)" */
	DRIVER( sfz2aj ) /* 1996 "Capcom" "Street Fighter Zero 2 Alpha (Japan 960805)" */
	DRIVER( sfz2j ) /* 1996 "Capcom" "Street Fighter Zero 2 (Japan 960227)" */
	DRIVER( sfz3ar1 ) /* 1998 "Capcom" "Street Fighter Zero 3 (Asia 980701)" */
	DRIVER( sfz3j ) /* 1998 "Capcom" "Street Fighter Zero 3 (Japan 980904)" */
	DRIVER( sfz3jr1 ) /* 1998 "Capcom" "Street Fighter Zero 3 (Japan 980727)" */
	DRIVER( sfz3jr2 ) /* 1998 "Capcom" "Street Fighter Zero 3 (Japan 980629)" */
	DRIVER( sfzj ) /* 1995 "Capcom" "Street Fighter Zero (Japan 950727)" */
	DRIVER( sfzjr1 ) /* 1995 "Capcom" "Street Fighter Zero (Japan 950627)" */
	DRIVER( sfzjr2 ) /* 1995 "Capcom" "Street Fighter Zero (Japan 950605)" */
	DRIVER( sgemf ) /* 1997 "Capcom" "Super Gem Fighter Mini Mix (US 970904)" */
	DRIVER( sgemfa ) /* 1997 "Capcom" "Super Gem Fighter: Mini Mix (Asia 970904)" */
	DRIVER( shiryu2 ) /* 1999 "Capcom" "Strider Hiryu 2 (JAPAN 991213)" */
	DRIVER( shngmtkb ) /* 1998 "Sunsoft / Activision" "Shanghai Matekibuyuu" */
	DRIVER( sidearjp ) /* 1986 "Capcom" "Side Arms - Hyper Dyne (Japan)" */
	DRIVER( sidearmr ) /* 1986 "Capcom (Romstar license)" "Side Arms - Hyper Dyne (US)" */
	DRIVER( sidearms ) /* 1986 "Capcom" "Side Arms - Hyper Dyne (World)" */
	DRIVER( sinvasn ) /* 1985 "Capcom" "Space Invasion (Europe)" */
	DRIVER( sinvasnb ) /* 1985 "bootleg" "Space Invasion (bootleg)" */
	DRIVER( slammast ) /* 1993 "Capcom" "Saturday Night Slam Masters (World 930713)" */
	DRIVER( slammasu ) /* 1993 "Capcom" "Saturday Night Slam Masters (US 930713)" */
	DRIVER( smbomb ) /* 1994 "Capcom" "Super Muscle Bomber: The International Blowout (Japan 940831)" */
	DRIVER( smbombr1 ) /* 1994 "Capcom" "Super Muscle Bomber: The International Blowout (Japan 940808)" */
	DRIVER( sncwgltd ) /* 1996 "Video System" "Sonic Wings Limited (JAPAN)" */
	DRIVER( sonson ) /* 1984 "Capcom" "Son Son" */
	DRIVER( sonsonj ) /* 1984 "Capcom" "Son Son (Japan)" */
	DRIVER( spang ) /* 1990 "Mitchell" "Super Pang (World 900914)" */
	DRIVER( spangj ) /* 1990 "Mitchell" "Super Pang (Japan 901023)" */
	DRIVER( spf2t ) /* 1996 "Capcom" "Super Puzzle Fighter II Turbo (US 960620)" */
	DRIVER( spf2ta ) /* 1996 "Capcom" "Super Puzzle Fighter II Turbo (Asia 960529)" */
	DRIVER( spf2xj ) /* 1996 "Capcom" "Super Puzzle Fighter II X (Japan 960531)" */
	DRIVER( srumbler ) /* 1986 "Capcom" "The Speed Rumbler (set 1)" */
	DRIVER( srumblr2 ) /* 1986 "Capcom" "The Speed Rumbler (set 2)" */
	DRIVER( ssf2 ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (World 930911)" */
	DRIVER( ssf2a ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (Asia 931005)" */
	DRIVER( ssf2ar1 ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (Asia 930914)" */
	DRIVER( ssf2j ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (Japan 931005)" */
	DRIVER( ssf2jr1 ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (Japan 930911)" */
	DRIVER( ssf2jr2 ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (Japan 930910)" */
	DRIVER( ssf2t ) /* 1994 "Capcom" "Super Street Fighter II Turbo (World 940223)" */
	DRIVER( ssf2ta ) /* 1994 "Capcom" "Super Street Fighter II Turbo (Asia 940223)" */
	DRIVER( ssf2tur1 ) /* 1994 "Capcom" "Super Street Fighter II Turbo (US 940223)" */
	DRIVER( ssf2u ) /* 1993 "Capcom" "Super Street Fighter II: The New Challengers (US 930911)" */
	DRIVER( ssf2xj ) /* 1994 "Capcom" "Super Street Fighter II X: Grand Master Challenge (Japan 940223)" */
	DRIVER( starglad ) /* 1996 "Capcom" "Star Gladiator (USA 960627)" */
	DRIVER( stargld2 ) /* 1998 "Capcom" "Star Gladiator 2 (JAPAN 980316)" */
	DRIVER( strider ) /* 1989 "Capcom" "Strider (US set 1)" */
	DRIVER( strider2 ) /* 1999 "Capcom" "Strider 2 (USA 991213)" */
	DRIVER( striderj ) /* 1989 "Capcom" "Strider Hiryu (Japan set 1)" */
	DRIVER( stridr2a ) /* 1999 "Capcom" "Strider 2 (ASIA 991213)" */
	DRIVER( stridrja ) /* 1989 "Capcom" "Strider Hiryu (Japan set 2)" */
	DRIVER( stridrua ) /* 1989 "Capcom" "Strider (US set 2)" */
	DRIVER( taitofx1 ) /* 1995 "Sony/Taito" "Taito FX1" */
	DRIVER( taitogn ) /* 1997 "Sony/Taito" "Taito GNET" */
	DRIVER( techromn ) /* 1998 "Capcom" "Tech Romancer (EURO 980914)" */
	DRIVER( techromu ) /* 1998 "Capcom" "Tech Romancer (USA 980914)" */
	DRIVER( tecmowcm ) /* 2000 "Tecmo" "Tecmo World Cup Millennium (JAPAN)" */
	DRIVER( tgmj ) /* 1998 "Capcom/Arika" "Tetris The Grand Master (JAPAN 980710)" */
	DRIVER( tigeroad ) /* 1987 "Capcom (Romstar license)" "Tiger Road (US)" */
	DRIVER( tigerodb ) /* 1987 "bootleg" "Tiger Road (US bootleg)" */
	DRIVER( tondemo ) /* 1999 "Tecmo" "Tondemo Crisis (JAPAN)" */
	DRIVER( topsecrt ) /* 1987 "Capcom" "Top Secret (Japan)" */
	DRIVER( toramich ) /* 1987 "Capcom" "Tora-he no Michi (Japan)" */
	DRIVER( tps ) /* 1997 "Sony/Tecmo" "TPS" */
	DRIVER( trojan ) /* 1986 "Capcom" "Trojan (US)" */
	DRIVER( trojanj ) /* 1986 "Capcom" "Tatakai no Banka (Japan)" */
	DRIVER( trojanr ) /* 1986 "Capcom (Romstar license)" "Trojan (Romstar)" */
	DRIVER( ts2 ) /* 1995 "Capcom/Takara" "Battle Arena Toshinden 2 (USA 951124)" */
	DRIVER( ts2j ) /* 1995 "Capcom/Takara" "Battle Arena Toshinden 2 (JAPAN 951124)" */
	DRIVER( turtshik ) /* 1988 "Philko" "Turtle Ship (Korea)" */
	DRIVER( turtship ) /* 1988 "Philko (Pacific Games license)" "Turtle Ship (Japan)" */
	DRIVER( twinfalc ) /* 1989 "Philko (Poara Enterprises license)" "Twin Falcons" */
	DRIVER( unsquad ) /* 1989 "Capcom" "U.N. Squadron (US)" */
	DRIVER( vampj ) /* 1994 "Capcom" "Vampire: The Night Warriors (Japan 940705)" */
	DRIVER( vampja ) /* 1994 "Capcom" "Vampire: The Night Warriors (Japan 940705 alt)" */
	DRIVER( vampjr1 ) /* 1994 "Capcom" "Vampire: The Night Warriors (Japan 940630)" */
	DRIVER( varth ) /* 1992 "Capcom" "Varth - Operation Thunderstorm (World 920714)" */
	DRIVER( varthj ) /* 1992 "Capcom" "Varth - Operation Thunderstorm (Japan 920714)" */
	DRIVER( varthr1 ) /* 1992 "Capcom" "Varth - Operation Thunderstorm (World 920612)" */
	DRIVER( varthu ) /* 1992 "Capcom (Romstar license)" "Varth - Operation Thunderstorm (US 920612)" */
	DRIVER( vhunt2 ) /* 1997 "Capcom" "Vampire Hunter 2: Darkstalkers Revenge (Japan 970929)" */
	DRIVER( vhunt2r1 ) /* 1997 "Capcom" "Vampire Hunter 2: Darkstalkers Revenge (Japan 970913)" */
	DRIVER( vhuntj ) /* 1995 "Capcom" "Vampire Hunter: Darkstalkers' Revenge (Japan 950316)" */
	DRIVER( vhuntjr2 ) /* 1995 "Capcom" "Vampire Hunter: Darkstalkers' Revenge (Japan 950302)" */
	DRIVER( vsav ) /* 1997 "Capcom" "Vampire Savior: The Lord of Vampire (Euro 970519)" */
	DRIVER( vsav2 ) /* 1997 "Capcom" "Vampire Savior 2: The Lord of Vampire (Japan 970913)" */
	DRIVER( vsava ) /* 1997 "Capcom" "Vampire Savior: The Lord of Vampire (Asia 970519)" */
	DRIVER( vsavj ) /* 1997 "Capcom" "Vampire Savior: The Lord of Vampire (Japan 970519)" */
	DRIVER( vsavu ) /* 1997 "Capcom" "Vampire Savior: The Lord of Vampire (US 970519)" */
	DRIVER( vulgus ) /* 1984 "Capcom" "Vulgus (set 1)" */
	DRIVER( vulgus2 ) /* 1984 "Capcom" "Vulgus (set 2)" */
	DRIVER( vulgusj ) /* 1984 "Capcom" "Vulgus (Japan?)" */
	DRIVER( whizz ) /* 1989 "Philko" "Whizz" */
	DRIVER( willow ) /* 1989 "Capcom" "Willow (US)" */
	DRIVER( willowj ) /* 1989 "Capcom" "Willow (Japan */
	DRIVER( willowje ) /* 1989 "Capcom" "Willow (Japan */
	DRIVER( wof ) /* 1992 "Capcom" "Warriors of Fate (World 921002)" */
	DRIVER( wofa ) /* 1992 "Capcom" "Sangokushi II (Asia 921005)" */
	DRIVER( wofj ) /* 1992 "Capcom" "Tenchi wo Kurau II - Sekiheki no Tatakai (Japan 921031)" */
	DRIVER( wofu ) /* 1992 "Capcom" "Warriors of Fate (US 921031)" */
	DRIVER( wonder3 ) /* 1991 "Capcom" "Wonder 3 (Japan 910520)" */
	DRIVER( xmcota ) /* 1994 "Capcom" "X-Men: Children of the Atom (Euro 950105)" */
	DRIVER( xmcotaa ) /* 1994 "Capcom" "X-Men: Children of the Atom (Asia 941217)" */
	DRIVER( xmcotaj ) /* 1994 "Capcom" "X-Men: Children of the Atom (Japan 941219)" */
	DRIVER( xmcotaj1 ) /* 1994 "Capcom" "X-Men: Children of the Atom (Japan 941217)" */
	DRIVER( xmcotajr ) /* 1994 "Capcom" "X-Men: Children of the Atom (Japan 941208 rent version)" */
	DRIVER( xmcotau ) /* 1994 "Capcom" "X-Men: Children of the Atom (US 950105)" */
	DRIVER( xmvsf ) /* 1996 "Capcom" "X-Men Vs. Street Fighter (Euro 961004)" */
	DRIVER( xmvsfr1 ) /* 1996 "Capcom" "X-Men Vs. Street Fighter (Euro 960910)" */
	DRIVER( xmvsfu ) /* 1996 "Capcom" "X-Men Vs. Street Fighter (US 961023)" */
	DRIVER( xmvsfur1 ) /* 1996 "Capcom" "X-Men Vs. Street Fighter (US 961004)" */
#endif
#ifdef LINK_CINEMAT
	DRIVER( aafb ) /* 1989 "Leland Corp." "All American Football (rev E)" */
	DRIVER( aafbb ) /* 1989 "Leland Corp." "All American Football (rev B)" */
	DRIVER( aafbc ) /* 1989 "Leland Corp." "All American Football (rev C)" */
	DRIVER( aafbd2p ) /* 1989 "Leland Corp." "All American Football (rev D */
	DRIVER( alleymas ) /* 1986 "Cinematronics" "Alley Master" */
	DRIVER( armora ) /* 1980 "Cinematronics" "Armor Attack" */
	DRIVER( armorap ) /* 1980 "Cinematronics" "Armor Attack (prototype)" */
	DRIVER( armorar ) /* 1980 "Cinematronics (Rock-ola license)" "Armor Attack (Rock-ola)" */
	DRIVER( asylum ) /* 1991 "Leland Corp." "Asylum (prototype)" */
	DRIVER( ataxx ) /* 1990 "Leland Corp." "Ataxx (set 1)" */
	DRIVER( ataxxa ) /* 1990 "Leland Corp." "Ataxx (set 2)" */
	DRIVER( ataxxj ) /* 1990 "Leland Corp." "Ataxx (Japan)" */
	DRIVER( barrier ) /* 1979 "Vectorbeam" "Barrier" */
	DRIVER( basebal2 ) /* 1987 "Cinematronics" "Baseball The Season II" */
	DRIVER( boxingb ) /* 1981 "Cinematronics" "Boxing Bugs" */
	DRIVER( brix ) /* 1982 "Cinematronics + Advanced Microcomputer Systems" "Brix" */
	DRIVER( brutforc ) /* 1991 "Leland Corp." "Brute Force" */
	DRIVER( cchasm ) /* 1983 "Cinematronics / GCE" "Cosmic Chasm (set 1)" */
	DRIVER( cchasm1 ) /* 1983 "Cinematronics / GCE" "Cosmic Chasm (set 2)" */
	DRIVER( cerberus ) /* 1985 "Cinematronics" "Cerberus" */
	DRIVER( dangerz ) /* 1986 "Cinematronics" "Danger Zone" */
	DRIVER( dblplay ) /* 1987 "Leland Corp. / Tradewest" "Super Baseball Double Play Home Run Derby" */
	DRIVER( demon ) /* 1982 "Rock-ola" "Demon" */
	DRIVER( embargo ) /* 1977 "Cinematronics" "Embargo" */
	DRIVER( freeze ) /* 1984 "Cinematronics" "Freeze" */
	DRIVER( indyheat ) /* 1991 "Leland Corp." "Danny Sullivan's Indy Heat" */
	DRIVER( jack ) /* 1982 "Cinematronics" "Jack the Giantkiller (set 1)" */
	DRIVER( jack2 ) /* 1982 "Cinematronics" "Jack the Giantkiller (set 2)" */
	DRIVER( jack3 ) /* 1982 "Cinematronics" "Jack the Giantkiller (set 3)" */
	DRIVER( joinem ) /* 1986 "Global Corporation" "Joinem" */
	DRIVER( mayhem ) /* 1985 "Cinematronics" "Mayhem 2002" */
	DRIVER( offroad ) /* 1989 "Leland Corp." "Ironman Stewart's Super Off-Road" */
	DRIVER( offroadt ) /* 1989 "Leland Corp." "Ironman Stewart's Super Off-Road Track Pack" */
	DRIVER( pigout ) /* 1990 "Leland Corp." "Pigout" */
	DRIVER( pigouta ) /* 1990 "Leland Corp." "Pigout (alternate)" */
	DRIVER( powrplay ) /* 1985 "Cinematronics" "Power Play" */
	DRIVER( qb3 ) /* 1982 "Rock-ola" "QB-3 (prototype)" */
	DRIVER( quarterb ) /* 1987 "Leland Corp." "Quarterback" */
	DRIVER( quartrba ) /* 1987 "Leland Corp." "Quarterback (set 2)" */
	DRIVER( redlin2p ) /* 1987 "Cinematronics (Tradewest license)" "Redline Racer (2 players)" */
	DRIVER( ripoff ) /* 1980 "Cinematronics" "Rip Off" */
	DRIVER( solarq ) /* 1981 "Cinematronics" "Solar Quest" */
	DRIVER( spaceftr ) /* 1981 "Zaccaria" "Space Fortress (Zaccaria)" */
	DRIVER( spacewar ) /* 1977 "Cinematronics" "Space Wars" */
	DRIVER( speedfrk ) /* 1979 "Vectorbeam" "Speed Freak" */
	DRIVER( starcas ) /* 1980 "Cinematronics" "Star Castle (version 3)" */
	DRIVER( starcas1 ) /* 1980 "Cinematronics" "Star Castle (older)" */
	DRIVER( starcase ) /* 1980 "Cinematronics (Mottoeis license)" "Star Castle (Mottoeis)" */
	DRIVER( starcasp ) /* 1980 "Cinematronics" "Star Castle (prototype)" */
	DRIVER( starhawk ) /* 1979 "Cinematronics" "Star Hawk" */
	DRIVER( stellcas ) /* 1980 "bootleg" "Stellar Castle (Elettronolo)" */
	DRIVER( striv ) /* 1985 "Hara Industries" "Super Triv" */
	DRIVER( strkzone ) /* 1988 "Leland Corp." "Strike Zone Baseball" */
	DRIVER( sucasino ) /* 1984 "Data Amusement" "Super Casino" */
	DRIVER( sundance ) /* 1979 "Cinematronics" "Sundance" */
	DRIVER( tailg ) /* 1979 "Cinematronics" "Tailgunner" */
	DRIVER( teamqb ) /* 1988 "Leland Corp." "John Elway's Team Quarterback" */
	DRIVER( teamqb2 ) /* 1988 "Leland Corp." "John Elway's Team Quarterback (set 2)" */
	DRIVER( treahunt ) /* 1982 "Hara Industries" "Treasure Hunt (Japan?)" */
	DRIVER( tripool ) /* 1981 "Noma (Casino Tech license)" "Tri-Pool (Casino Tech)" */
	DRIVER( tripoola ) /* 1981 "Noma (Costal Games license)" "Tri-Pool (Costal Games)" */
	DRIVER( upyoural ) /* 1987 "Cinematronics" "Up Your Alley" */
	DRIVER( viper ) /* 1988 "Leland Corp." "Viper" */
	DRIVER( warrior ) /* 1979 "Vectorbeam" "Warrior" */
	DRIVER( wotw ) /* 1981 "Cinematronics" "War of the Worlds" */
	DRIVER( wotwc ) /* 1981 "Cinematronics" "War of the Worlds (color)" */
	DRIVER( wseries ) /* 1985 "Cinematronics" "World Series: The Season" */
	DRIVER( wsf ) /* 1990 "Leland Corp." "World Soccer Finals" */
	DRIVER( zzyzzyx2 ) /* 1982 "Cinematronics + Advanced Microcomputer Systems" "Zzyzzyxx (set 2)" */
	DRIVER( zzyzzyxx ) /* 1982 "Cinematronics + Advanced Microcomputer Systems" "Zzyzzyxx (set 1)" */
#endif
#ifdef LINK_COMAD
	DRIVER( bballs ) /* 1991 "Comad" "Bouncing Balls" */
	DRIVER( funybubl ) /* 1999 "Comad" "Funny Bubble" */
	DRIVER( galspnbl ) /* 1996 "Comad" "Gals Pinball" */
	DRIVER( hotpinbl ) /* 1995 "Comad & New Japan System" "Hot Pinball" */
	DRIVER( lvgirl94 ) /* 1994 "Comad" "Las Vegas Girl (Girl '94)" */
	DRIVER( pushman ) /* 1990 "Comad" "Pushman (Korea */
	DRIVER( pushmana ) /* 1990 "Comad" "Pushman (Korea */
	DRIVER( pushmans ) /* 1990 "Comad (American Sammy license)" "Pushman (American Sammy license)" */
	DRIVER( zerozone ) /* 1993 "Comad" "Zero Zone" */
#endif
#ifdef LINK_CVS
	DRIVER( 8ball ) /* 1982 "Century Electronics" "Video Eight Ball" */
	DRIVER( 8ball1 ) /* 1982 "Century Electronics" "Video Eight Ball (Rev.1)" */
	DRIVER( cosmos ) /* 1981 "Century Electronics" "Cosmos" */
	DRIVER( cvs ) /* 1981 "Century Electronics" "CVS Bios" */
	DRIVER( darkwar ) /* 1981 "Century Electronics" "Dark Warrior" */
	DRIVER( dazzler ) /* 1982 "Century Electronics" "Dazzler" */
	DRIVER( diggerc ) /* 1982 "Century Electronics" "Digger (CVS)" */
	DRIVER( goldbug ) /* 1982 "Century Electronics" "Gold Bug" */
	DRIVER( heartatk ) /* 1983 "Century Electronics" "Heart Attack" */
	DRIVER( hero ) /* 1983 "Seatongrove Ltd" "Hero" */
	DRIVER( hunchbak ) /* 1983 "Century Electronics" "Hunchback" */
	DRIVER( huncholy ) /* 1984 "Seatongrove Ltd" "Hunchback Olympic" */
	DRIVER( logger ) /* 1982 "Century Electronics" "Logger" */
	DRIVER( outline ) /* 1982 "Century Electronics" "Outline" */
	DRIVER( quasar ) /* 1980 "Zelco / Zaccaria" "Quasar" */
	DRIVER( quasara ) /* 1980 "Zelco / Zaccaria" "Quasar (Alternate)" */
	DRIVER( radarzn1 ) /* 1982 "Century Electronics" "Radar Zone (Rev.1)" */
	DRIVER( radarznt ) /* 1982 "Century Electronics (Tuni Electro Service Inc)" "Radar Zone (Tuni)" */
	DRIVER( radarzon ) /* 1982 "Century Electronics" "Radar Zone" */
	DRIVER( raiders ) /* 1983 "Century Electronics" "Raiders" */
	DRIVER( spacefrt ) /* 1981 "Century Electronics" "Space Fortress" */
	DRIVER( superbik ) /* 1983 "Century Electronics" "Superbike" */
	DRIVER( wallst ) /* 1982 "Century Electronics" "Wall Street" */
#endif
#ifdef LINK_DATAEAST
	DRIVER( abattle ) /* 1979 "Sidam" "Astro Battle (set 1)" */
	DRIVER( abattle2 ) /* 1979 "Sidam" "Astro Battle (set 2)" */
	DRIVER( acombat ) /* 1979 "bootleg" "Astro Combat" */
	DRIVER( actfanc1 ) /* 1989 "Data East Corporation" "Act-Fancer Cybernetick Hyper Weapon (World revision 1)" */
	DRIVER( actfancj ) /* 1989 "Data East Corporation" "Act-Fancer Cybernetick Hyper Weapon (Japan revision 1)" */
	DRIVER( actfancr ) /* 1989 "Data East Corporation" "Act-Fancer Cybernetick Hyper Weapon (World revision 2)" */
	DRIVER( afire ) /* 1979 "Rene Pierre" "Astro Fire" */
	DRIVER( astrof ) /* 1979 "Data East" "Astro Fighter (set 1)" */
	DRIVER( astrof2 ) /* 1979 "Data East" "Astro Fighter (set 2)" */
	DRIVER( astrof3 ) /* 1979 "Data East" "Astro Fighter (set 3)" */
	DRIVER( avengrgj ) /* 1995 "Data East Corporation" "Avengers In Galactic Storm (Japan)" */
	DRIVER( avengrgs ) /* 1995 "Data East Corporation" "Avengers In Galactic Storm (US)" */
	DRIVER( backfira ) /* 1995 "Data East Corporation" "Backfire! (set 2)" */
	DRIVER( backfire ) /* 1995 "Data East Corporation" "Backfire!" */
	DRIVER( baddudes ) /* 1988 "Data East USA" "Bad Dudes vs. Dragonninja (US)" */
	DRIVER( battlera ) /* 1988 "Data East Corporation" "Battle Rangers (World)" */
	DRIVER( bcstry ) /* 1997 "SemiCom" "B.C. Story (set 1)" */
	DRIVER( bcstrya ) /* 1997 "SemiCom" "B.C. Story (set 2)" */
	DRIVER( birdtry ) /* 1988 "Data East Corporation" "Birdie Try (Japan)" */
	DRIVER( bldwolf ) /* 1988 "Data East USA" "Bloody Wolf (US)" */
	DRIVER( bnj ) /* 1982 "Data East USA (Bally Midway license)" "Bump 'n' Jump" */
	DRIVER( boogwina ) /* 1992 "Data East Corporation" "Boogie Wings (Asia v1.5 */
	DRIVER( boogwing ) /* 1992 "Data East Corporation" "Boogie Wings (Euro v1.5 */
	DRIVER( boomrang ) /* 1983 "Data East Corporation" "Boomer Rang'r / Genesis" */
	DRIVER( bouldash ) /* 1990 "Data East Corporation (licensed from First Star)" "Boulder Dash / Boulder Dash Part 2 (World)" */
	DRIVER( bouldshj ) /* 1990 "Data East Corporation (licensed from First Star)" "Boulder Dash / Boulder Dash Part 2 (Japan)" */
	DRIVER( breywood ) /* 1986 "Data East Corporation" "Breywood (Japan revision 2)" */
	DRIVER( brkthru ) /* 1986 "Data East USA" "Break Thru (US)" */
	DRIVER( brkthruj ) /* 1986 "Data East Corporation" "Kyohkoh-Toppa (Japan)" */
	DRIVER( brubber ) /* 1982 "Data East" "Burnin' Rubber" */
	DRIVER( btime ) /* 1982 "Data East Corporation" "Burger Time (Data East set 1)" */
	DRIVER( btime2 ) /* 1982 "Data East Corporation" "Burger Time (Data East set 2)" */
	DRIVER( btimem ) /* 1982 "Data East (Bally Midway license)" "Burger Time (Midway)" */
	DRIVER( bwing ) /* 1984 "Data East Corporation" "B-Wings (Japan)" */
	DRIVER( bwings ) /* 1984 "Data East Corporation" "Battle Wings" */
	DRIVER( candance ) /* 1996 "Mitchell (Atlus License)" "Cannon Dancer (Japan)" */
	DRIVER( captaven ) /* 1991 "Data East Corporation" "Captain America and The Avengers (Asia Rev 1.9)" */
	DRIVER( captavna ) /* 1991 "Data East Corporation" "Captain America and The Avengers (Asia Rev 1.0)" */
	DRIVER( captavne ) /* 1991 "Data East Corporation" "Captain America and The Avengers (UK Rev 1.4)" */
	DRIVER( captavnj ) /* 1991 "Data East Corporation" "Captain America and The Avengers (Japan Rev 0.2)" */
	DRIVER( captavnu ) /* 1991 "Data East Corporation" "Captain America and The Avengers (US Rev 1.9)" */
	DRIVER( captavuu ) /* 1991 "Data East Corporation" "Captain America and The Avengers (US Rev 1.6)" */
	DRIVER( caractn ) /* 1982 "bootleg" "Car Action" */
	DRIVER( castfant ) /* 1981 "Data East Corporation" "Astro Fantasia (Cassette)" */
	DRIVER( cbdash ) /* 1985 "Data East Corporation" "Boulder Dash (Cassette)" */
	DRIVER( cbnj ) /* 1982 "Data East Corporation" "Bump N Jump (Cassette)" */
	DRIVER( cbtime ) /* 1983 "Data East Corporation" "Burger Time (Cassette)" */
	DRIVER( cburnrb2 ) /* 1982 "Data East Corporation" "Burnin' Rubber (Cassette */
	DRIVER( cburnrub ) /* 1982 "Data East Corporation" "Burnin' Rubber (Cassette */
	DRIVER( cbuster ) /* 1990 "Data East Corporation" "Crude Buster (World FX version)" */
	DRIVER( cbusterj ) /* 1990 "Data East Corporation" "Crude Buster (Japan)" */
	DRIVER( cbusterw ) /* 1990 "Data East Corporation" "Crude Buster (World FU version)" */
	DRIVER( cdiscon1 ) /* 1982 "Data East Corporation" "Disco No.1 (Cassette)" */
	DRIVER( cfghtice ) /* 1984 "Data East Corporation" "Fighting Ice Hockey (Cassette)" */
	DRIVER( cflyball ) /* 1985 "Data East Corporation" "Flying Ball (Cassette)" */
	DRIVER( cgraplop ) /* 1983 "Data East Corporation" "Graplop (aka Cluster Buster) (Cassette */
	DRIVER( cgraplp2 ) /* 1983 "Data East Corporation" "Graplop (aka Cluster Buster) (Cassette */
	DRIVER( chainrec ) /* 1995 "Data East" "Chain Reaction (World */
	DRIVER( charlien ) /* 1995 "Mitchell" "Charlie Ninja" */
	DRIVER( chelnov ) /* 1988 "Data East Corporation" "Chelnov - Atomic Runner (World)" */
	DRIVER( chelnovj ) /* 1988 "Data East Corporation" "Chelnov - Atomic Runner (Japan)" */
	DRIVER( chelnovu ) /* 1988 "Data East USA" "Chelnov - Atomic Runner (US)" */
	DRIVER( chinatwn ) /* 1991 "Data East Corporation" "China Town (Japan)" */
	DRIVER( chokchok ) /* 1995 "SemiCom" "Choky! Choky!" */
	DRIVER( clapapa ) /* 1983 "Data East Corporation" "Rootin' Tootin' (aka La.Pa.Pa) (Cassette)" */
	DRIVER( clapapa2 ) /* 1983 "Data East Corporation" "Rootin' Tootin' (Cassette)" */
	DRIVER( clocknch ) /* 1981 "Data East Corporation" "Lock'n'Chase (Cassette)" */
	DRIVER( cluckypo ) /* 1981 "Data East Corporation" "Lucky Poker (Cassette)" */
	DRIVER( cmissnx ) /* 1982 "Data East Corporation" "Mission-X (Cassette)" */
	DRIVER( cnights2 ) /* 1983 "Data East Corporation" "Night Star (Cassette */
	DRIVER( cnightst ) /* 1983 "Data East Corporation" "Night Star (Cassette */
	DRIVER( cninja ) /* 1991 "Data East Corporation" "Caveman Ninja (World revision 3)" */
	DRIVER( cninja0 ) /* 1991 "Data East Corporation" "Caveman Ninja (World revision 0)" */
	DRIVER( cninjau ) /* 1991 "Data East Corporation" "Caveman Ninja (US)" */
	DRIVER( cobracmj ) /* 1988 "Data East Corporation" "Cobra-Command (Japan)" */
	DRIVER( cobracom ) /* 1988 "Data East Corporation" "Cobra-Command (World revision 5)" */
	DRIVER( compglfo ) /* 1985 "Data East" "Competition Golf Final Round (old version)" */
	DRIVER( compgolf ) /* 1986 "Data East" "Competition Golf Final Round (revision 3)" */
	DRIVER( cookbib ) /* 1995 "SemiCom" "Cookie & Bibi" */
	DRIVER( cookrace ) /* 1982 "bootleg" "Cook Race" */
	DRIVER( cppicf ) /* 1984 "Data East Corporation" "Peter Pepper's Ice Cream Factory (Cassette */
	DRIVER( cppicf2 ) /* 1984 "Data East Corporation" "Peter Pepper's Ice Cream Factory (Cassette */
	DRIVER( cprobowl ) /* 1983 "Data East Corporation" "Pro Bowling (Cassette)" */
	DRIVER( cprogolf ) /* 1981 "Data East Corporation" "Pro Golf (Cassette)" */
	DRIVER( cprosocc ) /* 1983 "Data East Corporation" "Pro Soccer (Cassette)" */
	DRIVER( cptennis ) /* 1982 "Data East Corporation" "Pro Tennis (Cassette)" */
	DRIVER( cscrtry ) /* 1984 "Data East Corporation" "Scrum Try (Cassette */
	DRIVER( cscrtry2 ) /* 1984 "Data East Corporation" "Scrum Try (Cassette */
	DRIVER( csilver ) /* 1987 "Data East Corporation" "Captain Silver (Japan)" */
	DRIVER( csuperas ) /* 1981 "Data East Corporation" "Super Astro Fighter (Cassette)" */
	DRIVER( csweetht ) /* 1982 "Data East Corporation" "Sweet Heart (Cassette)" */
	DRIVER( cterrani ) /* 1981 "Data East Corporation" "Terranean (Cassette)" */
	DRIVER( ctisland ) /* 1981 "Data East Corporation" "Treasure Island (Cassette */
	DRIVER( ctislnd2 ) /* 1981 "Data East Corporation" "Treasure Island (Cassette */
	DRIVER( ctislnd3 ) /* 1981 "Data East Corporation" "Treasure Island (Cassette */
	DRIVER( ctornado ) /* 1982 "Data East Corporation" "Tornado (Cassette)" */
	DRIVER( ctsttape ) /* 1981 "Data East Corporation" "Test Tape (Cassette)" */
	DRIVER( czeroize ) /* 1983 "Data East Corporation" "Zeroize (Cassette)" */
	DRIVER( darksea1 ) /* 1990 "Data East Corporation" "Dark Seal (World revision 1)" */
	DRIVER( darkseaj ) /* 1990 "Data East Corporation" "Dark Seal (Japan)" */
	DRIVER( darkseal ) /* 1990 "Data East Corporation" "Dark Seal (World revision 3)" */
	DRIVER( darksel2 ) /* 1992 "Data East Corporation" "Dark Seal 2 (Japan v2.1)" */
	DRIVER( darwin ) /* 1986 "Data East Corporation" "Darwin 4078 (Japan)" */
	DRIVER( dassaul4 ) /* 1991 "Data East Corporation" "Desert Assault (US 4 Players)" */
	DRIVER( dassault ) /* 1991 "Data East Corporation" "Desert Assault (US)" */
	DRIVER( ddream95 ) /* 1995 "Data East Corporation" "Dunk Dream '95 (Japan 1.4 EAM)" */
	DRIVER( deathbrd ) /* 1992 "Data East Corporation" "Death Brade (Japan Rev 2 */
	DRIVER( decocass ) /* 1981 "Data East Corporation" "Cassette System" */
	DRIVER( dietgo ) /* 1992 "Data East Corporation" "Diet Go Go (Euro v1.1 1992.09.26)" */
	DRIVER( dietgoe ) /* 1992 "Data East Corporation" "Diet Go Go (Euro v1.1 1992.08.04)" */
	DRIVER( dietgoj ) /* 1992 "Data East Corporation" "Diet Go Go (Japan v1.1 1992.09.26)" */
	DRIVER( dietgou ) /* 1992 "Data East Corporation" "Diet Go Go (USA v1.1 1992.09.26)" */
	DRIVER( disco ) /* 1982 "Data East" "Disco No.1" */
	DRIVER( discof ) /* 1982 "Data East" "Disco No.1 (Rev.F)" */
	DRIVER( dragngun ) /* 1993 "Data East Corporation" "Dragon Gun (US)" */
	DRIVER( drgninja ) /* 1988 "Data East Corporation" "Dragonninja (Japan)" */
	DRIVER( dualaslt ) /* 1984 "Data East USA" "Dual Assault" */
	DRIVER( edrandy ) /* 1990 "Data East Corporation" "The Cliffhanger - Edward Randy (World revision 2)" */
	DRIVER( edrandy1 ) /* 1990 "Data East Corporation" "The Cliffhanger - Edward Randy (World revision 1)" */
	DRIVER( edrandyj ) /* 1990 "Data East Corporation" "The Cliffhanger - Edward Randy (Japan)" */
	DRIVER( exprrada ) /* 1986 "Data East USA" "Express Raider (US set 2)" */
	DRIVER( exprraid ) /* 1986 "Data East USA" "Express Raider (US)" */
	DRIVER( ffantasa ) /* 1989 "Data East Corporation" "Fighting Fantasy (Japan)" */
	DRIVER( ffantasy ) /* 1989 "Data East Corporation" "Fighting Fantasy (Japan revision 2)" */
	DRIVER( fghthist ) /* 1993 "Data East Corporation" "Fighter's History (World ver 43-07)" */
	DRIVER( fghthsta ) /* 1993 "Data East Corporation" "Fighter's History (US ver 42-05 */
	DRIVER( fghthstu ) /* 1993 "Data East Corporation" "Fighter's History (US ver 42-03)" */
	DRIVER( firetpbl ) /* 1986 "bootleg" "Fire Trap (Japan bootleg)" */
	DRIVER( firetrap ) /* 1986 "Data East USA" "Fire Trap (US)" */
	DRIVER( fncywld ) /* 1996 "Unico" "Fancy World - Earth of Crisis" */
	DRIVER( forcebrk ) /* 1986 "bootleg" "Force Break (bootleg)" */
	DRIVER( funkyjej ) /* 1992 "Data East Corporation" "Funky Jet (Japan)" */
	DRIVER( funkyjet ) /* 1992 "[Data East] (Mitchell license)" "Funky Jet (World)" */
	DRIVER( gangonta ) /* 1995 "Mitchell" "Ganbare! Gonta!! 2 / Party Time: Gonta the Diver II (Japan Release)" */
	DRIVER( garyoret ) /* 1987 "Data East Corporation" "Garyo Retsuden (Japan)" */
	DRIVER( gatedom1 ) /* 1990 "Data East Corporation" "Gate of Doom (US revision 1)" */
	DRIVER( gatedoom ) /* 1990 "Data East Corporation" "Gate of Doom (US revision 4)" */
	DRIVER( ghostb ) /* 1987 "Data East USA" "The Real Ghostbusters (US 2 Players)" */
	DRIVER( ghostb3 ) /* 1987 "Data East USA" "The Real Ghostbusters (US 3 Players)" */
	DRIVER( gondo ) /* 1987 "Data East USA" "Gondomania (US)" */
	DRIVER( gunball ) /* 1992 "Data East Corporation" "Gun Ball (Japan)" */
	DRIVER( hbarrel ) /* 1987 "Data East USA" "Heavy Barrel (US)" */
	DRIVER( hbarrelw ) /* 1987 "Data East Corporation" "Heavy Barrel (World)" */
	DRIVER( hippodrm ) /* 1989 "Data East USA" "Hippodrome (US)" */
	DRIVER( hoops95 ) /* 1995 "Data East Corporation" "Hoops (Europe/Asia 1.7)" */
	DRIVER( hoops96 ) /* 1996 "Data East Corporation" "Hoops '96 (Europe/Asia 2.0)" */
	DRIVER( htchctch ) /* 1995 "SemiCom" "Hatch Catch" */
	DRIVER( hvysmsh ) /* 1993 "Data East Corporation" "Heavy Smash (Japan version -2)" */
	DRIVER( joemac ) /* 1991 "Data East Corporation" "Tatakae Genshizin Joe & Mac (Japan)" */
	DRIVER( joemacr ) /* 1994 "Data East" "Joe & Mac Returns (World */
	DRIVER( joemacra ) /* 1994 "Data East" "Joe & Mac Returns (World */
	DRIVER( jumpkids ) /* 1993 "Comad" "Jump Kids" */
	DRIVER( jumppop ) /* 2001 "ESD" "Jumping Pop" */
	DRIVER( kamikcab ) /* 1984 "Data East Corporation" "Kamikaze Cabbie" */
	DRIVER( karatedo ) /* 1984 "Data East Corporation" "Karate Dou (Japan)" */
	DRIVER( karatevs ) /* 1984 "Data East Corporation" "Taisen Karate Dou (Japan VS version)" */
	DRIVER( karnov ) /* 1987 "Data East USA" "Karnov (US)" */
	DRIVER( karnovj ) /* 1987 "Data East Corporation" "Karnov (Japan)" */
	DRIVER( kchamp ) /* 1984 "Data East USA" "Karate Champ (US)" */
	DRIVER( kchampvs ) /* 1984 "Data East USA" "Karate Champ (US VS version)" */
	DRIVER( kingofb ) /* 1985 "Woodplace" "King of Boxer (English)" */
	DRIVER( kuhga ) /* 1989 "Data East Corporation" "Kuhga - Operation Code 'Vapor Trail' (Japan revision 3)" */
	DRIVER( lastmisn ) /* 1986 "Data East USA" "Last Mission (US revision 6)" */
	DRIVER( lastmsnj ) /* 1986 "Data East Corporation" "Last Mission (Japan)" */
	DRIVER( lastmsno ) /* 1986 "Data East USA" "Last Mission (US revision 5)" */
	DRIVER( lemmings ) /* 1991 "Data East USA" "Lemmings (US Prototype)" */
	DRIVER( liberatb ) /* 1984 "bootleg" "Liberation (bootleg)" */
	DRIVER( liberate ) /* 1984 "Data East Corporation" "Liberation" */
	DRIVER( lnc ) /* 1981 "Data East Corporation" "Lock'n'Chase" */
	DRIVER( madalien ) /* 1980 "Data East Corporation" "Mad Alien" */
	DRIVER( madmotor ) /* 1989 "Mitchell" "Mad Motor" */
	DRIVER( magdrop ) /* 1995 "Data East" "Magical Drop (Japan */
	DRIVER( magdropp ) /* 1995 "Data East" "Magical Drop Plus 1 (Japan */
	DRIVER( makyosen ) /* 1987 "Data East Corporation" "Makyou Senshi (Japan)" */
	DRIVER( meikyuh ) /* 1987 "Data East Corporation" "Meikyuu Hunter G (Japan)" */
	DRIVER( metlclsh ) /* 1985 "Data East" "Metal Clash (Japan)" */
	DRIVER( metlsavr ) /* 1994 "First Amusement" "Metal Saver" */
	DRIVER( midres ) /* 1989 "Data East Corporation" "Midnight Resistance (World)" */
	DRIVER( midresj ) /* 1989 "Data East Corporation" "Midnight Resistance (Japan)" */
	DRIVER( midresu ) /* 1989 "Data East USA" "Midnight Resistance (US)" */
	DRIVER( mmonkey ) /* 1982 "Technos + Roller Tron" "Minky Monkey" */
	DRIVER( mutantf ) /* 1992 "Data East Corporation" "Mutant Fighter (World Rev 4 */
	DRIVER( mutantfa ) /* 1992 "Data East Corporation" "Mutant Fighter (World Rev 3 */
	DRIVER( nitrobal ) /* 1992 "Data East Corporation" "Nitro Ball (US)" */
	DRIVER( nslashej ) /* 1994 "Data East Corporation" "Night Slashers (Japan Rev 1.2)" */
	DRIVER( nslasher ) /* 1994 "Data East Corporation" "Night Slashers (Korea Rev 1.3)" */
	DRIVER( nslashes ) /* 1994 "Data East Corporation" "Night Slashers (Over Sea Rev 1.2)" */
	DRIVER( oscar ) /* 1988 "Data East USA" "Psycho-Nics Oscar (US)" */
	DRIVER( oscarj ) /* 1987 "Data East Corporation" "Psycho-Nics Oscar (Japan revision 2)" */
	DRIVER( oscarj0 ) /* 1987 "Data East Corporation" "Psycho-Nics Oscar (Japan revision 0)" */
	DRIVER( oscarj1 ) /* 1987 "Data East Corporation" "Psycho-Nics Oscar (Japan revision 1)" */
	DRIVER( osman ) /* 1996 "Mitchell" "Osman (World)" */
	DRIVER( pangpang ) /* 1994 "Dong Gue La Mi Ltd." "Pang Pang" */
	DRIVER( pcktgal ) /* 1987 "Data East Corporation" "Pocket Gal (Japan)" */
	DRIVER( pcktgal2 ) /* 1989 "Data East Corporation" "Pocket Gal 2 (World?)" */
	DRIVER( pcktgalb ) /* 1987 "bootleg" "Pocket Gal (bootleg)" */
	DRIVER( pktgaldb ) /* 1992 "bootleg" "Pocket Gal Deluxe (Euro v3.00 */
	DRIVER( pktgaldj ) /* 1993 "Nihon System" "Pocket Gal Deluxe (Japan v3.00)" */
	DRIVER( pktgaldx ) /* 1992 "Data East Corporation" "Pocket Gal Deluxe (Euro v3.00)" */
	DRIVER( prtytime ) /* 1995 "Mitchell" "Party Time: Gonta the Diver II / Ganbare! Gonta!! 2 (World Release)" */
	DRIVER( ragtime ) /* 1992 "Data East Corporation" "The Great Ragtime Show (Japan v1.5 */
	DRIVER( ringkin2 ) /* 1985 "Data East USA" "Ring King (US set 2)" */
	DRIVER( ringkin3 ) /* 1985 "Data East USA" "Ring King (US set 3)" */
	DRIVER( ringking ) /* 1985 "Data East USA" "Ring King (US set 1)" */
	DRIVER( ringkinw ) /* 1985 "Woodplace" "Ring King (US */
	DRIVER( robocop ) /* 1988 "Data East Corporation" "Robocop (World revision 4)" */
	DRIVER( robocop2 ) /* 1991 "Data East Corporation" "Robocop 2 (Euro/Asia v0.10)" */
	DRIVER( robocopb ) /* 1988 "bootleg" "Robocop (World bootleg)" */
	DRIVER( robocopj ) /* 1988 "Data East Corporation" "Robocop (Japan)" */
	DRIVER( robocopu ) /* 1988 "Data East USA" "Robocop (US revision 1)" */
	DRIVER( robocopw ) /* 1988 "Data East Corporation" "Robocop (World revision 3)" */
	DRIVER( robocp2j ) /* 1991 "Data East Corporation" "Robocop 2 (Japan v0.11)" */
	DRIVER( robocp2u ) /* 1991 "Data East Corporation" "Robocop 2 (US v0.05)" */
	DRIVER( robocpu0 ) /* 1988 "Data East USA" "Robocop (US revision 0)" */
	DRIVER( rohga ) /* 1991 "Data East Corporation" "Rohga Armor Force (Asia/Europe v5.0)" */
	DRIVER( rohga1 ) /* 1991 "Data East Corporation" "Rohga Armor Force (Asia/Europe v3.0 Set 1)" */
	DRIVER( rohga2 ) /* 1991 "Data East Corporation" "Rohga Armor Force (Asia/Europe v3.0 Set 2)" */
	DRIVER( rohgah ) /* 1991 "Data East Corporation" "Rohga Armor Force (Hong Kong v3.0)" */
	DRIVER( rohgau ) /* 1991 "Data East Corporation" "Rohga Armor Force (US v1.0)" */
	DRIVER( schmeisr ) /* 1993 "Hot B" "Schmeiser Robo (Japan)" */
	DRIVER( sdtennis ) /* 1983 "Data East Corporation" "Super Doubles Tennis" */
	DRIVER( secretag ) /* 1989 "Data East Corporation" "Secret Agent (World)" */
	DRIVER( semibase ) /* 1997 "SemiCom" "MuHanSeungBu (SemiCom Baseball)" */
	DRIVER( shackled ) /* 1986 "Data East USA" "Shackled (US)" */
	DRIVER( shootoub ) /* 1985 "bootleg" "Shoot Out (Korean Bootleg)" */
	DRIVER( shootouj ) /* 1985 "Data East USA" "Shoot Out (Japan)" */
	DRIVER( shootout ) /* 1985 "Data East USA" "Shoot Out (US)" */
	DRIVER( sidepckt ) /* 1986 "Data East Corporation" "Side Pocket (World)" */
	DRIVER( sidepctb ) /* 1986 "bootleg" "Side Pocket (bootleg)" */
	DRIVER( sidepctj ) /* 1986 "Data East Corporation" "Side Pocket (Japan)" */
	DRIVER( skullfng ) /* 1996 "Data East Corporation" "Skull Fang (Japan)" */
	DRIVER( slyspy ) /* 1989 "Data East USA" "Sly Spy (US revision 3)" */
	DRIVER( slyspy2 ) /* 1989 "Data East USA" "Sly Spy (US revision 2)" */
	DRIVER( sotsugyo ) /* 1995 "Mitchell (Atlus license)" "Sotsugyo Shousho" */
	DRIVER( spool3 ) /* 1989 "Data East Corporation" "Super Pool III (World?)" */
	DRIVER( spool3i ) /* 1990 "Data East Corporation (I-Vics license)" "Super Pool III (I-Vics)" */
	DRIVER( srdarwin ) /* 1987 "Data East Corporation" "Super Real Darwin (World)" */
	DRIVER( srdarwnj ) /* 1987 "Data East Corporation" "Super Real Darwin (Japan)" */
	DRIVER( sshangha ) /* 1992 "Hot-B." "Super Shanghai Dragon's Eye (Japan)" */
	DRIVER( sshanghb ) /* 1992 "bootleg" "Super Shanghai Dragon's Eye (World */
	DRIVER( sstarbtl ) /* 1979 "bootleg" "Super Star Battle" */
	DRIVER( stadhero ) /* 1988 "Data East Corporation" "Stadium Hero (Japan)" */
	DRIVER( stoneage ) /* 1991 "bootleg" "Stoneage" */
	DRIVER( supbtime ) /* 1990 "Data East Corporation" "Super Burger Time (World)" */
	DRIVER( supbtimj ) /* 1990 "Data East Corporation" "Super Burger Time (Japan)" */
	DRIVER( suprtrio ) /* 1994 "Gameace" "Super Trio" */
	DRIVER( tattass ) /* 1994 "Data East Pinball" "Tattoo Assassins (US Prototype)" */
	DRIVER( tattassa ) /* 1994 "Data East Pinball" "Tattoo Assassins (Asia Prototype)" */
	DRIVER( thndzone ) /* 1991 "Data East Corporation" "Thunder Zone (World)" */
	DRIVER( tomahaw5 ) /* 1980 "Data East" "Tomahawk 777 (Revision 5)" */
	DRIVER( tomahawk ) /* 1980 "Data East" "Tomahawk 777 (Revision 1)" */
	DRIVER( triothej ) /* 1989 "Data East Corporation" "Trio The Punch - Never Forget Me... (Japan)" */
	DRIVER( triothep ) /* 1989 "Data East Corporation" "Trio The Punch - Never Forget Me... (World)" */
	DRIVER( tryout ) /* 1985 "Data East Corporation" "Pro Baseball Skill Tryout (Japan)" */
	DRIVER( tumbleb ) /* 1991 "bootleg" "Tumble Pop (bootleg set 1)" */
	DRIVER( tumbleb2 ) /* 1991 "bootleg" "Tumble Pop (bootleg set 2)" */
	DRIVER( tumblep ) /* 1991 "Data East Corporation" "Tumble Pop (World)" */
	DRIVER( tumblepj ) /* 1991 "Data East Corporation" "Tumble Pop (Japan)" */
	DRIVER( twocrude ) /* 1990 "Data East USA" "Two Crude (US)" */
	DRIVER( vaportra ) /* 1989 "Data East Corporation" "Vapor Trail - Hyper Offence Formation (World revision 1)" */
	DRIVER( vaportru ) /* 1989 "Data East USA" "Vapor Trail - Hyper Offence Formation (US)" */
	DRIVER( wcvol95 ) /* 1995 "Data East Corporation" "World Cup Volley '95 (Japan v1.0)" */
	DRIVER( wexpresb ) /* 1986 "bootleg" "Western Express (bootleg set 1)" */
	DRIVER( wexpresc ) /* 1986 "bootleg" "Western Express (bootleg set 2)" */
	DRIVER( wexpress ) /* 1986 "Data East Corporation" "Western Express (World?)" */
	DRIVER( wizdfire ) /* 1992 "Data East Corporation" "Wizard Fire (US v1.1)" */
	DRIVER( wndrplnt ) /* 1987 "Data East Corporation" "Wonder Planet (Japan)" */
	DRIVER( wtennis ) /* 1982 "bootleg" "World Tennis" */
	DRIVER( yellowcb ) /* 1984 "bootleg" "Yellow Cab (bootleg)" */
	DRIVER( zaviga ) /* 1984 "Data East Corporation" "Zaviga" */
	DRIVER( zavigaj ) /* 1984 "Data East Corporation" "Zaviga (Japan)" */
	DRIVER( zoar ) /* 1982 "Data East USA" "Zoar" */
#endif
#ifdef LINK_DOOYONG
	DRIVER( bluehawk ) /* 1993 "Dooyong" "Blue Hawk" */
	DRIVER( bluehawn ) /* 1993 "[Dooyong] (NTC license)" "Blue Hawk (NTC)" */
	DRIVER( flytiger ) /* 1992 "Dooyong" "Flying Tiger" */
	DRIVER( gulfstr2 ) /* 1991 "Dooyong (Media Shoji license)" "Gulf Storm (Media Shoji)" */
	DRIVER( gulfstrm ) /* 1991 "Dooyong" "Gulf Storm" */
	DRIVER( gundeala ) /* 19?? "Dooyong" "Gun Dealer (set 2)" */
	DRIVER( gundealr ) /* 1990 "Dooyong" "Gun Dealer (set 1)" */
	DRIVER( gundealt ) /* 1990 "Tecmo" "Gun Dealer (Tecmo)" */
	DRIVER( gundl94 ) /* 1994 "Dooyong" "Gun Dealer '94" */
	DRIVER( lastday ) /* 1990 "Dooyong" "The Last Day (set 1)" */
	DRIVER( lastdaya ) /* 1990 "Dooyong" "The Last Day (set 2)" */
	DRIVER( pollux ) /* 1991 "Dooyong" "Pollux (set 1)" */
	DRIVER( polluxa ) /* 1991 "Dooyong" "Pollux (set 2)" */
	DRIVER( polluxa2 ) /* 1991 "Dooyong" "Pollux (set 3)" */
	DRIVER( popbingo ) /* 1996 "Dooyong" "Pop Bingo" */
	DRIVER( primella ) /* 1994 "[Dooyong] (NTC license)" "Primella" */
	DRIVER( rshark ) /* 1995 "Dooyong" "R-Shark" */
	DRIVER( sadari ) /* 1993 "[Dooyong] (NTC license)" "Sadari" */
	DRIVER( superx ) /* 1994 "NTC" "Super-X (NTC)" */
	DRIVER( superxm ) /* 1994 "Mitchell" "Super-X (Mitchell)" */
	DRIVER( wiseguy ) /* 1990 "Dooyong" "Wise Guy" */
	DRIVER( yamyam ) /* 1990 "Dooyong" "Yam! Yam!?" */
#endif
#ifdef LINK_DYNAX
	DRIVER( 7jigen ) /* 1990 "Dynax" "7jigen no Youseitachi - Mahjong 7 Dimensions (Japan)" */
	DRIVER( animaljr ) /* 1993 "Nakanihon + East Technology (Taito license)" "Animalandia Jr." */
	DRIVER( ddenlovr ) /* 1996 "Dynax" "Don Den Lover Vol. 1 (Hong Kong)" */
	DRIVER( dondenmj ) /* 1986 "Dyna Electronics" "Don Den Mahjong [BET] (Japan)" */
	DRIVER( drgpunch ) /* 1989 "Dynax" "Dragon Punch (Japan)" */
	DRIVER( funkyfig ) /* 1993 "Nakanihon + East Technology (Taito license)" "The First Funky Fighter" */
	DRIVER( hanakanz ) /* 1996 "Dynax" "Hana Kanzashi (Japan)" */
	DRIVER( hanamai ) /* 1988 "Dynax" "Hana no Mai (Japan)" */
	DRIVER( hkagerou ) /* 1997 "Nakanihon + Dynax" "Hana Kagerou [BET] (Japan)" */
	DRIVER( hnayayoi ) /* 1987 "Dyna Electronics" "Hana Yayoi (Japan)" */
	DRIVER( hnfubuki ) /* 1987 "Dynax" "Hana Fubuki [BET] (Japan)" */
	DRIVER( hnkochou ) /* 1989 "Dynax" "Hana Kochou [BET] (Japan)" */
	DRIVER( hnoridur ) /* 1989 "Dynax" "Hana Oriduru (Japan)" */
	DRIVER( inca ) /* 199? "<unknown>" "Inca" */
	DRIVER( jantouki ) /* 1990 "Dynax" "Jong Tou Ki (Japan)" */
	DRIVER( majs101b ) /* 1988 "Dynax" "Mahjong Studio 101 [BET] (Japan)" */
	DRIVER( maya ) /* 1994 "Promat" "Maya" */
	DRIVER( mcnpshnt ) /* 1990 "Dynax" "Mahjong Campus Hunting (Japan)" */
	DRIVER( mjangels ) /* 1991 "Dynax" "Mahjong Angels - Comic Theater Vol.2 (Japan)" */
	DRIVER( mjchuuka ) /* 1998 "Dynax" "Mahjong Chuukanejyo (China)" */
	DRIVER( mjdchuka ) /* 1995 "Dynax" "Mahjong The Dai Chuuka Ken (China */
	DRIVER( mjderngr ) /* 1989 "Dynax" "Mahjong Derringer (Japan)" */
	DRIVER( mjdialq2 ) /* 1991 "Dynax" "Mahjong Dial Q2 (Japan)" */
	DRIVER( mjdiplob ) /* 1987 "Dynax" "Mahjong Diplomat [BET] (Japan)" */
	DRIVER( mjelct3 ) /* 1990 "Dynax" "Mahjong Electron Base (parts 2 & 3 */
	DRIVER( mjelct3a ) /* 1990 "Dynax" "Mahjong Electron Base (parts 2 & 3 */
	DRIVER( mjelctrn ) /* 1993 "Dynax" "Mahjong Electron Base (parts 2 & 4 */
	DRIVER( mjfriday ) /* 1989 "Dynax" "Mahjong Friday (Japan)" */
	DRIVER( mjmyster ) /* 1994 "Dynax" "Mahjong The Mysterious World" */
	DRIVER( mjreach1 ) /* 1998 "Nihon System" "Mahjong Reach Ippatsu (Japan)" */
	DRIVER( mmpanic ) /* 1992 "Nakanihon + East Technology (Taito license)" "Monkey Mole Panic (USA)" */
	DRIVER( neruton ) /* 1990 "Dynax / Yukiyoshi Tokoro" "Mahjong Neruton Haikujirada (Japan)" */
	DRIVER( nettoqc ) /* 1995 "Nakanihon" "Nettoh Quiz Champion (Japan)" */
	DRIVER( pkgnsh ) /* 1998 "Nakanihon / Dynax" "Pachinko Gindama Shoubu (Japan)" */
	DRIVER( pkgnshdx ) /* 1998 "Nakanihon / Dynax" "Pachinko Gindama Shoubu DX (Japan)" */
	DRIVER( quiz365 ) /* 1994 "Nakanihon" "Quiz 365 (Japan)" */
	DRIVER( quiz365t ) /* 1994 "Nakanihon + Taito" "Quiz 365 (Hong Kong & Taiwan)" */
	DRIVER( quiztvqq ) /* 1992 "Dynax" "Quiz TV Gassyuukoku Q&Q (Japan)" */
	DRIVER( rcasino ) /* 1984 "Dyna Electronics" "Royal Casino" */
	DRIVER( realbrk ) /* 1998 "Nakanihon" "Billiard Academy Real Break (Europe)" */
	DRIVER( realbrkj ) /* 1998 "Nakanihon" "Billiard Academy Real Break (Japan)" */
	DRIVER( realbrkk ) /* 1998 "Nakanihon" "Billiard Academy Real Break (Korea)" */
	DRIVER( rongrngg ) /* 1994 "Nakanihon" "Rong Rong (Germany)" */
	DRIVER( rongrong ) /* 1994 "Nakanihon" "Rong Rong (Europe)" */
	DRIVER( royalmah ) /* 1982 "Falcon" "Royal Mahjong (Japan)" */
	DRIVER( shpeng ) /* 1995 "WSAC Systems?" "Sea Hunter Penguin" */
	DRIVER( sprtmtch ) /* 1989 "Dynax (Fabtek license)" "Sports Match" */
	DRIVER( tontonb ) /* 1987 "Dynax" "Tonton [BET] (Japan)" */
	DRIVER( untoucha ) /* 1987 "Dynax" "Untouchable (Japan)" */
	DRIVER( yarunara ) /* 1991 "Dynax" "Mahjong Yarunara (Japan)" */
#endif
#ifdef LINK_EDEVICES
	DRIVER( borntofi ) /* 19?? "International Games" "Born To Fight" */
	DRIVER( diverboy ) /* 1992 "Electronic Devices Italy" "Diver Boy" */
	DRIVER( fantland ) /* 19?? "Electronic Devices Italy" "Fantasy Land (set 1)" */
	DRIVER( fantlnda ) /* 19?? "Electronic Devices Italy" "Fantasy Land (set 2)" */
	DRIVER( galaxygn ) /* 1989 "Electronic Devices Italy" "Galaxy Gunners" */
	DRIVER( mugsmash ) /* 1990? "Electronic Devices Italy / 3D Games England" "Mug Smashers" */
	DRIVER( mwarr ) /* 199? "Elettronica Video-Games S.R.L." "Mighty Warriors" */
	DRIVER( ppmast93 ) /* 1993 "Electronic Devices S.R.L." "Ping Pong Masters '93" */
	DRIVER( stlforce ) /* 1994 "Electronic Devices Italy / Ecogames S.L. Spain" "Steel Force" */
	DRIVER( twinbrat ) /* 1995 "Elettronica Video-Games S.R.L." "Twin Brats" */
	DRIVER( twins ) /* 1994 "Electronic Devices" "Twins" */
#endif
#ifdef LINK_EOLITH
	DRIVER( fort2b ) /* 2001 "Eolith" "Fortress 2 Blue Arcade (ver 1.01 / pcb ver 3.05)" */
	DRIVER( fort2ba ) /* 2001 "Eolith" "Fortress 2 Blue Arcade (ver 1.00 / pcb ver 3.05)" */
	DRIVER( hidctch2 ) /* 1999 "Eolith" "Hidden Catch 2 (pcb ver 3.03)" */
	DRIVER( hidnctch ) /* 1998 "Eolith" "Hidden Catch (World) / Tul Lin Gu Lim Chat Ki '98 (Korea) (pcb ver 3.03)" */
	DRIVER( klondkp ) /* 1999 "Eolith" "KlonDike+" */
	DRIVER( landbrk ) /* 1999 "Eolith" "Land Breaker (World) / Miss Tang Ja Ru Gi (Korea) (pcb ver 3.02)" */
	DRIVER( landbrka ) /* 1999 "Eolith" "Land Breaker (World) / Miss Tang Ja Ru Gi (Korea) (pcb ver 3.03)" */
	DRIVER( nhidctch ) /* 1999 "Eolith" "New Hidden Catch (World) / New Tul Lin Gu Lim Chat Ki '98 (Korea) (pcb ver 3.02)" */
	DRIVER( puzzlekg ) /* 1998 "Eolith" "Puzzle King (Dance & Puzzle)" */
	DRIVER( raccoon ) /* 1998 "Eolith" "Raccoon World" */
#endif
#ifdef LINK_EXCELENT
	DRIVER( aquarium ) /* 1996 "Excellent System" "Aquarium" */
	DRIVER( gcpinbal ) /* 1994 "Excellent System" "Grand Cross" */
	DRIVER( vmetal ) /* 1995 "Excellent Systems" "Varia Metal" */
	DRIVER( vmetaln ) /* 1995 "[Excellent Systems] New Ways Trading Co." "Varia Metal (New Ways Trading Co.)" */
#endif
#ifdef LINK_EXIDY
	DRIVER( carpolo ) /* 1977 "Exidy" "Car Polo" */
	DRIVER( catch22 ) /* 1985 "Exidy" "Catch-22 (version 8.0)" */
	DRIVER( cheyenne ) /* 1984 "Exidy" "Cheyenne (version 1.0)" */
	DRIVER( chiller ) /* 1986 "Exidy" "Chiller (version 3.0)" */
	DRIVER( circus ) /* 1977 "Exidy" "Circus" */
	DRIVER( claypign ) /* 1986 "Exidy" "Clay Pigeon (version 2.0)" */
	DRIVER( combat ) /* 1985 "Exidy" "Combat (version 3.0)" */
	DRIVER( cracksht ) /* 1985 "Exidy" "Crackshot (version 2.0)" */
	DRIVER( crash ) /* 1979 "Exidy" "Crash" */
	DRIVER( crossbow ) /* 1983 "Exidy" "Crossbow (version 2.0)" */
	DRIVER( fax ) /* 1983 "Exidy" "Fax" */
	DRIVER( faxa ) /* 1983 "Exidy" "Fax (alt. questions)" */
	DRIVER( fireone ) /* 1979 "Exidy" "Fire One" */
	DRIVER( hardhat ) /* 1982 "Exidy" "Hard Hat" */
	DRIVER( hitnmis2 ) /* 1987 "Exidy" "Hit 'n Miss (version 2.0)" */
	DRIVER( hitnmiss ) /* 1987 "Exidy" "Hit 'n Miss (version 3.0)" */
	DRIVER( maxaflex ) /* 1984 "Exidy" "Max-A-Flex" */
	DRIVER( mf_achas ) /* 1982 "Exidy / First Star Software" "Astro Chase (Max-A-Flex)" */
	DRIVER( mf_bdash ) /* 1984 "Exidy / First Star Software" "Boulder Dash (Max-A-Flex)" */
	DRIVER( mf_brist ) /* 1983 "Exidy / First Star Software" "Bristles (Max-A-Flex)" */
	DRIVER( mf_flip ) /* 1983 "Exidy / First Star Software" "Flip & Flop (Max-A-Flex)" */
	DRIVER( mtrap ) /* 1981 "Exidy" "Mouse Trap (version 5)" */
	DRIVER( mtrap3 ) /* 1981 "Exidy" "Mouse Trap (version 3)" */
	DRIVER( mtrap4 ) /* 1981 "Exidy" "Mouse Trap (version 4)" */
	DRIVER( pepper2 ) /* 1982 "Exidy" "Pepper II" */
	DRIVER( phantoma ) /* 1980 "Jeutel" "Phantomas" */
	DRIVER( rallys ) /* 1980 "Novar" "Rallys (bootleg?)" */
	DRIVER( ripcord ) /* 1979 "Exidy" "Rip Cord" */
	DRIVER( robotbwl ) /* 1977 "Exidy" "Robot Bowl" */
	DRIVER( showdown ) /* 1988 "Exidy" "Showdown (version 5.0)" */
	DRIVER( sidetrac ) /* 1979 "Exidy" "Side Track" */
	DRIVER( spectar ) /* 1980 "Exidy" "Spectar (revision 3)" */
	DRIVER( spectar1 ) /* 1980 "Exidy" "Spectar (revision 1?)" */
	DRIVER( starfir2 ) /* 1979 "Exidy" "Star Fire 2" */
	DRIVER( starfira ) /* 1979 "Exidy" "Star Fire (set 2)" */
	DRIVER( starfire ) /* 1979 "Exidy" "Star Fire (set 1)" */
	DRIVER( targ ) /* 1980 "Exidy" "Targ" */
	DRIVER( targc ) /* 1980 "Exidy" "Targ (cocktail?)" */
	DRIVER( topgunnr ) /* 1986 "Exidy" "Top Gunner" */
	DRIVER( topsecex ) /* 1986 "Exidy" "Top Secret (Exidy) (version 1.0)" */
	DRIVER( venture ) /* 1981 "Exidy" "Venture (version 5 set 1)" */
	DRIVER( venture2 ) /* 1981 "Exidy" "Venture (version 5 set 2)" */
	DRIVER( venture4 ) /* 1981 "Exidy" "Venture (version 4)" */
	DRIVER( victorba ) /* 1982 "Exidy" "Victor Banana" */
	DRIVER( victory ) /* 1982 "Exidy" "Victory" */
	DRIVER( whodunit ) /* 1988 "Exidy" "Who Dunit (version 8.0)" */
#endif
#ifdef LINK_F32
	DRIVER( crospang ) /* 1998 "F2 System" "Cross Pang" */
	DRIVER( mosaicf2 ) /* 1999 "F2 System" "Mosaic (F2 System)" */
#endif
#ifdef LINK_FUUKI
	DRIVER( asurabld ) /* 1998 "Fuuki" "Asura Blade - Sword of Dynasty (Japan)" */
	DRIVER( gogomile ) /* 1995 "Fuuki" "Go Go! Mile Smile" */
	DRIVER( gogomilj ) /* 1995 "Fuuki" "Susume! Mile Smile (Japan)" */
	DRIVER( pbancho ) /* 1996 "Fuuki" "Gyakuten!! Puzzle Bancho (Japan)" */
#endif
#ifdef LINK_GAELCO
	DRIVER( aligator ) /* 1994 "Gaelco" "Alligator Hunt" */
	DRIVER( aligatun ) /* 1994 "Gaelco" "Alligator Hunt (unprotected)" */
	DRIVER( bang ) /* 1998 "Gaelco" "Bang!" */
	DRIVER( bangj ) /* 1998 "Gaelco" "Gun Gabacho (Japan)" */
	DRIVER( bigkarnk ) /* 1991 "Gaelco" "Big Karnak" */
	DRIVER( biomtoy ) /* 1995 "Gaelco" "Biomechanical Toy (unprotected)" */
	DRIVER( glass ) /* 1993 "Gaelco" "Glass (Ver 1.1)" */
	DRIVER( glass10 ) /* 1993 "Gaelco" "Glass (Ver 1.0)" */
	DRIVER( glass10a ) /* 1993 "Gaelco" "Glass (Ver 1.0 set 2)" */
	DRIVER( maniacsp ) /* 1996 "Gaelco" "Maniac Square (prototype)" */
	DRIVER( maniacsq ) /* 1996 "Gaelco" "Maniac Square (unprotected)" */
	DRIVER( mastboy ) /* 1991 "Gaelco" "Master Boy (Spanish */
	DRIVER( mastboyi ) /* 1991 "Gaelco" "Master Boy (Italian */
	DRIVER( paintlad ) /* 1992 "Gaelco" "Painted Lady (Splash) (Ver. 1.3 US)" */
	DRIVER( radikalb ) /* 1998 "Gaelco" "Radikal Bikers" */
	DRIVER( roldfrga ) /* 1993 "Microhard" "The Return of Lady Frog (set 2)" */
	DRIVER( roldfrog ) /* 1993 "Microhard" "The Return of Lady Frog" */
	DRIVER( snowbalt ) /* 1996 "Gaelco" "Snow Board Championship (set 2)" */
	DRIVER( snowboar ) /* 1996 "Gaelco" "Snow Board Championship (set 1)" */
	DRIVER( speedup ) /* 1996 "Gaelco" "Speed Up" */
	DRIVER( splash ) /* 1992 "Gaelco" "Splash! (Ver. 1.2 World)" */
	DRIVER( splash10 ) /* 1992 "Gaelco" "Splash! (Ver. 1.0 World)" */
	DRIVER( squash ) /* 1992 "Gaelco" "Squash (Ver. 1.0)" */
	DRIVER( surfplnt ) /* 1997 "Gaelco" "Surf Planet" */
	DRIVER( targeth ) /* 1994 "Gaelco" "Target Hits" */
	DRIVER( thoop ) /* 1992 "Gaelco" "Thunder Hoop (Ver. 1)" */
	DRIVER( thoop2 ) /* 1994 "Gaelco" "TH Strikes Back" */
	DRIVER( wrally2 ) /* 1995 "Gaelco" "World Rally 2: Twin Racing" */
	DRIVER( xorworld ) /* 1990 "Gaelco" "Xor World (prototype)" */
#endif
#ifdef LINK_GAMEPLAN
	DRIVER( challeng ) /* 1981 "GamePlan (Centuri license)" "Challenger" */
	DRIVER( enigma2 ) /* 1981 "GamePlan (Zilec Electronics license)" "Enigma 2" */
	DRIVER( enigma2a ) /* 1984 "Zilec Electronics" "Enigma 2 (Space Invaders Hardware)" */
	DRIVER( kaos ) /* 1981 "GamePlan" "Kaos" */
	DRIVER( killcom ) /* 1980 "GamePlan (Centuri license)" "Killer Comet" */
	DRIVER( megatack ) /* 1980 "GamePlan (Centuri license)" "Megatack" */
#endif
#ifdef LINK_GAMETRON
	DRIVER( gotya ) /* 1981 "Game-A-Tron" "Got-Ya (12/24/1981 */
	DRIVER( thehand ) /* 1981 "T.I.C." "The Hand" */
#endif
#ifdef LINK_GOTTLIEB
	DRIVER( 3stooges ) /* 1984 "Mylstar" "The Three Stooges In Brides Is Brides" */
	DRIVER( argusg ) /* 1984 "Gottlieb" "Argus (Gottlieb */
	DRIVER( curvebal ) /* 1984 "Mylstar" "Curve Ball" */
	DRIVER( exterm ) /* 1989 "Gottlieb / Premier Technology" "Exterminator" */
	DRIVER( insector ) /* 1982 "Gottlieb" "Insector (prototype)" */
	DRIVER( kngtmare ) /* 1983 "Gottlieb" "Knightmare (prototype)" */
	DRIVER( krull ) /* 1983 "Gottlieb" "Krull" */
	DRIVER( mplanets ) /* 1983 "Gottlieb" "Mad Planets" */
	DRIVER( mplanuk ) /* 1983 "Gottlieb (Taitel license)" "Mad Planets (UK)" */
	DRIVER( myqbert ) /* 1982 "Gottlieb" "Mello Yello Q*bert" */
	DRIVER( qbert ) /* 1982 "Gottlieb" "Q*bert (US set 1)" */
	DRIVER( qberta ) /* 1982 "Gottlieb" "Q*bert (US set 2)" */
	DRIVER( qbertjp ) /* 1982 "Gottlieb (Konami license)" "Q*bert (Japan)" */
	DRIVER( qbertqub ) /* 1983 "Mylstar" "Q*bert's Qubes" */
	DRIVER( qberttst ) /* 1982 "Gottlieb" "Q*bert (early test version)" */
	DRIVER( qbtrktst ) /* 1982 "Gottlieb" "Q*bert Board Input Test Rom" */
	DRIVER( reactor ) /* 1982 "Gottlieb" "Reactor" */
	DRIVER( screwloo ) /* 1983 "Mylstar" "Screw Loose (prototype)" */
	DRIVER( sqbert ) /* 1983 "Mylstar" "Faster */
	DRIVER( tylz ) /* 1982 "Mylstar" "Tylz (prototype)" */
	DRIVER( vidvince ) /* 1984 "Mylstar" "Video Vince and the Game Factory (prototype)" */
	DRIVER( wizwarz ) /* 1984 "Mylstar" "Wiz Warz (prototype)" */
#endif
#ifdef LINK_GREYHND
	DRIVER( findout ) /* 1987 "Elettronolo" "Find Out (Version 4.04)" */
	DRIVER( gepoker ) /* 1984 "Greyhound Electronics" "Poker (Version 50.02 ICB)" */
	DRIVER( gepoker2 ) /* 1984 "Greyhound Electronics" "Poker (Version 50.02 ICB set 2)" */
	DRIVER( gepoker3 ) /* 1984 "Greyhound Electronics" "Poker (Version 50.02 ICB set 3)" */
	DRIVER( gs4002 ) /* 1982 "G.E.I." "Selection (Version 40.02TMB) set 1" */
	DRIVER( gs4002a ) /* 1982 "G.E.I." "Selection (Version 40.02TMB) set 2" */
	DRIVER( gt102b ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.02B)" */
	DRIVER( gt102c ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.02C)" */
	DRIVER( gt102c1 ) /* 1985 "Greyhound Electronics" "Trivia (Version 1.02C Alt questions 1)" */
	DRIVER( gt102c2 ) /* 1985 "Greyhound Electronics" "Trivia (Version 1.02C Alt questions 2)" */
	DRIVER( gt102c3 ) /* 1985 "Greyhound Electronics" "Trivia (Version 1.02C Alt questions 3)" */
	DRIVER( gt103 ) /* 1986 "Grayhound Electronics" "Trivia (Version 1.03)" */
	DRIVER( gt103a ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a)" */
	DRIVER( gt103a1 ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a) (alt 1)" */
	DRIVER( gt103a2 ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a) (alt 2)" */
	DRIVER( gt103a3 ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a) (alt 3)" */
	DRIVER( gt103aa ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a Alt questions 1)" */
	DRIVER( gt103ab ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a Alt questions 2)" */
	DRIVER( gt103asx ) /* 1984 "Greyhound Electronics" "Trivia (Version 1.03a Sex questions)" */
	DRIVER( gt507uk ) /* 1986 "Grayhound Electronics" "Trivia (UK Version 5.07)" */
	DRIVER( quiz ) /* 1986 "Italian bootleg" "Quiz (Revision 2)" */
	DRIVER( quiz211 ) /* 1991 "Elettronolo" "Quiz (Revision 2.1)" */
	DRIVER( sextriv1 ) /* 1985 "Kinky Kit and Game Co." "Sexual Trivia (Version 1.02SB set 1)" */
	DRIVER( sextriv2 ) /* 1985 "Kinky Kit and Game Co." "Sexual Trivia (Version 1.02SB set 2)" */
#endif
#ifdef LINK_IGS
	DRIVER( chindrag ) /* 1995 "IGS / ALTA" "Zhong Guo Long (Japan */
	DRIVER( chmplst2 ) /* 1996 "IGS" "Long Hu Bang II" */
	DRIVER( chugokur ) /* 1995 "IGS / ALTA" "Zhong Guo Long (Japan */
	DRIVER( csk227it ) /* 198? "IGS" "Champion Skill (with Ability)" */
	DRIVER( csk234it ) /* 198? "IGS" "Champion Skill (Ability */
	DRIVER( drgnwrld ) /* 1997 "IGS" "Dragon World (World */
	DRIVER( drgw2 ) /* 1997 "IGS" "Dragon World II (ver. 110X */
	DRIVER( drgw2c ) /* 1997 "IGS" "Zhong Guo Long II (ver. 100C */
	DRIVER( drgw2j ) /* 1997 "IGS" "Chuugokuryuu II (ver. 100J */
	DRIVER( drgwrld3 ) /* 1995 "IGS" "Dragon World (World */
	DRIVER( goldstar ) /* 199? "IGS" "Golden Star" */
	DRIVER( goldstbl ) /* 199? "IGS" "Golden Star (Blue version)" */
	DRIVER( grndtour ) /* 1993 "IGS" "Grand Tour" */
	DRIVER( grtwall ) /* 1996 "IGS" "Wan Li Chang Cheng (V638C)" */
	DRIVER( iqblock ) /* 1993 "IGS" "IQ-Block" */
	DRIVER( killbld ) /* 1998 "IGS" "The Killing Blade" */
	DRIVER( killbldt ) /* 1998 "IGS" "The Killing Blade (Chinese Board)" */
	DRIVER( kov ) /* 1999 "IGS" "Knights of Valour / Sangoku Senki (ver. 117)" */
	DRIVER( kov115 ) /* 1999 "IGS" "Knights of Valour / Sangoku Senki (ver. 115)" */
	DRIVER( kovj ) /* 1999 "IGS" "Knights of Valour / Sangoku Senki (ver. 100 */
	DRIVER( kovplus ) /* 1999 "IGS" "Knights of Valour Plus / Sangoku Senki Plus (ver. 119)" */
	DRIVER( kovplusa ) /* 1999 "IGS" "Knights of Valour Plus / Sangoku Senki Plus (alt ver. 119)" */
	DRIVER( lhb ) /* 1995 "IGS" "Long Hu Bang" */
	DRIVER( lordgun ) /* 1994 "IGS" "Lord of Gun (USA)" */
	DRIVER( moonlght ) /* 199? "unknown" "Moon Light" */
	DRIVER( orld105k ) /* 1997 "IGS" "Oriental Legend / Xi Yo Gi Shi Re Zuang (ver. 105 */
	DRIVER( orld111c ) /* 1997 "IGS" "Oriental Legend / Xi Yo Gi Shi Re Zuang (ver. 111 */
	DRIVER( orlegend ) /* 1997 "IGS" "Oriental Legend / Xi Yo Gi Shi Re Zuang (ver. 126)" */
	DRIVER( orlegndc ) /* 1997 "IGS" "Oriental Legend / Xi Yo Gi Shi Re Zuang (ver. 112 */
	DRIVER( orlegnde ) /* 1997 "IGS" "Oriental Legend / Xi Yo Gi Shi Re Zuang (ver. 112)" */
	DRIVER( pgm ) /* 1997 "IGS" "PGM (Polygame Master) System BIOS" */
	DRIVER( photoy2k ) /* 1999 "IGS" "Photo Y2K" */
	DRIVER( puzlstar ) /* 1999 "IGS" "Puzzle Star" */
	DRIVER( raf102j ) /* 1999 "IGS" "Real and Fake / Photo Y2K (ver. 102 */
	DRIVER( vbowl ) /* 1996 "Alta / IGS" "Virtua Bowling (World */
	DRIVER( vbowlj ) /* 1996 "Alta / IGS" "Virtua Bowling (Japan */
	DRIVER( xymg ) /* 1996 "IGS" "Xing Yen Man Guan (V651C)" */
#endif
#ifdef LINK_IREM
	DRIVER( airduel ) /* 1990 "Irem" "Air Duel (Japan)" */
	DRIVER( atomboy ) /* 1985 "Irem (Memetron license)" "Atomic Boy" */
	DRIVER( atompunk ) /* 1991 "Irem America (licensed from Hudson Soft)" "Atomic Punk (US)" */
	DRIVER( battroad ) /* 1984 "Irem" "The Battle-Road" */
	DRIVER( bbmanw ) /* 1992 "Irem" "Bomber Man World / New Dyna Blaster - Global Quest" */
	DRIVER( bbmanwj ) /* 1992 "Irem" "Bomber Man World (Japan)" */
	DRIVER( bchopper ) /* 1987 "Irem" "Battle Chopper" */
	DRIVER( bmaster ) /* 1991 "Irem" "Blade Master (World)" */
	DRIVER( bombrman ) /* 1991 "Irem (licensed from Hudson Soft)" "Bomber Man (Japan)" */
	DRIVER( buccanra ) /* 1989 "Duintronic" "Buccaneers (set 2)" */
	DRIVER( buccanrs ) /* 1989 "Duintronic" "Buccaneers (set 1)" */
	DRIVER( cosmccop ) /* 1991 "Irem" "Cosmic Cop (World)" */
	DRIVER( dbreed ) /* 1989 "Irem" "Dragon Breed (M81 pcb version)" */
	DRIVER( dbreed72 ) /* 1989 "Irem" "Dragon Breed (M72 pcb version)" */
	DRIVER( demoneye ) /* 1981 "Irem" "Demoneye-X" */
	DRIVER( dkgenm72 ) /* 1990 "Irem" "Daiku no Gensan (Japan */
	DRIVER( dkgensan ) /* 1990 "Irem" "Daiku no Gensan (Japan)" */
	DRIVER( dsccr94j ) /* 1994 "Irem" "Dream Soccer '94 (Japan)" */
	DRIVER( dsoccr94 ) /* 1994 "Irem (Data East Corporation license)" "Dream Soccer '94" */
	DRIVER( dynablst ) /* 1991 "Irem (licensed from Hudson Soft)" "Dynablaster / Bomber Man" */
	DRIVER( fghtbskt ) /* 1984 "Paradise Co. Ltd." "Fighting Basketball" */
	DRIVER( firebarr ) /* 1993 "Irem" "Fire Barrel (Japan)" */
	DRIVER( gallop ) /* 1991 "Irem" "Gallop - Armed police Unit (Japan)" */
	DRIVER( geostorm ) /* 1994 "Irem" "Geostorm (Japan)" */
	DRIVER( gunforc2 ) /* 1994 "Irem" "Gunforce 2 (US)" */
	DRIVER( gunforce ) /* 1991 "Irem" "Gunforce - Battle Fire Engulfed Terror Island (World)" */
	DRIVER( gunforcj ) /* 1991 "Irem" "Gunforce - Battle Fire Engulfed Terror Island (Japan)" */
	DRIVER( gunforcu ) /* 1991 "Irem America" "Gunforce - Battle Fire Engulfed Terror Island (US)" */
	DRIVER( gunhohki ) /* 1992 "Irem" "Gun Hohki (Japan)" */
	DRIVER( hasamu ) /* 1991 "Irem" "Hasamu (Japan)" */
	DRIVER( headoni ) /* 1979? "Irem" "Head On (Irem */
	DRIVER( hharry ) /* 1990 "Irem" "Hammerin' Harry (World)" */
	DRIVER( hharryu ) /* 1990 "Irem America" "Hammerin' Harry (US)" */
	DRIVER( hook ) /* 1992 "Irem" "Hook (World)" */
	DRIVER( hookj ) /* 1992 "Irem" "Hook (Japan)" */
	DRIVER( hooku ) /* 1992 "Irem America" "Hook (US)" */
	DRIVER( horizon ) /* 1985 "Irem" "Horizon" */
	DRIVER( imgfight ) /* 1988 "Irem" "Image Fight (Japan)" */
	DRIVER( inthunt ) /* 1993 "Irem" "In The Hunt (World)" */
	DRIVER( inthuntu ) /* 1993 "Irem America" "In The Hunt (US)" */
	DRIVER( ipminvad ) /* 1979? "Irem" "IPM Invader" */
	DRIVER( kaiteids ) /* 1993 "Irem" "Kaitei Daisensou (Japan)" */
	DRIVER( kengo ) /* 1991 "Irem" "Ken-Go" */
	DRIVER( kidniki ) /* 1986 "Irem" "Kid Niki - Radical Ninja (World)" */
	DRIVER( kidnikiu ) /* 1986 "Irem (Data East USA license)" "Kid Niki - Radical Ninja (US)" */
	DRIVER( kikcubib ) /* 1988 "bootleg" "Kickle Cubele" */
	DRIVER( kikcubic ) /* 1988 "Irem" "Meikyu Jima (Japan)" */
	DRIVER( kungfub ) /* 1984 "bootleg" "Kung-Fu Master (bootleg set 1)" */
	DRIVER( kungfub2 ) /* 1984 "bootleg" "Kung-Fu Master (bootleg set 2)" */
	DRIVER( kungfud ) /* 1984 "Irem (Data East license)" "Kung-Fu Master (Data East)" */
	DRIVER( kungfum ) /* 1984 "Irem" "Kung-Fu Master" */
	DRIVER( ldrun ) /* 1984 "Irem (licensed from Broderbund)" "Lode Runner (set 1)" */
	DRIVER( ldrun2 ) /* 1984 "Irem (licensed from Broderbund)" "Lode Runner II - The Bungeling Strikes Back" */
	DRIVER( ldrun3 ) /* 1985 "Irem (licensed from Broderbund)" "Lode Runner III - The Golden Labyrinth" */
	DRIVER( ldrun3jp ) /* 1985 "Irem (licensed from Broderbund)" "Lode Runner III - Majin No Fukkatsu" */
	DRIVER( ldrun4 ) /* 1986 "Irem (licensed from Broderbund)" "Lode Runner IV - Teikoku Karano Dasshutsu" */
	DRIVER( ldruna ) /* 1984 "Irem (licensed from Broderbund)" "Lode Runner (set 2)" */
	DRIVER( leaguemn ) /* 1993 "Irem" "Yakyuu Kakutou League-Man (Japan)" */
	DRIVER( lethalth ) /* 1991 "Irem" "Lethal Thunder (World)" */
	DRIVER( lithero ) /* 1987 "bootleg" "Little Hero" */
	DRIVER( loht ) /* 1989 "Irem" "Legend of Hero Tonma" */
	DRIVER( lotlot ) /* 1985 "Irem (licensed from Tokuma Shoten)" "Lot Lot" */
	DRIVER( majtitl2 ) /* 1992 "Irem" "Major Title 2 (World)" */
	DRIVER( majtitle ) /* 1990 "Irem" "Major Title (Japan)" */
	DRIVER( matchit ) /* 1989 "Tamtex" "Match It" */
	DRIVER( motorace ) /* 1983 "Irem (Williams license)" "MotoRace USA" */
	DRIVER( mpatrol ) /* 1982 "Irem" "Moon Patrol" */
	DRIVER( mpatrolw ) /* 1982 "Irem (Williams license)" "Moon Patrol (Williams)" */
	DRIVER( mrheli ) /* 1987 "Irem" "Mr. HELI no Dai-Bouken" */
	DRIVER( mysticri ) /* 1992 "Irem" "Mystic Riders (World)" */
	DRIVER( nbbatman ) /* 1993 "Irem America" "Ninja Baseball Batman (US)" */
	DRIVER( newapunk ) /* 1992 "Irem America" "New Atomic Punk - Global Quest (US)" */
	DRIVER( nspirit ) /* 1988 "Irem" "Ninja Spirit" */
	DRIVER( nspiritj ) /* 1988 "Irem" "Saigo no Nindou (Japan)" */
	DRIVER( olibochu ) /* 1981 "Irem + GDI" "Oli-Boo-Chu" */
	DRIVER( poundfor ) /* 1990 "Irem" "Pound for Pound (World)" */
	DRIVER( poundfou ) /* 1990 "Irem America" "Pound for Pound (US)" */
	DRIVER( psoldier ) /* 1993 "Irem" "Perfect Soldiers (Japan)" */
	DRIVER( quizf1 ) /* 1992 "Irem" "Quiz F-1 1 */
	DRIVER( redalert ) /* 1981 "Irem + GDI" "Red Alert" */
	DRIVER( rtype ) /* 1987 "Irem" "R-Type (Japan)" */
	DRIVER( rtype2 ) /* 1989 "Irem" "R-Type II" */
	DRIVER( rtype2j ) /* 1989 "Irem" "R-Type II (Japan)" */
	DRIVER( rtypelej ) /* 1992 "Irem" "R-Type Leo (Japan)" */
	DRIVER( rtypeleo ) /* 1992 "Irem" "R-Type Leo (World)" */
	DRIVER( rtypepj ) /* 1987 "Irem" "R-Type (Japan prototype)" */
	DRIVER( rtypeu ) /* 1987 "Irem (Nintendo of America license)" "R-Type (US)" */
	DRIVER( shisen ) /* 1989 "Tamtex" "Shisensho - Joshiryo-Hen (Japan)" */
	DRIVER( shtrider ) /* 1985 "Seibu Kaihatsu" "Shot Rider" */
	DRIVER( shtridra ) /* 1984 "Seibu Kaihatsu (Sigma license)" "Shot Rider (Sigma license)" */
	DRIVER( sichuan2 ) /* 1989 "Tamtex" "Sichuan II (hack?) (set 1)" */
	DRIVER( sichuana ) /* 1989 "Tamtex" "Sichuan II (hack?) (set 2)" */
	DRIVER( skingam2 ) /* 1992 "Irem America" "The Irem Skins Game (US set 2)" */
	DRIVER( skingame ) /* 1992 "Irem America" "The Irem Skins Game (US set 1)" */
	DRIVER( skychut ) /* 1980 "Irem" "Sky Chuter" */
	DRIVER( spacbeam ) /* 1979 "Irem" "Space Beam" */
	DRIVER( spartanx ) /* 1984 "Irem" "Spartan X (Japan)" */
	DRIVER( spelnkrj ) /* 1985 "Irem (licensed from Broderbund)" "Spelunker (Japan)" */
	DRIVER( spelunk2 ) /* 1986 "Irem (licensed from Broderbund)" "Spelunker II" */
	DRIVER( spelunkr ) /* 1985 "Irem (licensed from Broderbund)" "Spelunker" */
	DRIVER( ssoldier ) /* 1993 "Irem America" "Superior Soldiers (US)" */
	DRIVER( thndblst ) /* 1991 "Irem" "Thunder Blaster (Japan)" */
	DRIVER( travrusa ) /* 1983 "Irem" "Traverse USA / Zippy Race" */
	DRIVER( troangel ) /* 1983 "Irem" "Tropical Angel" */
	DRIVER( uccops ) /* 1992 "Irem" "Undercover Cops (World)" */
	DRIVER( uccopsj ) /* 1992 "Irem" "Undercover Cops (Japan)" */
	DRIVER( vigilant ) /* 1988 "Irem" "Vigilante (World)" */
	DRIVER( vigilntj ) /* 1988 "Irem" "Vigilante (Japan)" */
	DRIVER( vigilntu ) /* 1988 "Irem (Data East USA license)" "Vigilante (US)" */
	DRIVER( vsyard ) /* 1984 "Irem" "Vs 10-Yard Fight (World */
	DRIVER( vsyard2 ) /* 1984 "Irem" "Vs 10-Yard Fight (Japan)" */
	DRIVER( wilytowr ) /* 1984 "Irem" "Wily Tower" */
	DRIVER( xmultipl ) /* 1989 "Irem" "X Multiply (Japan)" */
	DRIVER( yanchamr ) /* 1986 "Irem" "Kaiketsu Yanchamaru (Japan)" */
	DRIVER( yard ) /* 1983 "Irem" "10-Yard Fight (Japan)" */
	DRIVER( youjyudn ) /* 1986 "Irem" "Youjyuden (Japan)" */
#endif
#ifdef LINK_ITECH
	DRIVER( arlingtn ) /* 1991 "Strata/Incredible Technologies" "Arlington Horse Racing (v1.21-D)" */
	DRIVER( bloods11 ) /* 1994 "Strata/Incredible Technologies" "Blood Storm (v1.10)" */
	DRIVER( bloods21 ) /* 1994 "Strata/Incredible Technologies" "Blood Storm (v2.10)" */
	DRIVER( bloods22 ) /* 1994 "Strata/Incredible Technologies" "Blood Storm (v2.20)" */
	DRIVER( bloodstm ) /* 1994 "Strata/Incredible Technologies" "Blood Storm (v2.22)" */
	DRIVER( bowlrama ) /* 1991 "P&P Marketing" "Bowl-O-Rama" */
	DRIVER( capbowl ) /* 1988 "Incredible Technologies" "Capcom Bowling (set 1)" */
	DRIVER( capbowl2 ) /* 1988 "Incredible Technologies" "Capcom Bowling (set 2)" */
	DRIVER( capbowl3 ) /* 1988 "Incredible Technologies" "Capcom Bowling (set 3)" */
	DRIVER( capbowl4 ) /* 1988 "Incredible Technologies" "Capcom Bowling (set 4)" */
	DRIVER( clbowl ) /* 1989 "Incredible Technologies" "Coors Light Bowling" */
	DRIVER( drivedge ) /* 1994 "Strata/Incredible Technologies" "Driver's Edge" */
	DRIVER( dynobop ) /* 1990 "Grand Products/Incredible Technologies" "Dyno Bop" */
	DRIVER( gpgolf ) /* 1992 "Strata/Incredible Technologies" "Golden Par Golf (Joystick */
	DRIVER( gt2k ) /* 2000 "Incredible Technologies" "Golden Tee 2K (v1.00)" */
	DRIVER( gt2ks100 ) /* 2000 "Incredible Technologies" "Golden Tee 2K (v1.00S)" */
	DRIVER( gt2kt500 ) /* 2000 "Incredible Technologies" "Golden Tee 2K Tournament (v5.00)" */
	DRIVER( gt3d ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.93N)" */
	DRIVER( gt3dl191 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.91L)" */
	DRIVER( gt3dl192 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.92L)" */
	DRIVER( gt3ds192 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.92S)" */
	DRIVER( gt3dt211 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf Tournament (v2.11)" */
	DRIVER( gt3dt231 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf Tournament (v2.31)" */
	DRIVER( gt3dv14 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.4)" */
	DRIVER( gt3dv15 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.5)" */
	DRIVER( gt3dv16 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.6)" */
	DRIVER( gt3dv17 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.7)" */
	DRIVER( gt3dv18 ) /* 1995 "Incredible Technologies" "Golden Tee 3D Golf (v1.8)" */
	DRIVER( gt97 ) /* 1997 "Incredible Technologies" "Golden Tee '97 (v1.30)" */
	DRIVER( gt97s121 ) /* 1997 "Incredible Technologies" "Golden Tee '97 (v1.21S)" */
	DRIVER( gt97t240 ) /* 1997 "Incredible Technologies" "Golden Tee '97 Tournament (v2.40)" */
	DRIVER( gt97v120 ) /* 1997 "Incredible Technologies" "Golden Tee '97 (v1.20)" */
	DRIVER( gt97v121 ) /* 1997 "Incredible Technologies" "Golden Tee '97 (v1.21)" */
	DRIVER( gt97v122 ) /* 1997 "Incredible Technologies" "Golden Tee '97 (v1.22)" */
	DRIVER( gt98 ) /* 1998 "Incredible Technologies" "Golden Tee '98 (v1.10)" */
	DRIVER( gt98s100 ) /* 1998 "Incredible Technologies" "Golden Tee '98 (v1.00S)" */
	DRIVER( gt98t303 ) /* 1998 "Incredible Technologies" "Golden Tee '98 Tournament (v3.03)" */
	DRIVER( gt98v100 ) /* 1998 "Incredible Technologies" "Golden Tee '98 (v1.00)" */
	DRIVER( gt99 ) /* 1999 "Incredible Technologies" "Golden Tee '99 (v1.00)" */
	DRIVER( gt99s100 ) /* 1999 "Incredible Technologies" "Golden Tee '99 (v1.00S)" */
	DRIVER( gt99t400 ) /* 1999 "Incredible Technologies" "Golden Tee '99 Tournament (v4.00)" */
	DRIVER( gtclassc ) /* 2001 "Incredible Technologies" "Golden Tee Classic (v1.00)" */
	DRIVER( gtclassp ) /* 2001 "Incredible Technologies" "Golden Tee Classic (v1.00) Alt" */
	DRIVER( gtcls100 ) /* 2001 "Incredible Technologies" "Golden Tee Classic (v1.00S)" */
	DRIVER( gtg ) /* 1990 "Strata/Incredible Technologies" "Golden Tee Golf (Joystick */
	DRIVER( gtg2 ) /* 1992 "Strata/Incredible Technologies" "Golden Tee Golf II (Trackball */
	DRIVER( gtg2j ) /* 1991 "Strata/Incredible Technologies" "Golden Tee Golf II (Joystick */
	DRIVER( gtg2t ) /* 1989 "Strata/Incredible Technologies" "Golden Tee Golf II (Trackball */
	DRIVER( gtgt ) /* 1989 "Strata/Incredible Technologies" "Golden Tee Golf (Trackball */
	DRIVER( gtroyal ) /* 1999 "Incredible Technologies" "Golden Tee Royal Edition Tournament (v4.02)" */
	DRIVER( gtsuprem ) /* 2002 "Incredible Technologies" "Golden Tee Supreme Edition Tournament (v5.10)" */
	DRIVER( hardyard ) /* 1993 "Strata/Incredible Technologies" "Hard Yardage (v1.20)" */
	DRIVER( hardyd10 ) /* 1993 "Strata/Incredible Technologies" "Hard Yardage (v1.00)" */
	DRIVER( hotmemry ) /* 1994 "Tuning/Incredible Technologies" "Hot Memory (V1.2 */
	DRIVER( hstenn10 ) /* 1990 "Strata/Incredible Technologies" "Hot Shots Tennis (V1.0)" */
	DRIVER( hstennis ) /* 1990 "Strata/Incredible Technologies" "Hot Shots Tennis (V1.1)" */
	DRIVER( neckneck ) /* 1992 "Bundra Games/Incredible Technologies" "Neck-n-Neck (v1.2)" */
	DRIVER( ninclown ) /* 1991 "Strata/Incredible Technologies" "Ninja Clowns (08/27/91)" */
	DRIVER( pairs ) /* 1994 "Strata/Incredible Technologies" "Pairs (V1.2 */
	DRIVER( pairsa ) /* 1994 "Strata/Incredible Technologies" "Pairs (09/07/94)" */
	DRIVER( peggle ) /* 1991 "Strata/Incredible Technologies" "Peggle (Joystick */
	DRIVER( pegglet ) /* 1991 "Strata/Incredible Technologies" "Peggle (Trackball */
	DRIVER( pokrdice ) /* 1991 "Strata/Incredible Technologies" "Poker Dice" */
	DRIVER( rimrck12 ) /* 1991 "Strata/Incredible Technologies" "Rim Rockin' Basketball (V1.2)" */
	DRIVER( rimrck16 ) /* 1991 "Strata/Incredible Technologies" "Rim Rockin' Basketball (V1.6)" */
	DRIVER( rimrck20 ) /* 1991 "Strata/Incredible Technologies" "Rim Rockin' Basketball (V2.0)" */
	DRIVER( rimrockn ) /* 1991 "Strata/Incredible Technologies" "Rim Rockin' Basketball (V2.2)" */
	DRIVER( sftm ) /* 1995 "Capcom/Incredible Technologies" "Street Fighter: The Movie (v1.12)" */
	DRIVER( sftm110 ) /* 1995 "Capcom/Incredible Technologies" "Street Fighter: The Movie (v1.10)" */
	DRIVER( sftm111 ) /* 1995 "Capcom/Incredible Technologies" "Street Fighter: The Movie (v1.11)" */
	DRIVER( sftmj ) /* 1995 "Capcom/Incredible Technologies" "Street Fighter: The Movie (v1.12N */
	DRIVER( shufshot ) /* 1997 "Strata/Incredible Technologies" "Shuffleshot (v1.40)" */
	DRIVER( sliksh16 ) /* 1990 "Grand Products/Incredible Technologies" "Slick Shot (V1.6)" */
	DRIVER( sliksh17 ) /* 1990 "Grand Products/Incredible Technologies" "Slick Shot (V1.7)" */
	DRIVER( slikshot ) /* 1990 "Grand Products/Incredible Technologies" "Slick Shot (V2.2)" */
	DRIVER( sshot137 ) /* 1997 "Strata/Incredible Technologies" "Shuffleshot (v1.37)" */
	DRIVER( sshot139 ) /* 1997 "Strata/Incredible Technologies" "Shuffleshot (v1.39)" */
	DRIVER( sstrike ) /* 1990 "Strata/Incredible Technologies" "Super Strike Bowling" */
	DRIVER( stratab ) /* 1990 "Strata/Incredible Technologies" "Strata Bowling (V3)" */
	DRIVER( stratab1 ) /* 1990 "Strata/Incredible Technologies" "Strata Bowling (V1)" */
	DRIVER( timek131 ) /* 1992 "Strata/Incredible Technologies" "Time Killers (v1.31)" */
	DRIVER( timekill ) /* 1992 "Strata/Incredible Technologies" "Time Killers (v1.32)" */
	DRIVER( wcbowl ) /* 1995 "Incredible Technologies" "World Class Bowling (v1.66)" */
	DRIVER( wcbowldx ) /* 1999 "Incredible Technologies" "World Class Bowling Deluxe (v2.00)" */
	DRIVER( wcbwl12 ) /* 1995 "Incredible Technologies" "World Class Bowling (v1.2)" */
	DRIVER( wcbwl140 ) /* 1997 "Incredible Technologies" "World Class Bowling Tournament (v1.40)" */
	DRIVER( wcbwl161 ) /* 1995 "Incredible Technologies" "World Class Bowling (v1.61)" */
	DRIVER( wcbwl165 ) /* 1995 "Incredible Technologies" "World Class Bowling (v1.65)" */
	DRIVER( wfortuna ) /* 1989 "GameTek" "Wheel Of Fortune (alternate)" */
	DRIVER( wfortune ) /* 1989 "GameTek" "Wheel Of Fortune" */
#endif
#ifdef LINK_JALECO
	DRIVER( 47pie2 ) /* 1994 "Jaleco" "Idol Janshi Su-Chi-Pie 2 (v1.1)" */
	DRIVER( 47pie2o ) /* 1994 "Jaleco" "Idol Janshi Su-Chi-Pie 2 (v1.0)" */
	DRIVER( 64streej ) /* 1991 "Jaleco" "64th. Street - A Detective Story (Japan)" */
	DRIVER( 64street ) /* 1991 "Jaleco" "64th. Street - A Detective Story (World)" */
	DRIVER( aeroboto ) /* 1984 "[Jaleco] (Williams license)" "Aeroboto" */
	DRIVER( akiss ) /* 1995 "Jaleco" "Mahjong Angel Kiss" */
	DRIVER( argus ) /* 1986 "[NMK] (Jaleco license)" "Argus" */
	DRIVER( armchmp2 ) /* 1992 "Jaleco" "Arm Champs II" */
	DRIVER( astyanax ) /* 1989 "Jaleco" "The Astyanax" */
	DRIVER( avspirit ) /* 1991 "Jaleco" "Avenging Spirit" */
	DRIVER( bbbxing ) /* 1994 "Jaleco" "Best Bout Boxing" */
	DRIVER( bestleag ) /* 1993 "bootleg" "Best League" */
	DRIVER( bigrun ) /* 1989 "Jaleco" "Big Run (11th Rallye version)" */
	DRIVER( bigstrik ) /* 1992 "Jaleco" "Big Striker" */
	DRIVER( bigstrkb ) /* 1992 "bootleg" "Big Striker (bootleg)" */
	DRIVER( blueprnj ) /* 1982 "[Zilec Electronics] Jaleco" "Blue Print (Jaleco)" */
	DRIVER( blueprnt ) /* 1982 "[Zilec Electronics] Bally Midway" "Blue Print (Midway)" */
	DRIVER( butasan ) /* 1987 "[NMK] (Jaleco license)" "Butasan (Japan)" */
	DRIVER( chimerab ) /* 1993 "Jaleco" "Chimera Beast (prototype)" */
	DRIVER( cischeat ) /* 1990 "Jaleco" "Cisco Heat" */
	DRIVER( citycon ) /* 1985 "Jaleco" "City Connection (set 1)" */
	DRIVER( citycona ) /* 1985 "Jaleco" "City Connection (set 2)" */
	DRIVER( cruisin ) /* 1985 "Jaleco (Kitkorp license)" "Cruisin" */
	DRIVER( cybattlr ) /* 1993 "Jaleco" "Cybattler" */
	DRIVER( ddayjlc ) /* 1984 "Jaleco" "D-Day (Jaleco - set 1)" */
	DRIVER( ddayjlca ) /* 1984 "Jaleco" "D-Day (Jaleco - set 2)" */
	DRIVER( desertwr ) /* 1995 "Jaleco" "Desert War / Wangan Sensou" */
	DRIVER( edf ) /* 1991 "Jaleco" "E.D.F. : Earth Defense Force" */
	DRIVER( edfu ) /* 1991 "Jaleco" "E.D.F. : Earth Defense Force (North America)" */
	DRIVER( exerion ) /* 1983 "Jaleco" "Exerion" */
	DRIVER( exerionb ) /* 1983 "Jaleco" "Exerion (bootleg)" */
	DRIVER( exeriont ) /* 1983 "Jaleco (Taito America license)" "Exerion (Taito)" */
	DRIVER( exerizrb ) /* 1987 "Jaleco" "Exerizer (Japan) (bootleg)" */
	DRIVER( f1gpstar ) /* 1991 "Jaleco" "Grand Prix Star" */
	DRIVER( f1gpstr2 ) /* 1993 "Jaleco" "F-1 Grand Prix Star II" */
	DRIVER( fcombat ) /* 1985 "Jaleco" "Field Combat" */
	DRIVER( formatz ) /* 1984 "Jaleco" "Formation Z" */
	DRIVER( gametngk ) /* 1995 "Jaleco" "The Game Paradise - Master of Shooting! / Game Tengoku - The Game Paradise" */
	DRIVER( gingania ) /* 1987 "Jaleco" "Ginga NinkyouDen (set 2)" */
	DRIVER( ginganin ) /* 1987 "Jaleco" "Ginga NinkyouDen (set 1)" */
	DRIVER( gratia ) /* 1996 "Jaleco" "Gratia - Second Earth (92047-01 version)" */
	DRIVER( gratiaa ) /* 1996 "Jaleco" "Gratia - Second Earth (91022-10 version)" */
	DRIVER( hachoo ) /* 1989 "Jaleco" "Hachoo!" */
	DRIVER( hayaosi1 ) /* 1994 "Jaleco" "Hayaoshi Quiz Ouza Ketteisen" */
	DRIVER( homerun ) /* 1988 "Jaleco" "Moero Pro Yakyuu Homerun" */
	DRIVER( iganinju ) /* 1988 "Jaleco" "Iga Ninjyutsuden (Japan)" */
	DRIVER( jitsupro ) /* 1989 "Jaleco" "Jitsuryoku!! Pro Yakyuu (Japan)" */
	DRIVER( kazan ) /* 1988 "Jaleco" "Ninja Kazan (World)" */
	DRIVER( kickoff ) /* 1988 "Jaleco" "Kick Off (Japan)" */
	DRIVER( kirarast ) /* 1996 "Jaleco" "Ryuusei Janshi Kirara Star" */
	DRIVER( lomakai ) /* 1988 "Jaleco" "Legend of Makai (World)" */
	DRIVER( lordofk ) /* 1989 "Jaleco" "The Lord of King (Japan)" */
	DRIVER( makaiden ) /* 1988 "Jaleco" "Makai Densetsu (Japan)" */
	DRIVER( momoko ) /* 1986 "Jaleco" "Momoko 120%" */
	DRIVER( p47 ) /* 1988 "Jaleco" "P-47 - The Phantom Fighter (World)" */
	DRIVER( p47aces ) /* 1995 "Jaleco" "P-47 Aces" */
	DRIVER( p47j ) /* 1988 "Jaleco" "P-47 - The Freedom Fighter (Japan)" */
	DRIVER( peekaboo ) /* 1993 "Jaleco" "Peek-a-Boo!" */
	DRIVER( phantasm ) /* 1990 "Jaleco" "Phantasm (Japan)" */
	DRIVER( plusalph ) /* 1989 "Jaleco" "Plus Alpha" */
	DRIVER( psychic5 ) /* 1987 "Jaleco" "Psychic 5" */
	DRIVER( pturn ) /* 1984 "Jaleco" "Parallel Turn" */
	DRIVER( rockn ) /* 1999 "Jaleco" "Rock'n Tread (Japan)" */
	DRIVER( rockn2 ) /* 1999 "Jaleco" "Rock'n Tread 2 (Japan)" */
	DRIVER( rockn3 ) /* 1999 "Jaleco" "Rock'n 3 (Japan)" */
	DRIVER( rockn4 ) /* 2000 "Jaleco (PCCWJ)" "Rock'n 4 (Japan */
	DRIVER( rockna ) /* 1999 "Jaleco" "Rock'n Tread (Japan */
	DRIVER( rocknms ) /* 1999 "Jaleco" "Rock'n MegaSession (Japan)" */
	DRIVER( rodland ) /* 1990 "Jaleco" "Rod-Land (World)" */
	DRIVER( rodlandj ) /* 1990 "Jaleco" "Rod-Land (Japan)" */
	DRIVER( rodlndjb ) /* 1990 "Jaleco" "Rod-Land (Japan bootleg)" */
	DRIVER( saturn ) /* 1983 "[Zilec Electronics] Jaleco" "Saturn" */
	DRIVER( scudhamm ) /* 1994 "Jaleco" "Scud Hammer" */
	DRIVER( skyfox ) /* 1987 "Jaleco (Nichibutsu USA license)" "Sky Fox" */
	DRIVER( soldamj ) /* 1992 "Jaleco" "Soldam (Japan)" */
	DRIVER( stdragon ) /* 1989 "Jaleco" "Saint Dragon" */
	DRIVER( teplus2j ) /* 1997 "Jaleco / The Tetris Company" "Tetris Plus 2 (Japan)" */
	DRIVER( tetrisp ) /* 1995 "Jaleco / BPS" "Tetris Plus" */
	DRIVER( tetrisp2 ) /* 1997 "Jaleco / The Tetris Company" "Tetris Plus 2 (World?)" */
	DRIVER( tp2m32 ) /* 1997 "Jaleco" "Tetris Plus 2 (MegaSystem 32 Version)" */
	DRIVER( tshingen ) /* 1988 "Jaleco" "Takeda Shingen (Japan */
	DRIVER( tshingna ) /* 1988 "Jaleco" "Shingen Samurai-Fighter (Japan */
	DRIVER( valtric ) /* 1986 "[NMK] (Jaleco license)" "Valtric" */
	DRIVER( wildplt ) /* 1992 "Jaleco" "Wild Pilot" */
#endif
#ifdef LINK_JUSTDKONG
	DRIVER( dkong ) /* 1981 "Nintendo of America" "Donkey Kong (US set 1)" */
	DRIVER( dkong3 ) /* 1983 "Nintendo of America" "Donkey Kong 3 (US)" */
	DRIVER( dkong3j ) /* 1983 "Nintendo" "Donkey Kong 3 (Japan)" */
	DRIVER( dkongjp ) /* 1981 "Nintendo" "Donkey Kong (Japan set 1)" */
	DRIVER( dkongjr ) /* 1982 "Nintendo of America" "Donkey Kong Junior (US)" */
	DRIVER( dkongjrj ) /* 1982 "Nintendo" "Donkey Kong Jr. (Japan)" */
	DRIVER( dkongo ) /* 1981 "Nintendo" "Donkey Kong (US set 2)" */
#endif
#ifdef LINK_JUSTGALAGA
	DRIVER( battles ) /* 1982 "bootleg" "Battles" */
	DRIVER( bosco ) /* 1981 "Namco" "Bosconian (new version)" */
	DRIVER( boscomd ) /* 1981 "[Namco] (Midway license)" "Bosconian (Midway */
	DRIVER( boscomdo ) /* 1981 "[Namco] (Midway license)" "Bosconian (Midway */
	DRIVER( boscoo ) /* 1981 "Namco" "Bosconian (old version)" */
	DRIVER( boscoo2 ) /* 1981 "Namco" "Bosconian (older version)" */
	DRIVER( digdug ) /* 1982 "Namco" "Dig Dug (rev 2)" */
	DRIVER( digduga1 ) /* 1982 "[Namco] (Atari license)" "Dig Dug (Atari */
	DRIVER( digdugat ) /* 1982 "[Namco] (Atari license)" "Dig Dug (Atari */
	DRIVER( digdugb ) /* 1982 "Namco" "Dig Dug (rev 1)" */
	DRIVER( dzigzag ) /* 1982 "bootleg" "Zig Zag (Dig Dug hardware)" */
	DRIVER( galaga ) /* 1981 "Namco" "Galaga (Namco rev. B)" */
	DRIVER( galagamk ) /* 1981 "[Namco] (Midway license)" "Galaga (Midway set 2)" */
	DRIVER( galagamw ) /* 1981 "[Namco] (Midway license)" "Galaga (Midway set 1)" */
	DRIVER( galagao ) /* 1981 "Namco" "Galaga (Namco)" */
	DRIVER( gallag ) /* 1981 "bootleg" "Gallag" */
	DRIVER( gatsbee ) /* 1984 "hack" "Gatsbee" */
	DRIVER( sxevious ) /* 1984 "Namco" "Super Xevious" */
	DRIVER( xevios ) /* 1982 "bootleg" "Xevios" */
	DRIVER( xevious ) /* 1982 "Namco" "Xevious (Namco)" */
	DRIVER( xeviousa ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 1)" */
	DRIVER( xeviousb ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 2)" */
	DRIVER( xeviousc ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 3)" */
#endif
#ifdef LINK_KANEKO
	DRIVER( airbustj ) /* 1990 "Kaneko (Namco license)" "Air Buster: Trouble Specialty Raid Unit (Japan)" */
	DRIVER( airbustr ) /* 1990 "Kaneko (Namco license)" "Air Buster: Trouble Specialty Raid Unit (World)" */
	DRIVER( bakubrkr ) /* 1992 "Kaneko" "Bakuretsu Breaker" */
	DRIVER( berlwall ) /* 1991 "Kaneko" "The Berlin Wall" */
	DRIVER( berlwalt ) /* 1991 "Kaneko" "The Berlin Wall (bootleg ?)" */
	DRIVER( blazeon ) /* 1992 "Atlus" "Blaze On (Japan)" */
	DRIVER( bonkadv ) /* 1994 "Kaneko" "B.C. Kid / Bonk's Adventure / Kyukyoku!! PC Genjin" */
	DRIVER( cyvern ) /* 1998 "Kaneko" "Cyvern (Japan)" */
	DRIVER( explbrkr ) /* 1992 "Kaneko" "Explosive Breaker" */
	DRIVER( fantasia ) /* 1994 "Comad & New Japan System" "Fantasia" */
	DRIVER( fantsia2 ) /* 1997 "Comad" "Fantasia II (Explicit)" */
	DRIVER( fantsy95 ) /* 1995 "Hi-max Technology Inc." "Fantasy '95" */
	DRIVER( fntsia2a ) /* 1997 "Comad" "Fantasia II (Less Explicit)" */
	DRIVER( galhustl ) /* 1997 "ACE International" "Gals Hustler" */
	DRIVER( galpani4 ) /* 1996 "Kaneko" "Gals Panic 4 (Japan)" */
	DRIVER( galpania ) /* 1990 "Kaneko" "Gals Panic (set 3)" */
	DRIVER( galpanib ) /* 1990 "Kaneko" "Gals Panic (set 2)" */
	DRIVER( galpanic ) /* 1990 "Kaneko" "Gals Panic (set 1)" */
	DRIVER( galpanis ) /* 1997 "Kaneko" "Gals Panic S - Extra Edition (Japan)" */
	DRIVER( galpans2 ) /* 1999 "Kaneko" "Gals Panic S2 (Japan)" */
	DRIVER( gtmr ) /* 1994 "Kaneko" "1000 Miglia: Great 1000 Miles Rally (94/07/18)" */
	DRIVER( gtmr2 ) /* 1995 "Kaneko" "Mille Miglia 2: Great 1000 Miles Rally (95/05/24)" */
	DRIVER( gtmr2a ) /* 1995 "Kaneko" "Mille Miglia 2: Great 1000 Miles Rally (95/04/04)" */
	DRIVER( gtmr2u ) /* 1995 "Kaneko" "Great 1000 Miles Rally 2 USA (95/05/18)" */
	DRIVER( gtmra ) /* 1994 "Kaneko" "1000 Miglia: Great 1000 Miles Rally (94/06/13)" */
	DRIVER( gtmre ) /* 1994 "Kaneko" "Great 1000 Miles Rally: Evolution Model!!! (94/09/06)" */
	DRIVER( gtmrusa ) /* 1994 "Kaneko" "Great 1000 Miles Rally: U.S.A Version! (94/09/06)" */
	DRIVER( gutsn ) /* 2000 "Kaneko / Kouyousha" "Guts'n (Japan)" */
	DRIVER( jjparad2 ) /* 1997 "Electro Design" "Jan Jan Paradise 2" */
	DRIVER( jjparads ) /* 1996 "Electro Design" "Jan Jan Paradise" */
	DRIVER( mgcrystj ) /* 1991 "Kaneko (Atlus license)" "Magical Crystals (Japan */
	DRIVER( mgcrystl ) /* 1991 "Kaneko" "Magical Crystals (World */
	DRIVER( mgcrysto ) /* 1991 "Kaneko" "Magical Crystals (World */
	DRIVER( missmw96 ) /* 1996 "Comad" "Miss Mister World '96 (Nude)" */
	DRIVER( missw96 ) /* 1996 "Comad" "Miss World '96 (Nude)" */
	DRIVER( newfant ) /* 1995 "Comad & New Japan System" "New Fantasia" */
	DRIVER( panicstr ) /* 1999 "Kaneko" "Panic Street (Japan)" */
	DRIVER( puzloopj ) /* 1998 "Mitchell" "Puzz Loop (Japan)" */
	DRIVER( puzloopk ) /* 1998 "Mitchell" "Puzz Loop (Korea)" */
	DRIVER( puzloopu ) /* 1998 "Mitchell" "Puzz Loop (USA)" */
	DRIVER( puzzloop ) /* 1998 "Mitchell" "Puzz Loop (Europe)" */
	DRIVER( ryouran ) /* 1998 "Electro Design" "VS Mahjong Otome Ryouran" */
	DRIVER( sandscra ) /* 1992 "Face" "Sand Scorpion (set 2)" */
	DRIVER( sandscrp ) /* 1992 "Face" "Sand Scorpion (set 1)" */
	DRIVER( sarukani ) /* 1997 "Kaneko / Mediaworks" "Saru-Kani-Hamu-Zou (Japan)" */
	DRIVER( sengekij ) /* 1997 "Kaneko / Warashi" "Sengeki Striker (Japan)" */
	DRIVER( sengekis ) /* 1997 "Kaneko / Warashi" "Sengeki Striker (Asia)" */
	DRIVER( senknow ) /* 1999 "Kaneko / Kouyousha" "Sen-Know (Japan)" */
	DRIVER( skns ) /* 1996 "Kaneko" "Super Kaneko Nova System BIOS" */
	DRIVER( supmodel ) /* 1994 "Comad & New Japan System" "Super Model" */
	DRIVER( teljan ) /* 1999 "Electro Design" "Tel Jan" */
	DRIVER( vblokbrk ) /* 1997 "Kaneko / Mediaworks" "VS Block Breaker (Asia)" */
	DRIVER( zipzap ) /* 1995 "Barko Corp" "Zip & Zap" */
#endif
#ifdef LINK_KONAMI
	DRIVER( 600 ) /* 1981 "Konami" "600" */
	DRIVER( 800fath ) /* 1981 "Amenip (US Billiards Inc. license)" "800 Fathoms" */
	DRIVER( 88games ) /* 1988 "Konami" "'88 Games" */
	DRIVER( ad2083 ) /* 1983 "Midcoin" "A. D. 2083" */
	DRIVER( ajax ) /* 1987 "Konami" "Ajax" */
	DRIVER( ajaxj ) /* 1987 "Konami" "Ajax (Japan)" */
	DRIVER( aliens ) /* 1990 "Konami" "Aliens (World set 1)" */
	DRIVER( aliens2 ) /* 1990 "Konami" "Aliens (World set 2)" */
	DRIVER( aliens3 ) /* 1990 "Konami" "Aliens (World set 3)" */
	DRIVER( aliensj ) /* 1990 "Konami" "Aliens (Japan set 1)" */
	DRIVER( aliensj2 ) /* 1990 "Konami" "Aliens (Japan set 2)" */
	DRIVER( aliensu ) /* 1990 "Konami" "Aliens (US)" */
	DRIVER( amidar ) /* 1981 "Konami" "Amidar" */
	DRIVER( amidarb ) /* 1982 "bootleg" "Amidar (Bootleg)" */
	DRIVER( amidaro ) /* 1982 "Konami (Olympia license)" "Amidar (Olympia)" */
	DRIVER( amidars ) /* 1982 "Konami" "Amidar (Scramble hardware)" */
	DRIVER( amidaru ) /* 1982 "Konami (Stern license)" "Amidar (Stern)" */
	DRIVER( amigo ) /* 1982 "bootleg" "Amigo" */
	DRIVER( anteater ) /* 1982 "[Stern] (Tago license)" "Anteater" */
	DRIVER( anteatg ) /* 1983 "TV-Tuning 2000 (F.E.G. license)" "Ameisenbaer (German)" */
	DRIVER( anteatgb ) /* 1983 "Free Enterprise Games" "The Anteater (UK)" */
	DRIVER( aponow ) /* 1982 "bootleg" "Apocaljpse Now" */
	DRIVER( armorca2 ) /* 1981 "Stern" "Armored Car (set 2)" */
	DRIVER( armorcar ) /* 1981 "Stern" "Armored Car (set 1)" */
	DRIVER( asterix ) /* 1992 "Konami" "Asterix (ver EAD)" */
	DRIVER( astrxeaa ) /* 1992 "Konami" "Asterix (ver EAA)" */
	DRIVER( astrxeac ) /* 1992 "Konami" "Asterix (ver EAC)" */
	DRIVER( atlantis ) /* 1981 "Comsoft" "Battle of Atlantis (set 1)" */
	DRIVER( atlantol ) /* 1996 "bootleg" "Atlant Olimpic" */
	DRIVER( atlants2 ) /* 1981 "Comsoft" "Battle of Atlantis (set 2)" */
	DRIVER( battlntj ) /* 1987 "Konami" "Battlantis (Japan)" */
	DRIVER( battlnts ) /* 1987 "Konami" "Battlantis" */
	DRIVER( billiard ) /* 1981 "bootleg" "The Billiards" */
	DRIVER( bishi ) /* 1996 "Konami" "Bishi Bashi Championship Mini Game Senshuken (ver JAA)" */
	DRIVER( bladestl ) /* 1987 "Konami" "Blades of Steel (version T)" */
	DRIVER( bladstle ) /* 1987 "Konami" "Blades of Steel (version E)" */
	DRIVER( blkpnthr ) /* 1987 "Konami" "Black Panther" */
	DRIVER( blockhl ) /* 1989 "Konami" "Block Hole" */
	DRIVER( blswhstl ) /* 1991 "Konami" "Bells & Whistles (Version L)" */
	DRIVER( bm1stmix ) /* 1997 "Konami" "beatmania (ver JA-B)" */
	DRIVER( bm2ndmix ) /* 1998 "Konami" "beatmania 2nd MIX (ver JA-B)" */
	DRIVER( bm2ndmxa ) /* 1998 "Konami" "beatmania 2nd MIX (ver JA-A)" */
	DRIVER( bm4thmix ) /* 1999 "Konami" "beatmania 4th MIX (ver JA-A)" */
	DRIVER( bm5thmix ) /* 1999 "Konami" "beatmania 5th MIX (ver JA-A)" */
	DRIVER( bm6thmix ) /* 2001 "Konami" "beatmania 6th MIX (ver JA-A)" */
	DRIVER( bmcompm2 ) /* 2000 "Konami" "beatmania complete MIX 2 (ver JA-A)" */
	DRIVER( bmcompmx ) /* 1999 "Konami" "beatmania complete MIX (ver JA-B)" */
	DRIVER( bmcorerm ) /* 2000 "Konami" "beatmania CORE REMIX (ver JA-A)" */
	DRIVER( bmdct ) /* 2000 "Konami" "beatmania f. Dreams Come True (ver JA-A)" */
	DRIVER( boggy84 ) /* 1983 "bootleg" "Boggy '84" */
	DRIVER( bootcamp ) /* 1987 "Konami" "Boot Camp" */
	DRIVER( bottom9 ) /* 1989 "Konami" "Bottom of the Ninth (version T)" */
	DRIVER( bottom9n ) /* 1989 "Konami" "Bottom of the Ninth (version N)" */
	DRIVER( bucky ) /* 1992 "Konami" "Bucky O'Hare (ver EA)" */
	DRIVER( buckyua ) /* 1992 "Konami" "Bucky O'Hare (ver UA)" */
	DRIVER( calipso ) /* 1982 "[Stern] (Tago license)" "Calipso" */
	DRIVER( cavelon ) /* 1983 "Jetsoft" "Cavelon" */
	DRIVER( chqflag ) /* 1988 "Konami" "Chequered Flag" */
	DRIVER( chqflagj ) /* 1988 "Konami" "Chequered Flag (Japan)" */
	DRIVER( circusc ) /* 1984 "Konami" "Circus Charlie (Selectable level set 1)" */
	DRIVER( circusc2 ) /* 1984 "Konami" "Circus Charlie (Selectable level set 2)" */
	DRIVER( circusc3 ) /* 1984 "Konami" "Circus Charlie (No level select)" */
	DRIVER( circuscc ) /* 1984 "Konami (Centuri license)" "Circus Charlie (Centuri)" */
	DRIVER( circusce ) /* 1984 "Konami (Centuri license)" "Circus Charlie (Centuri */
	DRIVER( citybmrj ) /* 1987 "Konami" "City Bomber (Japan)" */
	DRIVER( citybomb ) /* 1987 "Konami" "City Bomber (World)" */
	DRIVER( ckongs ) /* 1981 "bootleg" "Crazy Kong (Scramble hardware)" */
	DRIVER( combasc ) /* 1988 "Konami" "Combat School (joystick)" */
	DRIVER( combascb ) /* 1988 "bootleg" "Combat School (bootleg)" */
	DRIVER( combascj ) /* 1987 "Konami" "Combat School (Japan trackball)" */
	DRIVER( combasct ) /* 1987 "Konami" "Combat School (trackball)" */
	DRIVER( contra ) /* 1987 "Konami" "Contra (US)" */
	DRIVER( contrab ) /* 1987 "bootleg" "Contra (US bootleg)" */
	DRIVER( contraj ) /* 1987 "Konami" "Contra (Japan)" */
	DRIVER( contrajb ) /* 1987 "bootleg" "Contra (Japan bootleg)" */
	DRIVER( crazycop ) /* 1988 "Konami" "Crazy Cop (Japan)" */
	DRIVER( crimfght ) /* 1989 "Konami" "Crime Fighters (US 4 players)" */
	DRIVER( crimfgt2 ) /* 1989 "Konami" "Crime Fighters (World 2 Players)" */
	DRIVER( crimfgtj ) /* 1989 "Konami" "Crime Fighters (Japan 2 Players)" */
	DRIVER( cryptklr ) /* 1995 "Konami" "Crypt Killer (GQ420 UAA)" */
	DRIVER( cuebrckj ) /* 1989 "Konami" "Cue Brick (World version D)" */
	DRIVER( cuebrick ) /* 1989 "Konami" "Cue Brick (Japan)" */
	DRIVER( dadandrn ) /* 1993 "Konami" "Kyukyoku Sentai Dadandarn (ver JAA)" */
	DRIVER( dairesya ) /* 1986 "[Konami] (Kawakusu license)" "Dai Ressya Goutou (Japan)" */
	DRIVER( daiskiss ) /* 1996 "Konami" "Daisu-Kiss (ver JAA)" */
	DRIVER( darkadv ) /* 1987 "Konami" "Dark Adventure" */
	DRIVER( darkplnt ) /* 1982 "Stern" "Dark Planet" */
	DRIVER( dbz ) /* 1993 "Banpresto" "Dragonball Z" */
	DRIVER( dbz2 ) /* 1994 "Banpresto" "Dragonball Z 2 - Super Battle" */
	DRIVER( ddribble ) /* 1986 "Konami" "Double Dribble" */
	DRIVER( detatwin ) /* 1991 "Konami" "Detana!! Twin Bee (Japan ver. J)" */
	DRIVER( devilfsh ) /* 1982 "Artic" "Devil Fish" */
	DRIVER( devilw ) /* 1987 "Konami" "Devil World" */
	DRIVER( devstor2 ) /* 1988 "Konami" "Devastators (ver. X)" */
	DRIVER( devstor3 ) /* 1988 "Konami" "Devastators (ver. V)" */
	DRIVER( devstors ) /* 1988 "Konami" "Devastators (ver. Z)" */
	DRIVER( dragoona ) /* 1995 "Konami" "Dragoon Might (ver AAB)" */
	DRIVER( dragoonj ) /* 1995 "Konami" "Dragoon Might (ver JAA)" */
	DRIVER( esckids ) /* 1991 "Konami" "Escape Kids (Asia */
	DRIVER( esckidsj ) /* 1991 "Konami" "Escape Kids (Japan */
	DRIVER( explorer ) /* 1981 "bootleg" "Explorer" */
	DRIVER( fastfred ) /* 1982 "Atari" "Fast Freddie" */
	DRIVER( fastlane ) /* 1987 "Konami" "Fast Lane" */
	DRIVER( finalizb ) /* 1985 "bootleg" "Finalizer - Super Transformation (bootleg)" */
	DRIVER( finalizr ) /* 1985 "Konami" "Finalizer - Super Transformation" */
	DRIVER( flkatck ) /* 1987 "Konami" "Flak Attack (Japan)" */
	DRIVER( flyboy ) /* 1982 "Kaneko" "Fly-Boy" */
	DRIVER( flyboyb ) /* 1982 "Kaneko" "Fly-Boy (bootleg)" */
	DRIVER( frogf ) /* 1981 "Falcon" "Frogger (Falcon bootleg)" */
	DRIVER( frogger ) /* 1981 "Konami" "Frogger" */
	DRIVER( froggers ) /* 1981 "bootleg" "Frog" */
	DRIVER( frogseg1 ) /* 1981 "[Konami] (Sega license)" "Frogger (Sega set 1)" */
	DRIVER( frogseg2 ) /* 1981 "[Konami] (Sega license)" "Frogger (Sega set 2)" */
	DRIVER( fround ) /* 1988 "Konami" "The Final Round (version M)" */
	DRIVER( froundl ) /* 1988 "Konami" "The Final Round (version L)" */
	DRIVER( gaiapolj ) /* 1993 "Konami" "Gaiapolis (ver JAF)" */
	DRIVER( gaiapols ) /* 1993 "Konami" "Gaiapolis (ver EAF)" */
	DRIVER( gaiapolu ) /* 1993 "Konami" "Gaiapolis (ver UAF)" */
	DRIVER( garuka ) /* 1988 "Konami" "Garuka (Japan ver. W)" */
	DRIVER( gberet ) /* 1985 "Konami" "Green Beret" */
	DRIVER( gberetb ) /* 1985 "bootleg" "Green Beret (bootleg)" */
	DRIVER( gbusters ) /* 1988 "Konami" "Gang Busters" */
	DRIVER( gijoe ) /* 1992 "Konami" "GI Joe (World)" */
	DRIVER( gijoej ) /* 1992 "Konami" "GI Joe (Japan)" */
	DRIVER( gijoeu ) /* 1992 "Konami" "GI Joe (US)" */
	DRIVER( glfgreat ) /* 1991 "Konami" "Golfing Greats" */
	DRIVER( glfgretj ) /* 1991 "Konami" "Golfing Greats (Japan)" */
	DRIVER( gokuparo ) /* 1994 "Konami" "Gokujyou Parodius (ver JAD)" */
	DRIVER( gradius ) /* 1985 "Konami" "Gradius" */
	DRIVER( gradius2 ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan New Ver.)" */
	DRIVER( gradius3 ) /* 1989 "Konami" "Gradius III (Japan)" */
	DRIVER( gradius4 ) /* 1998 "Konami" "Gradius 4: Fukkatsu" */
	DRIVER( grdius2a ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan Old Ver.)" */
	DRIVER( grdius2b ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan Older Ver.)" */
	DRIVER( grdius3a ) /* 1989 "Konami" "Gradius III (Asia)" */
	DRIVER( grdius3e ) /* 1989 "Konami" "Gradius III (World ?)" */
	DRIVER( gryzor ) /* 1987 "Konami" "Gryzor" */
	DRIVER( gwarrior ) /* 1985 "Konami" "Galactic Warriors" */
	DRIVER( gyruss ) /* 1983 "Konami" "Gyruss (Konami)" */
	DRIVER( gyrussce ) /* 1983 "Konami (Centuri license)" "Gyruss (Centuri)" */
	DRIVER( hcastle ) /* 1988 "Konami" "Haunted Castle (version M)" */
	DRIVER( hcastlej ) /* 1988 "Konami" "Akuma-Jou Dracula (Japan version P)" */
	DRIVER( hcastleo ) /* 1988 "Konami" "Haunted Castle (version K)" */
	DRIVER( hcastljo ) /* 1988 "Konami" "Akuma-Jou Dracula (Japan version N)" */
	DRIVER( hcrash ) /* 1987 "Konami" "Hyper Crash (version D)" */
	DRIVER( hcrashc ) /* 1987 "Konami" "Hyper Crash (version C)" */
	DRIVER( hexion ) /* 1992 "Konami" "Hexion (Japan)" */
	DRIVER( hmcompm2 ) /* 2000 "Konami" "hiphopmania complete MIX 2 (ver UA-A)" */
	DRIVER( hmcompmx ) /* 1999 "Konami" "hiphopmania complete MIX (ver UA-B)" */
	DRIVER( hncholms ) /* 1984 "Century Electronics" "Hunchback Olympic (Scramble hardware)" */
	DRIVER( hotchase ) /* 1988 "Konami" "Hot Chase" */
	DRIVER( hotshock ) /* 1982 "E.G. Felaco" "Hot Shocker" */
	DRIVER( hpolym84 ) /* 1984 "Konami" "Hyper Olympic '84" */
	DRIVER( hpuncher ) /* 1988 "Konami" "Hard Puncher (Japan)" */
	DRIVER( hunchbks ) /* 1983 "Century Electronics" "Hunchback (Scramble hardware)" */
	DRIVER( hustler ) /* 1981 "Konami" "Video Hustler" */
	DRIVER( hustlerb ) /* 1981 "bootleg" "Video Hustler (bootleg)" */
	DRIVER( hyperath ) /* 1996 "Konami" "Hyper Athlete (GV021 JAPAN 1.00)" */
	DRIVER( hyperspt ) /* 1984 "Konami (Centuri license)" "Hyper Sports" */
	DRIVER( hyprolyb ) /* 1983 "bootleg" "Hyper Olympic (bootleg)" */
	DRIVER( hyprolym ) /* 1983 "Konami" "Hyper Olympic" */
	DRIVER( hypsptsp ) /* 1988 "Konami" "Hyper Sports Special (Japan)" */
	DRIVER( imago ) /* 1983 "Acom" "Imago" */
	DRIVER( ironhors ) /* 1986 "Konami" "Iron Horse" */
	DRIVER( jackal ) /* 1986 "Konami" "Jackal (World)" */
	DRIVER( jackalj ) /* 1986 "Konami" "Tokushu Butai Jackal (Japan)" */
	DRIVER( jailbrek ) /* 1986 "Konami" "Jail Break" */
	DRIVER( jumpcoas ) /* 1983 "Kaneko" "Jump Coaster" */
	DRIVER( jumpcoat ) /* 1983 "Taito" "Jump Coaster (Taito)" */
	DRIVER( junofrst ) /* 1983 "Konami" "Juno First" */
	DRIVER( junofstg ) /* 1983 "Konami (Gottlieb license)" "Juno First (Gottlieb)" */
	DRIVER( kicker ) /* 1985 "Konami" "Kicker" */
	DRIVER( kittenk ) /* 1988 "Konami" "Kitten Kaboodle" */
	DRIVER( knockout ) /* 1982 "KKK" "Knock Out!!" */
	DRIVER( konami88 ) /* 1988 "Konami" "Konami '88" */
	DRIVER( konamigt ) /* 1985 "Konami" "Konami GT" */
	DRIVER( konamigv ) /* 1995 "Konami" "Baby Phoenix/GV System" */
	DRIVER( konamigx ) /* 1994 "Konami" "System GX" */
	DRIVER( labyrunk ) /* 1987 "Konami" "Labyrinth Runner (World Ver. K)" */
	DRIVER( labyrunr ) /* 1987 "Konami" "Labyrinth Runner (Japan)" */
	DRIVER( le2 ) /* 1994 "Konami" "Lethal Enforcers II: Gun Fighters (ver EAA)" */
	DRIVER( le2u ) /* 1994 "Konami" "Lethal Enforcers II: Gun Fighters (ver UAA)" */
	DRIVER( lethalen ) /* 1992 "Konami" "Lethal Enforcers (ver UAE)" */
	DRIVER( lgtnfght ) /* 1990 "Konami" "Lightning Fighters (World)" */
	DRIVER( lgtnfghu ) /* 1990 "Konami" "Lightning Fighters (US)" */
	DRIVER( lifefrce ) /* 1986 "Konami" "Lifeforce (US)" */
	DRIVER( lifefrcj ) /* 1986 "Konami" "Lifeforce (Japan)" */
	DRIVER( losttmbh ) /* 1982 "Stern" "Lost Tomb (hard)" */
	DRIVER( losttomb ) /* 1982 "Stern" "Lost Tomb (easy)" */
	DRIVER( mainev2p ) /* 1988 "Konami" "The Main Event (2 Players ver. X)" */
	DRIVER( mainevt ) /* 1988 "Konami" "The Main Event (4 Players ver. Y)" */
	DRIVER( mainevto ) /* 1988 "Konami" "The Main Event (4 Players ver. F)" */
	DRIVER( majuu ) /* 1987 "Konami" "Majuu no Ohkoku" */
	DRIVER( manhatan ) /* 1986 "Konami" "Manhattan 24 Bunsyo (Japan)" */
	DRIVER( mariner ) /* 1981 "Amenip" "Mariner" */
	DRIVER( mars ) /* 1981 "Artic" "Mars" */
	DRIVER( mastkin ) /* 1988 "Du Tech" "The Masters of Kin" */
	DRIVER( megaznik ) /* 1983 "Konami / Interlogic + Kosuka" "Mega Zone (Interlogic + Kosuka)" */
	DRIVER( megazona ) /* 1983 "Konami" "Mega Zone (Konami set 2)" */
	DRIVER( megazonb ) /* 1983 "Konami / Kosuka" "Mega Zone (Kosuka set 1)" */
	DRIVER( megazonc ) /* 1983 "Konami / Kosuka" "Mega Zone (Kosuka set 2)" */
	DRIVER( megazone ) /* 1983 "Konami" "Mega Zone (Konami set 1)" */
	DRIVER( merlinmm ) /* 1986 "Zilec - Zenitone" "Merlins Money Maze" */
	DRIVER( metamrph ) /* 1993 "Konami" "Metamorphic Force (ver EAA)" */
	DRIVER( metamrpj ) /* 1993 "Konami" "Metamorphic Force (ver JAA)" */
	DRIVER( metamrpu ) /* 1993 "Konami" "Metamorphic Force (ver UAA)" */
	DRIVER( mia ) /* 1989 "Konami" "M.I.A. - Missing in Action (version T)" */
	DRIVER( mia2 ) /* 1989 "Konami" "M.I.A. - Missing in Action (version S)" */
	DRIVER( miaj ) /* 1989 "Konami" "M.I.A. - Missing in Action (Japan)" */
	DRIVER( mikie ) /* 1984 "Konami" "Mikie" */
	DRIVER( mikiehs ) /* 1984 "Konami" "Mikie (High School Graffiti)" */
	DRIVER( mikiej ) /* 1984 "Konami" "Shinnyuushain Tooru-kun" */
	DRIVER( mimonkey ) /* 198? "Universal Video Games" "Mighty Monkey" */
	DRIVER( mimonsco ) /* 198? "bootleg" "Mighty Monkey (bootleg on Super Cobra hardware)" */
	DRIVER( mimonscr ) /* 198? "bootleg" "Mighty Monkey (bootleg on Scramble hardware)" */
	DRIVER( minefld ) /* 1983 "Stern" "Minefield" */
	DRIVER( mmaulers ) /* 1993 "Konami" "Monster Maulers (ver EAA)" */
	DRIVER( mogura ) /* 1991 "Konami" "Mogura Desse" */
	DRIVER( moo ) /* 1992 "Konami" "Wild West C.O.W.-Boys of Moo Mesa (ver EA)" */
	DRIVER( mooaa ) /* 1992 "Konami" "Wild West C.O.W.-Boys of Moo Mesa (ver AA)" */
	DRIVER( moonwar ) /* 1981 "Stern" "Moonwar" */
	DRIVER( moonwara ) /* 1981 "Stern" "Moonwar (older)" */
	DRIVER( mooua ) /* 1992 "Konami" "Wild West C.O.W.-Boys of Moo Mesa (ver UA)" */
	DRIVER( mrgoemon ) /* 1986 "Konami" "Mr. Goemon (Japan)" */
	DRIVER( mrkougar ) /* 1984 "ATW" "Mr. Kougar" */
	DRIVER( mrkougb ) /* 1983 "bootleg" "Mr. Kougar (bootleg)" */
	DRIVER( mrkougb2 ) /* 1983 "bootleg" "Mr. Kougar (bootleg Set 2)" */
	DRIVER( mrkougr2 ) /* 1983 "ATW" "Mr. Kougar (earlier)" */
	DRIVER( mstadium ) /* 1989 "Konami" "Main Stadium (Japan)" */
	DRIVER( mtlchamp ) /* 1993 "Konami" "Martial Champion (ver EAB)" */
	DRIVER( mtlchmp1 ) /* 1993 "Konami" "Martial Champion (ver EAA)" */
	DRIVER( mtlchmpa ) /* 1993 "Konami" "Martial Champion (ver AAA)" */
	DRIVER( mtlchmpj ) /* 1993 "Konami" "Martial Champion (ver JAA)" */
	DRIVER( mtlchmpu ) /* 1993 "Konami" "Martial Champion (ver UAD)" */
	DRIVER( mx5000 ) /* 1987 "Konami" "MX5000" */
	DRIVER( mystwarr ) /* 1993 "Konami" "Mystic Warriors (ver EAA)" */
	DRIVER( mystwaru ) /* 1993 "Konami" "Mystic Warriors (ver UAA)" */
	DRIVER( nagano98 ) /* 1998 "Konami" "Nagano Winter Olympics '98 (GX720 EAA)" */
	DRIVER( nbapbp ) /* 1998 "Konami" "NBA Play By Play" */
	DRIVER( nemesis ) /* 1985 "Konami" "Nemesis" */
	DRIVER( nemesuk ) /* 1985 "Konami" "Nemesis (World?)" */
	DRIVER( newsin7 ) /* 1983 "ATW USA Inc." */
	DRIVER( nyanpani ) /* 1988 "Konami" "Nyan Nyan Panic (Japan)" */
	DRIVER( overdriv ) /* 1990 "Konami" "Over Drive" */
	DRIVER( pandoras ) /* 1984 "Konami/Interlogic" "Pandora's Palace" */
	DRIVER( parodisj ) /* 1990 "Konami" "Parodius DA! (Japan)" */
	DRIVER( parodius ) /* 1990 "Konami" "Parodius DA! (World)" */
	DRIVER( pbball96 ) /* 1996 "Konami" "Powerful Baseball '96 (GV017 JAPAN 1.03)" */
	DRIVER( pingpong ) /* 1985 "Konami" "Ping Pong" */
	DRIVER( pootan ) /* 1982 "bootleg" "Pootan" */
	DRIVER( pooyan ) /* 1982 "Konami" "Pooyan" */
	DRIVER( pooyans ) /* 1982 "[Konami] (Stern license)" "Pooyan (Stern)" */
	DRIVER( prmrsocj ) /* 1993 "Konami" "Premier Soccer (ver JAB)" */
	DRIVER( prmrsocr ) /* 1993 "Konami" "Premier Soccer (ver EAB)" */
	DRIVER( psurge ) /* 1988 "<unknown>" "Power Surge" */
	DRIVER( punkshot ) /* 1990 "Konami" "Punk Shot (US 4 Players)" */
	DRIVER( punksht2 ) /* 1990 "Konami" "Punk Shot (US 2 Players)" */
	DRIVER( punkshtj ) /* 1990 "Konami" "Punk Shot (Japan 2 Players)" */
	DRIVER( puzldama ) /* 1994 "Konami" "Taisen Puzzle-dama (ver JAA)" */
	DRIVER( qdrmfgp ) /* 1994 "Konami" "Quiz Do Re Mi Fa Grand Prix (Japan)" */
	DRIVER( qdrmfgp2 ) /* 1995 "Konami" "Quiz Do Re Mi Fa Grand Prix2 - Shin-Kyoku Nyuukadayo (Japan)" */
	DRIVER( qgakumon ) /* 1993 "Konami" "Quiz Gakumon no Susume (Japan ver. JA2 Type L)" */
	DRIVER( quarth ) /* 1989 "Konami" "Quarth (Japan)" */
	DRIVER( rackemup ) /* 1987 "Konami" "Rack 'em Up" */
	DRIVER( redrobin ) /* 1986 "Elettronolo" "Red Robin" */
	DRIVER( rescue ) /* 1982 "Stern" "Rescue" */
	DRIVER( rf2 ) /* 1985 "Konami" "Konami RF2 - Red Fighter" */
	DRIVER( ringohja ) /* 1988 "Konami" "Ring no Ohja (Japan 2 Players ver. N)" */
	DRIVER( roadf ) /* 1984 "Konami" "Road Fighter (set 1)" */
	DRIVER( roadf2 ) /* 1984 "Konami" "Road Fighter (set 2)" */
	DRIVER( rockraga ) /* 1986 "Konami" "Rock 'n Rage (Prototype?)" */
	DRIVER( rockrage ) /* 1986 "Konami" "Rock 'n Rage (World?)" */
	DRIVER( rockragj ) /* 1986 "Konami" "Koi no Hotrock (Japan)" */
	DRIVER( rocnrope ) /* 1983 "Konami" "Roc'n Rope" */
	DRIVER( rocnropk ) /* 1983 "Konami + Kosuka" "Roc'n Rope (Kosuka)" */
	DRIVER( rollerg ) /* 1991 "Konami" "Rollergames (US)" */
	DRIVER( rollergj ) /* 1991 "Konami" "Rollergames (Japan)" */
	DRIVER( rungun ) /* 1993 "Konami" "Run and Gun (ver EAA 1993 10.8)" */
	DRIVER( runguna ) /* 1993 "Konami" "Run and Gun (ver EAA 1993 10.4)" */
	DRIVER( rungunu ) /* 1993 "Konami" "Run and Gun (ver UAB 1993 10.12)" */
	DRIVER( rungunua ) /* 1993 "Konami" "Run and Gun (ver UBA 1993 10.8)" */
	DRIVER( rushatck ) /* 1985 "Konami" "Rush'n Attack (US)" */
	DRIVER( salamand ) /* 1986 "Konami" "Salamander (version D)" */
	DRIVER( salamanj ) /* 1986 "Konami" "Salamander (version J)" */
	DRIVER( salmndr2 ) /* 1996 "Konami" "Salamander 2 (ver JAA)" */
	DRIVER( sbasketb ) /* 1984 "Konami" "Super Basketball (version G)" */
	DRIVER( sbasketo ) /* 1984 "Konami" "Super Basketball (version E)" */
	DRIVER( sbasketu ) /* 1984 "Konami" "Super Basketball (not encrypted)" */
	DRIVER( sbishi ) /* 1998 "Konami" "Super Bishi Bashi Championship (ver JAA)" */
	DRIVER( scobra ) /* 1981 "Konami" "Super Cobra" */
	DRIVER( scobrab ) /* 1981 "bootleg" "Super Cobra (bootleg)" */
	DRIVER( scobras ) /* 1981 "[Konami] (Stern license)" "Super Cobra (Stern)" */
	DRIVER( scobrase ) /* 1981 "[Konami] (Sega license)" "Super Cobra (Sega)" */
	DRIVER( scontra ) /* 1988 "Konami" "Super Contra" */
	DRIVER( scontraj ) /* 1988 "Konami" "Super Contra (Japan)" */
	DRIVER( scorpion ) /* 1982 "Zaccaria" "Scorpion (set 1)" */
	DRIVER( scotrsht ) /* 1985 "Konami" "Scooter Shooter" */
	DRIVER( scramble ) /* 1981 "Konami" "Scramble" */
	DRIVER( scrambls ) /* 1981 "[Konami] (Stern license)" "Scramble (Stern)" */
	DRIVER( sexyparo ) /* 1996 "Konami" "Sexy Parodius (ver JAA)" */
	DRIVER( sfx ) /* 1983 "Nichibutsu" "SF-X" */
	DRIVER( shaolins ) /* 1985 "Konami" "Shao-Lin's Road" */
	DRIVER( simpbowl ) /* 2000 "Konami" "Simpsons Bowling (GQ829 UAA)" */
	DRIVER( simps2pa ) /* 1991 "Konami" "The Simpsons (2 Players alt)" */
	DRIVER( simps2pj ) /* 1991 "Konami" "The Simpsons (2 Players Japan)" */
	DRIVER( simps4pa ) /* 1991 "Konami" "The Simpsons (4 Players alt)" */
	DRIVER( simpsn2p ) /* 1991 "Konami" "The Simpsons (2 Players)" */
	DRIVER( simpsons ) /* 1991 "Konami" "The Simpsons (4 Players)" */
	DRIVER( slmdunkj ) /* 1993 "Konami" "Slam Dunk (ver JAA 1993 10.8)" */
	DRIVER( spaceplt ) /* 1982 "bootleg" "Space Pilot" */
	DRIVER( spdcoin ) /* 1984 "Stern" "Speed Coin (prototype)" */
	DRIVER( spy ) /* 1989 "Konami" "S.P.Y. - Special Project Y (World ver. N)" */
	DRIVER( spyu ) /* 1989 "Konami" "S.P.Y. - Special Project Y (US ver. M)" */
	DRIVER( ssrdrabd ) /* 1991 "Konami" "Sunset Riders (2 Players ver ABD)" */
	DRIVER( ssrdradd ) /* 1991 "Konami" "Sunset Riders (4 Players ver ADD)" */
	DRIVER( ssrdreaa ) /* 1991 "Konami" "Sunset Riders (4 Players ver EAA)" */
	DRIVER( ssrdrebc ) /* 1991 "Konami" "Sunset Riders (2 Players ver EBC)" */
	DRIVER( ssrdrebd ) /* 1991 "Konami" "Sunset Riders (2 Players ver EBD)" */
	DRIVER( ssrdrjbd ) /* 1991 "Konami" "Sunset Riders (2 Players ver JBD)" */
	DRIVER( ssrdruac ) /* 1991 "Konami" "Sunset Riders (4 Players ver UAC)" */
	DRIVER( ssrdrubc ) /* 1991 "Konami" "Sunset Riders (2 Players ver UBC)" */
	DRIVER( ssrdruda ) /* 1991 "Konami" "Sunset Riders (4 Players ver UDA)" */
	DRIVER( ssriders ) /* 1991 "Konami" "Sunset Riders (4 Players ver EAC)" */
	DRIVER( stratgys ) /* 1981 "[Konami] (Stern license)" "Strategy X (Stern)" */
	DRIVER( stratgyx ) /* 1981 "Konami" "Strategy X" */
	DRIVER( strfbomb ) /* 1981 "Omni" "Strafe Bomb" */
	DRIVER( superbon ) /* 198? "bootleg" "Super Bond" */
	DRIVER( suratk ) /* 1990 "Konami" "Surprise Attack (World ver. K)" */
	DRIVER( suratka ) /* 1990 "Konami" "Surprise Attack (Asia ver. L)" */
	DRIVER( suratkj ) /* 1990 "Konami" "Surprise Attack (Japan ver. M)" */
	DRIVER( susume ) /* 1996 "Konami" "Susume! Taisen Puzzle-Dama (GV027 JAPAN 1.20)" */
	DRIVER( tazmani2 ) /* 1982 "Stern" "Tazz-Mania (set 2)" */
	DRIVER( tazmania ) /* 1982 "Stern" "Tazz-Mania (set 1)" */
	DRIVER( tbyahhoo ) /* 1995 "Konami" "Twin Bee Yahhoo! (ver JAA)" */
	DRIVER( theend ) /* 1980 "Konami" "The End" */
	DRIVER( theends ) /* 1980 "[Konami] (Stern license)" "The End (Stern)" */
	DRIVER( thehustj ) /* 1987 "Konami" "The Hustler (Japan version J)" */
	DRIVER( thehustl ) /* 1987 "Konami" "The Hustler (Japan version M)" */
	DRIVER( thnderxa ) /* 1988 "Konami" "Thunder Cross (Set 2)" */
	DRIVER( thnderxb ) /* 1988 "Konami" "Thunder Cross (Set 3)" */
	DRIVER( thnderxj ) /* 1988 "Konami" "Thunder Cross (Japan)" */
	DRIVER( thndrx2 ) /* 1991 "Konami" "Thunder Cross II (Japan)" */
	DRIVER( thndrx2a ) /* 1991 "Konami" "Thunder Cross II (Asia)" */
	DRIVER( thunderx ) /* 1988 "Konami" "Thunder Cross" */
	DRIVER( timeplt ) /* 1982 "Konami" "Time Pilot" */
	DRIVER( timeplta ) /* 1982 "Konami (Atari license)" "Time Pilot (Atari)" */
	DRIVER( timepltc ) /* 1982 "Konami (Centuri license)" "Time Pilot (Centuri)" */
	DRIVER( tkmmpzdm ) /* 1995 "Konami" "Tokimeki Memorial Taisen Puzzle-dama (ver JAB)" */
	DRIVER( tmht ) /* 1989 "Konami" "Teenage Mutant Hero Turtles (UK 4 Players)" */
	DRIVER( tmht2p ) /* 1989 "Konami" "Teenage Mutant Hero Turtles (UK 2 Players)" */
	DRIVER( tmnt ) /* 1989 "Konami" "Teenage Mutant Ninja Turtles (World 4 Players)" */
	DRIVER( tmnt2 ) /* 1991 "Konami" "Teenage Mutant Ninja Turtles - Turtles in Time (4 Players ver UAA)" */
	DRIVER( tmnt22p ) /* 1991 "Konami" "Teenage Mutant Ninja Turtles - Turtles in Time (2 Players ver UDA)" */
	DRIVER( tmnt2a ) /* 1991 "Konami" "Teenage Mutant Ninja Turtles - Turtles in Time (4 Players ver ADA)" */
	DRIVER( tmnt2pj ) /* 1990 "Konami" "Teenage Mutant Ninja Turtles (Japan 2 Players)" */
	DRIVER( tmnt2po ) /* 1989 "Konami" "Teenage Mutant Ninja Turtles (Oceania 2 Players)" */
	DRIVER( tmntj ) /* 1990 "Konami" "Teenage Mutant Ninja Turtles (Japan 4 Players)" */
	DRIVER( tmntu ) /* 1989 "Konami" "Teenage Mutant Ninja Turtles (US 4 Players */
	DRIVER( tmntua ) /* 1989 "Konami" "Teenage Mutant Ninja Turtles (US 4 Players */
	DRIVER( tokkae ) /* 1996 "Konami" "Taisen Tokkae-dama (ver JAA)" */
	DRIVER( topgunbl ) /* 1986 "bootleg" "Top Gunner (bootleg)" */
	DRIVER( topgunr ) /* 1986 "Konami" "Top Gunner (US)" */
	DRIVER( tp84 ) /* 1984 "Konami" "Time Pilot '84 (set 1)" */
	DRIVER( tp84a ) /* 1984 "Konami" "Time Pilot '84 (set 2)" */
	DRIVER( tp84b ) /* 1984 "Konami" "Time Pilot '84 (set 3)" */
	DRIVER( trackflc ) /* 1983 "Konami (Centuri license)" "Track & Field (Centuri)" */
	DRIVER( trackfld ) /* 1983 "Konami" "Track & Field" */
	DRIVER( tricktrp ) /* 1987 "Konami" "Trick Trap (World?)" */
	DRIVER( trigon ) /* 1990 "Konami" "Trigon (Japan)" */
	DRIVER( triplep ) /* 1982 "KKI" "Triple Punch" */
	DRIVER( turpin ) /* 1981 "[Konami] (Sega license)" "Turpin" */
	DRIVER( turtles ) /* 1981 "[Konami] (Stern license)" "Turtles" */
	DRIVER( tutankhm ) /* 1982 "Konami" "Tutankham" */
	DRIVER( tutankst ) /* 1982 "[Konami] (Stern license)" "Tutankham (Stern)" */
	DRIVER( twinbee ) /* 1985 "Konami" "TwinBee" */
	DRIVER( typhoon ) /* 1987 "Konami" "Typhoon" */
	DRIVER( ultraman ) /* 1991 "Banpresto/Bandai" "Ultraman (Japan)" */
	DRIVER( vendet2p ) /* 1991 "Konami" "Vendetta (World 2 Players ver. W)" */
	DRIVER( vendetao ) /* 1991 "Konami" "Vendetta (World 4 Players ver. R)" */
	DRIVER( vendetas ) /* 1991 "Konami" "Vendetta (Asia 2 Players ver. U)" */
	DRIVER( vendetta ) /* 1991 "Konami" "Vendetta (World 4 Players ver. T)" */
	DRIVER( vendettj ) /* 1991 "Konami" "Crime Fighters 2 (Japan 2 Players ver. P)" */
	DRIVER( vendtaso ) /* 1991 "Konami" "Vendetta (Asia 2 Players ver. D)" */
	DRIVER( venus ) /* 1983 "bootleg" "Venus" */
	DRIVER( viostorm ) /* 1993 "Konami" "Violent Storm (ver EAB)" */
	DRIVER( viostrma ) /* 1993 "Konami" "Violent Storm (ver AAC)" */
	DRIVER( viostrmj ) /* 1993 "Konami" "Violent Storm (ver JAC)" */
	DRIVER( viostrmu ) /* 1993 "Konami" "Violent Storm (ver UAB)" */
	DRIVER( vulcan ) /* 1988 "Konami" "Vulcan Venture" */
	DRIVER( wecleman ) /* 1986 "Konami" "WEC Le Mans 24" */
	DRIVER( weddingr ) /* 1997 "Konami" "Wedding Rhapsody (GX624 JAA)" */
	DRIVER( winspike ) /* 1997 "Konami" "Winning Spike (ver EAA)" */
	DRIVER( winspikj ) /* 1997 "Konami" "Winning Spike (ver JAA)" */
	DRIVER( wizzquiz ) /* 1985 "Konami" "Wizz Quiz (Konami version)" */
	DRIVER( wizzquza ) /* 1985 "Zilec - Zenitone" "Wizz Quiz (version 4)" */
	DRIVER( xexex ) /* 1991 "Konami" "Xexex (ver EAA)" */
	DRIVER( xexexa ) /* 1991 "Konami" "Xexex (ver AAA)" */
	DRIVER( xexexj ) /* 1991 "Konami" "Xexex (ver JAA)" */
	DRIVER( xmen ) /* 1992 "Konami" "X-Men (4 Players ver UBB)" */
	DRIVER( xmen2p ) /* 1992 "Konami" "X-Men (2 Players ver AAA)" */
	DRIVER( xmen2pj ) /* 1992 "Konami" "X-Men (2 Players ver JAA)" */
	DRIVER( xmen6p ) /* 1992 "Konami" "X-Men (6 Players ver ECB)" */
	DRIVER( xmen6pu ) /* 1992 "Konami" "X-Men (6 Players ver UCB)" */
	DRIVER( yiear ) /* 1985 "Konami" "Yie Ar Kung-Fu (set 1)" */
	DRIVER( yiear2 ) /* 1985 "Konami" "Yie Ar Kung-Fu (set 2)" */
#endif
#ifdef LINK_MEADOWS
	DRIVER( bbonk ) /* 1976 "Meadows Games Inc." */
	DRIVER( deadeye ) /* 1978 "Meadows" "Dead Eye" */
	DRIVER( gypsyjug ) /* 1978 "Meadows" "Gypsy Juggler" */
	DRIVER( lazercmd ) /* 1976 "Meadows Games Inc." */
	DRIVER( medlanes ) /* 1977 "Meadows Games Inc." */
	DRIVER( minferno ) /* 1978 "Meadows" "Inferno (S2650)" */
#endif
#ifdef LINK_MERIT
	DRIVER( phrcraze ) /* 1986 "Merit" "Phraze Craze" */
	DRIVER( phrcrazs ) /* 1986 "Merit" "Phraze Craze (Sex Kit)" */
	DRIVER( tictac ) /* 1985 "Merit" "Tic Tac Trivia" */
	DRIVER( trvwhzha ) /* 1985 "Merit" "Trivia ? Whiz (Horizontal - Question set 2)" */
	DRIVER( trvwhzho ) /* 1985 "Merit" "Trivia ? Whiz (Horizontal - Question set 1)" */
	DRIVER( trvwhzii ) /* 1985 "Merit" "Trivia ? Whiz (Edition 2)" */
	DRIVER( trvwhziv ) /* 1985 "Merit" "Trivia ? Whiz (Edition 4)" */
	DRIVER( trvwhzva ) /* 1985 "Merit" "Trivia ? Whiz (Vertical - Question set 2)" */
	DRIVER( trvwhzve ) /* 1985 "Merit" "Trivia ? Whiz (Vertical - Question set 1)" */
#endif
#ifdef LINK_METRO
	DRIVER( 3kokushi ) /* 1996 "Mitchell" "Sankokushi (Japan)" */
	DRIVER( balcube ) /* 1996 "Metro" "Bal Cube" */
	DRIVER( bangball ) /* 1996 "Banpresto / Kunihiko Tashiro+Goodhouse" "Bang Bang Ball (v1.05)" */
	DRIVER( batlbubl ) /* 1999 "Limenko" "Battle Bubble (v2.00)" */
	DRIVER( blzntrnd ) /* 1994 "Human Amusement" "Blazing Tornado" */
	DRIVER( daitorid ) /* 1995 "Metro" "Daitoride" */
	DRIVER( dharma ) /* 1994 "Metro" "Dharma Doujou" */
	DRIVER( dokyusei ) /* 1995 "Make Software / Elf / Media Trading" "Mahjong Doukyuusei" */
	DRIVER( dokyusp ) /* 1995 "Make Software / Elf / Media Trading" "Mahjong Doukyuusei Special" */
	DRIVER( gakusai ) /* 1997 "MakeSoft" "Mahjong Gakuensai (Japan)" */
	DRIVER( gakusai2 ) /* 1998 "MakeSoft" "Mahjong Gakuensai 2 (Japan)" */
	DRIVER( gstrik2 ) /* 1996 "Human Amusement" "Grand Striker 2 (Japan)" */
	DRIVER( gunmast ) /* 1994 "Metro" "Gun Master" */
	DRIVER( hyprdelj ) /* 1993 "Technosoft" "Hyper Duel (Japan set 2)" */
	DRIVER( hyprduel ) /* 1993 "Technosoft" "Hyper Duel (Japan set 1)" */
	DRIVER( karatour ) /* 1992 "Mitchell" "The Karate Tournament" */
	DRIVER( ladykill ) /* 1993? "Yanyaka (Mitchell license)" "Lady Killer" */
	DRIVER( lastfero ) /* 1994 "Metro" "Last Fortress - Toride (Erotic)" */
	DRIVER( lastforg ) /* 1994 "Metro" "Last Fortress - Toride (German)" */
	DRIVER( lastfort ) /* 1994 "Metro" "Last Fortress - Toride" */
	DRIVER( moegonta ) /* 1993? "Yanyaka" "Moeyo Gonta!! (Japan)" */
	DRIVER( mouja ) /* 1996 "Etona" "Mouja (Japan)" */
	DRIVER( pangpomm ) /* 1992 "Metro (Mitchell license)" "Pang Poms (Mitchell)" */
	DRIVER( pangpoms ) /* 1992 "Metro" "Pang Poms" */
	DRIVER( poitto ) /* 1993 "Metro / Able Corp." "Poitto!" */
	DRIVER( pururun ) /* 1995 "Metro / Banpresto" "Pururun" */
	DRIVER( puzzli ) /* 1995 "Metro / Banpresto" "Puzzli" */
	DRIVER( rabbit ) /* 1997 "Electronic Arts / Aorn" "Rabbit" */
	DRIVER( skyalert ) /* 1992 "Metro" "Sky Alert" */
	DRIVER( torid2gg ) /* 1994 "Metro" "Toride II Adauchi Gaiden (German)" */
	DRIVER( toride2g ) /* 1994 "Metro" "Toride II Adauchi Gaiden" */
#endif
#ifdef LINK_MIDCOIN
	DRIVER( wallc ) /* 1984 "Midcoin" "Wall Crash (set 1)" */
	DRIVER( wallca ) /* 1984 "Midcoin" "Wall Crash (set 2)" */
	DRIVER( wink ) /* 1985 "Midcoin" "Wink (set 1)" */
	DRIVER( winka ) /* 1985 "Midcoin" "Wink (set 2)" */
#endif
#ifdef LINK_MIDW8080
	DRIVER( 280zzzap ) /* 1976 "Midway" "Datsun 280 Zzzap" */
	DRIVER( alieninv ) /* 19?? "bootleg" "Alien Invasion Part II" */
	DRIVER( ballbomb ) /* 1980 "Taito" "Balloon Bomber" */
	DRIVER( blueshrk ) /* 1978 "Midway" "Blue Shark" */
	DRIVER( boothill ) /* 1977 "Midway" "Boot Hill" */
	DRIVER( bowler ) /* 1978 "Midway" "4 Player Bowling Alley" */
	DRIVER( checkmat ) /* 1977 "Midway" "Checkmate" */
	DRIVER( clowns ) /* 1978 "Midway" "Clowns (rev. 2)" */
	DRIVER( clowns1 ) /* 1978 "Midway" "Clowns (rev. 1)" */
	DRIVER( cosmicm2 ) /* 1979 "Universal" "Cosmic Monsters 2" */
	DRIVER( cosmicmo ) /* 1979 "Universal" "Cosmic Monsters" */
	DRIVER( cosmo ) /* 1979 "TDS & Mints" "Cosmo" */
	DRIVER( desertgu ) /* 1977 "Midway" "Desert Gun" */
	DRIVER( desterth ) /* 1979 "bootleg" "Destination Earth" */
	DRIVER( dogpatch ) /* 1977 "Midway" "Dog Patch" */
	DRIVER( dplay ) /* 1977 "Midway" "Double Play" */
	DRIVER( einnings ) /* 1978 "Midway" "Extra Inning" */
	DRIVER( galxwar2 ) /* 1979 "Universal" "Galaxy Wars (Universal set 2)" */
	DRIVER( galxwars ) /* 1979 "Universal" "Galaxy Wars (Universal set 1)" */
	DRIVER( galxwart ) /* 1979 "Taito?" "Galaxy Wars (Taito?)" */
	DRIVER( gmissile ) /* 1977 "Midway" "Guided Missile" */
	DRIVER( grescue ) /* 1979 "Taito (Universal license?)" "Galaxy Rescue" */
	DRIVER( gunfight ) /* 1975 "Midway" "Gun Fight" */
	DRIVER( indianbt ) /* 1980 "Taito" "Indian Battle" */
	DRIVER( invad2ct ) /* 1980 "Midway" "Space Invaders II (Midway */
	DRIVER( invaddlx ) /* 1980 "Midway" "Space Invaders Deluxe" */
	DRIVER( invader4 ) /* 1978 "bootleg" "Space Invaders Part Four" */
	DRIVER( invaderl ) /* 1978 "bootleg" "Space Invaders (Logitec)" */
	DRIVER( invaders ) /* 1978 "Midway" "Space Invaders" */
	DRIVER( invadpt2 ) /* 1979 "Taito" "Space Invaders Part II (Taito)" */
	DRIVER( invadrmr ) /* 1978 "Model Racing" "Space Invaders (Model Racing)" */
	DRIVER( invasion ) /* 19?? "Sidam" "Invasion" */
	DRIVER( invrvnga ) /* 19?? "Zenitone-Microsec Ltd. (Dutchford license)" "Invader's Revenge (Dutchford)" */
	DRIVER( invrvnge ) /* 19?? "Zenitone-Microsec Ltd." "Invader's Revenge" */
	DRIVER( jspecter ) /* 1979 "Jatre" "Jatre Specter (set 1)" */
	DRIVER( jspectr2 ) /* 1979 "Jatre" "Jatre Specter (set 2)" */
	DRIVER( lagunar ) /* 1977 "Midway" "Laguna Racer" */
	DRIVER( laser ) /* 1980 "<unknown>" "Laser" */
	DRIVER( lrescue ) /* 1979 "Taito" "Lunar Rescue" */
	DRIVER( lupin3 ) /* 1980 "Taito" "Lupin III" */
	DRIVER( m4 ) /* 1977 "Midway" "M-4" */
	DRIVER( m79amb ) /* 1977 "RamTek" "M79 Ambush" */
	DRIVER( maze ) /* 1976 "Midway" "Amazing Maze" */
	DRIVER( moonbase ) /* 1979 "Nichibutsu" "Moon Base" */
	DRIVER( ozmawar2 ) /* 1979 "SNK" "Ozma Wars (set 2)" */
	DRIVER( ozmawars ) /* 1979 "SNK" "Ozma Wars (set 1)" */
	DRIVER( phantom2 ) /* 1979 "Midway" "Phantom II" */
	DRIVER( polaris ) /* 1980 "Taito" "Polaris (set 1)" */
	DRIVER( polarisa ) /* 1980 "Taito" "Polaris (set 2)" */
	DRIVER( rollingc ) /* 1979 "Nichibutsu" "Rolling Crash / Moon Base" */
	DRIVER( rotaryf ) /* 19?? "<unknown>" "Rotary Fighter" */
	DRIVER( schaser ) /* 1979 "Taito" "Space Chaser" */
	DRIVER( schasrcv ) /* 1979 "Taito" "Space Chaser (CV version)" */
	DRIVER( searthia ) /* 1980 "bootleg" "Super Earth Invasion (set 2)" */
	DRIVER( searthin ) /* 1980 "bootleg" "Super Earth Invasion (set 1)" */
	DRIVER( seawolf ) /* 1976 "Midway" "Sea Wolf" */
	DRIVER( sflush ) /* 1979 "Taito" "Straight Flush" */
	DRIVER( shuffle ) /* 1978 "Midway" "Shuffleboard" */
	DRIVER( shuttlei ) /* 197? "Omori" "Shuttle Invader" */
	DRIVER( sicv ) /* 1979 "Taito" "Space Invaders (CV Version)" */
	DRIVER( sinvemag ) /* 19?? "bootleg" "Super Invaders (EMAG)" */
	DRIVER( sinvzen ) /* 19?? "Zenitone-Microsec Ltd" "Super Invaders (Zenitone-Microsec)" */
	DRIVER( sisv ) /* 1978 "Taito" "Space Invaders (SV Version)" */
	DRIVER( sisv2 ) /* 1978 "Taito" "Space Invaders (SV Version 2)" */
	DRIVER( sitv ) /* 1978 "Taito" "Space Invaders (TV Version)" */
	DRIVER( solfight ) /* 1979 "bootleg" "Solar Fight" */
	DRIVER( spaceat2 ) /* 1980 "Zenitone-Microsec Ltd" "Space Attack II" */
	DRIVER( spaceatt ) /* 1978 "Video Games GMBH" "Space Attack" */
	DRIVER( spaceph ) /* 1979 "Zilec Games" "Space Phantoms" */
	DRIVER( spacewr3 ) /* 1978 "bootleg" "Space War Part 3" */
	DRIVER( spceking ) /* 1978 "Leijac (Konami)" "Space King" */
	DRIVER( spcenctr ) /* 1980 "Midway" "Space Encounters" */
	DRIVER( spcewarl ) /* 1979 "Leijac (Konami)" "Space War (Leijac)" */
	DRIVER( spcewars ) /* 1978 "Sanritsu" "Space War (Sanritsu)" */
	DRIVER( spclaser ) /* 1980 "GamePlan (Taito)" "Space Laser" */
	DRIVER( sspeedr ) /* 1979 "Midway" "Super Speed Race" */
	DRIVER( sstrangr ) /* 1978 "Yachiyo Electronics Ltd." */
	DRIVER( sstrngr2 ) /* 1979 "Yachiyo Electronics Ltd." */
	DRIVER( starw ) /* 1979 "bootleg" "Star Wars" */
	DRIVER( superinv ) /* 19?? "bootleg" "Super Invaders" */
	DRIVER( tornbase ) /* 1976 "Midway" "Tornado Baseball" */
	DRIVER( tst_invd ) /* 19?? "Test ROM" "Space Invaders Test ROM" */
	DRIVER( yosakdoa ) /* 1979 "Wing" "Yosaku To Donbei (set 2)" */
	DRIVER( yosakdon ) /* 1979 "Wing" "Yosaku To Donbei (set 1)" */
#endif
#ifdef LINK_MIDWAY
	DRIVER( alienar ) /* 1985 "Duncan Brown" "Alien Arena" */
	DRIVER( alienaru ) /* 1985 "Duncan Brown" "Alien Arena (Stargate Upgrade)" */
	DRIVER( archriv2 ) /* 1989 "Bally Midway" "Arch Rivals (rev 2.0)" */
	DRIVER( archrivl ) /* 1989 "Bally Midway" "Arch Rivals (rev 4.0)" */
	DRIVER( biofreak ) /* 1997 "Midway Games" "BioFreaks (prototype)" */
	DRIVER( blast30 ) /* 1983 "Williams" "Blaster (early 30 wave version)" */
	DRIVER( blasted ) /* 1988 "Bally Midway" "Blasted" */
	DRIVER( blaster ) /* 1983 "Williams" "Blaster" */
	DRIVER( blastkit ) /* 1983 "Williams" "Blaster (kit)" */
	DRIVER( blitz ) /* 1997 "Midway Games" "NFL Blitz (boot ROM 1.2)" */
	DRIVER( blitz11 ) /* 1997 "Midway Games" "NFL Blitz (boot ROM 1.1)" */
	DRIVER( blitz2k ) /* 1999 "Midway Games" "NFL Blitz 2000 Gold Edition" */
	DRIVER( blitz99 ) /* 1998 "Midway Games" "NFL Blitz '99" */
	DRIVER( bubbles ) /* 1982 "Williams" "Bubbles" */
	DRIVER( bubblesp ) /* 1982 "Williams" "Bubbles (prototype version)" */
	DRIVER( bubblesr ) /* 1982 "Williams" "Bubbles (Solid Red label)" */
	DRIVER( calspeda ) /* 1998 "Atari Games" "California Speed (Version 1.0r7a 3/4/98)" */
	DRIVER( calspeed ) /* 1998 "Atari Games" "California Speed (Version 2.1a */
	DRIVER( carnevil ) /* 1998 "Midway Games" "CarnEvil" */
	DRIVER( colony7 ) /* 1981 "Taito" "Colony 7 (set 1)" */
	DRIVER( colony7a ) /* 1981 "Taito" "Colony 7 (set 2)" */
	DRIVER( crater ) /* 1984 "Bally Midway" "Crater Raider" */
	DRIVER( crusnu21 ) /* 1994 "Midway" "Cruis'n USA (rev L2.1)" */
	DRIVER( crusnu40 ) /* 1994 "Midway" "Cruis'n USA (rev L4.0)" */
	DRIVER( crusnusa ) /* 1994 "Midway" "Cruis'n USA (rev L4.1)" */
	DRIVER( crusnw13 ) /* 1996 "Midway" "Cruis'n World (rev L1.3)" */
	DRIVER( crusnw20 ) /* 1996 "Midway" "Cruis'n World (rev L2.0)" */
	DRIVER( crusnwld ) /* 1996 "Midway" "Cruis'n World (rev L2.3)" */
	DRIVER( cshift ) /* 1984 "Bally/Sente" "Chicken Shift" */
	DRIVER( defcmnd ) /* 1980 "bootleg" "Defense Command (set 1)" */
	DRIVER( defence ) /* 1981 "Outer Limits" "Defence Command" */
	DRIVER( defender ) /* 1980 "Williams" "Defender (Red label)" */
	DRIVER( defendg ) /* 1980 "Williams" "Defender (Green label)" */
	DRIVER( defendw ) /* 1980 "Williams" "Defender (White label)" */
	DRIVER( deltrace ) /* 1981 "Allied Leisure" "Delta Race" */
	DRIVER( demndrgn ) /* 1982 "Bally Midway" "Demons and Dragons (prototype)" */
	DRIVER( demoderb ) /* 1984 "Bally Midway" "Demolition Derby" */
	DRIVER( demoderm ) /* 1984 "Bally Midway" "Demolition Derby (2-Player Mono Board Version)" */
	DRIVER( domino ) /* 1982 "Bally Midway" "Domino Man" */
	DRIVER( dotron ) /* 1983 "Bally Midway" "Discs of Tron (Upright)" */
	DRIVER( dotrona ) /* 1983 "Bally Midway" "Discs of Tron (Upright alternate)" */
	DRIVER( dotrone ) /* 1983 "Bally Midway" "Discs of Tron (Environmental)" */
	DRIVER( ebases ) /* 1980 "Midway" "Extra Bases" */
	DRIVER( gauntd24 ) /* 1999 "Midway Games" "Gauntlet Dark Legacy (version DL 2.4)" */
	DRIVER( gauntdl ) /* 1999 "Midway Games" "Gauntlet Dark Legacy (version DL 2.52)" */
	DRIVER( gauntl12 ) /* 1998 "Atari Games" "Gauntlet Legends (version 1.2)" */
	DRIVER( gauntleg ) /* 1998 "Atari Games" "Gauntlet Legends (version 1.6)" */
	DRIVER( gghost ) /* 1984 "Bally/Sente" "Goalie Ghost" */
	DRIVER( gimeabrk ) /* 1985 "Bally/Sente" "Gimme A Break" */
	DRIVER( gorf ) /* 1981 "Midway" "Gorf" */
	DRIVER( gorfpgm1 ) /* 1981 "Midway" "Gorf (Program 1)" */
	DRIVER( gridlee ) /* 1983 "Videa" "Gridlee" */
	DRIVER( grudge ) /* 198? "Bally/Midway" "Grudge Match (prototype)" */
	DRIVER( hattrick ) /* 1984 "Bally/Sente" "Hat Trick" */
	DRIVER( hiimpac3 ) /* 1990 "Williams" "High Impact Football (rev LA3 12/27/90)" */
	DRIVER( hiimpacp ) /* 1990 "Williams" "High Impact Football (prototype */
	DRIVER( hiimpact ) /* 1990 "Williams" "High Impact Football (rev LA4 02/04/91)" */
	DRIVER( hyprdriv ) /* 1998 "Midway Games" "Hyperdrive" */
	DRIVER( inferno ) /* 1984 "Williams" "Inferno" */
	DRIVER( jdreddp ) /* 1993 "Midway" "Judge Dredd (rev LA1 */
	DRIVER( jin ) /* 1982 "Falcon" "Jin" */
	DRIVER( journey ) /* 1983 "Bally Midway" "Journey" */
	DRIVER( joust ) /* 1982 "Williams" "Joust (White/Green label)" */
	DRIVER( joust2 ) /* 1986 "Williams" "Joust 2 - Survival of the Fittest (set 1)" */
	DRIVER( joustr ) /* 1982 "Williams" "Joust (Solid Red label)" */
	DRIVER( joustwr ) /* 1982 "Williams" "Joust (White/Red label)" */
	DRIVER( kick ) /* 1981 "Midway" "Kick (upright)" */
	DRIVER( kicka ) /* 1981 "Midway" "Kick (cocktail)" */
	DRIVER( kroozr ) /* 1982 "Bally Midway" "Kozmik Kroozr" */
	DRIVER( lottofun ) /* 1987 "H.A.R. Management" "Lotto Fun" */
	DRIVER( mace ) /* 1996 "Atari Games" "Mace: The Dark Age (boot ROM 1.0ce */
	DRIVER( macea ) /* 1997 "Atari Games" "Mace: The Dark Age (HDD 1.0a" */
	DRIVER( maxrpm ) /* 1986 "Bally Midway" "Max RPM" */
	DRIVER( mayday ) /* 1980 "<unknown>" "Mayday (set 1)" */
	DRIVER( maydaya ) /* 1980 "<unknown>" "Mayday (set 2)" */
	DRIVER( maydayb ) /* 1980 "<unknown>" "Mayday (set 3)" */
	DRIVER( minigol2 ) /* 1985 "Bally/Sente" "Mini Golf (set 2)" */
	DRIVER( minigolf ) /* 1985 "Bally/Sente" "Mini Golf (set 1)" */
	DRIVER( mk ) /* 1992 "Midway" "Mortal Kombat (rev 5.0 T-Unit 03/19/93)" */
	DRIVER( mk2 ) /* 1993 "Midway" "Mortal Kombat II (rev L3.1)" */
	DRIVER( mk2chal ) /* 1993 "hack" "Mortal Kombat II Challenger (hack)" */
	DRIVER( mk2r14 ) /* 1993 "Midway" "Mortal Kombat II (rev L1.4)" */
	DRIVER( mk2r21 ) /* 1993 "Midway" "Mortal Kombat II (rev L2.1)" */
	DRIVER( mk2r32 ) /* 1993 "Midway" "Mortal Kombat II (rev L3.2 (European))" */
	DRIVER( mk2r42 ) /* 1993 "hack" "Mortal Kombat II (rev L4.2 */
	DRIVER( mk2r91 ) /* 1993 "hack" "Mortal Kombat II (rev L9.1 */
	DRIVER( mk3 ) /* 1994 "Midway" "Mortal Kombat 3 (rev 2.1)" */
	DRIVER( mk3r10 ) /* 1994 "Midway" "Mortal Kombat 3 (rev 1.0)" */
	DRIVER( mk3r20 ) /* 1994 "Midway" "Mortal Kombat 3 (rev 2.0)" */
	DRIVER( mkla1 ) /* 1992 "Midway" "Mortal Kombat (rev 1.0 08/09/92)" */
	DRIVER( mkla2 ) /* 1992 "Midway" "Mortal Kombat (rev 2.0 08/18/92)" */
	DRIVER( mkla3 ) /* 1992 "Midway" "Mortal Kombat (rev 3.0 08/31/92)" */
	DRIVER( mkla4 ) /* 1992 "Midway" "Mortal Kombat (rev 4.0 09/28/92)" */
	DRIVER( mkprot9 ) /* 1992 "Midway" "Mortal Kombat (prototype */
	DRIVER( mkr4 ) /* 1992 "Midway" "Mortal Kombat (rev 4.0 T-Unit 02/11/93)" */
	DRIVER( mkyawdim ) /* 1992 "Midway" "Mortal Kombat (Yawdim bootleg)" */
	DRIVER( mysticm ) /* 1983 "Williams" "Mystic Marathon" */
	DRIVER( nametune ) /* 1986 "Bally/Sente" "Name That Tune" */
	DRIVER( narc ) /* 1988 "Williams" "Narc (rev 7.00)" */
	DRIVER( narc3 ) /* 1988 "Williams" "Narc (rev 3.20)" */
	DRIVER( nbahangt ) /* 1996 "Midway" "NBA Hangtime (rev L1.1 04/16/96)" */
	DRIVER( nbajam ) /* 1993 "Midway" "NBA Jam (rev 3.01 04/07/93)" */
	DRIVER( nbajamr2 ) /* 1993 "Midway" "NBA Jam (rev 2.00 02/10/93)" */
	DRIVER( nbajamt1 ) /* 1994 "Midway" "NBA Jam TE (rev 1.0 01/17/94)" */
	DRIVER( nbajamt2 ) /* 1994 "Midway" "NBA Jam TE (rev 2.0 01/28/94)" */
	DRIVER( nbajamt3 ) /* 1994 "Midway" "NBA Jam TE (rev 3.0 03/04/94)" */
	DRIVER( nbajamte ) /* 1994 "Midway" "NBA Jam TE (rev 4.0 03/23/94)" */
	DRIVER( nbamht ) /* 1996 "Midway" "NBA Maximum Hangtime (rev 1.03 06/09/97)" */
	DRIVER( nbamht1 ) /* 1996 "Midway" "NBA Maximum Hangtime (rev 1.0 11/08/96)" */
	DRIVER( nstocker ) /* 1986 "Bally/Sente" "Night Stocker" */
	DRIVER( omegrace ) /* 1981 "Midway" "Omega Race" */
	DRIVER( openice ) /* 1995 "Midway" "2 On 2 Open Ice Challenge (rev 1.21)" */
	DRIVER( otwalls ) /* 1984 "Bally/Sente" "Off the Wall (Sente)" */
	DRIVER( pigskin ) /* 1990 "Midway" "Pigskin 621AD" */
	DRIVER( playball ) /* 1983 "Williams" "PlayBall! (prototype)" */
	DRIVER( powerdrv ) /* 1986 "Bally Midway" "Power Drive" */
	DRIVER( profpac ) /* 1983 "Bally Midway" "Professor PacMan" */
	DRIVER( rampage ) /* 1986 "Bally Midway" "Rampage (revision 3)" */
	DRIVER( rampage2 ) /* 1986 "Bally Midway" "Rampage (revision 2)" */
	DRIVER( rbtapper ) /* 1984 "Bally Midway" "Tapper (Root Beer)" */
	DRIVER( rescraid ) /* 1987 "Bally/Midway" "Rescue Raider" */
	DRIVER( rescrdsa ) /* 1987 "Bally/Midway" "Rescue Raider (Stand-Alone)" */
	DRIVER( revx ) /* 1994 "Midway" "Revolution X (Rev. 1.0 6/16/94)" */
	DRIVER( rmpgwt ) /* 1997 "Midway" "Rampage: World Tour (rev 1.3)" */
	DRIVER( rmpgwt11 ) /* 1997 "Midway" "Rampage: World Tour (rev 1.1)" */
	DRIVER( robby ) /* 1981 "Bally Midway" "Robby Roto" */
	DRIVER( robotron ) /* 1982 "Williams" "Robotron (Solid Blue label)" */
	DRIVER( robotryo ) /* 1982 "Williams" "Robotron (Yellow/Orange label)" */
	DRIVER( sarge ) /* 1985 "Bally Midway" "Sarge" */
	DRIVER( seawolf2 ) /* 1978 "Midway" "Sea Wolf II" */
	DRIVER( sentetst ) /* 1984 "Bally/Sente" "Sente Diagnostic Cartridge" */
	DRIVER( sfootbal ) /* 1986 "Bally/Sente" "Street Football" */
	DRIVER( sfrush ) /* 1996 "Atari Games" "San Francisco Rush" */
	DRIVER( shimpacp ) /* 1991 "Midway" "Super High Impact (prototype */
	DRIVER( shimpact ) /* 1991 "Midway" "Super High Impact (rev LA1 09/30/91)" */
	DRIVER( shollow ) /* 1981 "Bally Midway" "Satan's Hollow (set 1)" */
	DRIVER( shollow2 ) /* 1981 "Bally Midway" "Satan's Hollow (set 2)" */
	DRIVER( shrike ) /* 198? "Bally/Sente" "Shrike Avenger (prototype)" */
	DRIVER( sinista1 ) /* 1982 "Williams" "Sinistar (prototype version)" */
	DRIVER( sinista2 ) /* 1982 "Williams" "Sinistar (revision 2)" */
	DRIVER( sinistar ) /* 1982 "Williams" "Sinistar (revision 3)" */
	DRIVER( smashtv ) /* 1990 "Williams" "Smash T.V. (rev 8.00)" */
	DRIVER( smashtv4 ) /* 1990 "Williams" "Smash T.V. (rev 4.00)" */
	DRIVER( smashtv5 ) /* 1990 "Williams" "Smash T.V. (rev 5.00)" */
	DRIVER( smashtv6 ) /* 1990 "Williams" "Smash T.V. (rev 6.00)" */
	DRIVER( snakepit ) /* 1984 "Bally/Sente" "Snake Pit" */
	DRIVER( snakjack ) /* 1984 "Bally/Sente" "Snacks'n Jaxson" */
	DRIVER( solarfox ) /* 1981 "Bally Midway" "Solar Fox (upright)" */
	DRIVER( spacezap ) /* 1980 "Midway" "Space Zap" */
	DRIVER( spdball ) /* 1985 "Williams" "Speed Ball (prototype)" */
	DRIVER( spiker ) /* 1986 "Bally/Sente" "Spiker" */
	DRIVER( splat ) /* 1982 "Williams" "Splat!" */
	DRIVER( spyhnt2a ) /* 1987 "Bally Midway" "Spy Hunter 2 (rev 1)" */
	DRIVER( spyhunt ) /* 1983 "Bally Midway" "Spy Hunter" */
	DRIVER( spyhunt2 ) /* 1987 "Bally Midway" "Spy Hunter 2 (rev 2)" */
	DRIVER( stargate ) /* 1981 "Williams" "Stargate" */
	DRIVER( stargrds ) /* 1987 "Bally Midway" "Star Guards" */
	DRIVER( startrkd ) /* 1981 "bootleg" "Star Trek (Defender bootleg)" */
	DRIVER( stocker ) /* 1984 "Bally/Sente" "Stocker" */
	DRIVER( stompin ) /* 1986 "Bally/Sente" "Stompin'" */
	DRIVER( strkforc ) /* 1991 "Midway" "Strike Force (rev 1 02/25/91)" */
	DRIVER( sutapper ) /* 1983 "Bally Midway" "Tapper (Suntory)" */
	DRIVER( tapper ) /* 1983 "Bally Midway" "Tapper (Budweiser)" */
	DRIVER( tappera ) /* 1983 "Bally Midway" "Tapper (alternate)" */
	DRIVER( tenthdeg ) /* 1998 "Atari Games" "Tenth Degree" */
	DRIVER( term2 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA3 03/27/92)" */
	DRIVER( term2la1 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA1 11/01/91)" */
	DRIVER( term2la2 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA2 12/09/91)" */
	DRIVER( timber ) /* 1984 "Bally Midway" "Timber" */
	DRIVER( toggle ) /* 1985 "Bally/Sente" "Toggle (prototype)" */
	DRIVER( tornado1 ) /* 1980 "Jeutel" "Tornado (bootleg set 1)" */
	DRIVER( totcarn ) /* 1992 "Midway" "Total Carnage (rev LA1 03/10/92)" */
	DRIVER( totcarnp ) /* 1992 "Midway" "Total Carnage (prototype */
	DRIVER( trisport ) /* 1989 "Bally Midway" "Tri-Sports" */
	DRIVER( triviabb ) /* 1984 "Bally/Sente" "Trivial Pursuit (Baby Boomer Edition)" */
	DRIVER( triviaes ) /* 1987 "Bally/Sente" "Trivial Pursuit (Spanish Edition)" */
	DRIVER( triviag1 ) /* 1984 "Bally/Sente" "Trivial Pursuit (Genus I)" */
	DRIVER( triviag2 ) /* 1984 "Bally/Sente" "Trivial Pursuit (Genus II)" */
	DRIVER( triviasp ) /* 1984 "Bally/Sente" "Trivial Pursuit (All Star Sports Edition)" */
	DRIVER( triviayp ) /* 1984 "Bally/Sente" "Trivial Pursuit (Young Players Edition)" */
	DRIVER( trog ) /* 1990 "Midway" "Trog (rev LA4 03/11/91)" */
	DRIVER( trog3 ) /* 1990 "Midway" "Trog (rev LA3 02/14/91)" */
	DRIVER( trogp ) /* 1990 "Midway" "Trog (prototype */
	DRIVER( trogpa6 ) /* 1990 "Midway" "Trog (rev PA6-PAC 09/09/90)" */
	DRIVER( tron ) /* 1982 "Bally Midway" "Tron (set 1)" */
	DRIVER( tron2 ) /* 1982 "Bally Midway" "Tron (set 2)" */
	DRIVER( tron3 ) /* 1982 "Bally Midway" "Tron (set 3)" */
	DRIVER( tron4 ) /* 1982 "Bally Midway" "Tron (set 4)" */
	DRIVER( tshoot ) /* 1984 "Williams" "Turkey Shoot" */
	DRIVER( turbotag ) /* 1985 "Bally Midway" "Turbo Tag (prototype)" */
	DRIVER( twotiger ) /* 1984 "Bally Midway" "Two Tigers (dedicated)" */
	DRIVER( twotigrc ) /* 1984 "Bally Midway" "Two Tigers (Tron conversion)" */
	DRIVER( umk3 ) /* 1994 "Midway" "Ultimate Mortal Kombat 3 (rev 1.2)" */
	DRIVER( umk3r10 ) /* 1994 "Midway" "Ultimate Mortal Kombat 3 (rev 1.0)" */
	DRIVER( umk3r11 ) /* 1994 "Midway" "Ultimate Mortal Kombat 3 (rev 1.1)" */
	DRIVER( vaportrp ) /* 1998 "Atari Games" "Vapor TRX (prototype)" */
	DRIVER( vaportrx ) /* 1998 "Atari Games" "Vapor TRX" */
	DRIVER( wacko ) /* 1982 "Bally Midway" "Wacko" */
	DRIVER( wargods ) /* 1995 "Midway" "War Gods" */
	DRIVER( wg3dh ) /* 1996 "Atari Games" "Wayne Gretzky's 3D Hockey" */
	DRIVER( wow ) /* 1980 "Midway" "Wizard of Wor" */
	DRIVER( wwfmania ) /* 1995 "Midway" "WWF: Wrestlemania (rev 1.30 08/10/95)" */
	DRIVER( xenophob ) /* 1987 "Bally Midway" "Xenophobe" */
	DRIVER( zero ) /* 1980 "Jeutel" "Zero" */
	DRIVER( zwackery ) /* 1984 "Bally Midway" "Zwackery" */
#endif
#ifdef LINK_MIDWAYTUNIT
	DRIVER( hiimpac3 ) /* 1990 "Williams" "High Impact Football (rev LA3 12/27/90)" */
	DRIVER( hiimpacp ) /* 1990 "Williams" "High Impact Football (prototype */
	DRIVER( hiimpact ) /* 1990 "Williams" "High Impact Football (rev LA4 02/04/91)" */
	DRIVER( jdreddp ) /* 1993 "Midway" "Judge Dredd (rev LA1 */
	DRIVER( mk ) /* 1992 "Midway" "Mortal Kombat (rev 5.0 T-Unit 03/19/93)" */
	DRIVER( mk2 ) /* 1993 "Midway" "Mortal Kombat II (rev L3.1)" */
	DRIVER( mk2chal ) /* 1993 "hack" "Mortal Kombat II Challenger (hack)" */
	DRIVER( mk2r14 ) /* 1993 "Midway" "Mortal Kombat II (rev L1.4)" */
	DRIVER( mk2r21 ) /* 1993 "Midway" "Mortal Kombat II (rev L2.1)" */
	DRIVER( mk2r32 ) /* 1993 "Midway" "Mortal Kombat II (rev L3.2 (European))" */
	DRIVER( mk2r42 ) /* 1993 "hack" "Mortal Kombat II (rev L4.2 */
	DRIVER( mk2r91 ) /* 1993 "hack" "Mortal Kombat II (rev L9.1 */
	DRIVER( mkla1 ) /* 1992 "Midway" "Mortal Kombat (rev 1.0 08/09/92)" */
	DRIVER( mkla2 ) /* 1992 "Midway" "Mortal Kombat (rev 2.0 08/18/92)" */
	DRIVER( mkla3 ) /* 1992 "Midway" "Mortal Kombat (rev 3.0 08/31/92)" */
	DRIVER( mkla4 ) /* 1992 "Midway" "Mortal Kombat (rev 4.0 09/28/92)" */
	DRIVER( mkprot9 ) /* 1992 "Midway" "Mortal Kombat (prototype */
	DRIVER( mkr4 ) /* 1992 "Midway" "Mortal Kombat (rev 4.0 T-Unit 02/11/93)" */
	DRIVER( mkyawdim ) /* 1992 "Midway" "Mortal Kombat (Yawdim bootleg)" */
	DRIVER( narc ) /* 1988 "Williams" "Narc (rev 7.00)" */
	DRIVER( narc3 ) /* 1988 "Williams" "Narc (rev 3.20)" */
	DRIVER( nbajam ) /* 1993 "Midway" "NBA Jam (rev 3.01 04/07/93)" */
	DRIVER( nbajamr2 ) /* 1993 "Midway" "NBA Jam (rev 2.00 02/10/93)" */
	DRIVER( nbajamt1 ) /* 1994 "Midway" "NBA Jam TE (rev 1.0 01/17/94)" */
	DRIVER( nbajamt2 ) /* 1994 "Midway" "NBA Jam TE (rev 2.0 01/28/94)" */
	DRIVER( nbajamt3 ) /* 1994 "Midway" "NBA Jam TE (rev 3.0 03/04/94)" */
	DRIVER( nbajamte ) /* 1994 "Midway" "NBA Jam TE (rev 4.0 03/23/94)" */
	DRIVER( shimpacp ) /* 1991 "Midway" "Super High Impact (prototype */
	DRIVER( shimpact ) /* 1991 "Midway" "Super High Impact (rev LA1 09/30/91)" */
	DRIVER( smashtv ) /* 1990 "Williams" "Smash T.V. (rev 8.00)" */
	DRIVER( smashtv4 ) /* 1990 "Williams" "Smash T.V. (rev 4.00)" */
	DRIVER( smashtv5 ) /* 1990 "Williams" "Smash T.V. (rev 5.00)" */
	DRIVER( smashtv6 ) /* 1990 "Williams" "Smash T.V. (rev 6.00)" */
	DRIVER( strkforc ) /* 1991 "Midway" "Strike Force (rev 1 02/25/91)" */
	DRIVER( term2 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA3 03/27/92)" */
	DRIVER( term2la1 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA1 11/01/91)" */
	DRIVER( term2la2 ) /* 1991 "Midway" "Terminator 2 - Judgment Day (rev LA2 12/09/91)" */
	DRIVER( totcarn ) /* 1992 "Midway" "Total Carnage (rev LA1 03/10/92)" */
	DRIVER( totcarnp ) /* 1992 "Midway" "Total Carnage (prototype */
	DRIVER( trog ) /* 1990 "Midway" "Trog (rev LA4 03/11/91)" */
	DRIVER( trog3 ) /* 1990 "Midway" "Trog (rev LA3 02/14/91)" */
	DRIVER( trogp ) /* 1990 "Midway" "Trog (prototype */
	DRIVER( trogpa6 ) /* 1990 "Midway" "Trog (rev PA6-PAC 09/09/90)" */
#endif
#ifdef LINK_MINIATARI
	DRIVER( 720 ) /* 1986 "Atari Games" "720 Degrees (rev 4)" */
	DRIVER( 720g ) /* 1986 "Atari Games" "720 Degrees (German */
	DRIVER( 720gr1 ) /* 1986 "Atari Games" "720 Degrees (German */
	DRIVER( 720r1 ) /* 1986 "Atari Games" "720 Degrees (rev 1)" */
	DRIVER( 720r2 ) /* 1986 "Atari Games" "720 Degrees (rev 2)" */
	DRIVER( 720r3 ) /* 1986 "Atari Games" "720 Degrees (rev 3)" */
	DRIVER( apb ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 7)" */
	DRIVER( apb6 ) /* 1987 "Atari Games" "APB - All Points Bulletin (rev 6)" */
	DRIVER( apbf ) /* 1987 "Atari Games" "APB - All Points Bulletin (French)" */
	DRIVER( apbg ) /* 1987 "Atari Games" "APB - All Points Bulletin (German)" */
	DRIVER( bullsdrt ) /* 1985 "Shinkai Inc. (Magic Eletronics Inc. licence)" "Bulls Eye Darts" */
	DRIVER( ccastle2 ) /* 1983 "Atari" "Crystal Castles (version 2)" */
	DRIVER( ccastle3 ) /* 1983 "Atari" "Crystal Castles (version 3)" */
	DRIVER( ccastles ) /* 1983 "Atari" "Crystal Castles (version 4)" */
	DRIVER( centipd2 ) /* 1980 "Atari" "Centipede (revision 2)" */
	DRIVER( centiped ) /* 1980 "Atari" "Centipede (revision 3)" */
	DRIVER( centtime ) /* 1980 "Atari" "Centipede (1 player */
	DRIVER( csprint ) /* 1986 "Atari Games" "Championship Sprint (rev 3)" */
	DRIVER( csprint2 ) /* 1986 "Atari Games" "Championship Sprint (rev 2)" */
	DRIVER( csprintf ) /* 1986 "Atari Games" "Championship Sprint (French)" */
	DRIVER( csprintg ) /* 1986 "Atari Games" "Championship Sprint (German */
	DRIVER( csprints ) /* 1986 "Atari Games" "Championship Sprint (Spanish */
	DRIVER( gaunt2 ) /* 1986 "Atari Games" "Gauntlet II" */
	DRIVER( gaunt2g ) /* 1986 "Atari Games" "Gauntlet II (German)" */
	DRIVER( gauntg ) /* 1985 "Atari Games" "Gauntlet (German */
	DRIVER( gauntj ) /* 1985 "Atari Games" "Gauntlet (Japanese */
	DRIVER( gauntlet ) /* 1985 "Atari Games" "Gauntlet (rev 14)" */
	DRIVER( gaunts ) /* 1985 "Atari Games" "Gauntlet (Spanish */
	DRIVER( indytem2 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 2)" */
	DRIVER( indytem3 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 3)" */
	DRIVER( indytem4 ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 4)" */
	DRIVER( indytemd ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (German)" */
	DRIVER( indytemp ) /* 1985 "Atari Games" "Indiana Jones and the Temple of Doom (set 1)" */
	DRIVER( klax ) /* 1989 "Atari Games" "Klax (set 1)" */
	DRIVER( klaxd ) /* 1989 "Atari Games" "Klax (Germany)" */
	DRIVER( klaxj ) /* 1989 "Atari Games" "Klax (Japan)" */
	DRIVER( marble ) /* 1984 "Atari Games" "Marble Madness (set 1)" */
	DRIVER( marble2 ) /* 1984 "Atari Games" "Marble Madness (set 2)" */
	DRIVER( marble3 ) /* 1984 "Atari Games" "Marble Madness (set 3)" */
	DRIVER( marble4 ) /* 1984 "Atari Games" "Marble Madness (set 4)" */
	DRIVER( milliped ) /* 1982 "Atari" "Millipede" */
	DRIVER( paperboy ) /* 1984 "Atari Games" "Paperboy (rev 3)" */
	DRIVER( paperbr1 ) /* 1984 "Atari Games" "Paperboy (rev 1)" */
	DRIVER( paperbr2 ) /* 1984 "Atari Games" "Paperboy (rev 2)" */
	DRIVER( peterpak ) /* 1984 "Atari Games" "Peter Pack-Rat" */
	DRIVER( rampart ) /* 1990 "Atari Games" "Rampart (Trackball)" */
	DRIVER( rampartj ) /* 1990 "Atari Games" "Rampart (Japan */
	DRIVER( ramprt2p ) /* 1990 "Atari Games" "Rampart (Joystick)" */
	DRIVER( roadblc1 ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblcg ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblsc ) /* 1987 "Atari Games" "Road Blasters (cockpit */
	DRIVER( roadblsg ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadblst ) /* 1987 "Atari Games" "Road Blasters (upright */
	DRIVER( roadrun1 ) /* 1985 "Atari Games" "Road Runner (rev 1)" */
	DRIVER( roadrun2 ) /* 1985 "Atari Games" "Road Runner (rev 1+)" */
	DRIVER( roadrunn ) /* 1985 "Atari Games" "Road Runner (rev 2)" */
	DRIVER( ssprint ) /* 1986 "Atari Games" "Super Sprint (rev 4)" */
	DRIVER( ssprint1 ) /* 1986 "Atari Games" "Super Sprint (rev 1)" */
	DRIVER( ssprint3 ) /* 1986 "Atari Games" "Super Sprint (rev 3)" */
	DRIVER( ssprintg ) /* 1986 "Atari Games" "Super Sprint (German */
	DRIVER( vindctr2 ) /* 1988 "Atari Games" "Vindicators Part II (rev 3)" */
	DRIVER( warlords ) /* 1980 "Atari" "Warlords" */
#endif
#ifdef LINK_MINIDTEA
	DRIVER( baddudes ) /* 1988 "Data East USA" "Bad Dudes vs. Dragonninja (US)" */
	DRIVER( birdtry ) /* 1988 "Data East Corporation" "Birdie Try (Japan)" */
	DRIVER( bouldash ) /* 1990 "Data East Corporation (licensed from First Star)" "Boulder Dash / Boulder Dash Part 2 (World)" */
	DRIVER( bouldshj ) /* 1990 "Data East Corporation (licensed from First Star)" "Boulder Dash / Boulder Dash Part 2 (Japan)" */
	DRIVER( chelnov ) /* 1988 "Data East Corporation" "Chelnov - Atomic Runner (World)" */
	DRIVER( chelnovj ) /* 1988 "Data East Corporation" "Chelnov - Atomic Runner (Japan)" */
	DRIVER( chelnovu ) /* 1988 "Data East USA" "Chelnov - Atomic Runner (US)" */
	DRIVER( chinatwn ) /* 1991 "Data East Corporation" "China Town (Japan)" */
	DRIVER( drgninja ) /* 1988 "Data East Corporation" "Dragonninja (Japan)" */
	DRIVER( ffantasa ) /* 1989 "Data East Corporation" "Fighting Fantasy (Japan)" */
	DRIVER( ffantasy ) /* 1989 "Data East Corporation" "Fighting Fantasy (Japan revision 2)" */
	DRIVER( hbarrel ) /* 1987 "Data East USA" "Heavy Barrel (US)" */
	DRIVER( hbarrelw ) /* 1987 "Data East Corporation" "Heavy Barrel (World)" */
	DRIVER( hippodrm ) /* 1989 "Data East USA" "Hippodrome (US)" */
	DRIVER( karnov ) /* 1987 "Data East USA" "Karnov (US)" */
	DRIVER( karnovj ) /* 1987 "Data East Corporation" "Karnov (Japan)" */
	DRIVER( midres ) /* 1989 "Data East Corporation" "Midnight Resistance (World)" */
	DRIVER( midresj ) /* 1989 "Data East Corporation" "Midnight Resistance (Japan)" */
	DRIVER( midresu ) /* 1989 "Data East USA" "Midnight Resistance (US)" */
	DRIVER( robocop ) /* 1988 "Data East Corporation" "Robocop (World revision 4)" */
	DRIVER( robocopb ) /* 1988 "bootleg" "Robocop (World bootleg)" */
	DRIVER( robocopj ) /* 1988 "Data East Corporation" "Robocop (Japan)" */
	DRIVER( robocopu ) /* 1988 "Data East USA" "Robocop (US revision 1)" */
	DRIVER( robocopw ) /* 1988 "Data East Corporation" "Robocop (World revision 3)" */
	DRIVER( robocpu0 ) /* 1988 "Data East USA" "Robocop (US revision 0)" */
	DRIVER( secretag ) /* 1989 "Data East Corporation" "Secret Agent (World)" */
	DRIVER( slyspy ) /* 1989 "Data East USA" "Sly Spy (US revision 3)" */
	DRIVER( slyspy2 ) /* 1989 "Data East USA" "Sly Spy (US revision 2)" */
	DRIVER( supbtime ) /* 1990 "Data East Corporation" "Super Burger Time (World)" */
	DRIVER( supbtimj ) /* 1990 "Data East Corporation" "Super Burger Time (Japan)" */
	DRIVER( wndrplnt ) /* 1987 "Data East Corporation" "Wonder Planet (Japan)" */
#endif
#ifdef LINK_MINIKONAMI
	DRIVER( blswhstl ) /* 1991 "Konami" "Bells & Whistles (Version L)" */
	DRIVER( detatwin ) /* 1991 "Konami" "Detana!! Twin Bee (Japan ver. J)" */
	DRIVER( glfgreat ) /* 1991 "Konami" "Golfing Greats" */
	DRIVER( lgtnfght ) /* 1990 "Konami" "Lightning Fighters (World)" */
	DRIVER( lgtnfghu ) /* 1990 "Konami" "Lightning Fighters (US)" */
	DRIVER( prmrsocr ) /* 1993 "Konami" "Premier Soccer (ver EAB)" */
	DRIVER( punkshot ) /* 1990 "Konami" "Punk Shot (US 4 Players)" */
	DRIVER( simpsons ) /* 1991 "Konami" "The Simpsons (4 Players)" */
	DRIVER( ssriders ) /* 1991 "Konami" "Sunset Riders (4 Players ver EAC)" */
	DRIVER( tmnt ) /* 1989 "Konami" "Teenage Mutant Ninja Turtles (World 4 Players)" */
	DRIVER( tmnt2 ) /* 1991 "Konami" "Teenage Mutant Ninja Turtles - Turtles in Time (4 Players ver UAA)" */
#endif
#ifdef LINK_MINIKONAMI2
	DRIVER( blkpnthr ) /* 1987 "Konami" "Black Panther" */
	DRIVER( citybmrj ) /* 1987 "Konami" "City Bomber (Japan)" */
	DRIVER( citybomb ) /* 1987 "Konami" "City Bomber (World)" */
	DRIVER( cuebrick ) /* 1989 "Konami" "Cue Brick (Japan)" */
	DRIVER( darkadv ) /* 1987 "Konami" "Dark Adventure" */
	DRIVER( devilw ) /* 1987 "Konami" "Devil World" */
	DRIVER( fround ) /* 1988 "Konami" "The Final Round (version M)" */
	DRIVER( froundl ) /* 1988 "Konami" "The Final Round (version L)" */
	DRIVER( gberet ) /* 1985 "Konami" "Green Beret" */
	DRIVER( gberetb ) /* 1985 "bootleg" "Green Beret (bootleg)" */
	DRIVER( gradius ) /* 1985 "Konami" "Gradius" */
	DRIVER( gradius2 ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan New Ver.)" */
	DRIVER( gradius3 ) /* 1989 "Konami" "Gradius III (Japan)" */
	DRIVER( grdius2a ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan Old Ver.)" */
	DRIVER( grdius2b ) /* 1988 "Konami" "Gradius II - GOFER no Yabou (Japan Older Ver.)" */
	DRIVER( gwarrior ) /* 1985 "Konami" "Galactic Warriors" */
	DRIVER( gyruss ) /* 1983 "Konami" "Gyruss (Konami)" */
	DRIVER( gyrussce ) /* 1983 "Konami (Centuri license)" "Gyruss (Centuri)" */
	DRIVER( hcrash ) /* 1987 "Konami" "Hyper Crash (version D)" */
	DRIVER( hcrashc ) /* 1987 "Konami" "Hyper Crash (version C)" */
	DRIVER( hpuncher ) /* 1988 "Konami" "Hard Puncher (Japan)" */
	DRIVER( kittenk ) /* 1988 "Konami" "Kitten Kaboodle" */
	DRIVER( konamigt ) /* 1985 "Konami" "Konami GT" */
	DRIVER( lifefrce ) /* 1986 "Konami" "Lifeforce (US)" */
	DRIVER( lifefrcj ) /* 1986 "Konami" "Lifeforce (Japan)" */
	DRIVER( majuu ) /* 1987 "Konami" "Majuu no Ohkoku" */
	DRIVER( merlinmm ) /* 1986 "Zilec - Zenitone" "Merlins Money Maze" */
	DRIVER( miaj ) /* 1989 "Konami" "M.I.A. - Missing in Action (Japan)" */
	DRIVER( mrgoemon ) /* 1986 "Konami" "Mr. Goemon (Japan)" */
	DRIVER( nemesis ) /* 1985 "Konami" "Nemesis" */
	DRIVER( nemesuk ) /* 1985 "Konami" "Nemesis (World?)" */
	DRIVER( nyanpani ) /* 1988 "Konami" "Nyan Nyan Panic (Japan)" */
	DRIVER( parodisj ) /* 1990 "Konami" "Parodius DA! (Japan)" */
	DRIVER( parodius ) /* 1990 "Konami" "Parodius DA! (World)" */
	DRIVER( pingpong ) /* 1985 "Konami" "Ping Pong" */
	DRIVER( rf2 ) /* 1985 "Konami" "Konami RF2 - Red Fighter" */
	DRIVER( rushatck ) /* 1985 "Konami" "Rush'n Attack (US)" */
	DRIVER( salamand ) /* 1986 "Konami" "Salamander (version D)" */
	DRIVER( salamanj ) /* 1986 "Konami" "Salamander (version J)" */
	DRIVER( twinbee ) /* 1985 "Konami" "TwinBee" */
	DRIVER( vulcan ) /* 1988 "Konami" "Vulcan Venture" */
#endif
#ifdef LINK_MINIKONAMI3
	DRIVER( daiskiss ) /* 1996 "Konami" "Daisu-Kiss (ver JAA)" */
	DRIVER( dbz ) /* 1993 "Banpresto" "Dragonball Z" */
	DRIVER( dbz2 ) /* 1994 "Banpresto" "Dragonball Z 2 - Super Battle" */
	DRIVER( dragoona ) /* 1995 "Konami" "Dragoon Might (ver AAB)" */
	DRIVER( dragoonj ) /* 1995 "Konami" "Dragoon Might (ver JAA)" */
	DRIVER( gokuparo ) /* 1994 "Konami" "Gokujyou Parodius (ver JAD)" */
	DRIVER( konamigx ) /* 1994 "Konami" "System GX" */
	DRIVER( le2 ) /* 1994 "Konami" "Lethal Enforcers II: Gun Fighters (ver EAA)" */
	DRIVER( le2u ) /* 1994 "Konami" "Lethal Enforcers II: Gun Fighters (ver UAA)" */
	DRIVER( puzldama ) /* 1994 "Konami" "Taisen Puzzle-dama (ver JAA)" */
	DRIVER( salmndr2 ) /* 1996 "Konami" "Salamander 2 (ver JAA)" */
	DRIVER( sexyparo ) /* 1996 "Konami" "Sexy Parodius (ver JAA)" */
	DRIVER( tbyahhoo ) /* 1995 "Konami" "Twin Bee Yahhoo! (ver JAA)" */
	DRIVER( tkmmpzdm ) /* 1995 "Konami" "Tokimeki Memorial Taisen Puzzle-dama (ver JAB)" */
	DRIVER( tokkae ) /* 1996 "Konami" "Taisen Tokkae-dama (ver JAA)" */
	DRIVER( winspike ) /* 1997 "Konami" "Winning Spike (ver EAA)" */
	DRIVER( winspikj ) /* 1997 "Konami" "Winning Spike (ver JAA)" */
#endif
#ifdef LINK_MININAMCOS1
	DRIVER( bakutotu ) /*    */
	DRIVER( berabohm ) /*    */
	DRIVER( beraboho ) /*    */
	DRIVER( blastoff ) /*    */
	DRIVER( blazer ) /*    */
	DRIVER( boxyboy ) /*    */
	DRIVER( dangseed ) /*    */
	DRIVER( dspirit ) /*    */
	DRIVER( dspirito ) /*    */
	DRIVER( faceoff ) /*    */
	DRIVER( galag88j ) /*    */
	DRIVER( galaga88 ) /*    */
	DRIVER( mmaze ) /*    */
	DRIVER( pacmania ) /*    */
	DRIVER( pacmanij ) /*    */
	DRIVER( pistoldm ) /*    */
	DRIVER( puzlclub ) /*    */
	DRIVER( quester ) /*    */
	DRIVER( rompers ) /*    */
	DRIVER( romperso ) /*    */
	DRIVER( shadowld ) /*    */
	DRIVER( soukobdx ) /*    */
	DRIVER( splattej ) /*    */
	DRIVER( splatter ) /*    */
	DRIVER( tankfrce ) /*    */
	DRIVER( tankfrcj ) /*    */
	DRIVER( wldcourt ) /*    */
	DRIVER( ws ) /*    */
	DRIVER( ws89 ) /*    */
	DRIVER( ws90 ) /*    */
	DRIVER( yokaidko ) /*    */
	DRIVER( youkaidk ) /*    */
#endif
#ifdef LINK_MISC
	DRIVER( 1945kiii ) /* 2000 "Oriental" "1945k III" */
	DRIVER( 4enraya ) /* 1990 "IDSA" "4 En Raya" */
	DRIVER( 99lstwar ) /* 1985 "Proma" "'99: The Last War" */
	DRIVER( 99lstwra ) /* 1985 "Proma" "'99: The Last War (alternate)" */
	DRIVER( 99lstwrk ) /* 1985 "Kyugo" "'99: The Last War (Kyugo)" */
	DRIVER( 9ballsh2 ) /* 1993 "E-Scape EnterMedia (Bundra license)" "9-Ball Shootout (set 2)" */
	DRIVER( 9ballsh3 ) /* 1993 "E-Scape EnterMedia (Bundra license)" "9-Ball Shootout (set 3)" */
	DRIVER( 9ballsht ) /* 1993 "E-Scape EnterMedia (Bundra license)" "9-Ball Shootout (set 1)" */
	DRIVER( agallet ) /* 1996 "Banpresto / Gazelle" "Air Gallet" */
	DRIVER( airwolf ) /* 1987 "Kyugo" "Airwolf" */
	DRIVER( ambush ) /* 1983 "Nippon Amuse Co-Ltd" "Ambush" */
	DRIVER( ambusht ) /* 1983 "Tecfri" "Ambush (Tecfri)" */
	DRIVER( ambushv ) /* 1983 "Volt Elec co-ltd" "Ambush (Volt Elec co-ltd)" */
	DRIVER( amerdar2 ) /* 1989 "Ameri" "AmeriDarts (set 2)" */
	DRIVER( amerdart ) /* 1989 "Ameri" "AmeriDarts (set 1)" */
	DRIVER( amspdwy ) /* 1987 "Enerdyne Technologies Inc." */
	DRIVER( amspdwya ) /* 1987 "Enerdyne Technologies Inc." */
	DRIVER( attckufo ) /* 1980 "Ryoto Electric Co." "Attack Ufo" */
	DRIVER( aztarac ) /* 1983 "Centuri" "Aztarac" */
	DRIVER( battlcry ) /* 1991 "Home Data" "Battlecry" */
	DRIVER( beaminv ) /* 19?? "Tekunon Kougyou" "Beam Invader" */
	DRIVER( bigdeal ) /* 1986 "Fun World" "Big Deal (Hungary */
	DRIVER( bigdealb ) /* 1986 "Fun World" "Big Deal (Hungary */
	DRIVER( blkbustr ) /* 1983 "Kiwako (ECI license)" "BlockBuster" */
	DRIVER( bmcbowl ) /* 1994 "BMC" "BMC Bowling" */
	DRIVER( bubl2000 ) /* 1998 "Tuning" "Bubble 2000" */
	DRIVER( catt ) /* 1993 "Wintechno" "Catt (Japan)" */
	DRIVER( cheesech ) /* 1994 "Art & Magic" "Cheese Chase" */
	DRIVER( chinher2 ) /* 1984 "Taiyo" "Chinese Hero (older)" */
	DRIVER( chinhero ) /* 1984 "Taiyo" "Chinese Hero" */
	DRIVER( coolmini ) /* 1999 "Semicom" "Cool Minigame Collection" */
	DRIVER( coolpool ) /* 1992 "Catalina" "Cool Pool" */
	DRIVER( countrnb ) /* 1988 "bootleg" "Counter Run (bootleg set 1)" */
	DRIVER( countrun ) /* 1988 "Nihon System (Sega license)" "Counter Run" */
	DRIVER( crazyblk ) /* 1983 "Kiwako (ECI license)" "Crazy Blocks" */
	DRIVER( crysbios ) /* 2001 "Brezzasoft" "Crystal System BIOS" */
	DRIVER( crysking ) /* 2001 "Brezzasoft" "The Crystal of Kings" */
	DRIVER( dblpoint ) /* 1995 "Dong Bang Electron" "Double Point" */
	DRIVER( dcheese ) /* 1993 "HAR" "Double Cheese" */
	DRIVER( ddonpach ) /* 1997 "Atlus/Cave" "DoDonPachi (International)" */
	DRIVER( ddonpchj ) /* 1997 "Atlus/Cave" "DoDonPachi (Japan)" */
	DRIVER( dfeveron ) /* 1998 "Cave (Nihon System license)" "Dangun Feveron (Japan)" */
	DRIVER( donpachi ) /* 1995 "Atlus/Cave" "DonPachi (US)" */
	DRIVER( donpachj ) /* 1995 "Atlus/Cave" "DonPachi (Japan)" */
	DRIVER( donpachk ) /* 1995 "Atlus/Cave" "DonPachi (Korea)" */
	DRIVER( dorachan ) /* 1980 "Craul Denshi" "Dorachan" */
	DRIVER( dribling ) /* 1983 "Model Racing" "Dribbling" */
	DRIVER( driblino ) /* 1983 "Model Racing (Olympia license)" "Dribbling (Olympia)" */
	DRIVER( dynadice ) /* 19?? "<unknown>" "Dynamic Dice" */
	DRIVER( dynamski ) /* 1984 "Taiyo" "Dynamic Ski" */
	DRIVER( eggvent7 ) /* 1997 "The Game Room" "Egg Venture (Release 7)" */
	DRIVER( eggventr ) /* 1997 "The Game Room" "Egg Venture (Release 10)" */
	DRIVER( eggvntdx ) /* 1997 "The Game Room" "Egg Venture Deluxe" */
	DRIVER( ertictac ) /* 1990 "Sisteme" "Erotictac/Tactic" */
	DRIVER( esprade ) /* 1998 "Atlus/Cave" "ESP Ra.De. (International Ver 1998 4/22)" */
	DRIVER( espradej ) /* 1998 "Atlus/Cave" "ESP Ra.De. (Japan Ver 1998 4/21)" */
	DRIVER( espradeo ) /* 1998 "Atlus/Cave" "ESP Ra.De. (Japan Ver 1998 4/14)" */
	DRIVER( evosocc ) /* 2001 "Evoga" "Evolution Soccer" */
	DRIVER( feversos ) /* 1998 "Cave (Nihon System license)" "Fever SOS (International)" */
	DRIVER( flashgal ) /* 1985 "Sega" "Flashgal (set 1)" */
	DRIVER( flashgla ) /* 1985 "Sega" "Flashgal (set 2)" */
	DRIVER( flower ) /* 1986 "Komax" "Flower" */
	DRIVER( flowerbl ) /* 1986 "bootleg" "Flower (bootleg)" */
	DRIVER( fmaniac3 ) /* 2002 "Saero Entertainment" "Fishing Maniac 3" */
	DRIVER( fredmem ) /* 1994 "Coastal Amusements" "Fred Flintstones' Memory Match" */
	DRIVER( freekckb ) /* 1987 "bootleg" "Free Kick (bootleg)" */
	DRIVER( freekick ) /* 1987 "Nihon System (Sega license)" "Free Kick" */
	DRIVER( fx ) /* 1986 "bootleg" "F-X" */
	DRIVER( gaia ) /* 1999 "Noise Factory" "Gaia Crusaders" */
	DRIVER( gfire2 ) /* 1992 "Topis Corp" "Golden Fire II" */
	DRIVER( ghoshunt ) /* 1996 "Hanaho Games" "Ghost Hunter" */
	DRIVER( gigasb ) /* 1986 "bootleg" "Gigas (bootleg)" */
	DRIVER( gigasm2b ) /* 1986 "bootleg" "Gigas Mark II (bootleg)" */
	DRIVER( gotcha ) /* 1997 "Dongsung" "Got-cha Mini Game Festival" */
	DRIVER( gumbo ) /* 1994 "Min Corp." "Gumbo" */
	DRIVER( guwange ) /* 1999 "Atlus/Cave" "Guwange (Japan)" */
	DRIVER( gyrodinc ) /* 1984 "Taito Corporation (Crux license)" "Gyrodine (Crux)" */
	DRIVER( gyrodine ) /* 1984 "Taito Corporation" "Gyrodine" */
	DRIVER( hangman ) /* 1984 "Status Games" "Hangman" */
	DRIVER( hedpanic ) /* 2000 "ESD / Fuuki" "Head Panic (ver. 0315 */
	DRIVER( hexa ) /* 199? "D. R. Korea" "Hexa" */
	DRIVER( hiryuken ) /* 1985 "[Nihon Game] (Taito license)" "Hokuha Syourin Hiryu no Ken" */
	DRIVER( hotblock ) /* 1993 "Nics? / Nix?" "Hot Blocks - Tetrix II" */
	DRIVER( hotbubl ) /* 1998 "Pandora" "Hot Bubble" */
	DRIVER( hotdogst ) /* 1996 "Marble" "Hotdog Storm" */
	DRIVER( hourouki ) /* 1987 "Home Data" "Mahjong Hourouki Part 1 - Seisyun Hen (Japan)" */
	DRIVER( igmo ) /* 1984 "Epos Corporation" "IGMO" */
	DRIVER( jackpool ) /* 1997 "Electronic Projects" "Jackpot Pool (Italy */
	DRIVER( jogakuen ) /* 1992? "Windom" "Mahjong Jogakuen (Japan)" */
	DRIVER( jollycrd ) /* 1985 "TAB-Austria" "Jolly Card (Austria)" */
	DRIVER( jolycdab ) /* 1990 "Inter Games" "Jolly Card (Austria */
	DRIVER( jolycdat ) /* 1986 "Fun World" "Jolly Card (Austria */
	DRIVER( jolycdcr ) /* 1993 "Soft Design" "Jolly Card (Croatia)" */
	DRIVER( jolycdit ) /* 199? "bootleg?" "Jolly Card (Italia */
	DRIVER( kdynastg ) /* 1999 "EZ Graphics" "King of Dynast Gear (version 1.8)" */
	DRIVER( korokoro ) /* 1999 "Takumi" "Koro Koro Quest (Japan)" */
	DRIVER( ladyfrog ) /* 1990 "Mondial Games" "Lady Frog" */
	DRIVER( legend ) /* 1986? "Sega / Coreland ?" "Legend" */
	DRIVER( lemnangl ) /* 1990 "Home Data" "Mahjong Lemon Angel (Japan)" */
	DRIVER( lethalj ) /* 1996 "The Game Room" "Lethal Justice" */
	DRIVER( lottof2 ) /* 1993 "HAR" "Lotto Fun 2" */
	DRIVER( ltcasino ) /* 1982 "Digital Controls Inc." "Little Casino" */
	DRIVER( luplup ) /* 1999 "Omega System" "Lup Lup Puzzle / Zhuan Zhuan Puzzle (version 3.0 / 990128)" */
	DRIVER( maddonna ) /* 1995 "Tuning" "Mad Donna (set 1)" */
	DRIVER( magic10 ) /* 1995 "A.W.P. Games" "Magic's 10 (ver. 16.55)" */
	DRIVER( magic10a ) /* 1995 "A.W.P. Games" "Magic's 10 (ver. 16.45)" */
	DRIVER( magiccrd ) /* 1996 "Impera" "Magic Card II (Bulgaria)" */
	DRIVER( mangchi ) /* 2000 "Afega" "Mang-Chi" */
	DRIVER( mazinger ) /* 1994 "Banpresto/Dynamic Pl. Toei Animation" "Mazinger Z" */
	DRIVER( mcatadv ) /* 1993 "Wintechno" "Magical Cat Adventure" */
	DRIVER( mcatadvj ) /* 1993 "Wintechno" "Magical Cat Adventure (Japan)" */
	DRIVER( mchampda ) /* 1999 "ESD" "Multi Champ Deluxe (ver. 1126 */
	DRIVER( mchampdx ) /* 2000 "ESD" "Multi Champ Deluxe (ver. 0106 */
	DRIVER( megadon ) /* 1982 "Epos Corporation (Photar Industries license)" "Megadon" */
	DRIVER( metmqstr ) /* 1995 "Banpresto/Pandorabox" "Metamoqester" */
	DRIVER( mhgaiden ) /* 1987 "Home Data" "Mahjong Hourouki Gaiden (Japan)" */
	DRIVER( misncrft ) /* 2000 "Sun" "Mission Craft (version 2.4)" */
	DRIVER( mjclinic ) /* 1988 "Home Data" "Mahjong Clinic (Japan)" */
	DRIVER( mjhokite ) /* 1988 "Home Data" "Mahjong Hourouki Okite (Japan)" */
	DRIVER( mjkinjas ) /* 1991 "Home Data" "Mahjong Kinjirareta Asobi (Japan)" */
	DRIVER( mjkojink ) /* 1989 "Home Data" "Mahjong Kojinkyouju (Private Teacher) (Japan)" */
	DRIVER( mjyougo ) /* 1989 "Home Data" "Mahjong-yougo no Kisotairyoku (Japan)" */
	DRIVER( mole ) /* 1982 "Yachiyo Electronics Ltd." */
	DRIVER( mosaic ) /* 1990 "Space" "Mosaic" */
	DRIVER( mosaica ) /* 1990 "Space (Fuuki license)" "Mosaic (Fuuki)" */
	DRIVER( mrjong ) /* 1983 "Kiwako" "Mr. Jong (Japan)" */
	DRIVER( mrokumei ) /* 1988 "Home Data" "Mahjong Rokumeikan (Japan)" */
	DRIVER( msbingo ) /* 1994 "Min Corp." "Miss Bingo" */
	DRIVER( mspuzzle ) /* 1994 "Min Corp." "Miss Puzzle" */
	DRIVER( multchmk ) /* 1998 "ESD" "Multi Champ (Korea)" */
	DRIVER( multchmp ) /* 1999 "ESD" "Multi Champ (World)" */
	DRIVER( murogem ) /* 198? "unknown" "Muroge Monaco" */
	DRIVER( mv4in1 ) /* 1983 "Entertainment Enterprises" "Mini Vegas 4in1" */
	DRIVER( news ) /* 1993 "Poby / Virus" "News (set 1)" */
	DRIVER( newsa ) /* 1993 "Poby" "News (set 2)" */
	DRIVER( nmaster ) /* 1995 "Banpresto/Pandorabox" "Oni - The Ninja Master (Japan)" */
	DRIVER( noahsark ) /* 1983 "Enter-Tech" "Noah's Ark" */
	DRIVER( nost ) /* 1993 "Face" "Nostradamus" */
	DRIVER( nostj ) /* 1993 "Face" "Nostradamus (Japan)" */
	DRIVER( nostk ) /* 1993 "Face" "Nostradamus (Korea)" */
	DRIVER( oigas ) /* 1986 "bootleg" "Oigas (bootleg)" */
	DRIVER( oneshot ) /* 199? "<unknown>" "One Shot One Kill" */
	DRIVER( onetwo ) /* 1997 "Barko" "One + Two" */
	DRIVER( othldrby ) /* 1995 "Sunwise" "Othello Derby (Japan)" */
	DRIVER( pass ) /* 1992 "Oksan" "Pass" */
	DRIVER( pbillrd ) /* 1987 "Nihon System" "Perfect Billiard" */
	DRIVER( pbillrds ) /* 1987 "Nihon System" "Perfect Billiard (Sega)" */
	DRIVER( pipeline ) /* 1990 "Daehyun Electronics" "Pipeline" */
	DRIVER( pkscram ) /* 1993 "Cosmo Electronics Corporation" "PK Scramble" */
	DRIVER( plctr13b ) /* 1996 "P & P Marketing" "Police Trainer (Rev 1.3B)" */
	DRIVER( plegendj ) /* 1995 "Atlus/Cave" "Gouketsuji Ichizoku Saikyou Densetsu (Japan)" */
	DRIVER( plegends ) /* 1995 "Atlus/Cave" "Power Instinct Legends (USA)" */
	DRIVER( policetr ) /* 1996 "P & P Marketing" "Police Trainer (Rev 1.3)" */
	DRIVER( polict10 ) /* 1996 "P & P Marketing" "Police Trainer (Rev 1.0)" */
	DRIVER( polict11 ) /* 1996 "P & P Marketing" "Police Trainer (Rev 1.1)" */
	DRIVER( polyplay ) /* 1985 "VEB Polytechnik Karl-Marx-Stadt" "Poly-Play" */
	DRIVER( popspops ) /* 1999 "Afega" "Pop's Pop's" */
	DRIVER( ppcar ) /* 1999 "Icarus" "Pang Pang Car" */
	DRIVER( ppchamp ) /* 1997 "Dongsung" "Pasha Pasha Champ Mini Game Festival" */
	DRIVER( promutra ) /* 1985 "Enerdyne Technologies Inc." "Progressive Music Trivia (Question set 2)" */
	DRIVER( promutrb ) /* 1985 "Enerdyne Technologies Inc." "Progressive Music Trivia (Question set 3)" */
	DRIVER( promutrv ) /* 1985 "Enerdyne Technologies Inc." "Progressive Music Trivia (Question set 1)" */
	DRIVER( pwrins2j ) /* 1994 "Atlus/Cave" "Gouketsuji Ichizoku 2 (Japan)" */
	DRIVER( pwrinst2 ) /* 1994 "Atlus/Cave" "Power Instinct 2 (USA)" */
	DRIVER( reikaids ) /* 1988 "Home Data" "Reikai Doushi (Japan)" */
	DRIVER( repulse ) /* 1985 "Sega" "Repulse" */
	DRIVER( royalcdb ) /* 1991 "TAB-Austria" "Royal Card (Austria */
	DRIVER( royalcrd ) /* 1991 "TAB-Austria" "Royal Card (Austria */
	DRIVER( rundeep ) /* 1988 "Cream" "Run Deep" */
	DRIVER( sailormn ) /* 1995 "Banpresto" "Pretty Soldier Sailor Moon (95/03/22B)" */
	DRIVER( sailormo ) /* 1995 "Banpresto" "Pretty Soldier Sailor Moon (95/03/22)" */
	DRIVER( sextriv ) /* 1985 "Status Games" "Sex Triv" */
	DRIVER( shangkid ) /* 1985 "Taiyo (Data East license)" "Shanghai Kid" */
	DRIVER( skyarmy ) /* 1982 "Shoei" "Sky Army" */
	DRIVER( skywolf ) /* 1987 "bootleg" "Sky Wolf (set 1)" */
	DRIVER( skywolf2 ) /* 1987 "bootleg" "Sky Wolf (set 2)" */
	DRIVER( sonofphx ) /* 1985 "Associated Overseas MFR Inc" */
	DRIVER( sprcros2 ) /* 1986 "GM Shoji" "Super Cross 2 (Japan set 1)" */
	DRIVER( sprcrs2a ) /* 1986 "GM Shoji" "Super Cross 2 (Japan set 2)" */
	DRIVER( srdmissn ) /* 1986 "Taito Corporation" "S.R.D. Mission" */
	DRIVER( ssfindo ) /* 1999 "Icarus" "See See Find Out" */
	DRIVER( sshoot11 ) /* 1998 "P & P Marketing" "Sharpshooter (Rev 1.1)" */
	DRIVER( sshoot12 ) /* 1998 "P & P Marketing" "Sharpshooter (Rev 1.2)" */
	DRIVER( sshooter ) /* 1998 "P & P Marketing" "Sharpshooter (Rev 1.7)" */
	DRIVER( statriv2 ) /* 1984 "Status Games" "Triv Two" */
	DRIVER( statriv4 ) /* 1985 "Status Games" "Triv Four" */
	DRIVER( stoneba2 ) /* 1994 "Art & Magic" "Stone Ball (2 Players)" */
	DRIVER( stonebal ) /* 1994 "Art & Magic" "Stone Ball (4 Players)" */
	DRIVER( strvmstr ) /* 1986 "Enerdyne Technologies Inc." "Super Trivia Master" */
	DRIVER( supertnk ) /* 1981 "Video Games GmbH" "Super Tank" */
	DRIVER( supertr2 ) /* 1986 "Status Games" "Super Triv II" */
	DRIVER( supertr3 ) /* 1988 "Status Games" "Super Triv III" */
	DRIVER( suplup ) /* 1999 "Omega System" "Super Lup Lup Puzzle / Zhuan Zhuan Puzzle (version 4.0 / 990518)" */
	DRIVER( suprglob ) /* 1983 "Epos Corporation" "Super Glob" */
	DRIVER( tangtang ) /* 2000 "ESD" "Tang Tang (ver. 0526 */
	DRIVER( taxidrvr ) /* 1984 "Graphic Techno" "Taxi Driver" */
	DRIVER( thedeep ) /* 1987 "Woodplace Inc." "The Deep (Japan)" */
	DRIVER( theglob ) /* 1983 "Epos Corporation" "The Glob" */
	DRIVER( theglob2 ) /* 1983 "Epos Corporation" "The Glob (earlier)" */
	DRIVER( theglob3 ) /* 1983 "Epos Corporation" "The Glob (set 3)" */
	DRIVER( tickee ) /* 1994 "Raster Elite" "Tickee Tickats" */
	DRIVER( trivquiz ) /* 1984 "Status Games" "Triv Quiz" */
	DRIVER( truco ) /* 198? "Playtronic SRL" "Truco-Tron" */
	DRIVER( trucocl ) /* 1991 "Miky SRL" "Truco Clemente" */
	DRIVER( tugboat ) /* 1982 "ETM" "Tugboat" */
	DRIVER( tutstomb ) /* 1996 "Island Design" "Tut's Tomb" */
	DRIVER( twinactn ) /* 1995 "Afega" "Twin Action" */
	DRIVER( ultennis ) /* 1993 "Art & Magic" "Ultimate Tennis" */
	DRIVER( uopoko ) /* 1998 "Cave (Jaleco license)" "Puzzle Uo Poko (International)" */
	DRIVER( uopokoj ) /* 1998 "Cave (Jaleco license)" "Puzzle Uo Poko (Japan)" */
	DRIVER( usg182 ) /* 1989 "U.S. Games" "Games V18.2" */
	DRIVER( usg185 ) /* 1991 "U.S. Games" "Games V18.7C" */
	DRIVER( usg252 ) /* 1992 "U.S. Games" "Games V25.4X" */
	DRIVER( usg32 ) /* 1987 "U.S. Games" "Super Duper Casino (California V3.2)" */
	DRIVER( usg82 ) /* 1988 "U.S. Games" "Super Ten V8.2" */
	DRIVER( usg83 ) /* 1988 "U.S. Games" "Super Ten V8.3" */
	DRIVER( usg83x ) /* 1988 "U.S. Games" "Super Ten V8.3X" */
	DRIVER( vamphalf ) /* 1999 "Danbi & F2 System" "Vamp 1/2 (Korea version)" */
	DRIVER( vitaminc ) /* 1989 "Home Data" "Mahjong Vitamin C (Japan)" */
	DRIVER( vroulet ) /* 1989 "World Game" "Vegas Roulette" */
	DRIVER( xfiles ) /* 1999 "dgPIX Entertainment Inc." "X-Files" */
	DRIVER( xyonix ) /* 1989 "Philko" "Xyonix" */
#endif
#ifdef LINK_NAMCO
	DRIVER( 4in1 ) /* 1981 "Armenia / Food and Fun" "4 Fun in 1" */
	DRIVER( acedrvrw ) /* 1994 "Namco" "Ace Driver (Rev. AD2 */
	DRIVER( alpinerc ) /* 1995 "Namco" "Alpine Racer (Rev. AR2 Ver.C)" */
	DRIVER( alpinerd ) /* 1995 "Namco" "Alpine Racer (Rev. AR2 Ver.D)" */
	DRIVER( alpinesa ) /* 1996 "Namco" "Alpine Surfer (Rev. AF2 Ver.A)" */
	DRIVER( alpinr2b ) /* 1996 "Namco" "Alpine Racer 2 (Rev. ARS2 Ver.B)" */
	DRIVER( aquarush ) /* 1999 "Namco" "Aqua Rush (AQ1/VER.A1)" */
	DRIVER( assault ) /* 1988 "Namco" "Assault" */
	DRIVER( assaultj ) /* 1988 "Namco" "Assault (Japan)" */
	DRIVER( assaultp ) /* 1988 "Namco" "Assault Plus (Japan)" */
	DRIVER( azurian ) /* 1982 "Rait Electronics Ltd" "Azurian Attack" */
	DRIVER( bagmanmc ) /* 1982 "bootleg" "Bagman (Moon Cresta hardware)" */
	DRIVER( bakutotu ) /* 1988 "Namco" "Bakutotsu Kijuutei" */
	DRIVER( baraduka ) /* 1985 "Namco" "Baraduke (set 2)" */
	DRIVER( baraduke ) /* 1985 "Namco" "Baraduke (set 1)" */
	DRIVER( batman2 ) /* 1981 "bootleg" "Batman Part 2" */
	DRIVER( battles ) /* 1982 "bootleg" "Battles" */
	DRIVER( berabohm ) /* 1988 "Namco" "Beraboh Man (Japan version C)" */
	DRIVER( beraboho ) /* 1988 "Namco" "Beraboh Man (Japan version B)" */
	DRIVER( bkrtmaq ) /* 1992 "Namco" "Bakuretsu Quiz Ma-Q Dai Bouken (Japan)" */
	DRIVER( blastoff ) /* 1989 "Namco" "Blast Off (Japan)" */
	DRIVER( blazer ) /* 1987 "Namco" "Blazer (Japan)" */
	DRIVER( blkhole ) /* 1981 "TDS" "Black Hole" */
	DRIVER( bombbee ) /* 1979 "Namco" "Bomb Bee" */
	DRIVER( bongo ) /* 1983 "Jetsoft" "Bongo" */
	DRIVER( bosco ) /* 1981 "Namco" "Bosconian (new version)" */
	DRIVER( boscomd ) /* 1981 "[Namco] (Midway license)" "Bosconian (Midway */
	DRIVER( boscomdo ) /* 1981 "[Namco] (Midway license)" "Bosconian (Midway */
	DRIVER( boscoo ) /* 1981 "Namco" "Bosconian (old version)" */
	DRIVER( boscoo2 ) /* 1981 "Namco" "Bosconian (older version)" */
	DRIVER( boxyboy ) /* 1990 "Namco" "Boxy Boy (US)" */
	DRIVER( burnforc ) /* 1989 "Namco" "Burning Force (Japan new version)" */
	DRIVER( burnfrco ) /* 1989 "Namco" "Burning Force (Japan old version)" */
	DRIVER( cgangpzj ) /* 1992 "Namco" "Cosmo Gang the Puzzle (Japan)" */
	DRIVER( cgangpzl ) /* 1992 "Namco" "Cosmo Gang the Puzzle (US)" */
	DRIVER( checkmaj ) /* 1982 "Jaleco" "Check Man (Japan)" */
	DRIVER( checkman ) /* 1982 "Zilec-Zenitone" "Check Man" */
	DRIVER( chewing ) /* 19?? "unknown" "Chewing Gum" */
	DRIVER( commsega ) /* 1983 "Sega" "Commando (Sega)" */
	DRIVER( cosmogng ) /* 1991 "Namco" "Cosmo Gang the Video (US)" */
	DRIVER( cosmognj ) /* 1991 "Namco" "Cosmo Gang the Video (Japan)" */
	DRIVER( cottong ) /* 1982 "bootleg" "Cotocoto Cottong" */
	DRIVER( cutieq ) /* 1979 "Namco" "Cutie Q" */
	DRIVER( cybrcomm ) /* 1995 "Namco" "Cyber Commando (Rev. CY1 */
	DRIVER( cybrcycc ) /* 1995 "Namco" "Cyber Cycles (Rev. CB2 Ver.C)" */
	DRIVER( danceyes ) /* 1996 "Namco" "Dancing Eyes (DC1/VER.A)" */
	DRIVER( dangseed ) /* 1989 "Namco" "Dangerous Seed (Japan)" */
	DRIVER( devilfsg ) /* 1984 "Vision / Artic" "Devil Fish (Galaxian hardware */
	DRIVER( digdug ) /* 1982 "Namco" "Dig Dug (rev 2)" */
	DRIVER( digdug2 ) /* 1985 "Namco" "Dig Dug II (New Ver.)" */
	DRIVER( digdug2o ) /* 1985 "Namco" "Dig Dug II (Old Ver.)" */
	DRIVER( digduga1 ) /* 1982 "[Namco] (Atari license)" "Dig Dug (Atari */
	DRIVER( digdugat ) /* 1982 "[Namco] (Atari license)" "Dig Dug (Atari */
	DRIVER( digdugb ) /* 1982 "Namco" "Dig Dug (rev 1)" */
	DRIVER( dingo ) /* 1983 "Ashby Computers and Graphics LTD. (Jaleco license)" "Dingo" */
	DRIVER( dirtfoxj ) /* 1989 "Namco" "Dirt Fox (Japan)" */
	DRIVER( dkongjrm ) /* 1982 "bootleg" "Donkey Kong Jr. (Moon Cresta hardware)" */
	DRIVER( drgnbstr ) /* 1984 "Namco" "Dragon Buster" */
	DRIVER( drivfrcb ) /* 1985 "bootleg" "Driving Force (Galaxian conversion bootleg)" */
	DRIVER( drivfrcg ) /* 1984 "Shinkai Inc. (Magic Eletronics USA licence)" "Driving Force (Galaxian conversion)" */
	DRIVER( dsaber ) /* 1990 "Namco" "Dragon Saber" */
	DRIVER( dsaberj ) /* 1990 "Namco" "Dragon Saber (Japan)" */
	DRIVER( dspirit ) /* 1987 "Namco" "Dragon Spirit (new version)" */
	DRIVER( dspirito ) /* 1987 "Namco" "Dragon Spirit (old version)" */
	DRIVER( dunkmnia ) /* 1995 "Namco" "Dunk Mania (DM2/VER.C)" */
	DRIVER( dunkmnic ) /* 1995 "Namco" "Dunk Mania (DM1/VER.C)" */
	DRIVER( dzigzag ) /* 1982 "bootleg" "Zig Zag (Dig Dug hardware)" */
	DRIVER( eagle ) /* 1980 "Centuri" "Eagle (set 1)" */
	DRIVER( eagle2 ) /* 1980 "Centuri" "Eagle (set 2)" */
	DRIVER( eagle3 ) /* 1980 "Centuri" "Eagle (set 3)" */
	DRIVER( ehrgeiz ) /* 1998 "Square/Namco" "Ehrgeiz (EG3/VER.A)" */
	DRIVER( ehrgeiza ) /* 1998 "Square/Namco" "Ehrgeiz (EG2/VER.A)" */
	DRIVER( emeralda ) /* 1993 "Namco" "Emeraldia (Japan Version B)" */
	DRIVER( emerldaa ) /* 1993 "Namco" "Emeraldia (Japan)" */
	DRIVER( exodus ) /* 19?? "Subelectro" "Exodus (bootleg?)" */
	DRIVER( exvania ) /* 1992 "Namco" "Exvania (Japan)" */
	DRIVER( fa ) /* 1992 "Namco" "F/A (Japan)" */
	DRIVER( faceoff ) /* 1988 "Namco" "Face Off (Japan)" */
	DRIVER( fantazia ) /* 1980 "bootleg" "Fantazia" */
	DRIVER( fghtatck ) /* 1992 "Namco" "Fighter & Attacker (US)" */
	DRIVER( fgtlayer ) /* 1998 "Arika/Namco" "Fighting Layer (FTL0/VER.A)" */
	DRIVER( finalapc ) /* 1987 "Namco" "Final Lap (Rev C)" */
	DRIVER( finalapd ) /* 1987 "Namco" "Final Lap (Rev D)" */
	DRIVER( finalapo ) /* 1995 "Namco" "Final Lap R" */
	DRIVER( finalapr ) /* 1995 "Namco" "Final Lap R (Rev B)" */
	DRIVER( finallap ) /* 1987 "Namco" "Final Lap (Rev E)" */
	DRIVER( finehour ) /* 1989 "Namco" "Finest Hour (Japan)" */
	DRIVER( finlapjb ) /* 1987 "Namco" "Final Lap (Japan - Rev B)" */
	DRIVER( finlapjc ) /* 1987 "Namco" "Final Lap (Japan - Rev C)" */
	DRIVER( fourtrax ) /* 1989 "Namco" "Four Trax" */
	DRIVER( frogg ) /* 1981 "bootleg" "Frog (Galaxian hardware)" */
	DRIVER( froggrmc ) /* 1981 "bootleg?" "Frogger (Moon Cresta hardware)" */
	DRIVER( g13knd ) /* 1999 "Raizing/Namco" "Golgo 13 Kiseki no Dandou (GLS1/VER.A)" */
	DRIVER( galag88j ) /* 1987 "Namco" "Galaga '88 (Japan)" */
	DRIVER( galaga ) /* 1981 "Namco" "Galaga (Namco rev. B)" */
	DRIVER( galaga3 ) /* 1984 "Namco" "Galaga 3 (rev. C)" */
	DRIVER( galaga3a ) /* 1984 "Namco" "Galaga 3 (set 2)" */
	DRIVER( galaga3m ) /* 1984 "Namco" "Galaga 3 (set 3)" */
	DRIVER( galaga88 ) /* 1987 "Namco" "Galaga '88" */
	DRIVER( galagamk ) /* 1981 "[Namco] (Midway license)" "Galaga (Midway set 2)" */
	DRIVER( galagamw ) /* 1981 "[Namco] (Midway license)" "Galaga (Midway set 1)" */
	DRIVER( galagao ) /* 1981 "Namco" "Galaga (Namco)" */
	DRIVER( galap1 ) /* 1979 "hack" "Space Invaders Galactica" */
	DRIVER( galap4 ) /* 1979 "hack" "Galaxian Part 4" */
	DRIVER( galapx ) /* 1979 "hack" "Galaxian Part X" */
	DRIVER( galaxiaj ) /* 1979 "Namco" "Galaxian (Namco set 2)" */
	DRIVER( galaxian ) /* 1979 "Namco" "Galaxian (Namco set 1)" */
	DRIVER( gallag ) /* 1981 "bootleg" "Gallag" */
	DRIVER( galmidw ) /* 1979 "[Namco] (Midway license)" "Galaxian (Midway)" */
	DRIVER( galmidwo ) /* 1979 "[Namco] (Midway license)" "Galaxian (Midway */
	DRIVER( galturbo ) /* 1979 "hack" "Galaxian Turbo" */
	DRIVER( gaplus ) /* 1984 "Namco" "Gaplus (rev. D)" */
	DRIVER( gaplusa ) /* 1984 "Namco" "Gaplus (alternate hardware)" */
	DRIVER( gapluso ) /* 1984 "Namco" "Gaplus (rev. B)" */
	DRIVER( gatsbee ) /* 1984 "hack" "Gatsbee" */
	DRIVER( geebee ) /* 1978 "Namco" "Gee Bee" */
	DRIVER( geebeeg ) /* 1978 "[Namco] (Gremlin license)" "Gee Bee (Gremlin)" */
	DRIVER( genpeitd ) /* 1986 "Namco" "Genpei ToumaDen" */
	DRIVER( ghlpanic ) /* 1999 "Namco" "Ghoul Panic (OB2/VER.A)" */
	DRIVER( gmgalax ) /* 1981 "bootleg" "Ghostmuncher Galaxian (bootleg)" */
	DRIVER( golgo13 ) /* 1999 "Raizing/Namco" "Golgo 13 (GLG1/VER.A)" */
	DRIVER( gollygho ) /* 1990 "Namco" "Golly! Ghost!" */
	DRIVER( grobda ) /* 1984 "Namco" "Grobda (New Ver.)" */
	DRIVER( grobda2 ) /* 1984 "Namco" "Grobda (Old Ver. set 1)" */
	DRIVER( grobda3 ) /* 1984 "Namco" "Grobda (Old Ver. set 2)" */
	DRIVER( gslgr94u ) /* 1994 "Namco" "Great Sluggers '94" */
	DRIVER( gslugrsj ) /* 1993 "Namco" "Great Sluggers (Japan)" */
	DRIVER( gteikob2 ) /* 1980 "bootleg" "Gingateikoku No Gyakushu (bootleg set 2)" */
	DRIVER( gteikokb ) /* 1980 "bootleg" "Gingateikoku No Gyakushu (bootleg set 1)" */
	DRIVER( gteikoku ) /* 1980 "Irem" "Gingateikoku No Gyakushu" */
	DRIVER( gunbulet ) /* 1994 "Namco" "Gun Bullet (Japan)" */
	DRIVER( gutangtn ) /* 1982 "Konami (Sega license)" "Guttang Gottong" */
	DRIVER( hexpool ) /* 1986 "Shinkai" "Hex Pool" */
	DRIVER( hopmappy ) /* 1986 "Namco" "Hopping Mappy" */
	DRIVER( hunchbkg ) /* 1983 "Century Electronics" "Hunchback (Galaxian hardware)" */
	DRIVER( jumpbug ) /* 1981 "Rock-ola" "Jump Bug" */
	DRIVER( jumpbugb ) /* 1981 "bootleg" "Jump Bug (bootleg)" */
	DRIVER( jungler ) /* 1981 "Konami" "Jungler" */
	DRIVER( junglers ) /* 1981 "[Konami] (Stern license)" "Jungler (Stern)" */
	DRIVER( kaitei ) /* 1980 "Namco" "Kaitei Takara Sagashi" */
	DRIVER( kaiteik ) /* 1980 "K.K. Tokki" "Kaitei Takara Sagashi (K'K-Tokki)" */
	DRIVER( kingbalj ) /* 1980 "Namco" "King & Balloon (Japan)" */
	DRIVER( kingball ) /* 1980 "Namco" "King & Balloon (US)" */
	DRIVER( knckhead ) /* 1992 "Namco" "Knuckle Heads (World)" */
	DRIVER( knckhedj ) /* 1992 "Namco" "Knuckle Heads (Japan)" */
	DRIVER( kyukaidk ) /* 1990 "Namco" "Kyuukai Douchuuki (Japan new version)" */
	DRIVER( kyukaido ) /* 1990 "Namco" "Kyuukai Douchuuki (Japan old version)" */
	DRIVER( ladybugg ) /* 1983 "bootleg" "Ladybug (bootleg on Galaxian hardware)" */
	DRIVER( lbgrande ) /* 1997 "Namco" "Libero Grande (LG2/VER.A)" */
	DRIVER( lckywldj ) /* 1992 "Namco" "Lucky & Wild (Japan)" */
	DRIVER( levers ) /* 1983 "Rock-ola" "Levers" */
	DRIVER( liblrabl ) /* 1983 "Namco" "Libble Rabble" */
	DRIVER( locomotn ) /* 1982 "Konami (Centuri license)" "Loco-Motion" */
	DRIVER( luckywld ) /* 1992 "Namco" "Lucky & Wild" */
	DRIVER( luctoday ) /* 1980 "Sigma" "Lucky Today" */
	DRIVER( machbrkr ) /* 1995 "Namco" "Mach Breakers (Japan)" */
	DRIVER( mappy ) /* 1983 "Namco" "Mappy (US)" */
	DRIVER( mappyj ) /* 1983 "Namco" "Mappy (Japan)" */
	DRIVER( marvland ) /* 1989 "Namco" "Marvel Land (US)" */
	DRIVER( marvlanj ) /* 1989 "Namco" "Marvel Land (Japan)" */
	DRIVER( metlhawk ) /* 1988 "Namco" "Metal Hawk" */
	DRIVER( metlhwkj ) /* 1988 "Namco" "Metal Hawk (Japan)" */
	DRIVER( metrocra ) /* 1985 "Namco" "Metro-Cross (set 2)" */
	DRIVER( metrocrs ) /* 1985 "Namco" "Metro-Cross (set 1)" */
	DRIVER( mirninja ) /* 1988 "Namco" "Mirai Ninja (Japan)" */
	DRIVER( mmaze ) /* 1988 "Namco" "Marchen Maze (Japan)" */
	DRIVER( moonal2 ) /* 1980 "Nichibutsu" "Moon Alien Part 2" */
	DRIVER( moonal2b ) /* 1980 "Nichibutsu" "Moon Alien Part 2 (older version)" */
	DRIVER( moonaln ) /* 19?? "[Nichibutsu] (Karateco license)" "Moon Alien" */
	DRIVER( mooncrgx ) /* 1980 "bootleg" "Moon Cresta (Galaxian hardware)" */
	DRIVER( mooncrs2 ) /* 1980 "Nichibutsu" "Moon Cresta (bootleg set 2)" */
	DRIVER( mooncrs3 ) /* 1980 "bootleg" "Moon Cresta (bootleg set 3)" */
	DRIVER( mooncrsa ) /* 1980 "Nichibutsu" "Moon Cresta (Nichibutsu */
	DRIVER( mooncrsb ) /* 1980 "bootleg" "Moon Cresta (bootleg set 1)" */
	DRIVER( mooncrsg ) /* 1980 "Gremlin" "Moon Cresta (Gremlin)" */
	DRIVER( mooncrst ) /* 1980 "Nichibutsu" "Moon Cresta (Nichibutsu)" */
	DRIVER( mooncrsu ) /* 1980 "Nichibutsu USA" "Moon Cresta (Nichibutsu */
	DRIVER( moonqsr ) /* 1980 "Nichibutsu" "Moon Quasar" */
	DRIVER( motos ) /* 1985 "Namco" "Motos" */
	DRIVER( mrdrillr ) /* 1999 "Namco" "Mr Driller (DRI1/VER.A2)" */
	DRIVER( mshutlj2 ) /* 1981 "Nichibutsu" "Moon Shuttle (Japan set 2)" */
	DRIVER( mshuttle ) /* 1981 "Nichibutsu" "Moon Shuttle (US?)" */
	DRIVER( mshuttlj ) /* 1981 "Nichibutsu" "Moon Shuttle (Japan)" */
	DRIVER( myangel3 ) /* 1998 "Namco" "Kosodate Quiz My Angel 3 (KQT1/VER.A)" */
	DRIVER( navarone ) /* 1980 "Namco" "Navarone" */
	DRIVER( ncv1 ) /* 1995 "Namco" "Namco Classics Collection Vol.1" */
	DRIVER( ncv1j ) /* 1995 "Namco" "Namco Classics Collection Vol.1 (Japan */
	DRIVER( ncv1j2 ) /* 1995 "Namco" "Namco Classics Collection Vol.1 (Japan */
	DRIVER( ncv2 ) /* 1996 "Namco" "Namco Classics Collection Vol.2" */
	DRIVER( ncv2j ) /* 1996 "Namco" "Namco Classics Collection Vol.2 (Japan)" */
	DRIVER( nebulray ) /* 1994 "Namco" "Nebulas Ray (World)" */
	DRIVER( nebulryj ) /* 1994 "Namco" "Nebulas Ray (Japan)" */
	DRIVER( nrallyx ) /* 1981 "Namco" "New Rally X" */
	DRIVER( numanath ) /* 1993 "Namco" "Numan Athletics (World)" */
	DRIVER( numanatj ) /* 1993 "Namco" "Numan Athletics (Japan)" */
	DRIVER( omega ) /* 19?? "bootleg?" "Omega" */
	DRIVER( omni ) /* 19?? "bootleg" "Omni" */
	DRIVER( orbitron ) /* 19?? "Signatron USA" "Orbitron" */
	DRIVER( ordyne ) /* 1988 "Namco" "Ordyne (Japan */
	DRIVER( ordynej ) /* 1988 "Namco" "Ordyne (Japan)" */
	DRIVER( outfxesj ) /* 1994 "Namco" "Outfoxies (Japan)" */
	DRIVER( outfxies ) /* 1994 "Namco" "Outfoxies" */
	DRIVER( ozon1 ) /* 1983 "Proma" "Ozon I" */
	DRIVER( pacapp ) /* 1999 "Produce/Namco" "Paca Paca Passion (PPP1/VER.A2)" */
	DRIVER( pacapp2 ) /* 1999 "Produce/Namco" "Paca Paca Passion 2 (PKS1/VER.A)" */
	DRIVER( pacappsp ) /* 1999 "Produce/Namco" "Paca Paca Passion Special (PSP1/VER.A)" */
	DRIVER( pacland ) /* 1984 "Namco" "Pac-Land (set 1)" */
	DRIVER( pacland2 ) /* 1984 "Namco" "Pac-Land (set 2)" */
	DRIVER( pacland3 ) /* 1984 "Namco" "Pac-Land (set 3)" */
	DRIVER( paclandm ) /* 1984 "[Namco] (Bally Midway license)" "Pac-Land (Midway)" */
	DRIVER( pacmanbl ) /* 1981 "bootleg" "Pac-Man (Galaxian hardware)" */
	DRIVER( pacmania ) /* 1987 "Namco" "Pac-Mania" */
	DRIVER( pacmanij ) /* 1987 "Namco" "Pac-Mania (Japan)" */
	DRIVER( pacnchmp ) /* 1983 "Namco" "Pac-Man & Chomp Chomp" */
	DRIVER( pacnpal ) /* 1983 "Namco" "Pac & Pal" */
	DRIVER( pacnpal2 ) /* 1983 "Namco" "Pac & Pal (older)" */
	DRIVER( phelios ) /* 1988 "Namco" "Phelios (Japan)" */
	DRIVER( phozon ) /* 1983 "Namco" "Phozon (Japan)" */
	DRIVER( pisces ) /* 19?? "Subelectro" "Pisces" */
	DRIVER( piscesb ) /* 19?? "bootleg" "Pisces (bootleg)" */
	DRIVER( pistoldm ) /* 1990 "Namco" "Pistol Daimyo no Bouken (Japan)" */
	DRIVER( polepos ) /* 1982 "Namco" "Pole Position" */
	DRIVER( polepos1 ) /* 1982 "[Namco] (Atari license)" "Pole Position (Atari version 1)" */
	DRIVER( polepos2 ) /* 1983 "Namco" "Pole Position II" */
	DRIVER( poleposa ) /* 1982 "Namco (Atari license)" "Pole Position (Atari version 2)" */
	DRIVER( poleps2a ) /* 1983 "Namco (Atari license)" "Pole Position II (Atari)" */
	DRIVER( poleps2b ) /* 1983 "bootleg" "Pole Position II (bootleg)" */
	DRIVER( primglex ) /* 1996 "Namco" "Prime Goal EX (PG1/VER.A)" */
	DRIVER( propcycl ) /* 1996 "Namco" "Prop Cycle (Rev PR2 Ver.A)" */
	DRIVER( ptblank ) /* 1994 "Namco" "Point Blank" */
	DRIVER( ptblank2 ) /* 1999 "Namco" "Point Blank 2 (GNB5/VER.A)" */
	DRIVER( ptblnk2a ) /* 1999 "Namco" "Point Blank 2 (GNB3/VER.A)" */
	DRIVER( puzlclub ) /* 1990 "Namco" "Puzzle Club (Japan prototype)" */
	DRIVER( quester ) /* 1987 "Namco" "Quester (Japan)" */
	DRIVER( quiztou ) /* 1993 "Namco" "Nettou! Gekitou! Quiztou!! (Japan)" */
	DRIVER( racknrol ) /* 1986 "Status (Shinkai License)" "Rack + Roll" */
	DRIVER( rallyx ) /* 1980 "Namco" "Rally X" */
	DRIVER( rallyxm ) /* 1980 "[Namco] (Midway license)" "Rally X (Midway)" */
	DRIVER( raveracj ) /* 1995 "Namco" "Rave Racer (Rev. RV1 Ver.B */
	DRIVER( raveracw ) /* 1995 "Namco" "Rave Racer (Rev. RV2 */
	DRIVER( raveraja ) /* 1995 "Namco" "Rave Racer (Rev. RV1 */
	DRIVER( redufo ) /* 19?? "bootleg" "Defend the Terra Attack on the Red UFO" */
	DRIVER( ridger2a ) /* 1994 "Namco" "Ridge Racer 2 (Rev. RRS1 */
	DRIVER( ridger2b ) /* 1994 "Namco" "Ridge Racer 2 (Rev. RRS1 */
	DRIVER( ridgera2 ) /* 1994 "Namco" "Ridge Racer 2 (Rev. RRS2 */
	DRIVER( ridgerac ) /* 1993 "Namco" "Ridge Racer (Rev. RR2 */
	DRIVER( ridgeraj ) /* 1993 "Namco" "Ridge Racer (Rev. RR1 */
	DRIVER( rockclim ) /* 1981 "Taito" "Rock Climber" */
	DRIVER( roishtar ) /* 1986 "Namco" "The Return of Ishtar" */
	DRIVER( rompers ) /* 1989 "Namco" "Rompers (Japan)" */
	DRIVER( romperso ) /* 1989 "Namco" "Rompers (Japan old version)" */
	DRIVER( rthun2 ) /* 1990 "Namco" "Rolling Thunder 2" */
	DRIVER( rthun2j ) /* 1990 "Namco" "Rolling Thunder 2 (Japan)" */
	DRIVER( rthunder ) /* 1986 "Namco" "Rolling Thunder (new version)" */
	DRIVER( rthundro ) /* 1986 "Namco" "Rolling Thunder (old version)" */
	DRIVER( scorpnmc ) /* 19?? "Dorneer" "Scorpion (Moon Cresta hardware)" */
	DRIVER( scramblb ) /* 1981 "bootleg" "Scramble (Galaxian hardware)" */
	DRIVER( sgunner ) /* 1990 "Namco" "Steel Gunner" */
	DRIVER( sgunner2 ) /* 1991 "Namco" "Steel Gunner 2 (US)" */
	DRIVER( sgunnerj ) /* 1990 "Namco" "Steel Gunner (Japan)" */
	DRIVER( sgunnr2j ) /* 1991 "Namco" "Steel Gunner 2 (Japan)" */
	DRIVER( shadowld ) /* 1987 "Namco" "Shadowland" */
	DRIVER( skybase ) /* 1982 "Omori Electric Co. Ltd." */
	DRIVER( skykid ) /* 1985 "Namco" "Sky Kid (New Ver.)" */
	DRIVER( skykidd ) /* 1985 "Namco" "Sky Kid (60A1 Ver.)" */
	DRIVER( skykiddo ) /* 1986 "Namco" "Sky Kid Deluxe (set 2)" */
	DRIVER( skykiddx ) /* 1986 "Namco" "Sky Kid Deluxe (set 1)" */
	DRIVER( skykido ) /* 1985 "Namco" "Sky Kid (Old Ver.)" */
	DRIVER( skyraidr ) /* 1980 "bootleg" "Sky Raiders" */
	DRIVER( smooncrs ) /* 1980? "Gremlin" "Super Moon Cresta" */
	DRIVER( sos ) /* 1980 "Namco" "SOS" */
	DRIVER( soukobdx ) /* 1990 "Namco" "Souko Ban Deluxe (Japan)" */
	DRIVER( spacbatt ) /* 1980 "bootleg" "Space Battle" */
	DRIVER( spctbird ) /* 1981? "Fortrek" "Space Thunderbird" */
	DRIVER( speedrcr ) /* 1995 "Namco" "Speed Racer" */
	DRIVER( splattej ) /* 1988 "Namco" "Splatter House (Japan)" */
	DRIVER( splatter ) /* 1988 "Namco" "Splatter House (World)" */
	DRIVER( starblad ) /* 1991 "Namco" "Starblade" */
	DRIVER( starfght ) /* 1979 "Jeutel" "Star Fighter" */
	DRIVER( starswep ) /* 1997 "Axela/Namco" "Star Sweep (STP1/VER.A)" */
	DRIVER( streakng ) /* 1981 "Shoei" "Streaking" */
	DRIVER( superg ) /* 1979 "hack" "Super Galaxians" */
	DRIVER( superpac ) /* 1982 "Namco" "Super Pac-Man" */
	DRIVER( superpcm ) /* 1982 "[Namco] (Bally Midway license)" "Super Pac-Man (Midway)" */
	DRIVER( suzuk8h2 ) /* 1993 "Namco" "Suzuka 8 Hours 2 (World)" */
	DRIVER( suzuk8hj ) /* 1992 "Namco" "Suzuka 8 Hours (Japan)" */
	DRIVER( suzuka8h ) /* 1992 "Namco" "Suzuka 8 Hours (World)" */
	DRIVER( swarm ) /* 1979 "hack" "Swarm" */
	DRIVER( swcourt ) /* 1992 "Namco" "Super World Court (World)" */
	DRIVER( swcourtj ) /* 1992 "Namco" "Super World Court (Japan)" */
	DRIVER( sws ) /* 1992 "Namco" "Super World Stadium (Japan)" */
	DRIVER( sws92 ) /* 1992 "Namco" "Super World Stadium '92 (Japan)" */
	DRIVER( sws92g ) /* 1992 "Namco" "Super World Stadium '92 Gekitouban (Japan)" */
	DRIVER( sws93 ) /* 1993 "Namco" "Super World Stadium '93 (Japan)" */
	DRIVER( sws95 ) /* 1995 "Namco" "Super World Stadium '95 (Japan)" */
	DRIVER( sws96 ) /* 1996 "Namco" "Super World Stadium '96 (Japan)" */
	DRIVER( sws97 ) /* 1997 "Namco" "Super World Stadium '97 (Japan)" */
	DRIVER( sws99 ) /* 1999 "Namco" "Super World Stadium '99 (SS91/VER.A3)" */
	DRIVER( sxevious ) /* 1984 "Namco" "Super Xevious" */
	DRIVER( tactcan2 ) /* 1981 "[Konami] (Sega license)" "Tactician (set 2)" */
	DRIVER( tactcian ) /* 1982 "[Konami] (Sega license)" "Tactician (set 1)" */
	DRIVER( tankbatt ) /* 1980 "Namco" "Tank Battalion" */
	DRIVER( tankfrce ) /* 1991 "Namco" "Tank Force (US)" */
	DRIVER( tankfrcj ) /* 1991 "Namco" "Tank Force (Japan)" */
	DRIVER( tazzmang ) /* 1982 "bootleg" "Tazz-Mania (Galaxian Hardware)" */
	DRIVER( tceptor ) /* 1986 "Namco" "Thunder Ceptor" */
	DRIVER( tceptor2 ) /* 1986 "Namco" "Thunder Ceptor II" */
	DRIVER( tekken ) /* 1994 "Namco" "Tekken (TE4/VER.C)" */
	DRIVER( tekken2 ) /* 1995 "Namco" "Tekken 2 Ver.B (TES3/VER.B)" */
	DRIVER( tekken2a ) /* 1995 "Namco" "Tekken 2 Ver.B (TES2/VER.B)" */
	DRIVER( tekken2b ) /* 1995 "Namco" "Tekken 2 (TES2/VER.A)" */
	DRIVER( tekken3 ) /* 1996 "Namco" "Tekken 3 (TET1/VER.E1)" */
	DRIVER( tekken3a ) /* 1996 "Namco" "Tekken 3 (TET2/VER.B)" */
	DRIVER( tekken3b ) /* 1996 "Namco" "Tekken 3 (TET2/VER.A)" */
	DRIVER( tekkena ) /* 1994 "Namco" "Tekken (TE2/VER.C)" */
	DRIVER( tekkenb ) /* 1994 "Namco" "Tekken (TE2/VER.B)" */
	DRIVER( tekkenc ) /* 1994 "Namco" "Tekken (TE1/VER.B)" */
	DRIVER( tenkomoj ) /* 1998 "Namco" "Tenkomori Shooting (TKM1/VER.A1)" */
	DRIVER( tenkomor ) /* 1998 "Namco" "Tenkomori Shooting (TKM2/VER.A1)" */
	DRIVER( timecris ) /* 1995 "Namco" "Time Crisis (Rev. TS2 Ver.B)" */
	DRIVER( timecrsa ) /* 1995 "Namco" "Time Crisis (Rev. TS2 Ver.A)" */
	DRIVER( tinklpit ) /* 1993 "Namco" "Tinkle Pit (Japan)" */
	DRIVER( todruaga ) /* 1984 "Namco" "Tower of Druaga (New Ver.)" */
	DRIVER( todruago ) /* 1984 "Namco" "Tower of Druaga (Old Ver.)" */
	DRIVER( topracer ) /* 1982 "bootleg" "Top Racer (set 1)" */
	DRIVER( topracra ) /* 1982 "bootleg" "Top Racer (set 2)" */
	DRIVER( topracrb ) /* 1983 "bootleg" "Top Racer (set 3)" */
	DRIVER( toypop ) /* 1986 "Namco" "Toypop" */
	DRIVER( tst_galx ) /* 19?? "Test ROM" "Galaxian Test ROM" */
	DRIVER( uniwars ) /* 1980 "Irem" "UniWar S" */
	DRIVER( valkyrie ) /* 1989 "Namco" "Valkyrie No Densetsu (Japan)" */
	DRIVER( victlapw ) /* 1996 "Namco" "Ace Driver: Victory Lap (Rev. ADV2 */
	DRIVER( vpool ) /* 1980 "bootleg" "Video Pool (bootleg on Moon Cresta hardware)" */
	DRIVER( vshoot ) /* 1994 "Namco" "J-League Soccer V-Shoot" */
	DRIVER( warofbug ) /* 1981 "Armenia" "War of the Bugs or Monsterous Manouvers in a Mushroom Maze" */
	DRIVER( warpwar2 ) /* 1981 "[Namco] (Rock-ola license)" "Warp Warp (Rock-ola set 2)" */
	DRIVER( warpwarp ) /* 1981 "Namco" "Warp & Warp" */
	DRIVER( warpwarr ) /* 1981 "[Namco] (Rock-ola license)" "Warp Warp (Rock-ola set 1)" */
	DRIVER( wldcourt ) /* 1988 "Namco" "World Court (Japan)" */
	DRIVER( wndrmomo ) /* 1987 "Namco" "Wonder Momo" */
	DRIVER( ws ) /* 1988 "Namco" "World Stadium (Japan)" */
	DRIVER( ws89 ) /* 1989 "Namco" "World Stadium '89 (Japan)" */
	DRIVER( ws90 ) /* 1990 "Namco" "World Stadium '90 (Japan)" */
	DRIVER( xevi3dg ) /* 1995 "Namco" "Xevious 3D/G (XV31/VER.A)" */
	DRIVER( xevios ) /* 1982 "bootleg" "Xevios" */
	DRIVER( xevious ) /* 1982 "Namco" "Xevious (Namco)" */
	DRIVER( xeviousa ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 1)" */
	DRIVER( xeviousb ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 2)" */
	DRIVER( xeviousc ) /* 1982 "Namco (Atari license)" "Xevious (Atari set 3)" */
	DRIVER( yokaidko ) /* 1987 "Namco" "Yokai Douchuuki (Japan old version)" */
	DRIVER( youkaidk ) /* 1987 "Namco" "Yokai Douchuuki (Japan new version)" */
	DRIVER( zerotime ) /* 1979 "Petaco S.A." "Zero Time" */
	DRIVER( zigzag ) /* 1982 "LAX" "Zig Zag (Galaxian hardware */
	DRIVER( zigzag2 ) /* 1982 "LAX" "Zig Zag (Galaxian hardware */
#endif
#ifdef LINK_NAMCOS
	DRIVER( bakutotu ) /*    */
	DRIVER( berabohm ) /*    */
	DRIVER( beraboho ) /*    */
	DRIVER( blastoff ) /*    */
	DRIVER( blazer ) /*    */
	DRIVER( boxyboy ) /*    */
	DRIVER( dangseed ) /*    */
	DRIVER( dspirit ) /*    */
	DRIVER( dspirito ) /*    */
	DRIVER( faceoff ) /*    */
	DRIVER( galag88j ) /*    */
	DRIVER( galaga88 ) /*    */
	DRIVER( mmaze ) /*    */
	DRIVER( pacmania ) /*    */
	DRIVER( pacmanij ) /*    */
	DRIVER( pistoldm ) /*    */
	DRIVER( puzlclub ) /*    */
	DRIVER( quester ) /*    */
	DRIVER( rompers ) /*    */
	DRIVER( romperso ) /*    */
	DRIVER( shadowld ) /*    */
	DRIVER( soukobdx ) /*    */
	DRIVER( splattej ) /*    */
	DRIVER( splatter ) /*    */
	DRIVER( tankfrce ) /*    */
	DRIVER( tankfrcj ) /*    */
	DRIVER( wldcourt ) /*    */
	DRIVER( ws ) /*    */
	DRIVER( ws89 ) /*    */
	DRIVER( ws90 ) /*    */
	DRIVER( yokaidko ) /*    */
	DRIVER( youkaidk ) /*    */
#endif
#ifdef LINK_NASCO
	DRIVER( crgolf ) /* 1984 "Nasco Japan" "Crowns Golf (set 1)" */
	DRIVER( crgolfa ) /* 1984 "Nasco Japan" "Crowns Golf (set 2)" */
	DRIVER( crgolfb ) /* 1984 "Nasco Japan" "Champion Golf (bootleg Set 1)" */
	DRIVER( crgolfc ) /* 1984 "Nasco Japan" "Champion Golf" */
	DRIVER( crgolfhi ) /* 1985 "Nasco Japan" "Crowns Golf in Hawaii" */
#endif
#ifdef LINK_NEOGEO
	DRIVER( 2020bb ) /* 1991 "SNK / Pallas" "2020 Super Baseball (set 1)" */
	DRIVER( 2020bba ) /* 1991 "SNK / Pallas" "2020 Super Baseball (set 2)" */
	DRIVER( 2020bbh ) /* 1991 "SNK / Pallas" "2020 Super Baseball (set 3)" */
	DRIVER( 3countb ) /* 1993 "SNK" "3 Count Bout / Fire Suplex" */
	DRIVER( alpham2 ) /* 1991 "SNK" "Alpha Mission II / ASO II - Last Guardian" */
	DRIVER( androdun ) /* 1992 "Visco" "Andro Dunos" */
	DRIVER( aodk ) /* 1994 "ADK / SNK" "Aggressors of Dark Kombat / Tsuukai GANGAN Koushinkyoku" */
	DRIVER( aof ) /* 1992 "SNK" "Art of Fighting / Ryuuko no Ken" */
	DRIVER( aof2 ) /* 1994 "SNK" "Art of Fighting 2 / Ryuuko no Ken 2 (set 1)" */
	DRIVER( aof2a ) /* 1994 "SNK" "Art of Fighting 2 / Ryuuko no Ken 2 (set 2)" */
	DRIVER( aof3 ) /* 1996 "SNK" "Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden" */
	DRIVER( aof3k ) /* 1996 "SNK" "Art of Fighting 3 - The Path of the Warrior (Korean release)" */
	DRIVER( b2b ) /* 1994 "Visco" "Bang Bang Busters (2010 NCI release)" */
	DRIVER( bakatono ) /* 1991 "Monolith Corp." "Bakatonosama Mahjong Manyuki" */
	DRIVER( bangbead ) /* 2000 "Visco" "Bang Bead" */
	DRIVER( bjourney ) /* 1990 "Alpha Denshi Co." "Blue's Journey / Raguy" */
	DRIVER( blazstar ) /* 1998 "Yumekobo" "Blazing Star" */
	DRIVER( breakers ) /* 1996 "Visco" "Breakers" */
	DRIVER( breakrev ) /* 1998 "Visco" "Breakers Revenge" */
	DRIVER( bstars ) /* 1990 "SNK" "Baseball Stars Professional" */
	DRIVER( bstars2 ) /* 1992 "SNK" "Baseball Stars 2" */
	DRIVER( burningf ) /* 1991 "SNK" "Burning Fight (set 1)" */
	DRIVER( burningh ) /* 1991 "SNK" "Burning Fight (set 2)" */
	DRIVER( crsword ) /* 1991 "Alpha Denshi Co." "Crossed Swords" */
	DRIVER( ct2k3sp ) /* 2003 "bootleg" "Crouching Tiger Hidden Dragon 2003 Super Plus (The King of Fighters 2001 bootleg)" */
	DRIVER( cthd2003 ) /* 2003 "bootleg" "Crouching Tiger Hidden Dragon 2003 (The King of Fighters 2001 bootleg)" */
	DRIVER( ctomaday ) /* 1999 "Visco" "Captain Tomaday" */
	DRIVER( cyberlip ) /* 1990 "SNK" "Cyber-Lip" */
	DRIVER( doubledr ) /* 1995 "Technos" "Double Dragon (Neo-Geo)" */
	DRIVER( eightman ) /* 1991 "SNK / Pallas" "Eight Man" */
	DRIVER( fatfursa ) /* 1993 "SNK" "Fatal Fury Special / Garou Densetsu Special (set 2)" */
	DRIVER( fatfursp ) /* 1993 "SNK" "Fatal Fury Special / Garou Densetsu Special (set 1)" */
	DRIVER( fatfury1 ) /* 1991 "SNK" "Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai" */
	DRIVER( fatfury2 ) /* 1992 "SNK" "Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai" */
	DRIVER( fatfury3 ) /* 1995 "SNK" "Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai" */
	DRIVER( fbfrenzy ) /* 1992 "SNK" "Football Frenzy" */
	DRIVER( fightfev ) /* 1994 "Viccom" "Fight Fever (set 1)" */
	DRIVER( fightfva ) /* 1994 "Viccom" "Fight Fever (set 2)" */
	DRIVER( flipshot ) /* 1998 "Visco" "Battle Flip Shot" */
	DRIVER( fswords ) /* 1995 "SNK" "Fighters Swords (Korean release of Samurai Shodown III)" */
	DRIVER( galaxyfg ) /* 1995 "Sunsoft" "Galaxy Fight - Universal Warriors" */
	DRIVER( ganryu ) /* 1999 "Visco" "Ganryu / Musashi Ganryuki" */
	DRIVER( garou ) /* 1999 "SNK" "Garou - Mark of the Wolves (set 1)" */
	DRIVER( garouo ) /* 1999 "SNK" "Garou - Mark of the Wolves (set 2)" */
	DRIVER( garoup ) /* 1999 "SNK" "Garou - Mark of the Wolves (prototype)" */
	DRIVER( ghostlop ) /* 1996 "Data East Corporation" "Ghostlop (prototype)" */
	DRIVER( goalx3 ) /* 1995 "Visco" "Goal! Goal! Goal!" */
	DRIVER( gowcaizr ) /* 1995 "Technos" "Voltage Fighter - Gowcaizer / Choujin Gakuen Gowcaizer" */
	DRIVER( gpilots ) /* 1991 "SNK" "Ghost Pilots" */
	DRIVER( gururin ) /* 1994 "Face" "Gururin" */
	DRIVER( ironclad ) /* 1996 "Saurus" "Choutetsu Brikin'ger - Ironclad (Prototype)" */
	DRIVER( irrmaze ) /* 1997 "SNK / Saurus" "The Irritating Maze / Ultra Denryu Iraira Bou" */
	DRIVER( janshin ) /* 1994 "Aicom" "Jyanshin Densetsu - Quest of Jongmaster" */
	DRIVER( jockeygp ) /* 2001 "Sun Amusement / BrezzaSoft" "Jockey Grand Prix" */
	DRIVER( joyjoy ) /* 1990 "SNK" "Puzzled / Joy Joy Kid" */
	DRIVER( kabukikl ) /* 1995 "Hudson" "Far East of Eden - Kabuki Klash / Tengai Makyou - Shin Den" */
	DRIVER( karnovr ) /* 1994 "Data East Corporation" "Karnov's Revenge / Fighter's History Dynamite" */
	DRIVER( kf10thep ) /* 2005 "bootleg" "The King of Fighters 10th Anniversary Extra Plus (The King of Fighters 2002 bootleg)" */
	DRIVER( kf2k2mp ) /* 2002 "bootleg" "The King of Fighters 2002 Magic Plus (bootleg)" */
	DRIVER( kf2k2mp2 ) /* 2002 "bootleg" "The King of Fighters 2002 Magic Plus II (bootleg)" */
	DRIVER( kf2k2pla ) /* 2002 "bootleg" "The King of Fighters 2002 Plus (set 2 */
	DRIVER( kf2k2pls ) /* 2002 "bootleg" "The King of Fighters 2002 Plus (set 1 */
	DRIVER( kf2k3bl ) /* 2003 "bootleg" "The King of Fighters 2003 (bootleg */
	DRIVER( kf2k3bla ) /* 2003 "bootleg" "The King of Fighters 2003 (bootleg */
	DRIVER( kf2k3pl ) /* 2003 "bootleg" "The King of Fighters 2004 Plus / Hero (The King of Fighters 2003 bootleg)" */
	DRIVER( kf2k3upl ) /* 2003 "bootleg" "The King of Fighters 2004 Ultra Plus (The King of Fighters 2003 bootleg)" */
	DRIVER( kf2k5uni ) /* 2004 "bootleg" "The King of Fighters 10th Anniversary 2005 Unique (The King of Fighters 2002 bootleg)" */
	DRIVER( kizuna ) /* 1996 "SNK" "Kizuna Encounter - Super Tag Battle / Fu'un Super Tag Battle" */
	DRIVER( kof10th ) /* 2002 "bootleg" "The King Of Fighters 10th Anniversary (The King of Fighters 2002 bootleg)" */
	DRIVER( kof2000 ) /* 2000 "SNK" "The King of Fighters 2000" */
	DRIVER( kof2000n ) /* 2000 "SNK" "The King of Fighters 2000 (not encrypted)" */
	DRIVER( kof2001 ) /* 2001 "Eolith / SNK" "The King of Fighters 2001 (set 1)" */
	DRIVER( kof2001h ) /* 2001 "Eolith / SNK" "The King of Fighters 2001 (set 2)" */
	DRIVER( kof2002 ) /* 2002 "Eolith / Playmore" "The King of Fighters 2002" */
	DRIVER( kof2003 ) /* 2003 "SNK Playmore" "The King of Fighters 2003 (World / US */
	DRIVER( kof2k4se ) /* 2004 "bootleg" "The King of Fighters Special Edition 2004 (The King of Fighters 2002 bootleg)" */
	DRIVER( kof94 ) /* 1994 "SNK" "The King of Fighters '94" */
	DRIVER( kof95 ) /* 1995 "SNK" "The King of Fighters '95 (set 1)" */
	DRIVER( kof95a ) /* 1995 "SNK" "The King of Fighters '95 (set 2)" */
	DRIVER( kof96 ) /* 1996 "SNK" "The King of Fighters '96 (set 1)" */
	DRIVER( kof96h ) /* 1996 "SNK" "The King of Fighters '96 (set 2)" */
	DRIVER( kof97 ) /* 1997 "SNK" "The King of Fighters '97 (set 1)" */
	DRIVER( kof97a ) /* 1997 "SNK" "The King of Fighters '97 (set 2)" */
	DRIVER( kof97pls ) /* 1997 "bootleg" "The King of Fighters '97 Plus (bootleg)" */
	DRIVER( kof98 ) /* 1998 "SNK" "The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends" */
	DRIVER( kof98k ) /* 1998 "SNK" "The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (Korean board)" */
	DRIVER( kof98n ) /* 1998 "SNK" "The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (not encrypted)" */
	DRIVER( kof99 ) /* 1999 "SNK" "The King of Fighters '99 - Millennium Battle (set 1)" */
	DRIVER( kof99a ) /* 1999 "SNK" "The King of Fighters '99 - Millennium Battle (set 2)" */
	DRIVER( kof99e ) /* 1999 "SNK" "The King of Fighters '99 - Millennium Battle (earlier)" */
	DRIVER( kof99n ) /* 1999 "SNK" "The King of Fighters '99 - Millennium Battle (not encrypted)" */
	DRIVER( kof99p ) /* 1999 "SNK" "The King of Fighters '99 - Millennium Battle (prototype)" */
	DRIVER( kotm ) /* 1991 "SNK" "King of the Monsters (set 1)" */
	DRIVER( kotm2 ) /* 1992 "SNK" "King of the Monsters 2 - The Next Thing" */
	DRIVER( kotmh ) /* 1991 "SNK" "King of the Monsters (set 2)" */
	DRIVER( lans2004 ) /* 1998 "bootleg" "Lansquenet 2004 (Shock Troopers - 2nd Squad bootleg)" */
	DRIVER( lastblad ) /* 1997 "SNK" "Last Blade / Bakumatsu Roman - Gekka no Kenshi */
	DRIVER( lastbld2 ) /* 1998 "SNK" "Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi */
	DRIVER( lastblda ) /* 1997 "SNK" "Last Blade / Bakumatsu Roman - Gekka no Kenshi */
	DRIVER( lastsold ) /* 1997 "SNK" "The Last Soldier (Korean release of The Last Blade)" */
	DRIVER( lbowling ) /* 1990 "SNK" "League Bowling" */
	DRIVER( legendos ) /* 1991 "SNK" "Legend of Success Joe / Ashitano Joe Densetsu" */
	DRIVER( lresort ) /* 1992 "SNK" "Last Resort" */
	DRIVER( magdrop2 ) /* 1996 "Data East Corporation" "Magical Drop II" */
	DRIVER( magdrop3 ) /* 1997 "Data East Corporation" "Magical Drop III" */
	DRIVER( maglord ) /* 1990 "Alpha Denshi Co." "Magician Lord (set 1)" */
	DRIVER( maglordh ) /* 1990 "Alpha Denshi Co." "Magician Lord (set 2)" */
	DRIVER( mahretsu ) /* 1990 "SNK" "Mahjong Kyoretsuden" */
	DRIVER( marukodq ) /* 1995 "Takara" "Chibi Marukochan Deluxe Quiz" */
	DRIVER( matrim ) /* 2002 "Noise Factory / Atlus" "Matrimelee / Shin Gouketsuji Ichizoku Toukon" */
	DRIVER( miexchng ) /* 1997 "Face" "Money Puzzle Exchanger / Money Idol Exchanger" */
	DRIVER( minasan ) /* 1990 "Monolith Corp." "Minnasanno Okagesamadesu" */
	DRIVER( mosyougi ) /* 1995 "ADK / SNK" "Syougi No Tatsujin - Master of Syougi" */
	DRIVER( ms4plus ) /* 2002 "bootleg" "Metal Slug 4 Plus (bootleg)" */
	DRIVER( ms5plus ) /* 2003 "bootleg" "Metal Slug 5 Plus (bootleg)" */
	DRIVER( mslug ) /* 1996 "Nazca" "Metal Slug - Super Vehicle-001" */
	DRIVER( mslug2 ) /* 1998 "SNK" "Metal Slug 2 - Super Vehicle-001/II" */
	DRIVER( mslug3 ) /* 2000 "SNK" "Metal Slug 3" */
	DRIVER( mslug3b6 ) /* 2000 "bootleg" "Metal Slug 6 (Metal Slug 3 bootleg)" */
	DRIVER( mslug3n ) /* 2000 "SNK" "Metal Slug 3 (not encrypted)" */
	DRIVER( mslug4 ) /* 2002 "Mega" "Metal Slug 4" */
	DRIVER( mslug5 ) /* 2003 "SNK Playmore" "Metal Slug 5" */
	DRIVER( mslugx ) /* 1999 "SNK" "Metal Slug X - Super Vehicle-001" */
	DRIVER( mutnat ) /* 1992 "SNK" "Mutation Nation" */
	DRIVER( nam1975 ) /* 1990 "SNK" "NAM-1975" */
	DRIVER( ncombat ) /* 1990 "Alpha Denshi Co." "Ninja Combat (set 1)" */
	DRIVER( ncombata ) /* 1990 "Alpha Denshi Co." "Ninja Combat (set 2)" */
	DRIVER( ncommand ) /* 1992 "Alpha Denshi Co." "Ninja Commando" */
	DRIVER( neobombe ) /* 1997 "Hudson" "Neo Bomberman" */
	DRIVER( neocup98 ) /* 1998 "SNK" "Neo-Geo Cup '98 - The Road to the Victory" */
	DRIVER( neodrift ) /* 1996 "Visco" "Neo Drift Out - New Technology" */
	DRIVER( neogeo ) /* 1990 "SNK" "Neo-Geo" */
	DRIVER( neomrdo ) /* 1996 "Visco" "Neo Mr. Do!" */
	DRIVER( ninjamas ) /* 1996 "ADK / SNK" "Ninja Master's - haoh-ninpo-cho" */
	DRIVER( nitd ) /* 2000 "Eleven / Gavaking" "Nightmare in the Dark" */
	DRIVER( overtop ) /* 1996 "ADK" "Over Top" */
	DRIVER( panicbom ) /* 1994 "Eighting / Hudson" "Panic Bomber" */
	DRIVER( pbobbl2n ) /* 1999 "Taito (SNK license)" "Puzzle Bobble 2 / Bust-A-Move Again (Neo-Geo)" */
	DRIVER( pbobblen ) /* 1994 "Taito" "Puzzle Bobble / Bust-A-Move (Neo-Geo) (set 1)" */
	DRIVER( pbobblna ) /* 1994 "Taito" "Puzzle Bobble / Bust-A-Move (Neo-Geo) (set 2)" */
	DRIVER( pgoal ) /* 1996 "Saurus" "Pleasure Goal / Futsal - 5 on 5 Mini Soccer" */
	DRIVER( pnyaa ) /* 2003 "Aiky / Taito" "Pochi and Nyaa" */
	DRIVER( popbounc ) /* 1997 "Video System Co." "Pop 'n Bounce / Gapporin" */
	DRIVER( preisle2 ) /* 1999 "Yumekobo" "Prehistoric Isle 2" */
	DRIVER( pspikes2 ) /* 1994 "Video System Co." "Power Spikes II" */
	DRIVER( pulstar ) /* 1995 "Aicom" "Pulstar" */
	DRIVER( puzzldpr ) /* 1997 "Taito (Visco license)" "Puzzle De Pon! R!" */
	DRIVER( puzzledp ) /* 1995 "Taito (Visco license)" "Puzzle De Pon!" */
	DRIVER( quizdai2 ) /* 1992 "SNK" "Quiz Meitantei Neo & Geo - Quiz Daisousa Sen part 2" */
	DRIVER( quizdais ) /* 1991 "SNK" "Quiz Daisousa Sen - The Last Count Down" */
	DRIVER( quizkof ) /* 1995 "Saurus" "Quiz King of Fighters" */
	DRIVER( ragnagrd ) /* 1996 "Saurus" "Ragnagard / Shin-Oh-Ken" */
	DRIVER( rbff1 ) /* 1995 "SNK" "Real Bout Fatal Fury / Real Bout Garou Densetsu" */
	DRIVER( rbff2 ) /* 1998 "SNK" "Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (set 1)" */
	DRIVER( rbff2a ) /* 1998 "SNK" "Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (set 2)" */
	DRIVER( rbff2k ) /* 1998 "SNK" "Real Bout Fatal Fury 2 - The Newcomers (Korean release)" */
	DRIVER( rbffspec ) /* 1996 "SNK" "Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special" */
	DRIVER( ridhero ) /* 1990 "SNK" "Riding Hero (set 1)" */
	DRIVER( ridheroh ) /* 1990 "SNK" "Riding Hero (set 2)" */
	DRIVER( roboarma ) /* 1991 "SNK" "Robo Army (set 2)" */
	DRIVER( roboarmy ) /* 1991 "SNK" "Robo Army (set 1)" */
	DRIVER( rotd ) /* 2002 "Evoga / Playmore" "Rage of the Dragons" */
	DRIVER( s1945p ) /* 1999 "Psikyo" "Strikers 1945 Plus" */
	DRIVER( samsh5sh ) /* 2003 "Yuki Enterprise / SNK Playmore" "Samurai Shodown V Special / Samurai Spirits Zero Special (set 2 */
	DRIVER( samsh5sn ) /* 2003 "Yuki Enterprise / SNK Playmore" "Samurai Shodown V Special / Samurai Spirits Zero Special (set 3 */
	DRIVER( samsh5sp ) /* 2003 "Yuki Enterprise / SNK Playmore" "Samurai Shodown V Special / Samurai Spirits Zero Special (set 1 */
	DRIVER( samsho ) /* 1993 "SNK" "Samurai Shodown / Samurai Spirits" */
	DRIVER( samsho2 ) /* 1994 "SNK" "Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen" */
	DRIVER( samsho3 ) /* 1995 "SNK" "Samurai Shodown III / Samurai Spirits - Zankurou Musouken (set 1)" */
	DRIVER( samsho3a ) /* 1995 "SNK" "Samurai Shodown III / Samurai Spirits - Zankurou Musouken (set 2)" */
	DRIVER( samsho4 ) /* 1996 "SNK" "Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin" */
	DRIVER( samsho5 ) /* 2003 "Yuki Enterprise / SNK Playmore" "Samurai Shodown V / Samurai Spirits Zero (set 1)" */
	DRIVER( samsho5b ) /* 2003 "bootleg" "Samurai Shodown V / Samurai Spirits Zero (bootleg)" */
	DRIVER( samsho5h ) /* 2003 "Yuki Enterprise / SNK Playmore" "Samurai Shodown V / Samurai Spirits Zero (set 2)" */
	DRIVER( savagere ) /* 1995 "SNK" "Savage Reign / Fu'un Mokushiroku - kakutou sousei" */
	DRIVER( sdodgeb ) /* 1996 "Technos" "Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu" */
	DRIVER( sengokh ) /* 1991 "SNK" "Sengoku / Sengoku Denshou (set 2)" */
	DRIVER( sengoku ) /* 1991 "SNK" "Sengoku / Sengoku Denshou (set 1)" */
	DRIVER( sengoku2 ) /* 1993 "SNK" "Sengoku 2 / Sengoku Denshou 2" */
	DRIVER( sengoku3 ) /* 2001 "SNK" "Sengoku 3" */
	DRIVER( shocktr2 ) /* 1998 "Saurus" "Shock Troopers - 2nd Squad" */
	DRIVER( shocktra ) /* 1997 "Saurus" "Shock Troopers (set 2)" */
	DRIVER( shocktro ) /* 1997 "Saurus" "Shock Troopers (set 1)" */
	DRIVER( socbrawl ) /* 1991 "SNK" "Soccer Brawl" */
	DRIVER( sonicwi2 ) /* 1994 "Video System Co." "Aero Fighters 2 / Sonic Wings 2" */
	DRIVER( sonicwi3 ) /* 1995 "Video System Co." "Aero Fighters 3 / Sonic Wings 3" */
	DRIVER( spinmast ) /* 1993 "Data East Corporation" "Spin Master / Miracle Adventure" */
	DRIVER( ssideki ) /* 1992 "SNK" "Super Sidekicks / Tokuten Ou" */
	DRIVER( ssideki2 ) /* 1994 "SNK" "Super Sidekicks 2 - The World Championship / Tokuten Ou 2 - real fight football" */
	DRIVER( ssideki3 ) /* 1995 "SNK" "Super Sidekicks 3 - The Next Glory / Tokuten Ou 3 - eikoue no michi" */
	DRIVER( ssideki4 ) /* 1996 "SNK" "Ultimate 11 - The SNK Football Championship / Tokuten Ou - Honoo no Libero */
	DRIVER( stakwin ) /* 1995 "Saurus" "Stakes Winner / Stakes Winner - GI kinzen seihae no michi" */
	DRIVER( stakwin2 ) /* 1996 "Saurus" "Stakes Winner 2" */
	DRIVER( strhoop ) /* 1994 "Data East Corporation" "Street Hoop / Street Slam / Dunk Dream" */
	DRIVER( superspy ) /* 1990 "SNK" "The Super Spy" */
	DRIVER( svc ) /* 2003 "Playmore" "SvC Chaos - SNK vs Capcom (MVS)" */
	DRIVER( svcboot ) /* 2003 "bootleg" "SvC Chaos - SNK vs Capcom (MVS) (bootleg)" */
	DRIVER( svcplus ) /* 2003 "bootleg" "SvC Chaos - SNK vs Capcom Plus (set 1 */
	DRIVER( svcplusa ) /* 2003 "bootleg" "SvC Chaos - SNK vs Capcom Plus (set 2 */
	DRIVER( svcsplus ) /* 2003 "bootleg" "SvC Chaos - SNK vs Capcom Super Plus (bootleg)" */
	DRIVER( tophunta ) /* 1994 "SNK" "Top Hunter - Roddy & Cathy (set 2)" */
	DRIVER( tophuntr ) /* 1994 "SNK" "Top Hunter - Roddy & Cathy (set 1)" */
	DRIVER( tpgolf ) /* 1990 "SNK" "Top Player's Golf" */
	DRIVER( trally ) /* 1991 "Alpha Denshi Co." "Thrash Rally" */
	DRIVER( turfmast ) /* 1996 "Nazca" "Neo Turf Masters / Big Tournament Golf" */
	DRIVER( twinspri ) /* 1996 "ADK" "Twinkle Star Sprites" */
	DRIVER( tws96 ) /* 1996 "Tecmo" "Tecmo World Soccer '96" */
	DRIVER( viewpoin ) /* 1992 "Sammy" "Viewpoint" */
	DRIVER( vliner ) /* 2001 "Dyna / BreezaSoft" "V-Liner (set 1)" */
	DRIVER( vlinero ) /* 2001 "Dyna / BreezaSoft" "V-Liner (set 2)" */
	DRIVER( wakuwak7 ) /* 1996 "Sunsoft" "Waku Waku 7" */
	DRIVER( wh1 ) /* 1992 "Alpha Denshi Co." "World Heroes (set 1)" */
	DRIVER( wh1h ) /* 1992 "Alpha Denshi Co." "World Heroes (set 2)" */
	DRIVER( wh2 ) /* 1993 "ADK" "World Heroes 2" */
	DRIVER( wh2j ) /* 1994 "ADK / SNK" "World Heroes 2 Jet" */
	DRIVER( whp ) /* 1995 "ADK / SNK" "World Heroes Perfect" */
	DRIVER( wjammers ) /* 1994 "Data East Corporation" "Windjammers / Flying Power Disc" */
	DRIVER( zedblade ) /* 1994 "NMK" "Zed Blade / Operation Ragnarok" */
	DRIVER( zintrckb ) /* 1996 "hack / bootleg" "Zintrick / Oshidashi Zentrix (hack / bootleg)" */
	DRIVER( zupapa ) /* 2001 "SNK" "Zupapa!" */
#endif
#ifdef LINK_NICHIBUT
	DRIVER( 4psimasy ) /* 1994 "SPHINX/AV JAPAN" "Mahjong 4P Simasyo (Japan)" */
	DRIVER( abunai ) /* 1989 "GREEN SOFT" "Abunai Houkago - Mou Matenai (Japan 890325)" */
	DRIVER( amatelas ) /* 1986 "Nichibutsu" "Sei Senshi Amatelass" */
	DRIVER( amazon ) /* 1986 "Nichibutsu" "Soldier Girl Amazon" */
	DRIVER( apparel ) /* 1986 "Central Denshi" "Apparel Night (Japan 860929)" */
	DRIVER( armedf ) /* 1988 "Nichibutsu" "Armed Formation" */
	DRIVER( bakuhatu ) /* 1991 "Nichibutsu" "Mahjong Bakuhatsu Junjouden (Japan)" */
	DRIVER( bananadr ) /* 1989 "Digital Soft" "Mahjong Banana Dream [BET] (Japan 891124)" */
	DRIVER( bigkong ) /* 198? "bootleg" "Big Kong" */
	DRIVER( bijokkog ) /* 1988 "Nichibutsu" "Bijokko Gakuen (Japan 880116)" */
	DRIVER( bijokkoy ) /* 1987 "Nichibutsu" "Bijokko Yume Monogatari (Japan 870925)" */
	DRIVER( cannonb ) /* 1985 "Soft" "Cannon Ball (Crazy Climber hardware)" */
	DRIVER( cannonb2 ) /* 1985 "TV Game Gruenberg" "Cannonball (set 2)" */
	DRIVER( ccboot ) /* 1980 "bootleg" "Crazy Climber (bootleg set 1)" */
	DRIVER( ccboot2 ) /* 1980 "bootleg" "Crazy Climber (bootleg set 2)" */
	DRIVER( cclimber ) /* 1980 "Nichibutsu" "Crazy Climber (US)" */
	DRIVER( cclimbr2 ) /* 1988 "Nichibutsu" "Crazy Climber 2 (Japan)" */
	DRIVER( cclimbrj ) /* 1980 "Nichibutsu" "Crazy Climber (Japan)" */
	DRIVER( cclmbr2a ) /* 1988 "Nichibutsu" "Crazy Climber 2 (Japan Harder)" */
	DRIVER( chinmoku ) /* 1990 "Nichibutsu" "Mahjong Chinmoku no Hentai (Japan 900511)" */
	DRIVER( citylove ) /* 1986 "Nichibutsu" "City Love (Japan 860908)" */
	DRIVER( ckong ) /* 1981 "Falcon" "Crazy Kong (set 1)" */
	DRIVER( ckonga ) /* 1981 "Falcon" "Crazy Kong (set 2)" */
	DRIVER( ckongalc ) /* 1981 "bootleg" "Crazy Kong (Alca bootleg)" */
	DRIVER( ckongb ) /* 198? "bootleg" "Crazy Kong (Alternative levels)" */
	DRIVER( ckongjeu ) /* 1981 "bootleg" "Crazy Kong (Jeutel bootleg)" */
	DRIVER( ckongo ) /* 1981 "bootleg" "Crazy Kong (Orca bootleg)" */
	DRIVER( clshrdst ) /* 1986 "Woodplace Inc. (Status Game Corp. license)" "Clash-Road (Status license)" */
	DRIVER( clshroad ) /* 1986 "Woodplace Inc." "Clash-Road" */
	DRIVER( club90s ) /* 1990 "Nichibutsu" "Mahjong CLUB 90's (set 1) (Japan 900919)" */
	DRIVER( club90sa ) /* 1990 "Nichibutsu" "Mahjong CLUB 90's (set 2) (Japan 900919)" */
	DRIVER( cmehyou ) /* 1992 "Nichibutsu/Kawakusu" "Mahjong Circuit no Mehyou (Japan)" */
	DRIVER( cop01 ) /* 1985 "Nichibutsu" "Cop 01 (set 1)" */
	DRIVER( cop01a ) /* 1985 "Nichibutsu" "Cop 01 (set 2)" */
	DRIVER( crystal2 ) /* 1986 "Nichibutsu" "Crystal Gal 2 (Japan 860620)" */
	DRIVER( crystalg ) /* 1986 "Nichibutsu" "Crystal Gal (Japan 860512)" */
	DRIVER( dacholer ) /* 1983 "Nichibutsu" "Dacholer" */
	DRIVER( dangar ) /* 1986 "Nichibutsu" "Dangar - Ufo Robo (12/1/1986)" */
	DRIVER( dangar2 ) /* 1986 "Nichibutsu" "Dangar - Ufo Robo (9/26/1986)" */
	DRIVER( dangarb ) /* 1986 "bootleg" "Dangar - Ufo Robo (bootleg)" */
	DRIVER( finalbny ) /* 1991 "Nichibutsu" "Mahjong Final Bunny [BET] (Japan)" */
	DRIVER( firebatl ) /* 1984 "Taito" "Fire Battle" */
	DRIVER( friskyt ) /* 1981 "Nichibutsu" "Frisky Tom (set 1)" */
	DRIVER( friskyta ) /* 1981 "Nichibutsu" "Frisky Tom (set 2)" */
	DRIVER( gal10ren ) /* 1993 "FUJIC" "Mahjong Gal 10-renpatsu (Japan)" */
	DRIVER( galivan ) /* 1985 "Nichibutsu" "Galivan - Cosmo Police (12/16/1985)" */
	DRIVER( galivan2 ) /* 1985 "Nichibutsu" "Galivan - Cosmo Police (12/11/1985)" */
	DRIVER( galkaika ) /* 1989 "Nichibutsu/T.R.TEC" "Mahjong Gal no Kaika (Japan)" */
	DRIVER( galkoku ) /* 1989 "Nichibutsu/T.R.TEC" "Mahjong Gal no Kokuhaku (Japan)" */
	DRIVER( gionbana ) /* 1989 "Nichibutsu" "Gionbana (Japan 890120)" */
	DRIVER( gomoku ) /* 1981 "Nichibutsu" "Gomoku Narabe Renju" */
	DRIVER( guzzler ) /* 1983 "Tehkan" "Guzzler" */
	DRIVER( hanamomb ) /* 1988 "Nichibutsu" "Mahjong Hana no Momoko gumi (Japan 881125)" */
	DRIVER( hanamomo ) /* 1988 "Nichibutsu" "Mahjong Hana no Momoko gumi (Japan 881201)" */
	DRIVER( hanaoji ) /* 1991 "Nichibutsu" "Hana to Ojisan [BET] (Japan 911209)" */
	DRIVER( horekid ) /* 1987 "Nichibutsu" "Kid no Hore Hore Daisakusen" */
	DRIVER( horekidb ) /* 1987 "bootleg" "Kid no Hore Hore Daisakusen (bootleg)" */
	DRIVER( housemn2 ) /* 1987 "Nichibutsu" "House Mannequin Roppongi Live hen (Japan 870418)" */
	DRIVER( housemnq ) /* 1987 "Nichibutsu" "House Mannequin (Japan 870217)" */
	DRIVER( hyhoo ) /* 1987 "Nichibutsu" "Taisen Quiz HYHOO (Japan)" */
	DRIVER( hyhoo2 ) /* 1987 "Nichibutsu" "Taisen Quiz HYHOO 2 (Japan)" */
	DRIVER( hyouban ) /* 1989 "Nichibutsu/T.R.TEC" "Mahjong Hyouban Musume [BET] (Japan)" */
	DRIVER( idhimitu ) /* 1989 "Digital Soft" "Idol no Himitsu [BET] (Japan 890304)" */
	DRIVER( iemoto ) /* 1987 "Nichibutsu" "Iemoto (Japan 871020)" */
	DRIVER( iemotom ) /* 1987 "Nichibutsu" "Iemoto [BET] (Japan 871118)" */
	DRIVER( imekura ) /* 1994 "SPHINX/AV JAPAN" "Imekura Mahjong (Japan)" */
	DRIVER( janbari ) /* 1992 "Nichibutsu/Yubis/AV JAPAN" "Mahjong Janjan Baribari (Japan)" */
	DRIVER( jituroku ) /* 1993 "Windom" "Jitsuroku Maru-chi Mahjong (Japan)" */
	DRIVER( kaguya ) /* 1988 "MIKI SYOUJI" "Mahjong Kaguyahime [BET] (Japan 880521)" */
	DRIVER( kaguya2 ) /* 1989 "MIKI SYOUJI" "Mahjong Kaguyahime Sono2 [BET] (Japan 890829)" */
	DRIVER( kaguya2f ) /* 1989 "MIKI SYOUJI" "Mahjong Kaguyahime Sono2 Fukkokuban [BET] (Japan 010808)" */
	DRIVER( kanatuen ) /* 1988 "Panac" "Kanatsuen no Onna [BET] (Japan 880905)" */
	DRIVER( kickboy ) /* 1983 "Nichibutsu" "Kick Boy" */
	DRIVER( kodure ) /* 1987 "Nichibutsu" "Kodure Ookami (Japan)" */
	DRIVER( koinomp ) /* 1992 "Nichibutsu" "Mahjong Koi no Magic Potion (Japan)" */
	DRIVER( korinai ) /* 1988 "Nichibutsu" "Mahjong-zukino Korinai Menmen (Japan 880425)" */
	DRIVER( korinaim ) /* 1988 "Nichibutsu" "Mahjong-zukino Korinai Menmen [BET] (Japan 880920)" */
	DRIVER( kyuhito ) /* 1988 "ROLLER TRON" "Kyukyoku no Hito [BET] (Japan 880824)" */
	DRIVER( legion ) /* 1987 "Nichibutsu" "Legion (ver 2.03)" */
	DRIVER( legiono ) /* 1987 "Nichibutsu" "Legion (ver 1.05)" */
	DRIVER( livegal ) /* 1987 "Central Denshi" "Live Gal (Japan 870530)" */
	DRIVER( lovehous ) /* 1990 "Nichibutsu" "Mahjong Love House [BET] (Japan 901024)" */
	DRIVER( magmax ) /* 1985 "Nichibutsu" "Mag Max" */
	DRIVER( maiko ) /* 1990 "Nichibutsu" "Maikobana (Japan 900802)" */
	DRIVER( mcitylov ) /* 1986 "Nichibutsu" "City Love [BET] (Japan 860904)" */
	DRIVER( mcontest ) /* 1989 "Nichibutsu" "Miss Mahjong Contest (Japan)" */
	DRIVER( mgion ) /* 1989 "Nichibutsu" "Gionbana [BET] (Japan 890207)" */
	DRIVER( mgmen89 ) /* 1989 "Nichibutsu" "Mahjong G-MEN'89 (Japan 890425)" */
	DRIVER( mhhonban ) /* 199? "Nichibutsu?" "Mahjong Housoukyoku Honbanchuu (Japan)" */
	DRIVER( mjanbari ) /* 1992 "Nichibutsu/Yubis/AV JAPAN" "Medal Mahjong Janjan Baribari [BET] (Japan)" */
	DRIVER( mjcamera ) /* 1988 "MIKI SYOUJI" "Mahjong Camera Kozou (set 1) (Japan 881109)" */
	DRIVER( mjcamerb ) /* 1989 "MIKI SYOUJI" "Mahjong Camera Kozou (set 2) (Japan 881109)" */
	DRIVER( mjegolf ) /* 1994 "FUJIC/AV JAPAN" "Mahjong Erotica Golf (Japan)" */
	DRIVER( mjfocus ) /* 1989 "Nichibutsu" "Mahjong Focus (Japan 890313)" */
	DRIVER( mjfocusm ) /* 1989 "Nichibutsu" "Mahjong Focus [BET] (Japan 890510)" */
	DRIVER( mjgottsu ) /* 1991 "Nichibutsu" "Mahjong Gottsu ee-kanji (Japan)" */
	DRIVER( mjgottub ) /* 1991 "Nichibutsu" "Medal Mahjong Gottsu ee-kanji [BET] (Japan)" */
	DRIVER( mjkoiura ) /* 1992 "Nichibutsu" "Mahjong Koi Uranai (Japan)" */
	DRIVER( mjlaman ) /* 1993 "Nichibutsu/AV JAPAN" "Mahjong La Man (Japan)" */
	DRIVER( mjlstory ) /* 1991 "Nichibutsu" "Mahjong Jikken Love Story (Japan)" */
	DRIVER( mjnanpaa ) /* 1989 "BROOKS" "Mahjong Nanpa Story (Japan 890712)" */
	DRIVER( mjnanpas ) /* 1989 "BROOKS" "Mahjong Nanpa Story (Japan 890713)" */
	DRIVER( mjnanpau ) /* 1989 "BROOKS" "Mahjong Nanpa Story (Ura) (Japan 890805)" */
	DRIVER( mjsikakb ) /* 1988 "Nichibutsu" "Mahjong Shikaku (Japan 880722)" */
	DRIVER( mjsikakc ) /* 1988 "Nichibutsu" "Mahjong Shikaku (Japan 880806)" */
	DRIVER( mjsikakd ) /* 1988 "Nichibutsu" "Mahjong Shikaku (Japan 880802)" */
	DRIVER( mjsikaku ) /* 1988 "Nichibutsu" "Mahjong Shikaku (Japan 880908)" */
	DRIVER( mjuraden ) /* 1992 "Nichibutsu/Yubis" "Mahjong Uranai Densetsu (Japan)" */
	DRIVER( mkeibaou ) /* 1993 "Nichibutsu" "Mahjong Keibaou (Japan)" */
	DRIVER( mkoiuraa ) /* 1992 "Nichibutsu" "Mahjong Koi Uranai (set 2) (Japan)" */
	DRIVER( mladyhtr ) /* 1990 "Nichibutsu" "Mahjong THE LADY HUNTER (Japan 900509)" */
	DRIVER( mmaiko ) /* 1990 "Nichibutsu" "Maikobana [BET] (Japan 900911)" */
	DRIVER( mmcamera ) /* 1989 "MIKI SYOUJI" "Mahjong Camera Kozou [BET] (Japan 890509)" */
	DRIVER( mmehyou ) /* 1992 "Nichibutsu/Kawakusu" "Medal Mahjong Circuit no Mehyou [BET] (Japan)" */
	DRIVER( mmsikaku ) /* 1988 "Nichibutsu" "Mahjong Shikaku [BET] (Japan 880929)" */
	DRIVER( monkeyd ) /* 1981 "bootleg" "Monkey Donkey" */
	DRIVER( mscoutm ) /* 1994 "SPHINX/AV JAPAN" "Mahjong Scout Man (Japan)" */
	DRIVER( msjiken ) /* 1988 "Nichibutsu" "Mahjong Satsujin Jiken (Japan 881017)" */
	DRIVER( musobana ) /* 1995 "Nichibutsu/Yubis" "Musoubana (Japan)" */
	DRIVER( ngpgal ) /* 1991 "Nichibutsu" "Nekketsu Grand-Prix Gal (Japan)" */
	DRIVER( ninjemak ) /* 1986 "Nichibutsu" "Ninja Emaki (US)" */
	DRIVER( niyanpai ) /* 1996 "Nichibutsu" "Niyanpai (Japan)" */
	DRIVER( ntopstar ) /* 1990 "Nichibutsu" "Mahjong Nerae! Top Star (Japan)" */
	DRIVER( ojousan ) /* 1987 "Nichibutsu" "Ojousan (Japan 871204)" */
	DRIVER( ojousanm ) /* 1987 "Nichibutsu" "Ojousan [BET] (Japan 870108)" */
	DRIVER( omotesnd ) /* 1989 "ANIME TEC" "Omotesandou (Japan 890215)" */
	DRIVER( orangec ) /* 1988 "Daiichi Denshi" "Orange Club - Maruhi Kagai Jugyou (Japan 880213)" */
	DRIVER( orangeci ) /* 1988 "Daiichi Denshi" "Orange Club - Maru-hi Ippatsu Kaihou [BET] (Japan 880221)" */
	DRIVER( otatidai ) /* 1995 "SPHINX" "Disco Mahjong Otachidai no Okite (Japan)" */
	DRIVER( otonano ) /* 1988 "Apple" "Otona no Mahjong (Japan 880628)" */
	DRIVER( pachiten ) /* 1993 "Nichibutsu/MIKI SYOUJI/AV JAPAN" "Medal Mahjong Pachi-Slot Tengoku [BET] (Japan)" */
	DRIVER( pairsnb ) /* 1989 "Nichibutsu" "Pairs (Nichibutsu) (Japan 890822)" */
	DRIVER( pairsten ) /* 1989 "System Ten" "Pairs (System Ten) (Japan 890826)" */
	DRIVER( pastelg ) /* 1985 "Nichibutsu" "Pastel Gal (Japan 851224)" */
	DRIVER( patimono ) /* 1992 "Nichibutsu" "Mahjong Pachinko Monogatari (Japan)" */
	DRIVER( peepshow ) /* 1989 "AC" "Nozokimeguri Mahjong Peep Show (Japan 890404)" */
	DRIVER( psailor1 ) /* 1994 "SPHINX" "Bishoujo Janshi Pretty Sailor 18-kin (Japan)" */
	DRIVER( psailor2 ) /* 1994 "SPHINX" "Bishoujo Janshi Pretty Sailor 2 (Japan)" */
	DRIVER( pstadium ) /* 1990 "Nichibutsu" "Mahjong Panic Stadium (Japan)" */
	DRIVER( qmhayaku ) /* 1991 "Nichibutsu" "Quiz-Mahjong Hayaku Yatteyo! (Japan)" */
	DRIVER( radrad ) /* 1982 "Nichibutsu USA" "Radical Radial" */
	DRIVER( renaiclb ) /* 1993 "FUJIC" "Mahjong Ren-ai Club (Japan)" */
	DRIVER( rjammer ) /* 1984 "Nichibutsu + Alice" "Roller Jammer" */
	DRIVER( rpatrolb ) /* 1981 "bootleg" "River Patrol (bootleg)" */
	DRIVER( rpatrolo ) /* 1981 "Orca" "River Patrol (Orca)" */
	DRIVER( rugrats ) /* 1983 "Nichibutsu" "Rug Rats" */
	DRIVER( ryuuha ) /* 1987 "Central Denshi" "Ryuuha [BET] (Japan 871027)" */
	DRIVER( sailorwa ) /* 1993 "Nichibutsu" "Mahjong Sailor Wars (Japan) (set 2)" */
	DRIVER( sailorwr ) /* 1993 "Nichibutsu" "Mahjong Sailor Wars-R [BET] (Japan)" */
	DRIVER( sailorws ) /* 1993 "Nichibutsu" "Mahjong Sailor Wars (Japan)" */
	DRIVER( scandal ) /* 1989 "Nichibutsu" "Scandal Mahjong (Japan 890213)" */
	DRIVER( scandalm ) /* 1989 "Nichibutsu" "Scandal Mahjong [BET] (Japan 890217)" */
	DRIVER( secolove ) /* 1986 "Nichibutsu" "Second Love (Japan 861201)" */
	DRIVER( sectrzon ) /* 1984 "Nichibutsu + Alice" "Sector Zone" */
	DRIVER( seicross ) /* 1984 "Nichibutsu + Alice" "Seicross" */
	DRIVER( seiha ) /* 1987 "Nichibutsu" "Seiha (Japan 870725)" */
	DRIVER( seiham ) /* 1987 "Nichibutsu" "Seiha [BET] (Japan 870723)" */
	DRIVER( silvland ) /* 1981 "Falcon" "Silver Land" */
	DRIVER( swimmer ) /* 1982 "Tehkan" "Swimmer (set 1)" */
	DRIVER( swimmera ) /* 1982 "Tehkan" "Swimmer (set 2)" */
	DRIVER( swimmerb ) /* 1982 "Tehkan" "Swimmer (set 3)" */
	DRIVER( taiwanmb ) /* 1988 "MIKI SYOUJI" "Taiwan Mahjong [BET] (Japan 881208)" */
	DRIVER( telmahjn ) /* 1988 "Nichibutsu" "Telephone Mahjong (Japan 890111)" */
	DRIVER( terracra ) /* 1985 "Nichibutsu" "Terra Cresta (YM2203)" */
	DRIVER( terracrb ) /* 1985 "Nichibutsu" "Terra Cresta (YM3526 set 2)" */
	DRIVER( terracre ) /* 1985 "Nichibutsu" "Terra Cresta (YM3526 set 1)" */
	DRIVER( terraf ) /* 1987 "Nichibutsu" "Terra Force" */
	DRIVER( terrafu ) /* 1987 "Nichibutsu USA" "Terra Force (US)" */
	DRIVER( tokimbsj ) /* 1989 "Nichibutsu" "Tokimeki Bishoujo [BET] (Japan)" */
	DRIVER( tokyogal ) /* 1989 "Nichibutsu" "Tokyo Gal Zukan (Japan)" */
	DRIVER( toprollr ) /* 1983 "Jaleco" "Top Roller" */
	DRIVER( triplew1 ) /* 1989 "Nichibutsu" "Mahjong Triple Wars (Japan)" */
	DRIVER( triplew2 ) /* 1990 "Nichibutsu" "Mahjong Triple Wars 2 (Japan)" */
	DRIVER( tubep ) /* 1984 "Nichibutsu + Fujitek" "Tube Panic" */
	DRIVER( uchuuai ) /* 1989 "Nichibutsu" "Mahjong Uchuu yori Ai wo komete (Japan)" */
	DRIVER( ultramhm ) /* 1993 "Apple" "Ultra Maru-hi Mahjong (Japan)" */
	DRIVER( vanilla ) /* 1991 "Nichibutsu" "Mahjong Vanilla Syndrome (Japan)" */
	DRIVER( vipclub ) /* 1988 "Daiichi Denshi" "Vip Club - Maru-hi Ippatsu Kaihou [BET] (Japan 880310)" */
	DRIVER( wcatcher ) /* 1993 "Nichibutsu" "Mahjong Wakuwaku Catcher (Japan)" */
	DRIVER( wiping ) /* 1982 "Nichibutsu" "Wiping" */
	DRIVER( yamato ) /* 1983 "Sega" "Yamato (US)" */
	DRIVER( yamato2 ) /* 1983 "Sega" "Yamato (World?)" */
	DRIVER( yosimoto ) /* 1994 "Nichibutsu/Yoshimoto Kougyou" "Mahjong Yoshimoto Gekijou (Japan)" */
	DRIVER( youma ) /* 1986 "Nichibutsu" "Youma Ninpou Chou (Japan)" */
#endif
#ifdef LINK_NINTENDO
	DRIVER( 8ballact ) /* 1984 "Seatongrove Ltd (Magic Eletronics USA licence)" "Eight Ball Action (DK conversion)" */
	DRIVER( 8ballat2 ) /* 1984 "Seatongrove Ltd (Magic Eletronics USA licence)" "Eight Ball Action (DKJr conversion)" */
	DRIVER( armwrest ) /* 1985 "Nintendo" "Arm Wrestling" */
	DRIVER( balonfgt ) /* 1984 "Nintendo" "Vs. Balloon Fight" */
	DRIVER( bandido ) /* 1980 "Exidy" "Bandido" */
	DRIVER( bnglngby ) /* 1985 "Nintendo / Broderbund Software Inc." "Vs. Raid on Bungeling Bay (Japan)" */
	DRIVER( btlecity ) /* 1985 "Namco" "Vs. Battle City" */
	DRIVER( cluclu ) /* 1984 "Nintendo" "Vs. Clu Clu Land" */
	DRIVER( cstlevna ) /* 1987 "Konami" "Vs. Castlevania" */
	DRIVER( dkngjnrb ) /* 1982 "Nintendo of America" "Donkey Kong Junior (bootleg?)" */
	DRIVER( dkngjnrj ) /* 1982 "Nintendo" "Donkey Kong Junior (Japan?)" */
	DRIVER( dkong ) /* 1981 "Nintendo of America" "Donkey Kong (US set 1)" */
	DRIVER( dkong3 ) /* 1983 "Nintendo of America" "Donkey Kong 3 (US)" */
	DRIVER( dkong3b ) /* 1984 "bootleg" "Donkey Kong 3 (bootleg on Donkey Kong Jr. hardware)" */
	DRIVER( dkong3j ) /* 1983 "Nintendo" "Donkey Kong 3 (Japan)" */
	DRIVER( dkongjo ) /* 1981 "Nintendo" "Donkey Kong (Japan set 2)" */
	DRIVER( dkongjo1 ) /* 1981 "Nintendo" "Donkey Kong (Japan set 3) (bad dump?)" */
	DRIVER( dkongjp ) /* 1981 "Nintendo" "Donkey Kong (Japan set 1)" */
	DRIVER( dkongjr ) /* 1982 "Nintendo of America" "Donkey Kong Junior (US)" */
	DRIVER( dkongjrb ) /* 1982 "bootleg" "Donkey Kong Jr. (bootleg)" */
	DRIVER( dkongjrj ) /* 1982 "Nintendo" "Donkey Kong Jr. (Japan)" */
	DRIVER( dkongo ) /* 1981 "Nintendo" "Donkey Kong (US set 2)" */
	DRIVER( drakton ) /* 1984 "Epos Corporation" "Drakton" */
	DRIVER( drmario ) /* 1990 "Nintendo" "Vs. Dr. Mario" */
	DRIVER( duckhunt ) /* 1985 "Nintendo" "Vs. Duck Hunt" */
	DRIVER( excitbkj ) /* 1984 "Nintendo" "Vs. Excitebike (Japan)" */
	DRIVER( excitebk ) /* 1984 "Nintendo" "Vs. Excitebike" */
	DRIVER( goonies ) /* 1986 "Konami" "Vs. The Goonies" */
	DRIVER( helifira ) /* 1980 "Nintendo" "HeliFire (set 2)" */
	DRIVER( helifire ) /* 1980 "Nintendo" "HeliFire (set 1)" */
	DRIVER( herbiedk ) /* 1984 "CVS" "Herbie at the Olympics (DK conversion)" */
	DRIVER( herodk ) /* 1984 "Seatongrove Ltd (Crown license)" "Hero in the Castle of Doom (DK conversion)" */
	DRIVER( herodku ) /* 1984 "Seatongrove Ltd (Crown license)" "Hero in the Castle of Doom (DK conversion not encrypted)" */
	DRIVER( highspla ) /* 1979 "Nintendo" "Space Fever High Splitter (set 2)" */
	DRIVER( highsplt ) /* 1979 "Nintendo" "Space Fever High Splitter (set 1)" */
	DRIVER( hogalley ) /* 1985 "Nintendo" "Vs. Hogan's Alley" */
	DRIVER( iceclimb ) /* 1984 "Nintendo" "Vs. Ice Climber" */
	DRIVER( iceclmbj ) /* 1984 "Nintendo" "Vs. Ice Climber (Japan)" */
	DRIVER( iceclmrj ) /* 1984 "Nintendo" "Vs. Ice Climber Dual (Japan)" */
	DRIVER( jajamaru ) /* 1985 "Jaleco" "Vs. Ninja Jajamaru Kun (Japan)" */
	DRIVER( ladygolf ) /* 1984 "Nintendo" "Vs. Stroke & Match Golf (Ladies Version)" */
	DRIVER( machridj ) /* 1985 "Nintendo" "Vs. Mach Rider (Japan */
	DRIVER( machridr ) /* 1985 "Nintendo" "Vs. Mach Rider (Endurance Course Version)" */
	DRIVER( mario ) /* 1983 "Nintendo of America" "Mario Bros. (US)" */
	DRIVER( mariojp ) /* 1983 "Nintendo" "Mario Bros. (Japan)" */
	DRIVER( masao ) /* 1983 "bootleg" "Masao" */
	DRIVER( mightybj ) /* 1986 "Tecmo" "Vs. Mighty Bomb Jack (Japan)" */
	DRIVER( nss ) /* 199? "Nintendo" "Nintendo Super System BIOS" */
	DRIVER( pc_1942 ) /* 1987 "Capcom" "1942 (PlayChoice-10)" */
	DRIVER( pc_bball ) /* 1984 "Nintendo of America" "Baseball (PlayChoice-10)" */
	DRIVER( pc_bfght ) /* 1984 "Nintendo" "Balloon Fight (PlayChoice-10)" */
	DRIVER( pc_bstar ) /* 1989 "SNK (Nintendo of America license)" "Baseball Stars: Be a Champ! (PlayChoice-10)" */
	DRIVER( pc_cntra ) /* 1988 "Konami (Nintendo of America license)" "Contra (PlayChoice-10)" */
	DRIVER( pc_cshwk ) /* 1989 "Rare (Nintendo of America license)" "Captain Sky Hawk (PlayChoice-10)" */
	DRIVER( pc_cvnia ) /* 1987 "Konami (Nintendo of America license)" "Castlevania (PlayChoice-10)" */
	DRIVER( pc_dbldr ) /* 1987 "Konami (Nintendo of America license)" "Double Dribble (PlayChoice-10)" */
	DRIVER( pc_ddrgn ) /* 1988 "Technos" "Double Dragon (PlayChoice-10)" */
	DRIVER( pc_drmro ) /* 1990 "Nintendo" "Dr. Mario (PlayChoice-10)" */
	DRIVER( pc_duckh ) /* 1984 "Nintendo" "Duck Hunt (PlayChoice-10)" */
	DRIVER( pc_ebike ) /* 1984 "Nintendo" "Excite Bike (PlayChoice-10)" */
	DRIVER( pc_ftqst ) /* 1989 "Sunsoft (Nintendo of America license)" "Uncle Fester's Quest: The Addams Family (PlayChoice-10)" */
	DRIVER( pc_gntlt ) /* 1985 "Atari/Tengen (Nintendo of America license)" "Gauntlet (PlayChoice-10)" */
	DRIVER( pc_golf ) /* 1984 "Nintendo" "Golf (PlayChoice-10)" */
	DRIVER( pc_goons ) /* 1986 "Konami" "The Goonies (PlayChoice-10)" */
	DRIVER( pc_grdue ) /* 1986 "Konami" "Gradius (PlayChoice-10 */
	DRIVER( pc_grdus ) /* 1986 "Konami" "Gradius (PlayChoice-10)" */
	DRIVER( pc_hgaly ) /* 1984 "Nintendo" "Hogan's Alley (PlayChoice-10)" */
	DRIVER( pc_kngfu ) /* 1985 "Irem (Nintendo license)" "Kung Fu (PlayChoice-10)" */
	DRIVER( pc_mario ) /* 1983 "Nintendo" "Mario Bros. (PlayChoice-10)" */
	DRIVER( pc_miket ) /* 1987 "Nintendo" "Mike Tyson's Punch-Out!! (PlayChoice-10)" */
	DRIVER( pc_mman3 ) /* 1990 "Capcom USA (Nintendo of America license)" "Mega Man III (PlayChoice-10)" */
	DRIVER( pc_moglf ) /* 1991 "Nintendo" "Mario's Open Golf (PlayChoice-10)" */
	DRIVER( pc_mtoid ) /* 1986 "Nintendo" "Metroid (PlayChoice-10)" */
	DRIVER( pc_ngai2 ) /* 1990 "Tecmo (Nintendo of America license)" "Ninja Gaiden Episode II: The Dark Sword of Chaos (PlayChoice-10)" */
	DRIVER( pc_ngai3 ) /* 1991 "Tecmo (Nintendo of America license)" "Ninja Gaiden Episode III: The Ancient Ship of Doom (PlayChoice-10)" */
	DRIVER( pc_ngaid ) /* 1989 "Tecmo (Nintendo of America license)" "Ninja Gaiden (PlayChoice-10)" */
	DRIVER( pc_pwbld ) /* 1991 "Taito (Nintendo of America license)" "Power Blade (PlayChoice-10)" */
	DRIVER( pc_pwrst ) /* 1986 "Nintendo" "Pro Wrestling (PlayChoice-10)" */
	DRIVER( pc_radr2 ) /* 1990 "Square (Nintendo of America license)" "Rad Racer II (PlayChoice-10)" */
	DRIVER( pc_radrc ) /* 1987 "Square" "Rad Racer (PlayChoice-10)" */
	DRIVER( pc_rcpam ) /* 1987 "Rare" "R.C. Pro-Am (PlayChoice-10)" */
	DRIVER( pc_rkats ) /* 1991 "Atlus (Nintendo of America license)" "Rockin' Kats (PlayChoice-10)" */
	DRIVER( pc_rnatk ) /* 1987 "Konami (Nintendo of America license)" "Rush'n Attack (PlayChoice-10)" */
	DRIVER( pc_rrngr ) /* 1987 "Capcom USA (Nintendo of America license)" "Chip'n Dale: Rescue Rangers (PlayChoice-10)" */
	DRIVER( pc_rygar ) /* 1987 "Tecmo (Nintendo of America license)" "Rygar (PlayChoice-10)" */
	DRIVER( pc_sjetm ) /* 1990 "Rare" "Solar Jetman (PlayChoice-10)" */
	DRIVER( pc_smb ) /* 1985 "Nintendo" "Super Mario Bros. (PlayChoice-10)" */
	DRIVER( pc_smb2 ) /* 1988 "Nintendo" "Super Mario Bros. 2 (PlayChoice-10)" */
	DRIVER( pc_smb3 ) /* 1988 "Nintendo" "Super Mario Bros. 3 (PlayChoice-10)" */
	DRIVER( pc_suprc ) /* 1990 "Konami (Nintendo of America license)" "Super C (PlayChoice-10)" */
	DRIVER( pc_tbowl ) /* 1989 "Tecmo (Nintendo of America license)" "Tecmo Bowl (PlayChoice-10)" */
	DRIVER( pc_tenis ) /* 1983 "Nintendo" "Tennis (PlayChoice-10)" */
	DRIVER( pc_tkfld ) /* 1987 "Konami (Nintendo of America license)" "Track & Field (PlayChoice-10)" */
	DRIVER( pc_tmnt ) /* 1989 "Konami (Nintendo of America license)" "Teenage Mutant Ninja Turtles (PlayChoice-10)" */
	DRIVER( pc_tmnt2 ) /* 1990 "Konami (Nintendo of America license)" "Teenage Mutant Ninja Turtles II: The Arcade Game (PlayChoice-10)" */
	DRIVER( pc_trjan ) /* 1986 "Capcom USA (Nintendo of America license)" "Trojan (PlayChoice-10)" */
	DRIVER( pc_vball ) /* 1986 "Nintendo" "Volley Ball (PlayChoice-10)" */
	DRIVER( pc_wcup ) /* 1990 "Technos (Nintendo license)" "Nintendo World Cup (PlayChoice-10)" */
	DRIVER( pc_wgnmn ) /* 1984 "Nintendo" "Wild Gunman (PlayChoice-10)" */
	DRIVER( pc_ynoid ) /* 1990 "Capcom USA (Nintendo of America license)" "Yo! Noid (PlayChoice-10)" */
	DRIVER( pestplce ) /* 1983 "bootleg" "Pest Place" */
	DRIVER( platoon ) /* 1987 "Ocean Software Limited" "Vs. Platoon" */
	DRIVER( playch10 ) /* 1986 "Nintendo of America" "PlayChoice-10 BIOS" */
	DRIVER( popeye ) /* 1982 "Nintendo" "Popeye (revision D)" */
	DRIVER( popeyebl ) /* 1982 "bootleg" "Popeye (bootleg)" */
	DRIVER( popeyef ) /* 1982 "Nintendo" "Popeye (revision F)" */
	DRIVER( popeyeu ) /* 1982 "Nintendo" "Popeye (revision D not protected)" */
	DRIVER( punchout ) /* 1984 "Nintendo" "Punch-Out!!" */
	DRIVER( radarscp ) /* 1980 "Nintendo" "Radar Scope" */
	DRIVER( rbibb ) /* 1986 "Namco" "Vs. Atari R.B.I. Baseball (set 1)" */
	DRIVER( rbibba ) /* 1986 "Namco" "Vs. Atari R.B.I. Baseball (set 2)" */
	DRIVER( sbdk ) /* 1984 "Century Electronics" "Super Bike (DK conversion)" */
	DRIVER( sheriff ) /* 1979 "Nintendo" "Sheriff" */
	DRIVER( skyskipr ) /* 1981 "Nintendo" "Sky Skipper" */
	DRIVER( smgolf ) /* 1984 "Nintendo" "Vs. Stroke & Match Golf (Men Version)" */
	DRIVER( smgolfj ) /* 1984 "Nintendo" "Vs. Stroke & Match Golf (Men Version) (Japan)" */
	DRIVER( spacebrd ) /* 1980 "bootleg" "Space Bird (bootleg)" */
	DRIVER( spacedem ) /* 1980 "Nintendo (Fortrek license)" "Space Demon" */
	DRIVER( spacefb ) /* 1980 "Nintendo" "Space Firebird (Nintendo)" */
	DRIVER( spacefbb ) /* 1980 "bootleg" "Space Firebird (bootleg)" */
	DRIVER( spacefbg ) /* 1980 "Gremlin" "Space Firebird (Gremlin)" */
	DRIVER( spacefev ) /* 1979 "Nintendo" "Space Fever (set 1)" */
	DRIVER( spacefva ) /* 1979 "Nintendo" "Space Fever (set 2)" */
	DRIVER( spacelnc ) /* 1979 "Nintendo" "Space Launcher" */
	DRIVER( spcfrcii ) /* 1985 "Senko Industries (Magic Eletronics Inc. licence)" "Special Forces II" */
	DRIVER( spclforc ) /* 1985 "Senko Industries (Magic Eletronics Inc. licence)" "Special Forces" */
	DRIVER( spnchotj ) /* 1984 "Nintendo" "Super Punch-Out!! (Japan)" */
	DRIVER( spnchout ) /* 1984 "Nintendo" "Super Punch-Out!!" */
	DRIVER( starlstr ) /* 1985 "Namco" "Vs. Star Luster" */
	DRIVER( strtheat ) /* 1985 "Epos Corporation" "Street Heat - Cardinal Amusements" */
	DRIVER( suprmrio ) /* 1986 "Nintendo" "Vs. Super Mario Bros." */
	DRIVER( supxevs ) /* 1986 "Namco" "Vs. Super Xevious" */
	DRIVER( tkoboxng ) /* 1987 "Namco LTD." "Vs. TKO Boxing" */
	DRIVER( topgun ) /* 1987 "Konami" "Vs. Top Gun" */
	DRIVER( vsbbalja ) /* 1984 "Nintendo of America" "Vs. BaseBall (Japan set 2)" */
	DRIVER( vsbball ) /* 1984 "Nintendo of America" "Vs. BaseBall" */
	DRIVER( vsbballj ) /* 1984 "Nintendo of America" "Vs. BaseBall (Japan set 1)" */
	DRIVER( vsfdf ) /* 1988 "Sunsoft" "Vs. Freedom Force" */
	DRIVER( vsgradus ) /* 1986 "Konami" "Vs. Gradius" */
	DRIVER( vsgshoe ) /* 1986 "Nintendo" "Vs. Gumshoe" */
	DRIVER( vsmahjng ) /* 1984 "Nintendo" "Vs. Mahjang (Japan)" */
	DRIVER( vspinbal ) /* 1984 "Nintendo" "Vs. Pinball" */
	DRIVER( vspinblj ) /* 1984 "Nintendo" "Vs. Pinball (Japan)" */
	DRIVER( vsskykid ) /* 1985 "Namco" "Vs. Super SkyKid" */
	DRIVER( vsslalom ) /* 1986 "Rare LTD." "Vs. Slalom" */
	DRIVER( vssoccer ) /* 1985 "Nintendo" "Vs. Soccer" */
	DRIVER( vstennij ) /* 1984 "Nintendo" "Vs. Tennis (Japan)" */
	DRIVER( vstennis ) /* 1984 "Nintendo" "Vs. Tennis" */
	DRIVER( vstetris ) /* 1987 "Academysoft-Elory" "Vs. Tetris" */
	DRIVER( wrecking ) /* 1984 "Nintendo" "Vs. Wrecking Crew" */
#endif
#ifdef LINK_NIX
	DRIVER( bbprot ) /* 199? "<unknown>" "Untitled Fighter 'BB' (prototype)" */
	DRIVER( fitfight ) /* 199? "bootleg" "Fit of Fighting" */
	DRIVER( genix ) /* 1994 "NIX" "Genix Family" */
	DRIVER( histryma ) /* 199? "bootleg" "The History of Martial Arts" */
	DRIVER( pirates ) /* 1994 "NIX" "Pirates" */
#endif
#ifdef LINK_NMK
	DRIVER( acrobatm ) /* 1991 "UPL (Taito license)" "Acrobat Mission" */
	DRIVER( airattck ) /* 1996 "Comad" "Air Attack" */
	DRIVER( bigbang ) /* 1993 "NMK" "Big Bang" */
	DRIVER( bioship ) /* 1990 "UPL (American Sammy license)" "Bio-ship Paladin" */
	DRIVER( bjtwin ) /* 1993 "NMK" "Bombjack Twin" */
	DRIVER( blkhearj ) /* 1991 "UPL" "Black Heart (Japan)" */
	DRIVER( blkheart ) /* 1991 "UPL" "Black Heart" */
	DRIVER( gakupara ) /* 1991 "NMK" "Quiz Gakuen Paradise (Japan)" */
	DRIVER( gekiretu ) /* 1992 "Face" "Quiz Gekiretsu Scramble (Japan)" */
	DRIVER( gunnail ) /* 1993 "NMK / Tecmo" "GunNail" */
	DRIVER( hachamf ) /* 1991 "NMK" "Hacha Mecha Fighter" */
	DRIVER( kakumei ) /* 1990 "Jaleco" "Mahjong Kakumei" */
	DRIVER( kakumei2 ) /* 1992 "Jaleco" "Mahjong Kakumei 2 - Princess League" */
	DRIVER( macross ) /* 1992 "Banpresto" "Super Spacefortress Macross / Chou-Jikuu Yousai Macross" */
	DRIVER( macross2 ) /* 1993 "Banpresto" "Super Spacefortress Macross II / Chou-Jikuu Yousai Macross II" */
	DRIVER( macrossp ) /* 1996 "Banpresto" "Macross Plus" */
	DRIVER( manybloc ) /* 1991 "Bee-Oh" "Many Block" */
	DRIVER( mjzoomin ) /* 1990 "Jaleco" "Mahjong Channel Zoom In" */
	DRIVER( mustang ) /* 1990 "UPL" "US AAF Mustang (Japan)" */
	DRIVER( mustangb ) /* 1990 "bootleg" "US AAF Mustang (bootleg)" */
	DRIVER( mustangs ) /* 1990 "UPL (Seoul Trading license)" "US AAF Mustang (Seoul Trading)" */
	DRIVER( nouryoku ) /* 1995 "Tecmo" "Nouryoku Koujou Iinkai" */
	DRIVER( quizdna ) /* 1992 "Face" "Quiz DNA no Hanran (Japan)" */
	DRIVER( quizmoon ) /* 1997 "Banpresto" "Quiz Bisyoujo Senshi Sailor Moon - Chiryoku Tairyoku Toki no Un" */
	DRIVER( quizpani ) /* 1993 "NMK" "Quiz Panicuru Fantasy" */
	DRIVER( raphero ) /* 1994 "Media Trading Corp" "Rapid Hero (Japan?)" */
	DRIVER( sabotenb ) /* 1992 "NMK / Tecmo" "Saboten Bombers (set 1)" */
	DRIVER( sabotnba ) /* 1992 "NMK / Tecmo" "Saboten Bombers (set 2)" */
	DRIVER( ssmissin ) /* 1992 "Comad" "S.S. Mission" */
	DRIVER( strahl ) /* 1992 "UPL" "Koutetsu Yousai Strahl (Japan set 1)" */
	DRIVER( strahla ) /* 1992 "UPL" "Koutetsu Yousai Strahl (Japan set 2)" */
	DRIVER( suchipi ) /* 1993 "Jaleco" "Idol Janshi Su-Chi-Pie Special" */
	DRIVER( tdragon ) /* 1991 "NMK (Tecmo license)" "Thunder Dragon" */
	DRIVER( tdragon2 ) /* 1993 "NMK" "Thunder Dragon 2" */
	DRIVER( tdragonb ) /* 1991 "NMK / Tecmo" "Thunder Dragon (Bootleg)" */
	DRIVER( vandyja2 ) /* 1990 "UPL (Jaleco license)" "Vandyke (Jaleco */
	DRIVER( vandyjal ) /* 1990 "UPL (Jaleco license)" "Vandyke (Jaleco */
	DRIVER( vandyke ) /* 1990 "UPL" "Vandyke (Japan)" */
#endif
#ifdef LINK_OLYMPIA
	DRIVER( dday ) /* 1982 "Olympia" "D-Day" */
	DRIVER( ddayc ) /* 1982 "Olympia (Centuri license)" "D-Day (Centuri)" */
	DRIVER( portrait ) /* 1983 "Olympia" "Portraits (set 1)" */
	DRIVER( portrata ) /* 1983 "Olympia" "Portraits (set 2)" */
#endif
#ifdef LINK_OMORI
	DRIVER( battlex ) /* 1982 "Omori Electric" "Battle Cross" */
	DRIVER( carjmbre ) /* 1983 "Omori Electric Co. Ltd." */
	DRIVER( popper ) /* 1983 "Omori Electric Co. Ltd." */
	DRIVER( spaceg ) /* 1979 "Omori" "Space Guerilla" */
#endif
#ifdef LINK_ORCA
	DRIVER( bcruzm12 ) /* 1983 "Sigma Enterprises Inc." "Battle Cruiser M-12" */
	DRIVER( bounty ) /* 1982 "Orca" "The Bounty" */
	DRIVER( changes ) /* 1982 "Orca" "Changes" */
	DRIVER( changesa ) /* 1982 "Orca (Eastern Micro Electronics Inc. license)" */
	DRIVER( dogfight ) /* 1983 "[Orca] Thunderbolt" "Dog Fight (Thunderbolt)" */
	DRIVER( espial ) /* 1983 "[Orca] Thunderbolt" "Espial (Europe)" */
	DRIVER( espialu ) /* 1983 "[Orca] Thunderbolt" "Espial (US?)" */
	DRIVER( funkybee ) /* 1982 "Orca" "Funky Bee" */
	DRIVER( hoccer ) /* 1983 "Eastern Micro Electronics Inc." */
	DRIVER( hoccer2 ) /* 1983 "Eastern Micro Electronics Inc." */
	DRIVER( hopprobo ) /* 1983 "Sega" "Hopper Robo" */
	DRIVER( looper ) /* 1982 "Orca" "Looper" */
	DRIVER( marineb ) /* 1982 "Orca" "Marine Boy" */
	DRIVER( moguchan ) /* 1982 "Orca (Eastern Commerce Inc. license) (bootleg?)" /* this is in the ROM at $0b5c */ "Moguchan" */
	DRIVER( netwars ) /* 1983 "Orca (Esco Trading Co license)" "Net Wars" */
	DRIVER( percuss ) /* 1981 "Orca" "The Percussor" */
	DRIVER( skylance ) /* 1983 "Orca (Esco Trading Co license)" "Sky Lancer (Esco Trading Co license)" */
	DRIVER( skylancr ) /* 1983 "Orca" "Sky Lancer" */
	DRIVER( springer ) /* 1982 "Orca" "Springer" */
	DRIVER( vastar ) /* 1983 "Sesame Japan" "Vastar (set 1)" */
	DRIVER( vastar2 ) /* 1983 "Sesame Japan" "Vastar (set 2)" */
	DRIVER( wanted ) /* 1984 "Sigma Enterprises Inc." "Wanted" */
	DRIVER( zodiack ) /* 1983 "Orca (Esco Trading Co)" "Zodiack" */
#endif
#ifdef LINK_PACIFIC
	DRIVER( mrflea ) /* 1982 "Pacific Novelty" "The Amazing Adventures of Mr. F. Lea" */
	DRIVER( natodef ) /* 1982 "Pacific Novelty" "NATO Defense" */
	DRIVER( natodefa ) /* 1982 "Pacific Novelty" "NATO Defense (alternate mazes)" */
	DRIVER( sharkatt ) /* 1980 "Pacific Novelty" "Shark Attack" */
	DRIVER( thief ) /* 1981 "Pacific Novelty" "Thief" */
#endif
#ifdef LINK_PACMAN
	DRIVER( 8bpm ) /* 1985 "Seatongrove Ltd (Magic Eletronics USA licence)" "Eight Ball Action (Pac-Man conversion)" */
	DRIVER( abscam ) /* 1981 "GL (US Billiards License)" "Abscam" */
	DRIVER( acitya ) /* 1983 "Epos Corporation" "Atlantic City Action" */
	DRIVER( alibaba ) /* 1982 "Sega" "Ali Baba and 40 Thieves" */
	DRIVER( beastf ) /* 1984 "Epos Corporation" "Beastie Feastie" */
	DRIVER( bigbucks ) /* 1986 "Dynasoft Inc." "Big Bucks" */
	DRIVER( bwcasino ) /* 1983 "Epos Corporation" "Boardwalk Casino" */
	DRIVER( crush ) /* 1981 "Kural Samno Electric" "Crush Roller (Kural Samno)" */
	DRIVER( crush2 ) /* 1981 "Kural Esco Electric" "Crush Roller (Kural Esco - bootleg?)" */
	DRIVER( crush3 ) /* 1981 "Kural Electric" "Crush Roller (Kural - bootleg?)" */
	DRIVER( crush4 ) /* 19?? "Kural TWT" "Crush Roller (Kural TWT)" */
	DRIVER( ctrpllrp ) /* 1982 "hack" "Caterpillar Pacman Hack" */
	DRIVER( dremshpr ) /* 1982 "Sanritsu" "Dream Shopper" */
	DRIVER( drivfrcp ) /* 1984 "Shinkai Inc. (Magic Eletronics Inc. licence)" "Driving Force (Pac-Man conversion)" */
	DRIVER( eggor ) /* 1983 "Telko" "Eggor" */
	DRIVER( eyes ) /* 1982 "Digitrex Techstar (Rock-ola license)" "Eyes (Digitrex Techstar)" */
	DRIVER( eyes2 ) /* 1982 "Techstar (Rock-ola license)" "Eyes (Techstar)" */
	DRIVER( eyeszac ) /* 1982 "Zaccaria / bootleg" "Eyes (Zaccaria)" */
	DRIVER( gorkans ) /* 1983 "Techstar" "Gorkans" */
	DRIVER( hangly ) /* 1981 "hack" "Hangly-Man (set 1)" */
	DRIVER( hangly2 ) /* 1981 "hack" "Hangly-Man (set 2)" */
	DRIVER( hangly3 ) /* 1981 "hack" "Hangly-Man (set 3)" */
	DRIVER( joyman ) /* 1982 "hack" "Joyman" */
	DRIVER( jrpacman ) /* 1983 "Bally Midway" "Jr. Pac-Man" */
	DRIVER( jrpacmbl ) /* 1983 "bootleg" "Jr. Pac-Man (Pengo hardware)" */
	DRIVER( jumpshot ) /* 1985 "Bally Midway" "Jump Shot" */
	DRIVER( jumpshtp ) /* 1985 "Bally Midway" "Jump Shot Engineering Sample" */
	DRIVER( korosuke ) /* 1981 "Kural Electric" "Korosuke Roller" */
	DRIVER( lizwiz ) /* 1985 "Techstar (Sunn license)" "Lizard Wizard" */
	DRIVER( maketrax ) /* 1981 "[Kural] (Williams license)" "Make Trax (set 1)" */
	DRIVER( maketrxb ) /* 1981 "[Kural] (Williams license)" "Make Trax (set 2)" */
	DRIVER( mbrush ) /* 1981 "bootleg" "Magic Brush" */
	DRIVER( mrtnt ) /* 1983 "Telko" "Mr. TNT" */
	DRIVER( mschamp ) /* 1995 "hack" "Ms. Pacman Champion Edition / Super Zola Pac Gal" */
	DRIVER( mspacmab ) /* 1981 "bootleg" "Ms. Pac-Man (bootleg)" */
	DRIVER( mspacman ) /* 1981 "Midway" "Ms. Pac-Man" */
	DRIVER( mspacmat ) /* 1981 "hack" "Ms. Pac Attack" */
	DRIVER( mspacmbe ) /* 1981 "bootleg" "Ms. Pac-Man (bootleg */
	DRIVER( mspacmnf ) /* 1981 "Midway" "Ms. Pac-Man (with speedup hack)" */
	DRIVER( mspacpls ) /* 1981 "hack" "Ms. Pac-Man Plus" */
	DRIVER( newpuc2 ) /* 1980 "hack" "Newpuc2" */
	DRIVER( newpuc2b ) /* 1980 "hack" "Newpuc2 (set 2)" */
	DRIVER( newpuckx ) /* 1980 "hack" "New Puck-X" */
	DRIVER( nmouse ) /* 1981 "Amenip (Palcom Queen River)" "Naughty Mouse (set 1)" */
	DRIVER( nmouseb ) /* 1981 "Amenip Nova Games Ltd." "Naughty Mouse (set 2)" */
	DRIVER( pacgal ) /* 1981 "hack" "Pac-Gal" */
	DRIVER( pacheart ) /* 1981 "hack" "Pac-Man (Hearts)" */
	DRIVER( pacman ) /* 1980 "[Namco] (Midway license)" "Pac-Man (Midway)" */
	DRIVER( pacmanf ) /* 1980 "[Namco] (Midway license)" "Pac-Man (Midway */
	DRIVER( pacmod ) /* 1981 "[Namco] (Midway license)" "Pac-Man (Midway */
	DRIVER( pacplus ) /* 1982 "[Namco] (Midway license)" "Pac-Man Plus" */
	DRIVER( paintrlr ) /* 1981 "bootleg" "Paint Roller" */
	DRIVER( pengo ) /* 1982 "Sega" "Pengo (set 1 rev c)" */
	DRIVER( pengo2 ) /* 1982 "Sega" "Pengo (set 2)" */
	DRIVER( pengo2u ) /* 1982 "Sega" "Pengo (set 2 not encrypted)" */
	DRIVER( pengo3u ) /* 1982 "Sega" "Pengo (set 3 not encrypted)" */
	DRIVER( pengo4 ) /* 1982 "Sega" "Pengo (set 4)" */
	DRIVER( pengob ) /* 1982 "bootleg" "Pengo (bootleg)" */
	DRIVER( penta ) /* 1982 "bootleg" "Penta" */
	DRIVER( piranha ) /* 1981 "GL (US Billiards License)" "Piranha" */
	DRIVER( piranhah ) /* 1981 "hack" "Piranha (hack)" */
	DRIVER( piranhao ) /* 1981 "GL (US Billiards License)" "Piranha (older)" */
	DRIVER( ponpoko ) /* 1982 "Sigma Enterprises Inc." "Ponpoko" */
	DRIVER( ponpokov ) /* 1982 "Sigma Enterprises Inc. (Venture Line license)" "Ponpoko (Venture Line)" */
	DRIVER( porky ) /* 1985 "Shinkai Inc. (Magic Eletronics Inc. licence)" "Porky" */
	DRIVER( puckman ) /* 1980 "Namco" "PuckMan (Japan set 1 */
	DRIVER( puckmana ) /* 1980 "Namco" "PuckMan (Japan set 2)" */
	DRIVER( puckmanf ) /* 1980 "Namco" "PuckMan (Japan set 1 with speedup hack)" */
	DRIVER( puckmanh ) /* 1980 "hack" "Puckman (Falcom?)" */
	DRIVER( puckmod ) /* 1981 "Namco" "PuckMan (Japan set 3)" */
	DRIVER( rocktrv2 ) /* 1986 "Triumph Software Inc." "MTV Rock-N-Roll Trivia (Part 2)" */
	DRIVER( shootbul ) /* 1985 "Bally Midway" "Shoot the Bull" */
	DRIVER( sprglbpg ) /* 1983 "Bootleg" "Super Glob (Pac-Man hardware) German" */
	DRIVER( sprglobp ) /* 1983 "Epos Corporation" "Super Glob (Pac-Man hardware)" */
	DRIVER( theglobp ) /* 1983 "Epos Corporation" "The Glob (Pac-Man hardware)" */
	DRIVER( vanvan ) /* 1983 "Sanritsu" "Van-Van Car" */
	DRIVER( vanvanb ) /* 1983 "Karateco" "Van-Van Car (set 3)" */
	DRIVER( vanvank ) /* 1983 "Karateco" "Van-Van Car (Karateco)" */
	DRIVER( woodpek ) /* 1981 "Amenip (Palcom Queen River)" "Woodpecker (set 1)" */
#endif
#ifdef LINK_PHOENIX
	DRIVER( capitol ) /* 1981 "Universal Video Spiel" "Capitol" */
	DRIVER( condor ) /* 1981 "Sidam" "Condor" */
	DRIVER( falcon ) /* 1980 "bootleg" "Falcon (bootleg set 1)" */
	DRIVER( falcona ) /* 1980 "bootleg" "Falcon (bootleg set 2)" */
	DRIVER( griffon ) /* 1980 "Videotron" "Griffon" */
	DRIVER( naughtya ) /* 1982 "bootleg" "Naughty Boy (bootleg)" */
	DRIVER( naughtyb ) /* 1982 "Jaleco" "Naughty Boy" */
	DRIVER( naughtyc ) /* 1982 "Jaleco (Cinematronics license)" "Naughty Boy (Cinematronics)" */
	DRIVER( phoenix ) /* 1980 "Amstar" "Phoenix (Amstar)" */
	DRIVER( phoenix3 ) /* 1980 "bootleg" "Phoenix (T.P.N.)" */
	DRIVER( phoenixa ) /* 1980 "Amstar (Centuri license)" "Phoenix (Centuri)" */
	DRIVER( phoenixc ) /* 1981 "bootleg?" "Phoenix (IRECSA */
	DRIVER( phoenixt ) /* 1980 "Taito" "Phoenix (Taito)" */
	DRIVER( pleiadbl ) /* 1981 "bootleg" "Pleiads (bootleg)" */
	DRIVER( pleiadce ) /* 1981 "Tehkan (Centuri license)" "Pleiads (Centuri)" */
	DRIVER( pleiads ) /* 1981 "Tehkan" "Pleiads (Tehkan)" */
	DRIVER( popflama ) /* 1982 "Jaleco" "Pop Flamer (not protected)" */
	DRIVER( popflamb ) /* 1982 "Jaleco" "Pop Flamer (hack?)" */
	DRIVER( popflame ) /* 1982 "Jaleco" "Pop Flamer (protected)" */
	DRIVER( safarir ) /* 1979 "SNK" "Safari Rally (Japan)" */
	DRIVER( trvgns ) /* 198? "Enerdyne Technologies Inc." "Trivia Genius" */
	DRIVER( trvmstr ) /* 1985 "Enerdyne Technologies Inc." "Trivia Master (set 1)" */
	DRIVER( trvmstra ) /* 1985 "Enerdyne Technologies Inc." "Trivia Master (set 2)" */
	DRIVER( vautour ) /* 1980 "bootleg" "Vautour (set 1 - Jeutel France)" */
	DRIVER( vautour2 ) /* 1980 "bootleg" "Vautour (set 2)" */
#endif
#ifdef LINK_PLAYMARK
	DRIVER( bigtwin ) /* 1995 "Playmark" "Big Twin" */
	DRIVER( drtomy ) /* 1993 "Playmark" "Dr. Tomy" */
	DRIVER( excelsr ) /* 1996 "Playmark" "Excelsior" */
	DRIVER( hotmind ) /* 1995 "Playmark" "Hot Mind" */
	DRIVER( magicstk ) /* 1995 "Playmark" "Magic Sticks" */
	DRIVER( powerbal ) /* 1994 "Playmark" "Power Balls" */
	DRIVER( powerbls ) /* 1994 "Playmark" "Power Balls (Super Slam conversion)" */
	DRIVER( sderby ) /* 1996 "Playmark" "Super Derby" */
	DRIVER( sslam ) /* 1993 "Playmark" "Super Slam (set 1)" */
	DRIVER( sslama ) /* 1993 "Playmark" "Super Slam (set 2)" */
	DRIVER( wbeachv2 ) /* 1995 "Playmark" "World Beach Volley (set 2)" */
	DRIVER( wbeachvl ) /* 1995 "Playmark" "World Beach Volley (set 1)" */
#endif
#ifdef LINK_PSIKYO
	DRIVER( btlkroad ) /* 1994 "Psikyo" "Battle K-Road" */
	DRIVER( daraku ) /* 1998 "Psikyo" "Daraku Tenshi - The Fallen Angels" */
	DRIVER( dragnblz ) /* 2000 "Psikyo" "Dragon Blaze" */
	DRIVER( gnbarich ) /* 2001 "Psikyo" "Gunbarich" */
	DRIVER( gunbird ) /* 1994 "Psikyo" "Gunbird (World)" */
	DRIVER( gunbird2 ) /* 1998 "Psikyo" "Gunbird 2" */
	DRIVER( gunbirdj ) /* 1994 "Psikyo" "Gunbird (Japan)" */
	DRIVER( gunbirdk ) /* 1994 "Psikyo" "Gunbird (Korea)" */
	DRIVER( hgkairak ) /* 1998 "Psikyo" "Taisen Hot Gimmick Kairakuten (Japan)" */
	DRIVER( hotdebut ) /* 2000 "Psikyo / Moss" "Quiz de Idol! Hot Debut (Japan)" */
	DRIVER( hotgmck ) /* 1997 "Psikyo" "Taisen Hot Gimmick (Japan)" */
	DRIVER( hotgmck3 ) /* 1999 "Psikyo" "Taisen Hot Gimmick 3 Digital Surfing (Japan)" */
	DRIVER( loderdfa ) /* 2000 "Psikyo" "Lode Runner - The Dig Fight (ver. A)" */
	DRIVER( loderndf ) /* 2000 "Psikyo" "Lode Runner - The Dig Fight (ver. B)" */
	DRIVER( s1945 ) /* 1995 "Psikyo" "Strikers 1945" */
	DRIVER( s1945a ) /* 1995 "Psikyo" "Strikers 1945 (Alt)" */
	DRIVER( s1945ii ) /* 1997 "Psikyo" "Strikers 1945 II" */
	DRIVER( s1945iii ) /* 1999 "Psikyo" "Strikers 1945 III (World) / Strikers 1999 (Japan)" */
	DRIVER( s1945j ) /* 1995 "Psikyo" "Strikers 1945 (Japan)" */
	DRIVER( s1945jn ) /* 1995 "Psikyo" "Strikers 1945 (Japan */
	DRIVER( samuraia ) /* 1993 "Psikyo" "Samurai Aces (World)" */
	DRIVER( sbomberb ) /* 1998 "Psikyo" "Space Bomber (ver. B)" */
	DRIVER( sngkace ) /* 1993 "Psikyo" "Sengoku Ace (Japan)" */
	DRIVER( soldivid ) /* 1997 "Psikyo" "Sol Divide - The Sword Of Darkness" */
	DRIVER( tengai ) /* 1996 "Psikyo" "Tengai / Sengoku Blade: Sengoku Ace Episode II" */
#endif
#ifdef LINK_RAMTEK
	DRIVER( barricad ) /* 1976 "RamTek" "Barricade" */
	DRIVER( brickyrd ) /* 1976 "RamTek" "Brickyard" */
	DRIVER( hitme ) /* 1976 "RamTek" "Hit Me" */
	DRIVER( mblkjack ) /* 197? "Mirco" "Black Jack (Mirco)" */
	DRIVER( starcrus ) /* 1977 "RamTek" "Star Cruiser" */
#endif
#ifdef LINK_RARE
	DRIVER( btoads ) /* 1994 "Rare" "Battle Toads" */
	DRIVER( kinst ) /* 1994 "Rare" "Killer Instinct (v1.5d)" */
	DRIVER( kinst13 ) /* 1994 "Rare" "Killer Instinct (v1.3)" */
	DRIVER( kinst14 ) /* 1994 "Rare" "Killer Instinct (v1.4)" */
	DRIVER( kinst2 ) /* 1995 "Rare" "Killer Instinct 2 (v1.4)" */
	DRIVER( kinst210 ) /* 1995 "Rare" "Killer Instinct 2 (v1.0)" */
	DRIVER( kinst211 ) /* 1995 "Rare" "Killer Instinct 2 (v1.1)" */
	DRIVER( kinst213 ) /* 1995 "Rare" "Killer Instinct 2 (v1.3)" */
	DRIVER( kinst2k ) /* 1995 "Rare" "Killer Instinct 2 (v1.4k */
	DRIVER( kinstp ) /* 1994 "Rare" "Killer Instinct (proto v4.7)" */
	DRIVER( xtheball ) /* 1991 "Rare" "X the Ball" */
#endif
#ifdef LINK_SANRITSU
	DRIVER( appoooh ) /* 1984 "[Sanritsu] Sega" "Appoooh" */
	DRIVER( bankp ) /* 1984 "[Sanritsu] Sega" "Bank Panic" */
	DRIVER( drmicro ) /* 1983 "Sanritsu" "Dr. Micro" */
	DRIVER( mayumi ) /* 1988 "[Sanritsu] Victory L.L.C." "Kikiippatsu Mayumi-chan (Japan)" */
	DRIVER( mjkjidai ) /* 1986 "Sanritsu" "Mahjong Kyou Jidai (Japan)" */
	DRIVER( robowrb ) /* 1986 "bootleg" "Robo Wres 2001 (bootleg)" */
	DRIVER( robowres ) /* 1986 "Sega" "Robo Wres 2001" */
	DRIVER( rougien ) /* 1982 "Sanritsu" "Rougien" */
#endif
#ifdef LINK_SEGA
	DRIVER( 005 ) /* 1981 "Sega" "005" */
	DRIVER( 4dwarrio ) /* 1985 "Coreland / Sega" "4-D Warriors" */
	DRIVER( abcop ) /* 1990 "Sega" "A.B. Cop (FD1094 317-0169b)" */
	DRIVER( aburner ) /* 1987 "Sega" "After Burner (Japan)" */
	DRIVER( aburner2 ) /* 1987 "Sega" "After Burner II" */
	DRIVER( afighter ) /* 1986 "Sega" "Action Fighter */
	DRIVER( alexkid1 ) /* 1986 "Sega" "Alex Kidd: The Lost Stars (set 1 */
	DRIVER( alexkidd ) /* 1986 "Sega" "Alex Kidd: The Lost Stars (set 2 */
	DRIVER( alien3 ) /* 1993 "Sega" "Alien3: The Gun (World)" */
	DRIVER( alien3u ) /* 1993 "Sega" "Alien3: The Gun (US)" */
	DRIVER( aliensy1 ) /* 1987 "Sega" "Alien Syndrome (set 1 */
	DRIVER( aliensy2 ) /* 1987 "Sega" "Alien Syndrome (set 2 */
	DRIVER( aliensy3 ) /* 1987 "Sega" "Alien Syndrome (set 3 */
	DRIVER( aliensyn ) /* 1987 "Sega" "Alien Syndrome (set 4 */
	DRIVER( alphaho ) /* 19?? "Data East Corporation" "Alpha Fighter / Head On" */
	DRIVER( altbeaj1 ) /* 1988 "Sega" "Juuouki (set 1 */
	DRIVER( altbeaj3 ) /* 1988 "Sega" "Juuouki (set 3 */
	DRIVER( altbeas2 ) /* 1988 "Sega" "Altered Beast (set 2 */
	DRIVER( altbeas4 ) /* 1988 "Sega" "Altered Beast (set 4 */
	DRIVER( altbeas5 ) /* 1988 "Sega" "Altered Beast (set 5 */
	DRIVER( altbeasj ) /* 1988 "Sega" "Juuouki (set 6 */
	DRIVER( altbeast ) /* 1988 "Sega" "Altered Beast (set 7 */
	DRIVER( angelkds ) /* 1988 "Sega / Nasco?" "Angel Kids (Japan)" */
	DRIVER( arabfgt ) /* 1992 "Sega" "Arabian Fight (World)" */
	DRIVER( arabfgtj ) /* 1992 "Sega" "Arabian Fight (Japan)" */
	DRIVER( arabfgtu ) /* 1992 "Sega" "Arabian Fight (US)" */
	DRIVER( arescue ) /* 1992 "Sega" "Air Rescue" */
	DRIVER( astorm ) /* 1990 "Sega" "Alien Storm (set 4 */
	DRIVER( astorm3 ) /* 1990 "Sega" "Alien Storm (set 3 */
	DRIVER( astormbl ) /* 1990 "bootleg" "Alien Storm (bootleg)" */
	DRIVER( astormj ) /* 1990 "Sega" "Alien Storm (set 1 */
	DRIVER( astormu ) /* 1990 "Sega" "Alien Storm (set 2 */
	DRIVER( astrass ) /* 1998 "Sunsoft" "Astra SuperStars (J 980514 V1.002)" */
	DRIVER( astrob ) /* 1981 "Sega" "Astro Blaster (version 3)" */
	DRIVER( astrob2 ) /* 1981 "Sega" "Astro Blaster (version 2)" */
	DRIVER( astrob2a ) /* 1981 "Sega" "Astro Blaster (version 2a)" */
	DRIVER( astrofl ) /* 1986 "Sega" "Astro Flash (Japan)" */
	DRIVER( atomicp ) /* 1990 "Philko" "Atomic Point (Korea)" */
	DRIVER( aurail ) /* 1990 "Sega / Westone" "Aurail (set 3 */
	DRIVER( aurail1 ) /* 1990 "Sega / Westone" "Aurail (set 2 */
	DRIVER( aurailj ) /* 1990 "Sega / Westone" "Aurail (set 1 */
	DRIVER( bakubaku ) /* 1996 "Sega" "Baku Baku Animal (J 950407 V1.000)" */
	DRIVER( batmanfr ) /* 1996 "Acclaim" "Batman Forever (JUE 960507 V1.000)" */
	DRIVER( bayrout1 ) /* 1989 "Sunsoft / Sega" "Bay Route (set 1 */
	DRIVER( bayroute ) /* 1989 "Sunsoft / Sega" "Bay Route (set 3 */
	DRIVER( bayroutj ) /* 1989 "Sunsoft / Sega" "Bay Route (set 2 */
	DRIVER( blasto ) /* 1978 "Gremlin" "Blasto" */
	DRIVER( blckgalb ) /* 1987 "bootleg" "Block Gal (bootleg)" */
	DRIVER( blockade ) /* 1976 "Gremlin" "Blockade" */
	DRIVER( blockgal ) /* 1987 "Sega / Vic Tokai" "Block Gal" */
	DRIVER( bloxeed ) /* 1990 "Sega" "Bloxeed (Japan */
	DRIVER( bloxeedc ) /* 1989 "Sega / Elorg" "Bloxeed (World */
	DRIVER( bloxeedu ) /* 1989 "Sega / Elorg" "Bloxeed (US */
	DRIVER( bnzabros ) /* 1990 "Sega" "Bonanza Bros (US */
	DRIVER( bnzabrsj ) /* 1990 "Sega" "Bonanza Bros (Japan */
	DRIVER( bodyslam ) /* 1986 "Sega" "Body Slam (8751 317-unknown)" */
	DRIVER( borench ) /* 1990 "Sega" "Borench" */
	DRIVER( brain ) /* 1986 "Coreland / Sega" "Brain" */
	DRIVER( brdrlinb ) /* 1981 "bootleg" "Borderline (bootleg)" */
	DRIVER( brdrline ) /* 1981 "Sega" "Borderline" */
	DRIVER( brival ) /* 1992 "Sega" "Burning Rival (World)" */
	DRIVER( brivalj ) /* 1992 "Sega" "Burning Rival (Japan)" */
	DRIVER( buckrog ) /* 1982 "Sega" "Buck Rogers: Planet of Zoom" */
	DRIVER( buckrogn ) /* 1982 "Sega" "Buck Rogers: Planet of Zoom (not encrypted)" */
	DRIVER( bullfgt ) /* 1984 "Coreland / Sega" "Bullfight" */
	DRIVER( calorie ) /* 1986 "Sega" "Calorie Kun vs Moguranian" */
	DRIVER( calorieb ) /* 1986 "bootleg" "Calorie Kun vs Moguranian (bootleg)" */
	DRIVER( carnival ) /* 1980 "Sega" "Carnival (upright)" */
	DRIVER( carnvckt ) /* 1980 "Sega" "Carnival (cocktail)" */
	DRIVER( chboxing ) /* 1984 "Sega" "Champion Boxing" */
	DRIVER( chplft ) /* 1985 "Sega" "Choplifter" */
	DRIVER( chplftb ) /* 1985 "Sega" "Choplifter (alternate)" */
	DRIVER( chplftbl ) /* 1985 "bootleg" "Choplifter (bootleg)" */
	DRIVER( chwrestl ) /* 1985 "Sega" "Champion Pro Wrestling" */
	DRIVER( cltchitr ) /* 1991 "Sega" "Clutch Hitter (set 2 */
	DRIVER( cltchtrj ) /* 1991 "Sega" "Clutch Hitter (set 1 */
	DRIVER( colmns97 ) /* 1996 "Sega" "Columns '97 (JET 961209 V1.000)" */
	DRIVER( column2j ) /* 1990 "Sega" "Columns II: The Voyage Through Time (Japan)" */
	DRIVER( columns ) /* 1990 "Sega" "Columns (World)" */
	DRIVER( columns2 ) /* 1990 "Sega" "Columns II: The Voyage Through Time (World)" */
	DRIVER( columnsj ) /* 1990 "Sega" "Columns (Japan)" */
	DRIVER( columnsu ) /* 1990 "Sega" "Columns (US */
	DRIVER( comotion ) /* 1976 "Gremlin" "Comotion" */
	DRIVER( congo ) /* 1983 "Sega" "Congo Bongo" */
	DRIVER( cotton ) /* 1991 "Sega / Success" "Cotton (set 3 */
	DRIVER( cotton2 ) /* 1997 "Success" "Cotton 2 (JUET 970902 V1.000)" */
	DRIVER( cottonbm ) /* 1998 "Success" "Cotton Boomerang (JUET 980709 V1.000)" */
	DRIVER( cottonj ) /* 1991 "Sega / Success" "Cotton (set 1 */
	DRIVER( cottonu ) /* 1991 "Sega / Success" "Cotton (set 2 */
	DRIVER( crkdown ) /* 1989 "Sega" "Crack Down (US */
	DRIVER( crkdownj ) /* 1989 "Sega" "Crack Down (Japan */
	DRIVER( croquis ) /* 1996 "Deniam" "Croquis (Germany)" */
	DRIVER( danchih ) /* 1999 "Altron (Tecmo license)" "Danchi de Hanafuoda (J 990607 V1.400)" */
	DRIVER( darkedge ) /* 1992 "Sega" "Dark Edge (World)" */
	DRIVER( darkedgj ) /* 1992 "Sega" "Dark Edge (Japan)" */
	DRIVER( dbzvrvs ) /* 1994 "Sega / Banpresto" "Dragon Ball Z V.R.V.S." */
	DRIVER( dcclub ) /* 1991 "Sega" "Dynamic Country Club" */
	DRIVER( dcclubfd ) /* 1991 "Sega" "Dynamic Country Club (Floppy DS3-5000-09d */
	DRIVER( ddcrew ) /* 1991 "Sega" "D. D. Crew (set 4 */
	DRIVER( ddcrew1 ) /* 1991 "Sega" "D. D. Crew (set 1 */
	DRIVER( ddcrew2 ) /* 1991 "Sega" "D. D. Crew (set 2 */
	DRIVER( ddcrewj ) /* 1991 "Sega" "D. D. Crew (set 5 */
	DRIVER( ddcrewu ) /* 1991 "Sega" "D. D. Crew (set 3 */
	DRIVER( ddux ) /* 1989 "Sega" "Dynamite Dux (set 2 */
	DRIVER( ddux1 ) /* 1989 "Sega" "Dynamite Dux (set 1 */
	DRIVER( dduxbl ) /* 1989 "bootleg" "Dynamite Dux (bootleg)" */
	DRIVER( defense ) /* 1987 "Sega" "Defense (System 16B */
	DRIVER( depthch ) /* 1977 "Gremlin" "Depthcharge" */
	DRIVER( depthv1 ) /* 1977 "Gremlin" "Depthcharge (older)" */
	DRIVER( desertbr ) /* 1992 "Sega" "Desert Breaker (FD1094 317-0196)" */
	DRIVER( diehard ) /* 1996 "Sega" "Die Hard Arcade (UET 960515 V1.000)" */
	DRIVER( digger ) /* 1980 "Sega" "Digger" */
	DRIVER( dnmtdeka ) /* 1996 "Sega" "Dynamite Deka (J 960515 V1.000)" */
	DRIVER( dotriku2 ) /* 1990 "Sega" "Dottori Kun (old version)" */
	DRIVER( dotrikun ) /* 1990 "Sega" "Dottori Kun (new version)" */
	DRIVER( dumpmtmt ) /* 1986 "Sega" "Dump Matsumoto (Japan */
	DRIVER( dunkshot ) /* 1986 "Sega" "Dunk Shot (FD1089 317-0022)" */
	DRIVER( ejihon ) /* 1995 "Sega" "Ejihon Tantei Jimusyo (J 950613 V1.000)" */
	DRIVER( elim2 ) /* 1981 "Gremlin" "Eliminator (2 Players */
	DRIVER( elim2a ) /* 1981 "Gremlin" "Eliminator (2 Players */
	DRIVER( elim2c ) /* 1981 "Gremlin" "Eliminator (2 Players */
	DRIVER( elim4 ) /* 1981 "Gremlin" "Eliminator (4 Players)" */
	DRIVER( elim4p ) /* 1981 "Gremlin" "Eliminator (4 Players */
	DRIVER( endurobl ) /* 1986 "bootleg" "Enduro Racer (bootleg set 1)" */
	DRIVER( enduror ) /* 1986 "Sega" "Enduro Racer (YM2151 */
	DRIVER( enduror1 ) /* 1986 "Sega" "Enduro Racer (YM2203 */
	DRIVER( eswat ) /* 1989 "Sega" "E-Swat - Cyber Police (set 3 */
	DRIVER( eswatbl ) /* 1989 "bootleg" "E-Swat - Cyber Police (bootleg)" */
	DRIVER( eswatj ) /* 1989 "Sega" "E-Swat - Cyber Police (set 1 */
	DRIVER( eswatu ) /* 1989 "Sega" "E-Swat - Cyber Police (set 2 */
	DRIVER( f1en ) /* 1991 "Sega" "F1 Exhaust Note" */
	DRIVER( fantzon1 ) /* 1986 "Sega" "Fantasy Zone (set 1 */
	DRIVER( fantzone ) /* 1986 "Sega" "Fantasy Zone (set 2 */
	DRIVER( fhboxers ) /* 1995 "Sega" "Funky Head Boxers (JUETBKAL 951218 V1.000)" */
	DRIVER( finlarch ) /* 1995 "Sega" "Final Arch (J 950714 V1.001)" */
	DRIVER( flicky ) /* 1984 "Sega" "Flicky (128k Ver.)" */
	DRIVER( flickyo ) /* 1984 "Sega" "Flicky (64k Ver.)" */
	DRIVER( fpoint ) /* 1989 "Sega" "Flash Point (set 2 */
	DRIVER( fpoint1 ) /* 1989 "Sega" "Flash Point (set 1 */
	DRIVER( fpointbj ) /* 1989 "bootleg" "Flash Point (Japan */
	DRIVER( fpointbl ) /* 1989 "bootleg" "Flash Point (World */
	DRIVER( frogs ) /* 1978 "Gremlin" "Frogs" */
	DRIVER( futspy ) /* 1984 "Sega" "Future Spy" */
	DRIVER( ga2 ) /* 1992 "Sega" "Golden Axe: The Revenge of Death Adder (World)" */
	DRIVER( ga2j ) /* 1992 "Sega" "Golden Axe: The Revenge of Death Adder (Japan)" */
	DRIVER( ga2u ) /* 1992 "Sega" "Golden Axe: The Revenge of Death Adder (US)" */
	DRIVER( gardia ) /* 1986 "Sega / Coreland" "Gardia" */
	DRIVER( gaxeduel ) /* 1994 "Sega" "Golden Axe - The Duel (JUETL 950117 V1.000)" */
	DRIVER( gforce2 ) /* 1988 "Sega" "Galaxy Force 2" */
	DRIVER( gforce2j ) /* 1988 "Sega" "Galaxy Force 2 (Japan)" */
	DRIVER( gground ) /* 1988 "Sega" "Gain Ground (FD1094 317-0058-03?)" */
	DRIVER( gloc ) /* 1990 "Sega" "G-LOC Air Battle (US)" */
	DRIVER( glocr360 ) /* 1990 "Sega" "G-LOC R360" */
	DRIVER( goldnax1 ) /* 1989 "Sega" "Golden Axe (set 1 */
	DRIVER( goldnax2 ) /* 1989 "Sega" "Golden Axe (set 2 */
	DRIVER( goldnax3 ) /* 1989 "Sega" "Golden Axe (set 3 */
	DRIVER( goldnaxe ) /* 1989 "Sega" "Golden Axe (set 6 */
	DRIVER( goldnaxj ) /* 1989 "Sega" "Golden Axe (set 4 */
	DRIVER( goldnaxu ) /* 1989 "Sega" "Golden Axe (set 5 */
	DRIVER( gprider ) /* 1990 "Sega" "GP Rider (set 2 */
	DRIVER( gprider1 ) /* 1990 "Sega" "GP Rider (set 1 */
	DRIVER( grdforce ) /* 1998 "Success" "Guardian Force (JUET 980318 V0.105)" */
	DRIVER( groovef ) /* 1996 "Atlus" "Power Instinct 3 - Groove On Fight (J 970416 V1.001)" */
	DRIVER( hanagumi ) /* 1998 "Sega" "Hanagumi Taisen Columns - Sakura Wars (J 971007 V1.010)" */
	DRIVER( hangon ) /* 1985 "Sega" "Hang-On" */
	DRIVER( hangonjr ) /* 1985 "Sega" "Hang-On Jr." */
	DRIVER( harddunj ) /* 1994 "Sega" "Hard Dunk (Japan)" */
	DRIVER( harddunk ) /* 1994 "Sega" "Hard Dunk (World)" */
	DRIVER( headon ) /* 1979 "Gremlin" "Head On (2 players)" */
	DRIVER( headon2 ) /* 1979 "Sega" "Head On 2" */
	DRIVER( headonb ) /* 1979 "Gremlin" "Head On (1 player)" */
	DRIVER( heiankyo ) /* 1979 "Denki Onkyo" "Heiankyo Alien" */
	DRIVER( holo ) /* 1992 "Sega" "Holosseum (US)" */
	DRIVER( hotrod ) /* 1988 "Sega" "Hot Rod (World */
	DRIVER( hotroda ) /* 1988 "Sega" "Hot Rod (World */
	DRIVER( hotrodj ) /* 1988 "Sega" "Hot Rod (Japan */
	DRIVER( hustle ) /* 1977 "Gremlin" "Hustle" */
	DRIVER( hvymetal ) /* 1985 "Sega" "Heavy Metal" */
	DRIVER( hwchamp ) /* 1987 "Sega" "Heavyweight Champ" */
	DRIVER( ichir ) /* 1994 "Sega" "Puzzle & Action: Ichidant-R (World)" */
	DRIVER( ichirj ) /* 1994 "Sega" "Puzzle & Action: Ichidant-R (Japan)" */
	DRIVER( ichirjbl ) /* 1994 "bootleg" "Puzzle & Action: Ichidant-R (Japan) (bootleg)" */
	DRIVER( ichirk ) /* 1994 "Sega" "Puzzle & Action: Ichidant-R (Korea)" */
	DRIVER( imsorry ) /* 1985 "Coreland / Sega" "I'm Sorry (US)" */
	DRIVER( imsorryj ) /* 1985 "Coreland / Sega" "Gonbee no I'm Sorry (Japan)" */
	DRIVER( introdon ) /* 1996 "Sunsoft / Success" "Karaoke Quiz Intro Don Don! (J 960213 V1.000)" */
	DRIVER( invds ) /* 1979 "Sega" "Invinco / Deep Scan" */
	DRIVER( invho2 ) /* 1979 "Sega" "Invinco / Head On 2" */
	DRIVER( invinco ) /* 1979 "Sega" "Invinco" */
	DRIVER( ixion ) /* 1983 "Sega" "Ixion (prototype)" */
	DRIVER( jleague ) /* 1994 "Sega" "The J.League 1994 (Japan)" */
	DRIVER( jpark ) /* 1994 "Sega" "Jurassic Park" */
	DRIVER( karianx ) /* 1996 "Deniam" "Karian Cross (Rev. 1.0)" */
	DRIVER( kiwames ) /* 1995 "Athena" "Pro Mahjong Kiwame S (J 951020 V1.208)" */
	DRIVER( lghost ) /* 1990 "Sega" "Laser Ghost (set 2 */
	DRIVER( lghostu ) /* 1990 "Sega" "Laser Ghost (set 1 */
	DRIVER( loffire ) /* 1989 "Sega" "Line of Fire / Bakudan Yarou (World */
	DRIVER( loffirej ) /* 1989 "Sega" "Line of Fire / Bakudan Yarou (Japan */
	DRIVER( loffireu ) /* 1989 "Sega" "Line of Fire / Bakudan Yarou (US */
	DRIVER( logicpr2 ) /* 1997 "Deniam" "Logic Pro 2 (Japan)" */
	DRIVER( logicpro ) /* 1996 "Deniam" "Logic Pro (Japan)" */
	DRIVER( mahmajn ) /* 1992 "Sega" "Tokoro San no MahMahjan" */
	DRIVER( mahmajn2 ) /* 1994 "Sega" "Tokoro San no MahMahjan 2" */
	DRIVER( maruchan ) /* 1997 "Sega / Toyosuisan" "Maru-Chan de Goo! (J 971216 V1.000)" */
	DRIVER( mausuke ) /* 1995 "Data East" "Mausuke no Ojama the World (J 960314 V1.000)" */
	DRIVER( megaplay ) /* 1993 "Sega" "Mega Play BIOS" */
	DRIVER( megatech ) /* 1989 "Sega" "Mega-Tech BIOS" */
	DRIVER( minesw4p ) /* 1977 "Amutech" "Minesweeper (4-Player)" */
	DRIVER( mineswpr ) /* 1977 "Amutech" "Minesweeper" */
	DRIVER( mjleague ) /* 1985 "Sega" "Major League" */
	DRIVER( monster2 ) /* 1982 "Sega" "Monster Bash (2 board version)" */
	DRIVER( monsterb ) /* 1982 "Sega" "Monster Bash" */
	DRIVER( mp_bio ) /* 1993 "Sega" "Bio-hazard Battle (Mega Play)" */
	DRIVER( mp_col3 ) /* 1993 "Sega" "Columns III (Mega Play)" */
	DRIVER( mp_gaxe2 ) /* 1993 "Sega" "Golden Axe II (Mega Play)" */
	DRIVER( mp_soni2 ) /* 1993 "Sega" "Sonic The Hedgehog 2 (Mega Play)" */
	DRIVER( mp_sonic ) /* 1993 "Sega" "Sonic The Hedgehog (Mega Play)" */
	DRIVER( mp_sor2 ) /* 1993 "Sega" "Streets of Rage II (Mega Play)" */
	DRIVER( mp_twc ) /* 1993 "Sega" "Tecmo World Cup (Mega Play)" */
	DRIVER( mrviking ) /* 1984 "Sega" "Mister Viking" */
	DRIVER( mrvikngj ) /* 1984 "Sega" "Mister Viking (Japan)" */
	DRIVER( mt_sonia ) /* 1991 "Sega" "Sonic The Hedgehog (Mega-Tech */
	DRIVER( mt_sonic ) /* 1991 "Sega" "Sonic The Hedgehog (Mega-Tech */
	DRIVER( mvp ) /* 1989 "Sega" "MVP (set 2 */
	DRIVER( mvpj ) /* 1989 "Sega" "MVP (set 1 */
	DRIVER( mwalk ) /* 1990 "Sega" "Michael Jackson's Moonwalker (set 3 */
	DRIVER( mwalkbl ) /* 1990 "bootleg" "Michael Jackson's Moonwalker (bootleg)" */
	DRIVER( mwalkj ) /* 1990 "Sega" "Michael Jackson's Moonwalker (set 1 */
	DRIVER( mwalku ) /* 1990 "Sega" "Michael Jackson's Moonwalker (set 2 */
	DRIVER( myhero ) /* 1985 "Sega" "My Hero (US)" */
	DRIVER( myherok ) /* 1985 "Coreland / Sega" "My Hero (Korea)" */
	DRIVER( ninja ) /* 1985 "Sega" "Ninja" */
	DRIVER( noboranb ) /* 1986 "bootleg" "Noboranka (Japan)" */
	DRIVER( nprinces ) /* 1985 "bootleg?" "Ninja Princess (64k Ver. bootleg?)" */
	DRIVER( nprincsb ) /* 1985 "bootleg?" "Ninja Princess (128k Ver. bootleg?)" */
	DRIVER( nprincso ) /* 1985 "Sega" "Ninja Princess (128k Ver.)" */
	DRIVER( nprincsu ) /* 1985 "Sega" "Ninja Princess (64k Ver. not encrypted)" */
	DRIVER( nsub ) /* 1980 "Sega" "N-Sub (upright)" */
	DRIVER( orunners ) /* 1992 "Sega" "OutRunners (World)" */
	DRIVER( orunneru ) /* 1992 "Sega" "OutRunners (US)" */
	DRIVER( othellos ) /* 1998 "Success" "Othello Shiyouyo (J 980423 V1.002)" */
	DRIVER( outrun ) /* 1986 "Sega" "Out Run (set 3)" */
	DRIVER( outrun1 ) /* 1986 "Sega" "Out Run (set 1 */
	DRIVER( outrun2 ) /* 1986 "Sega" "Out Run (set 2)" */
	DRIVER( outrunb ) /* 1986 "bootleg" "Out Run (bootleg)" */
	DRIVER( passht4b ) /* 1988 "bootleg" "Passing Shot (4 Players) (bootleg)" */
	DRIVER( passsht ) /* 1988 "Sega" "Passing Shot (World */
	DRIVER( passshta ) /* 1988 "Sega" "Passing Shot (World */
	DRIVER( passshtb ) /* 1988 "bootleg" "Passing Shot (2 Players) (bootleg)" */
	DRIVER( passshtj ) /* 1988 "Sega" "Passing Shot (Japan */
	DRIVER( pblbeach ) /* 1995 "T&E Soft" "Pebble Beach - The Great Shot (JUE 950913 V0.990)" */
	DRIVER( pdrift ) /* 1988 "Sega" "Power Drift (World Set 1)" */
	DRIVER( pdrifta ) /* 1988 "Sega" "Power Drift (World Set 2)" */
	DRIVER( pdriftj ) /* 1988 "Sega" "Power Drift (Japan)" */
	DRIVER( pignewt ) /* 1983 "Sega" "Pig Newton (version C)" */
	DRIVER( pignewta ) /* 1983 "Sega" "Pig Newton (version A)" */
	DRIVER( pitfall2 ) /* 1985 "Sega" "Pitfall II" */
	DRIVER( pitfallu ) /* 1985 "Sega" "Pitfall II (not encrypted)" */
	DRIVER( potopoto ) /* 1994 "Sega" "Poto Poto (Japan)" */
	DRIVER( prikura ) /* 1996 "Atlus" "Princess Clara Daisakusen (J 960910 V1.000)" */
	DRIVER( puckpkmn ) /* 2000 "Genie" "Puckman Pockimon" */
	DRIVER( pulsar ) /* 1981 "Sega" "Pulsar" */
	DRIVER( puyo ) /* 1992 "Sega / Compile" "Puyo Puyo (World)" */
	DRIVER( puyobl ) /* 1992 "bootleg" "Puyo Puyo (World */
	DRIVER( puyoj ) /* 1992 "Sega / Compile" "Puyo Puyo (Japan)" */
	DRIVER( puyoja ) /* 1992 "Sega / Compile" "Puyo Puyo (Japan */
	DRIVER( puyopuy2 ) /* 1994 "Compile (Sega license)" "Puyo Puyo 2 (Japan)" */
	DRIVER( puyosun ) /* 1996 "Compile" "Puyo Puyo Sun (J 961115 V0.001)" */
	DRIVER( qgh ) /* 1994 "Sega" "Quiz Ghost Hunter" */
	DRIVER( qrouka ) /* 1994 "Sega" "Quiz Rouka Ni Tattenasai" */
	DRIVER( qsww ) /* 1991 "Sega" "Quiz Syukudai wo Wasuremashita" */
	DRIVER( quartet ) /* 1986 "Sega" "Quartet (8751 317-unknown)" */
	DRIVER( quartet2 ) /* 1986 "Sega" "Quartet 2 (8751 317-unknown)" */
	DRIVER( quartetj ) /* 1986 "Sega" "Quartet (Japan */
	DRIVER( quartt2j ) /* 1986 "Sega" "Quartet 2 (Japan */
	DRIVER( quizmeku ) /* 1994 "Sega" "Quiz Mekurumeku Story" */
	DRIVER( rachero ) /* 1989 "Sega" "Racing Hero (FD1094 317-0144)" */
	DRIVER( radm ) /* 1991 "Sega" "Rad Mobile (World)" */
	DRIVER( radmu ) /* 1991 "Sega" "Rad Mobile (US)" */
	DRIVER( radr ) /* 1991 "Sega" "Rad Rally (World)" */
	DRIVER( radru ) /* 1991 "Sega" "Rad Rally (US)" */
	DRIVER( raflesia ) /* 1986 "Coreland / Sega" "Rafflesia" */
	DRIVER( razmataz ) /* 1983 "Sega" "Razzmatazz" */
	DRIVER( rchase ) /* 1991 "Sega" "Rail Chase (Japan)" */
	DRIVER( regulus ) /* 1983 "Sega" "Regulus (New Ver.)" */
	DRIVER( reguluso ) /* 1983 "Sega" "Regulus (Old Ver.)" */
	DRIVER( regulusu ) /* 1983 "Sega" "Regulus (not encrypted)" */
	DRIVER( ribbit ) /* 1991 "Sega" "Ribbit!" */
	DRIVER( ridleofp ) /* 1986 "Sega / Nasco" "Riddle of Pythagoras (Japan)" */
	DRIVER( riotcity ) /* 1991 "Sega / Westone" "Riot City (Japan)" */
	DRIVER( roughrac ) /* 1990 "Sega" "Rough Racer (Japan */
	DRIVER( rsgun ) /* 1998 "Treasure" "Radiant Silvergun (JUET 980523 V1.000)" */
	DRIVER( ryukyu ) /* 1990 "Success / Sega" "RyuKyu (Japan */
	DRIVER( safari ) /* 1977 "Gremlin" "Safari" */
	DRIVER( samurai ) /* 1980 "Sega" "Samurai" */
	DRIVER( sandor ) /* 1995 "Sega" "Sando-R (J 951114 V1.000)" */
	DRIVER( sassisu ) /* 1996 "Sega" "Taisen Tanto-R Sashissu!! (J 980216 V1.000)" */
	DRIVER( scross ) /* 1992 "Sega" "Stadium Cross (World)" */
	DRIVER( sdi ) /* 1987 "Sega" "SDI - Strategic Defense Initiative (Europe */
	DRIVER( sdib ) /* 1987 "Sega" "SDI - Strategic Defense Initiative (System 16B */
	DRIVER( sdibl ) /* 1987 "bootleg" "SDI - Strategic Defense Initiative (bootleg)" */
	DRIVER( seabass ) /* 1998 "A wave inc. (Able license)" "Sea Bass Fishing (JUET 971110 V0.001)" */
	DRIVER( seganinj ) /* 1985 "Sega" "Sega Ninja" */
	DRIVER( seganinu ) /* 1985 "Sega" "Sega Ninja (not encrypted)" */
	DRIVER( sgmast ) /* 1989 "Sega" "Super Masters Golf (FD1094 317-0058-05d?)" */
	DRIVER( sgmastc ) /* 1989 "Sega" "Jumbo Ozaki Super Masters Golf (World */
	DRIVER( sgmastj ) /* 1989 "Sega" "Jumbo Ozaki Super Masters Golf (Japan */
	DRIVER( shangnle ) /* 1991 "Sega" "Limited Edition Hang-On" */
	DRIVER( shangon ) /* 1987 "Sega" "Super Hang-On (set 4 */
	DRIVER( shangon1 ) /* 1987 "Sega" "Super Hang-On (set 1 */
	DRIVER( shangon2 ) /* 1987 "Sega" "Super Hang-On (set 2 */
	DRIVER( shangon3 ) /* 1987 "Sega" "Super Hang-On (set 3 */
	DRIVER( shangupb ) /* 1992 "bootleg" "Super Hang-On (Hang-On upgrade */
	DRIVER( shanhigw ) /* 1995 "Sunsoft / Activision" "Shanghai - The Great Wall / Shanghai Triple Threat (JUE 950623 V1.005)" */
	DRIVER( sharrier ) /* 1985 "Sega" "Space Harrier (Rev A */
	DRIVER( sharrirb ) /* 1985 "Sega" "Space Harrier (8751 315-5163)" */
	DRIVER( shdancbl ) /* 1989 "bootleg" "Shadow Dancer (bootleg)" */
	DRIVER( shdance1 ) /* 1989 "Sega" "Shadow Dancer (set 1)" */
	DRIVER( shdancej ) /* 1989 "Sega" "Shadow Dancer (set 2 */
	DRIVER( shdancer ) /* 1989 "Sega" "Shadow Dancer (set 3 */
	DRIVER( shienryu ) /* 1997 "Warashi" "Shienryu (JUET 961226 V1.000)" */
	DRIVER( shinobi ) /* 1987 "Sega" "Shinobi (set 5 */
	DRIVER( shinobi1 ) /* 1987 "Sega" "Shinobi (set 1 */
	DRIVER( shinobi2 ) /* 1987 "Sega" "Shinobi (set 2 */
	DRIVER( shinobi3 ) /* 1987 "Sega" "Shinobi (set 3 */
	DRIVER( shinobi4 ) /* 1987 "Sega" "Shinobi (set 4 */
	DRIVER( shinobl ) /* 1987 "bootleg" "Shinobi (bootleg)" */
	DRIVER( sindbadm ) /* 1983 "Sega" "Sindbad Mystery" */
	DRIVER( sjryuko ) /* 1987 "White Board" "Sukeban Jansi Ryuko (set 2 */
	DRIVER( sjryuko1 ) /* 1987 "White Board" "Sukeban Jansi Ryuko (set 1 */
	DRIVER( slipstrm ) /* 1995 "Capcom" "Slipstream" */
	DRIVER( smgp ) /* 1989 "Sega" "Super Monaco GP (set 8 */
	DRIVER( smgp5 ) /* 1989 "Sega" "Super Monaco GP (set 6 */
	DRIVER( smgp6 ) /* 1989 "Sega" "Super Monaco GP (set 7 */
	DRIVER( smgpj ) /* 1989 "Sega" "Super Monaco GP (set 1 */
	DRIVER( smgpu ) /* 1989 "Sega" "Super Monaco GP (set 5 */
	DRIVER( smgpu1 ) /* 1989 "Sega" "Super Monaco GP (set 4 */
	DRIVER( smgpu2 ) /* 1989 "Sega" "Super Monaco GP (set 3 */
	DRIVER( smgpu3 ) /* 1989 "Sega" "Super Monaco GP (set 2 */
	DRIVER( smleague ) /* 1995 "Sega" "Super Major League (U 960108 V1.000)" */
	DRIVER( snapper ) /* 1990 "Philko" "Snapper (Korea)" */
	DRIVER( sokyugrt ) /* 1996 "Raizing / 8ing" "Soukyugurentai / Terra Diver (JUET 960821 V1.000)" */
	DRIVER( sonic ) /* 1992 "Sega" "SegaSonic The Hedgehog (Japan */
	DRIVER( sonicbom ) /* 1987 "Sega" "Sonic Boom (FD1094 317-0053)" */
	DRIVER( sonicp ) /* 1992 "Sega" "SegaSonic The Hedgehog (Japan */
	DRIVER( spaceod ) /* 1981 "Sega" "Space Odyssey" */
	DRIVER( spacetrk ) /* 1980 "Sega" "Space Trek (upright)" */
	DRIVER( spacfura ) /* 1981 "Sega" "Space Fury (revision A)" */
	DRIVER( spacfury ) /* 1981 "Sega" "Space Fury (revision C)" */
	DRIVER( spatter ) /* 1984 "Sega" "Spatter" */
	DRIVER( spcpostn ) /* 1986 "Sega / Nasco" "Space Position (Japan)" */
	DRIVER( spidman ) /* 1991 "Sega" "Spider-Man: The Videogame (World)" */
	DRIVER( spidmanu ) /* 1991 "Sega" "Spider-Man: The Videogame (US)" */
	DRIVER( sptrekct ) /* 1980 "Sega" "Space Trek (cocktail)" */
	DRIVER( ssanchan ) /* 1984 "Sega" "Sanrin San Chan (Japan)" */
	DRIVER( sscandal ) /* 1985 "Coreland / Sega" "Seishun Scandal (Japan)" */
	DRIVER( sspacaho ) /* 1979 "Sega" "Space Attack / Head On" */
	DRIVER( sspacat2 ) /* 1979 "Sega" "Space Attack (upright set 2)" */
	DRIVER( sspacat3 ) /* 1979 "Sega" "Space Attack (upright set 3)" */
	DRIVER( sspacatc ) /* 1979 "Sega" "Space Attack (cocktail)" */
	DRIVER( sspaceat ) /* 1979 "Sega" "Space Attack (upright set 1)" */
	DRIVER( sspiritj ) /* 1988 "Sega" "Scramble Spirits (Japan */
	DRIVER( sspirits ) /* 1988 "Sega" "Scramble Spirits" */
	DRIVER( sss ) /* 1998 "Capcom / Cave / Victor" "Steep Slope Sliders (JUET 981110 V1.000)" */
	DRIVER( stactics ) /* 1981 "Sega" "Space Tactics" */
	DRIVER( starjack ) /* 1983 "Sega" "Star Jacker (Sega)" */
	DRIVER( starjacs ) /* 1983 "Stern" "Star Jacker (Stern)" */
	DRIVER( startrek ) /* 1982 "Sega" "Star Trek" */
	DRIVER( stkclmnj ) /* 1994 "Sega" "Stack Columns (Japan)" */
	DRIVER( stkclmns ) /* 1994 "Sega" "Stack Columns (World)" */
	DRIVER( strkfgtr ) /* 1991 "Sega" "Strike Fighter (Japan)" */
	DRIVER( stvbios ) /* 1996 "Sega" "ST-V Bios" */
	DRIVER( subhunt ) /* 1977 "Taito" "Sub Hunter" */
	DRIVER( subroc3d ) /* 1982 "Sega" "Subroc-3D" */
	DRIVER( suikoenb ) /* 1995 "Data East" "Suikoenbu (J 950314 V2.001)" */
	DRIVER( suprloco ) /* 1982 "Sega" "Super Locomotive" */
	DRIVER( svf ) /* 1994 "Sega" "Super Visual Football: European Sega Cup" */
	DRIVER( svs ) /* 1994 "Sega" "Super Visual Soccer: Sega Cup (US)" */
	DRIVER( swat ) /* 1984 "Coreland / Sega" "SWAT" */
	DRIVER( szaxxon ) /* 1982 "Sega" "Super Zaxxon" */
	DRIVER( tacscan ) /* 1982 "Sega" "Tac/Scan" */
	DRIVER( tantr ) /* 1992 "Sega" "Puzzle & Action: Tant-R (Japan)" */
	DRIVER( tantrbl ) /* 1992 "bootleg" "Puzzle & Action: Tant-R (Japan) (bootleg set 1)" */
	DRIVER( tantrbl2 ) /* 1994 "bootleg" "Puzzle & Action: Tant-R (Japan) (bootleg set 2)" */
	DRIVER( tantrkor ) /* 1993 "Sega" "Puzzle & Action: Tant-R (Korea)" */
	DRIVER( teddybb ) /* 1985 "Sega" "TeddyBoy Blues (New Ver.)" */
	DRIVER( teddybbo ) /* 1985 "Sega" "TeddyBoy Blues (Old Ver.)" */
	DRIVER( tetris ) /* 1988 "Sega" "Tetris (set 4 */
	DRIVER( tetris1 ) /* 1988 "Sega" "Tetris (set 1 */
	DRIVER( tetris2 ) /* 1988 "Sega" "Tetris (set 2 */
	DRIVER( tetris3 ) /* 1988 "Sega" "Tetris (set 3 */
	DRIVER( tetrisbl ) /* 1988 "bootleg" "Tetris (bootleg)" */
	DRIVER( tetrisse ) /* 1988 "Sega" "Tetris (Japan */
	DRIVER( tfrceac ) /* 1990 "Sega / Technosoft" "ThunderForce AC" */
	DRIVER( tfrceacb ) /* 1990 "bootleg" "ThunderForce AC (bootleg)" */
	DRIVER( tfrceacj ) /* 1990 "Sega / Technosoft" "ThunderForce AC (Japan)" */
	DRIVER( thetogyu ) /* 1984 "Coreland / Sega" "The Togyu (Japan)" */
	DRIVER( thndrbdj ) /* 1987 "Sega" "Thunder Blade (Japan)" */
	DRIVER( thndrbld ) /* 1987 "Sega" "Thunder Blade (FD1094 317-0056)" */
	DRIVER( thunt ) /* 1995 "Sega (Deniam license?)" "Treasure Hunt (JUET 970901 V2.00E)" */
	DRIVER( timesca1 ) /* 1987 "Sega" "Time Scanner (set 1 */
	DRIVER( timescan ) /* 1987 "Sega" "Time Scanner (set 2 */
	DRIVER( tiptop ) /* 1983 "Sega" "Tip Top" */
	DRIVER( titlef ) /* 1992 "Sega" "Title Fight (World)" */
	DRIVER( titlefu ) /* 1992 "Sega" "Title Fight (US)" */
	DRIVER( tokisens ) /* 1987 "Sega" "Toki no Senshi - Chrono Soldier" */
	DRIVER( toryumon ) /* 1994 "Sega" "Toryumon" */
	DRIVER( toutrun ) /* 1989 "Sega" "Turbo Out Run (set 3 */
	DRIVER( toutrun1 ) /* 1989 "Sega" "Turbo Out Run (set 1 */
	DRIVER( toutrun2 ) /* 1989 "Sega" "Turbo Out Run (set 2 */
	DRIVER( tranqgun ) /* 1980 "Sega" "Tranquilizer Gun" */
	DRIVER( transfrm ) /* 1986 "Sega" "Transformer" */
	DRIVER( tturf ) /* 1989 "Sega / Sunsoft" "Tough Turf (set 2 */
	DRIVER( tturfbl ) /* 1989 "bootleg" "Tough Turf (bootleg)" */
	DRIVER( tturfu ) /* 1989 "Sega / Sunsoft" "Tough Turf (set 1 */
	DRIVER( turbo ) /* 1981 "Sega" "Turbo" */
	DRIVER( turboa ) /* 1981 "Sega" "Turbo (encrypted set 1)" */
	DRIVER( turbob ) /* 1981 "Sega" "Turbo (encrypted set 2)" */
	DRIVER( twinsqua ) /* 1991 "Sega" "Twin Squash" */
	DRIVER( ufosensb ) /* 1988 "bootleg" "Ufo Senshi Yohko Chan (not encrypted)" */
	DRIVER( ufosensi ) /* 1988 "Sega" "Ufo Senshi Yohko Chan" */
	DRIVER( upndown ) /* 1983 "Sega" "Up'n Down" */
	DRIVER( upndownu ) /* 1983 "Sega" "Up'n Down (not encrypted)" */
	DRIVER( vf ) /* 1993 "Sega" "Virtua Fighter" */
	DRIVER( vfkids ) /* 1996 "Sega" "Virtua Fighter Kids (JUET 960319 V0.000)" */
	DRIVER( vmahjong ) /* 1997 "Micronet" "Virtual Mahjong (J 961214 V1.000)" */
	DRIVER( wb3 ) /* 1988 "Sega / Westone" "Wonder Boy III - Monster Lair (set 5 */
	DRIVER( wb31 ) /* 1988 "Sega / Westone" "Wonder Boy III - Monster Lair (set 1 */
	DRIVER( wb32 ) /* 1988 "Sega / Westone" "Wonder Boy III - Monster Lair (set 2 */
	DRIVER( wb33 ) /* 1988 "Sega / Westone" "Wonder Boy III - Monster Lair (set 3 */
	DRIVER( wb34 ) /* 1988 "Sega / Westone" "Wonder Boy III - Monster Lair (set 4 */
	DRIVER( wb3bbl ) /* 1988 "bootleg" "Wonder Boy III - Monster Lair (bootleg)" */
	DRIVER( wbdeluxe ) /* 1986 "Sega (Escape license)" "Wonder Boy Deluxe" */
	DRIVER( wbml ) /* 1987 "Sega / Westone" "Wonder Boy in Monster Land (Japan New Ver.)" */
	DRIVER( wbmlb ) /* 1987 "bootleg" "Wonder Boy in Monster Land" */
	DRIVER( wbmljb ) /* 1987 "bootleg" "Wonder Boy in Monster Land (Japan not encrypted)" */
	DRIVER( wbmljo ) /* 1987 "Sega / Westone" "Wonder Boy in Monster Land (Japan Old Ver.)" */
	DRIVER( wboy ) /* 1986 "Sega (Escape license)" "Wonder Boy (set 1 */
	DRIVER( wboy2 ) /* 1986 "Sega (Escape license)" "Wonder Boy (set 2)" */
	DRIVER( wboy2u ) /* 1986 "Sega (Escape license)" "Wonder Boy (set 2 not encrypted)" */
	DRIVER( wboy3 ) /* 1986 "Sega (Escape license)" "Wonder Boy (set 3)" */
	DRIVER( wboyo ) /* 1986 "Sega (Escape license)" "Wonder Boy (set 1 */
	DRIVER( wboysys2 ) /* 1986 "Sega (Escape license)" "Wonder Boy (system 2)" */
	DRIVER( wboyu ) /* 1986 "Sega (Escape license)" "Wonder Boy (not encrypted)" */
	DRIVER( winterht ) /* 1997 "Sega" "Winter Heat (JUET 971012 V1.000)" */
	DRIVER( wmatch ) /* 1984 "Sega" "Water Match" */
	DRIVER( wrestwa1 ) /* 1989 "Sega" "Wrestle War (set 1 */
	DRIVER( wrestwa2 ) /* 1989 "Sega" "Wrestle War (set 2 */
	DRIVER( wrestwar ) /* 1989 "Sega" "Wrestle War (set 3 */
	DRIVER( wwallyj ) /* 1992 "Sega" "Wally wo Sagase! (rev B */
	DRIVER( wwallyja ) /* 1992 "Sega" "Wally wo Sagase! (rev A */
	DRIVER( zaxxon ) /* 1982 "Sega" "Zaxxon (set 1)" */
	DRIVER( zaxxon2 ) /* 1982 "Sega" "Zaxxon (set 2)" */
	DRIVER( zaxxonb ) /* 1982 "bootleg" "Jackson" */
	DRIVER( zektor ) /* 1982 "Sega" "Zektor (revision B)" */
	DRIVER( znpwfv ) /* 1997 "Sega" "Zen Nippon Pro-Wrestling Featuring Virtua (J 971123 V1.000)" */
	DRIVER( zoom909 ) /* 1982 "Sega" "Zoom 909" */
	DRIVER( zunkyou ) /* 1994 "Sega" "Zunzunkyou No Yabou (Japan)" */
#endif
#ifdef LINK_SEIBU
	DRIVER( batlbala ) /* 1995 "Seibu Kaihatsu (Metrotainment License)" "Battle Balls (Asia)" */
	DRIVER( batlball ) /* 1995 "Seibu Kaihatsu (Tuning License)" "Battle Balls (Germany)" */
	DRIVER( bcrusher ) /* 1985 "bootleg" "Bone Crusher" */
	DRIVER( cshooter ) /* 1987 "[Seibu Kaihatsu] (Taito license)" "Cross Shooter (not encrypted)" */
	DRIVER( dbldyn ) /* 1989 "Seibu Kaihatsu" "The Double Dynamites (Japan)" */
	DRIVER( dbldynf ) /* 1989 "Seibu Kaihatsu (Fabtek license)" "The Double Dynamites (US)" */
	DRIVER( dcon ) /* 1992 "Success" "D-Con" */
	DRIVER( deadang ) /* 1988 "Seibu Kaihatsu" "Dead Angle" */
	DRIVER( dynduke ) /* 1989 "Seibu Kaihatsu" "Dynamite Duke (Japan)" */
	DRIVER( dyndukf ) /* 1989 "Seibu Kaihatsu (Fabtek license)" "Dynamite Duke (US)" */
	DRIVER( ejanhs ) /* 1996 "Seibu Kaihatsu" "E-Jan High School (Japan)" */
	DRIVER( empcity ) /* 1986 "Seibu Kaihatsu" "Empire City: 1931 (bootleg?)" */
	DRIVER( empcityj ) /* 1986 "[Seibu Kaihatsu] (Taito license)" "Empire City: 1931 (Japan)" */
	DRIVER( empcityu ) /* 1986 "[Seibu Kaihatsu] (Taito / Romstar license)" "Empire City: 1931 (US)" */
	DRIVER( ghunter ) /* 1988 "Seibu Kaihatsu (Segasa/Sonic license)" "Gang Hunter (Spain)" */
	DRIVER( kncljoe ) /* 1985 "[Seibu Kaihatsu] (Taito license)" "Knuckle Joe (set 1)" */
	DRIVER( kncljoea ) /* 1985 "[Seibu Kaihatsu] (Taito license)" "Knuckle Joe (set 2)" */
	DRIVER( kungfut ) /* 1984 "Seibu Kaihatsu Inc." "Kung-Fu Taikun" */
	DRIVER( kungfuta ) /* 1984 "Seibu Kaihatsu Inc." "Kung-Fu Taikun (alt)" */
	DRIVER( mustache ) /* 1987 "[Seibu Kaihatsu] (March license)" "Mustache Boy" */
	DRIVER( raiden ) /* 1990 "Seibu Kaihatsu" "Raiden" */
	DRIVER( raiden2 ) /* 1993 "Seibu Kaihatsu" "Raiden 2 (set 1 */
	DRIVER( raiden2a ) /* 1993 "Seibu Kaihatsu" "Raiden 2 (set 2 */
	DRIVER( raiden2b ) /* 1993 "Seibu Kaihatsu" "Raiden 2 (set 3 */
	DRIVER( raiden2c ) /* 1993 "Seibu Kaihatsu" "Raiden 2 (set 4 */
	DRIVER( raidena ) /* 1990 "Seibu Kaihatsu" "Raiden (Alternate Hardware)" */
	DRIVER( raidenk ) /* 1990 "Seibu Kaihatsu (IBL Corporation license)" "Raiden (Korea)" */
	DRIVER( raident ) /* 1990 "Seibu Kaihatsu (Liang HWA Electronics license)" "Raiden (Taiwan)" */
	DRIVER( raidndx ) /* 1993 "Seibu Kaihatsu" "Raiden DX (set 1)" */
	DRIVER( raidndxb ) /* 1993 "Seibu Kaihatsu" "Raiden DX (set 3 */
	DRIVER( rdft ) /* 1996 "Seibu Kaihatsu" "Raiden Fighters (Japan set 1)" */
	DRIVER( rdft2 ) /* 1997 "Seibu Kaihatsu (Tuning license)" "Raiden Fighters 2" */
	DRIVER( rdft22kc ) /* 2000 "Seibu Kaihatsu" "Raiden Fighters 2 - 2000 (China)" */
	DRIVER( rdft2a ) /* 1997 "Seibu Kaihatsu (Metrotainment license)" "Raiden Fighters 2 (Asia */
	DRIVER( rdft2a2 ) /* 1997 "Seibu Kaihatsu (Dream Island license)" "Raiden Fighters 2 (Asia */
	DRIVER( rdft2j ) /* 1997 "Seibu Kaihatsu" "Raiden Fighters 2 (Japan */
	DRIVER( rdft2us ) /* 1997 "Seibu Kaihatsu (Fabtek license)" "Raiden Fighters 2.1 (US */
	DRIVER( rdftau ) /* 1996 "Seibu Kaihatsu" "Raiden Fighters (Australia)" */
	DRIVER( rdftdi ) /* 1996 "Seibu Kaihatsu (Dream Island license)" "Raiden Fighters (Dream Island Co. license)" */
	DRIVER( rdftj ) /* 1996 "Seibu Kaihatsu" "Raiden Fighters (Japan set 2)" */
	DRIVER( rdftu ) /* 1996 "Seibu Kaihatsu (Fabtek license)" "Raiden Fighters (US)" */
	DRIVER( rfjet ) /* 1998 "Seibu Kaihatsu (Tuning license)" "Raiden Fighters Jet" */
	DRIVER( rfjeta ) /* 1998 "Seibu Kaihatsu (Dream Island license)" "Raiden Fighters Jet (Asia)" */
	DRIVER( rfjetj ) /* 1998 "Seibu Kaihatsu" "Raiden Fighters Jet (Japan)" */
	DRIVER( rfjetu ) /* 1998 "Seibu Kaihatsu (Fabtek license)" "Raiden Fighters Jet (US)" */
	DRIVER( rfjetus ) /* 1999 "Seibu Kaihatsu (Fabtek license)" "Raiden Fighters Jet (US */
	DRIVER( scion ) /* 1984 "Seibu Denshi" "Scion" */
	DRIVER( scionc ) /* 1984 "Seibu Denshi (Cinematronics license)" "Scion (Cinematronics)" */
	DRIVER( sdgndmps ) /* 1991 "Banpresto / Bandai" "SD Gundam Psycho Salamander no Kyoui" */
	DRIVER( sengokmj ) /* 1991 "Sigma" "Sengoku Mahjong (Japan)" */
	DRIVER( senkyu ) /* 1995 "Seibu Kaihatsu" "Senkyu (Japan)" */
	DRIVER( senkyua ) /* 1995 "Seibu Kaihatsu" "Senkyu (Japan */
	DRIVER( stfight ) /* 1986 "Seibu Kaihatsu" "Street Fight (Germany)" */
	DRIVER( stfighta ) /* 1986 "Seibu Kaihatsu" "Street Fight" */
	DRIVER( stinger ) /* 1983 "Seibu Denshi" "Stinger" */
	DRIVER( stinger2 ) /* 1983 "Seibu Denshi" "Stinger (prototype?)" */
	DRIVER( viprp1 ) /* 1995 "Seibu Kaihatsu" "Viper Phase 1 (Japan */
	DRIVER( viprp1o ) /* 1995 "Seibu Kaihatsu" "Viper Phase 1 (Japan)" */
	DRIVER( viprp1ot ) /* 1995 "Seibu Kaihatsu (Tuning License)" "Viper Phase 1 (Germany)" */
	DRIVER( viprp1s ) /* 1995 "Seibu Kaihatsu" "Viper Phase 1 (Switzerland */
	DRIVER( wiz ) /* 1985 "Seibu Kaihatsu Inc." "Wiz" */
	DRIVER( wizt ) /* 1985 "[Seibu] (Taito license)" "Wiz (Taito)" */
#endif
#ifdef LINK_SETA
	DRIVER( NAME ) /* YEAR COMPANY FULLNAME */
	DRIVER( arbalest ) /* 1989 "Seta" "Arbalester" */
	DRIVER( atehate ) /* 1993 "Athena" "Athena no Hatena ?" */
	DRIVER( blandia ) /* 1992 "Allumer" "Blandia" */
	DRIVER( blandiap ) /* 1992 "Allumer" "Blandia (prototype)" */
	DRIVER( blockcar ) /* 1992 "Visco" "Block Carnival / Thunder & Lightning 2" */
	DRIVER( cairblad ) /* 1999 "Sammy" "Change Air Blade (Japan)" */
	DRIVER( calibr50 ) /* 1989 "Athena / Seta" "Caliber 50" */
	DRIVER( crazyfgt ) /* 1996 "Subsino" "Crazy Fight" */
	DRIVER( daioh ) /* 1993 "Athena" "Daioh" */
	DRIVER( darkhors ) /* 2001 "bootleg" "Dark Horse" */
	DRIVER( downtow2 ) /* 1989 "Seta" "DownTown (Set 2)" */
	DRIVER( downtown ) /* 1989 "Seta" "DownTown (Set 1)" */
	DRIVER( downtowp ) /* 1989 "Seta" "DownTown (prototype)" */
	DRIVER( drgnunit ) /* 1989 "Seta" "Dragon Unit / Castle of Dragon" */
	DRIVER( drifto94 ) /* 1994 "Visco" "Drift Out '94 - The Hard Order (Japan)" */
	DRIVER( dynagear ) /* 1993 "Sammy" "Dyna Gear" */
	DRIVER( eaglshot ) /* 1994 "Sammy" "Eagle Shot Golf" */
	DRIVER( eightfrc ) /* 1994 "Tecmo" "Eight Forces" */
	DRIVER( extdwnhl ) /* 1995 "Sammy Industries Japan" "Extreme Downhill (v1.5)" */
	DRIVER( gdfs ) /* 1995 "Banpresto" "Mobil Suit Gundam Final Shooting (Japan)" */
	DRIVER( grdians ) /* 1995 "Banpresto" "Guardians / Denjin Makai II" */
	DRIVER( gundamex ) /* 1994 "Banpresto" "Mobile Suit Gundam EX Revue" */
	DRIVER( gundhara ) /* 1995 "Banpresto" "Gundhara" */
	DRIVER( hanaawas ) /* 1982 "Seta" "Hana Awase (Flower Matching)" */
	DRIVER( hypreac2 ) /* 1997 "Sammy" "Mahjong Hyper Reaction 2 (Japan)" */
	DRIVER( hypreact ) /* 1995 "Sammy" "Mahjong Hyper Reaction (Japan)" */
	DRIVER( janjans1 ) /* 1996 "Visco" "Lovely Pop Mahjong JangJang Shimasho (Japan)" */
	DRIVER( janjans2 ) /* 2000 "Visco" "Lovely Pop Mahjong JangJang Shimasho 2 (Japan)" */
	DRIVER( jjsquawk ) /* 1993 "Athena / Able" "J. J. Squawkers" */
	DRIVER( kamenrid ) /* 1993 "Toei / Banpresto" "Masked Riders Club Battle Race" */
	DRIVER( keithlcy ) /* 1993 "Visco" "Dramatic Adventure Quiz Keith & Lucy (Japan)" */
	DRIVER( kiwame ) /* 1994 "Athena" "Pro Mahjong Kiwame" */
	DRIVER( koikois2 ) /* 1997 "Visco" "Koi Koi Shimasho 2 - Super Real Hanafuda (Japan)" */
	DRIVER( krzybowl ) /* 1994 "American Sammy" "Krazy Bowl" */
	DRIVER( macsbios ) /* 1995 "I'Max" "Multi Amenity Cassette System BIOS" */
	DRIVER( madshark ) /* 1993 "Allumer" "Mad Shark" */
	DRIVER( mayjisn2 ) /* 1994 "Seta" "Mayjinsen 2" */
	DRIVER( meosism ) /* 1996? "Sammy" "Meosis Magic (Japan)" */
	DRIVER( metafox ) /* 1989 "Seta" "Meta Fox" */
	DRIVER( mj4simai ) /* 1996 "Maboroshi Ware" "Wakakusamonogatari Mahjong Yonshimai (Japan)" */
	DRIVER( mjyuugi ) /* 1990 "Visco" "Mahjong Yuugi (Japan set 1)" */
	DRIVER( mjyuugia ) /* 1990 "Visco" "Mahjong Yuugi (Japan set 2)" */
	DRIVER( msgunda1 ) /* 1993 "Banpresto" "Mobile Suit Gundam (Japan)" */
	DRIVER( msgundam ) /* 1993 "Banpresto" "Mobile Suit Gundam" */
	DRIVER( mslider ) /* 1997 "Visco / Datt Japan" "Monster Slider (Japan)" */
	DRIVER( myangel ) /* 1996 "Namco" "Kosodate Quiz My Angel (Japan)" */
	DRIVER( myangel2 ) /* 1997 "Namco" "Kosodate Quiz My Angel 2 (Japan)" */
	DRIVER( neobattl ) /* 1992 "Banpresto / Sotsu Agency. Sunrise" "SD Gundam Neo Battling (Japan)" */
	DRIVER( nratechu ) /* 1996 "Seta" "Neratte Chu" */
	DRIVER( oisipuzl ) /* 1993 "Sunsoft + Atlus" "Oishii Puzzle Ha Irimasenka" */
	DRIVER( pairlove ) /* 1991 "Athena" "Pairs Love" */
	DRIVER( penbros ) /* 2000 "Subsino" "Penguin Brothers (Japan)" */
	DRIVER( ponchin ) /* 1991 "Visco" "Mahjong Pon Chin Kan (Japan set 1)" */
	DRIVER( ponchina ) /* 1991 "Visco" "Mahjong Pon Chin Kan (Japan set 2)" */
	DRIVER( pzlbowl ) /* 1999 "Nihon System / Moss" "Puzzle De Bowling (Japan)" */
	DRIVER( qzkklgy2 ) /* 1993 "Tecmo" "Quiz Kokology 2" */
	DRIVER( qzkklogy ) /* 1992 "Tecmo" "Quiz Kokology" */
	DRIVER( renju ) /* 1994 "Visco" "Renju Kizoku" */
	DRIVER( rezon ) /* 1991 "Allumer" "Rezon" */
	DRIVER( rezont ) /* 1992 "Allumer (Taito license)" "Rezon (Taito)" */
	DRIVER( ryorioh ) /* 1998 "Visco" "Gourmet Battle Quiz Ryohrioh CooKing (Japan)" */
	DRIVER( sokonuke ) /* 1995 "Sammy Industries" "Sokonuke Taisen Game (Japan)" */
	DRIVER( speedatk ) /* 1984 "Seta Kikaku Corp." "Speed Attack!" */
	DRIVER( srmp2 ) /* 1987 "Seta" "Super Real Mahjong Part 2 (Japan)" */
	DRIVER( srmp3 ) /* 1988 "Seta" "Super Real Mahjong Part 3 (Japan)" */
	DRIVER( srmp4 ) /* 1993 "Seta" "Super Real Mahjong PIV (Japan)" */
	DRIVER( srmp4o ) /* 1993 "Seta" "Super Real Mahjong PIV (Japan */
	DRIVER( srmp7 ) /* 1997 "Seta" "Super Real Mahjong P7 (Japan)" */
	DRIVER( stg ) /* 1991 "Athena / Tecmo" "Strike Gunner S.T.G" */
	DRIVER( stmblade ) /* 1996 "Visco" "Storm Blade (US)" */
	DRIVER( survarts ) /* 1993 "Sammy" "Survival Arts (World)" */
	DRIVER( survartu ) /* 1993 "American Sammy" "Survival Arts (USA)" */
	DRIVER( sxyreac2 ) /* 1999 "Sammy" "Pachinko Sexy Reaction 2 (Japan)" */
	DRIVER( sxyreact ) /* 1998 "Sammy" "Pachinko Sexy Reaction (Japan)" */
	DRIVER( thunderl ) /* 1990 "Seta" "Thunder & Lightning" */
	DRIVER( tndrcade ) /* 1987 "[Seta] (Taito license)" "Thundercade / Twin Formation" */
	DRIVER( tndrcadj ) /* 1987 "[Seta] (Taito license)" "Tokusyu Butai U.A.G. (Japan)" */
	DRIVER( triplfun ) /* 1993 "bootleg" "Triple Fun" */
	DRIVER( twineag2 ) /* 1994 "Seta" "Twin Eagle II - The Rescue Mission" */
	DRIVER( twineagl ) /* 1988 "Seta (Taito license)" "Twin Eagle - Revenge Joe's Brother" */
	DRIVER( ultrax ) /* 1995 "Banpresto + Tsuburaya Prod." "Ultra X Weapons / Ultra Keibitai" */
	DRIVER( umanclub ) /* 1992 "Tsuburaya Prod. / Banpresto" "Ultraman Club - Tatakae! Ultraman Kyoudai!!" */
	DRIVER( usclssic ) /* 1989 "Seta" "U.S. Classic" */
	DRIVER( utoukond ) /* 1993 "Banpresto + Tsuburaya Prod." "Ultra Toukon Densetsu (Japan)" */
	DRIVER( vasara ) /* 2000 "Visco" "Vasara" */
	DRIVER( vasara2 ) /* 2001 "Visco" "Vasara 2 (set 1)" */
	DRIVER( vasara2a ) /* 2001 "Visco" "Vasara 2 (set 2)" */
	DRIVER( wiggie ) /* 1994 "Promat" "Wiggie Waggie" */
	DRIVER( wits ) /* 1989 "Athena (Visco license)" "Wit's (Japan)" */
	DRIVER( wrofaero ) /* 1993 "Yang Cheng" "War of Aero - Project MEIOU" */
	DRIVER( yujan ) /* 1999 "Yubis / T.System" "Yu-Jan" */
	DRIVER( yuka ) /* 1999 "Yubis / T.System" "Yu-Ka" */
	DRIVER( zingzip ) /* 1992 "Allumer + Tecmo" "Zing Zing Zip" */
	DRIVER( zombraid ) /* 1995 "American Sammy" "Zombie Raid (US)" */
#endif
#ifdef LINK_SHARED
#endif
#ifdef LINK_SIGMA
	DRIVER( arcadia ) /* 1980 "Sigma Enterprises Inc." "Waga Seishun no Arcadia" */
	DRIVER( nyny ) /* 1980 "Sigma Enterprises Inc." "New York New York" */
	DRIVER( nynyg ) /* 1980 "Sigma Enterprises Inc. (Gottlieb license)" "New York New York (Gottlieb)" */
	DRIVER( r2dtank ) /* 1980 "Sigma Enterprises Inc." "R2D Tank" */
	DRIVER( spiders ) /* 1981 "Sigma Enterprises Inc." "Spiders (set 1)" */
	DRIVER( spiders2 ) /* 1981 "Sigma Enterprises Inc." "Spiders (set 2)" */
	DRIVER( spinner ) /* 1981 "bootleg" "Spinner" */
#endif
#ifdef LINK_SNK
	DRIVER( alphamis ) /* 1985 "SNK" "Alpha Mission" */
	DRIVER( aso ) /* 1985 "SNK" "ASO - Armored Scrum Object" */
	DRIVER( athena ) /* 1986 "SNK" "Athena" */
	DRIVER( bbusters ) /* 1989 "SNK" "Beast Busters" */
	DRIVER( bermudaa ) /* 1987 "SNK" "Bermuda Triangle (US older version)" */
	DRIVER( bermudao ) /* 1987 "SNK" "Bermuda Triangle (Japan old version)" */
	DRIVER( bermudat ) /* 1987 "SNK" "Bermuda Triangle (Japan)" */
	DRIVER( canvas ) /* 1985 "SNK" "Canvas Croquis" */
	DRIVER( chameleo ) /* 1983 "Jaleco" "Chameleon" */
	DRIVER( chopper ) /* 1988 "SNK" "Chopper I (US set 1)" */
	DRIVER( choppera ) /* 1988 "SNK" "Chopper I (US set 2)" */
	DRIVER( chopperb ) /* 1988 "SNK" "Chopper I (US set 3)" */
	DRIVER( countryc ) /* 1988 "SNK" "Country Club" */
	DRIVER( dogosoke ) /* 1986 "SNK" "Dogou Souken" */
	DRIVER( dogosokj ) /* 1986 "bootleg" "Dogou Souken (Joystick bootleg)" */
	DRIVER( fantasy ) /* 1981 "SNK" "Fantasy (World)" */
	DRIVER( fantasyj ) /* 1981 "SNK" "Fantasy (Japan)" */
	DRIVER( fantasyu ) /* 1981 "[SNK] (Rock-Ola license)" "Fantasy (US)" */
	DRIVER( fitegol2 ) /* 1988 "SNK" "Fighting Golf (US)" */
	DRIVER( fitegolf ) /* 1988 "SNK" "Fighting Golf (World?)" */
	DRIVER( ftsoccer ) /* 1988 "SNK" "Fighting Soccer" */
	DRIVER( gensitou ) /* 1989 "SNK" "Genshi-Tou 1930's" */
	DRIVER( gwar ) /* 1987 "SNK" "Guerrilla War (US)" */
	DRIVER( gwara ) /* 1987 "SNK" "Guerrilla War (Version 1)" */
	DRIVER( gwarb ) /* 1987 "bootleg" "Guerrilla War (bootleg)" */
	DRIVER( gwarj ) /* 1987 "SNK" "Guevara (Japan)" */
	DRIVER( hal21 ) /* 1985 "SNK" "HAL21" */
	DRIVER( hal21j ) /* 1985 "SNK" "HAL21 (Japan)" */
	DRIVER( ikari ) /* 1986 "SNK" "Ikari Warriors (US)" */
	DRIVER( ikari3 ) /* 1989 "SNK" "Ikari III - The Rescue (Rotary Joystick)" */
	DRIVER( ikari3nr ) /* 1989 "SNK" "Ikari III - The Rescue (8-Way Joystick)" */
	DRIVER( ikarijp ) /* 1986 "SNK" "Ikari (Japan)" */
	DRIVER( ikarijpb ) /* 1986 "bootleg" "Ikari (Japan bootleg)" */
	DRIVER( jcross ) /* 1984 "SNK" "Jumping Cross" */
	DRIVER( joyfulr ) /* 1983 "SNK" "Joyful Road (Japan)" */
	DRIVER( lasso ) /* 1982 "SNK" "Lasso" */
	DRIVER( legofair ) /* 1988 "SNK" "Koukuu Kihei Monogatari - The Legend of Air Cavalry (Japan)" */
	DRIVER( madcrash ) /* 1984 "SNK" "Mad Crasher" */
	DRIVER( mainsnk ) /* 1984 "SNK" "Main Event (1984)" */
	DRIVER( marvins ) /* 1983 "SNK" "Marvin's Maze" */
	DRIVER( mechatt ) /* 1989 "SNK" "Mechanized Attack" */
	DRIVER( mechattu ) /* 1989 "SNK" "Mechanized Attack (US)" */
	DRIVER( mnchmobl ) /* 1983 "SNK (Centuri license)" "Munch Mobile (US)" */
	DRIVER( nibbler ) /* 1982 "Rock-Ola" "Nibbler (set 1)" */
	DRIVER( nibblera ) /* 1982 "Rock-Ola" "Nibbler (set 2)" */
	DRIVER( nibblerb ) /* 1982 "Rock-Ola" "Nibbler (set 3)" */
	DRIVER( nibblero ) /* 1983 "Olympia" "Nibbler (Olympia)" */
	DRIVER( pballoon ) /* 1982 "SNK" "Pioneer Balloon" */
	DRIVER( pinbo ) /* 1984 "Jaleco" "Pinbo (set 1)" */
	DRIVER( pinboa ) /* 1984 "Jaleco" "Pinbo (set 2)" */
	DRIVER( pinbos ) /* 1985 "bootleg?" "Pinbo (Strike)" */
	DRIVER( pow ) /* 1988 "SNK" "P.O.W. - Prisoners of War (US)" */
	DRIVER( powj ) /* 1988 "SNK" "Datsugoku - Prisoners of War (Japan)" */
	DRIVER( prehisle ) /* 1989 "SNK" "Prehistoric Isle in 1930 (World)" */
	DRIVER( prehislu ) /* 1989 "SNK of America" "Prehistoric Isle in 1930 (US)" */
	DRIVER( psychos ) /* 1987 "SNK" "Psycho Soldier (US)" */
	DRIVER( psychosj ) /* 1987 "SNK" "Psycho Soldier (Japan)" */
	DRIVER( sasuke ) /* 1980 "SNK" "Sasuke vs. Commander" */
	DRIVER( satansat ) /* 1981 "SNK" "Satan of Saturn" */
	DRIVER( searchar ) /* 1989 "SNK" "SAR - Search And Rescue (World)" */
	DRIVER( sercharj ) /* 1989 "SNK" "SAR - Search And Rescue (Japan)" */
	DRIVER( sercharu ) /* 1989 "SNK" "SAR - Search And Rescue (US)" */
	DRIVER( sgladiat ) /* 1984 "SNK" "Gladiator 1984" */
	DRIVER( streets1 ) /* 1989 "SNK" "Street Smart (US version 1)" */
	DRIVER( streetsj ) /* 1989 "SNK" "Street Smart (Japan version 1)" */
	DRIVER( streetsm ) /* 1989 "SNK" "Street Smart (US version 2)" */
	DRIVER( streetsw ) /* 1989 "SNK" "Street Smart (World version 1)" */
	DRIVER( tdfever ) /* 1987 "SNK" "TouchDown Fever" */
	DRIVER( tdfever2 ) /* 1988 "SNK" "TouchDown Fever 2" */
	DRIVER( tdfeverj ) /* 1987 "SNK" "TouchDown Fever (Japan)" */
	DRIVER( tnk3 ) /* 1985 "SNK" "T.N.K. III (US)" */
	DRIVER( tnk3j ) /* 1985 "SNK" "T.A.N.K. (Japan)" */
	DRIVER( vangrd2 ) /* 1984 "SNK" "Vanguard II" */
	DRIVER( vangrdce ) /* 1981 "SNK (Centuri license)" "Vanguard (Centuri)" */
	DRIVER( vanguard ) /* 1981 "SNK" "Vanguard (SNK)" */
	DRIVER( vanguarj ) /* 1981 "SNK" "Vanguard (Japan)" */
	DRIVER( victroad ) /* 1986 "SNK" "Victory Road" */
	DRIVER( worldwar ) /* 1987 "SNK" "World Wars (World)" */
	DRIVER( wwjgtin ) /* 1984 "Jaleco / Casio" "Wai Wai Jockey Gate-In!" */
	DRIVER( zarzon ) /* 1981 "[SNK] (Taito America license)" "Zarzon" */
#endif
#ifdef LINK_STERN
	DRIVER( astinvad ) /* 1980 "Stern" "Astro Invader" */
	DRIVER( berzerk ) /* 1980 "Stern" "Berzerk (set 1)" */
	DRIVER( berzerk1 ) /* 1980 "Stern" "Berzerk (set 2)" */
	DRIVER( frenzy ) /* 1982 "Stern" "Frenzy" */
	DRIVER( greatgun ) /* 1983 "Stern" "Great Guns" */
	DRIVER( kamikaze ) /* 1979 "Leijac" "Kamikaze" */
	DRIVER( kosmokil ) /* 19?? "bootleg" "Kosmo Killer" */
	DRIVER( spaceinj ) /* 1980 "Shoei" "Space Intruder (Japan)" */
	DRIVER( spaceint ) /* 1980 "Shoei" "Space Intruder" */
	DRIVER( spcking2 ) /* 1979 "Konami" "Space King 2" */
#endif
#ifdef LINK_SUN
	DRIVER( arabian ) /* 1983 "Sun Electronics" "Arabian" */
	DRIVER( arabiana ) /* 1983 "[Sun Electronics] (Atari license)" "Arabian (Atari)" */
	DRIVER( blocken ) /* 1994 "KID / Visco" "Blocken (Japan)" */
	DRIVER( farmer ) /* 1985 "Sun Electronics" "Farmers Rebellion" */
	DRIVER( fnkyfish ) /* 1981 "Sun Electronics" "Funky Fish" */
	DRIVER( guiness ) /* 1984 "Sun Electronics" "The Guiness (Japan)" */
	DRIVER( heberpop ) /* 1994 "Sunsoft / Atlus" "Hebereke no Popoon (Japan)" */
	DRIVER( ikki ) /* 1985 "Sun Electronics" "Ikki (Japan)" */
	DRIVER( kangaroa ) /* 1982 "[Sun Electronics] (Atari license)" "Kangaroo (Atari)" */
	DRIVER( kangarob ) /* 1982 "bootleg" "Kangaroo (bootleg)" */
	DRIVER( kangaroo ) /* 1982 "Sun Electronics" "Kangaroo" */
	DRIVER( markham ) /* 1983 "Sun Electronics" "Markham" */
	DRIVER( pettanp ) /* 1984 "Sun Electronics" "Pettan Pyuu (Japan)" */
	DRIVER( route16 ) /* 1981 "Tehkan/Sun (Centuri license)" "Route 16" */
	DRIVER( route16a ) /* 1981 "Tehkan/Sun (Centuri license)" "Route 16 (set 2)" */
	DRIVER( route16b ) /* 1981 "bootleg" "Route 16 (bootleg)" */
	DRIVER( routex ) /* 1981 "bootleg" "Route X (bootleg)" */
	DRIVER( shangha2 ) /* 1989 "Sunsoft" "Shanghai II (Japan)" */
	DRIVER( shangha3 ) /* 1993 "Sunsoft" "Shanghai III (Japan)" */
	DRIVER( shanghai ) /* 1988 "Sunsoft" "Shanghai (Japan)" */
	DRIVER( spacecho ) /* 1980 "bootleg" "Space Echo" */
	DRIVER( speakres ) /* 1980 "Sun Electronics" "Speak & Rescue" */
	DRIVER( stratvox ) /* 1980 "[Sun Electronics] (Taito license)" "Stratovox" */
	DRIVER( strnskil ) /* 1984 "Sun Electronics" "Strength & Skill" */
	DRIVER( ttmahjng ) /* 1981 "Taito" "Mahjong" */
#endif
#ifdef LINK_SUNA
	DRIVER( bssoccer ) /* 1996 "SunA" "Back Street Soccer" */
	DRIVER( goindol ) /* 1987 "Sun a Electronics" "Goindol (World)" */
	DRIVER( goindolj ) /* 1987 "Sun a Electronics" "Goindol (Japan)" */
	DRIVER( goindolu ) /* 1987 "Sun a Electronics" "Goindol (US)" */
	DRIVER( hardhea2 ) /* 1991 "SunA" "Hard Head 2 (v2.0)" */
	DRIVER( hardhead ) /* 1988 "SunA" "Hard Head" */
	DRIVER( hardhedb ) /* 1988 "bootleg" "Hard Head (bootleg)" */
	DRIVER( homo ) /* 1987 "bootleg" "Homo" */
	DRIVER( pop_hh ) /* 1988 "bootleg" "Popper (Hard Head bootleg)" */
	DRIVER( rranger ) /* 1988 "SunA (Sharp Image license)" "Rough Ranger (v2.0)" */
	DRIVER( uballoon ) /* 1996 "SunA" "Ultra Balloon" */
#endif
#ifdef LINK_TAD
	DRIVER( bloodbra ) /* 1990 "Tad" "Blood Bros. (set 2)" */
	DRIVER( bloodbro ) /* 1990 "Tad" "Blood Bros. (set 1)" */
	DRIVER( cabal ) /* 1988 "Tad (Fabtek license)" "Cabal (US set 1)" */
	DRIVER( cabal2 ) /* 1988 "Tad (Fabtek license)" "Cabal (US set 2)" */
	DRIVER( cabalbl ) /* 1988 "bootleg" "Cabal (bootleg)" */
	DRIVER( cupsoc ) /* 1992 "Seibu" "Seibu Cup Soccer" */
	DRIVER( goal92 ) /* 1992 "bootleg" "Goal! '92" */
	DRIVER( skysmash ) /* 1990 "Nihon System" "Sky Smasher" */
	DRIVER( toki ) /* 1989 "Tad" "Toki (World set 1)" */
	DRIVER( tokia ) /* 1989 "Tad" "Toki (World set 2)" */
	DRIVER( tokib ) /* 1989 "bootleg" "Toki (bootleg)" */
	DRIVER( tokij ) /* 1989 "Tad" "JuJu Densetsu (Japan)" */
	DRIVER( tokiu ) /* 1989 "Tad (Fabtek license)" "Toki (US)" */
	DRIVER( weststry ) /* 1990 "bootleg" "West Story" */
#endif
#ifdef LINK_TAITO
	DRIVER( 40love ) /* 1984 "Taito Corporation" "Forty-Love" */
	DRIVER( akkanvdr ) /* 1995 "Taito Corporation" "Akkanbeder (Ver 2.5J 1995/06/14)" */
	DRIVER( alphaxz ) /* 1986 "Ed / Woodplace Inc." "The Alphax Z (Japan)" */
	DRIVER( alpine ) /* 1982 "Taito Corporation" "Alpine Ski (set 1)" */
	DRIVER( alpinea ) /* 1982 "Taito Corporation" "Alpine Ski (set 2)" */
	DRIVER( aquajack ) /* 1990 "Taito Corporation Japan" "Aqua Jack (World)" */
	DRIVER( aquajckj ) /* 1990 "Taito Corporation" "Aqua Jack (Japan)" */
	DRIVER( arabiamj ) /* 1992 "Taito Corporation" "Arabian Magic (Ver 1.0J 1992/07/06)" */
	DRIVER( arabiamu ) /* 1992 "Taito America Corporation" "Arabian Magic (Ver 1.0A 1992/07/06)" */
	DRIVER( arabianm ) /* 1992 "Taito Corporation Japan" "Arabian Magic (Ver 1.0O 1992/07/06)" */
	DRIVER( arkangc ) /* 1986 "bootleg" "Arkanoid (Game Corporation bootleg)" */
	DRIVER( arkanoid ) /* 1986 "Taito Corporation Japan" "Arkanoid (World)" */
	DRIVER( arkatayt ) /* 1986 "bootleg" "Arkanoid (Tayto bootleg */
	DRIVER( arkatour ) /* 1987 "Taito America Corporation (Romstar license)" "Tournament Arkanoid (US)" */
	DRIVER( arkbloc2 ) /* 1986 "bootleg" "Block (Game Corporation bootleg)" */
	DRIVER( arkblock ) /* 1986 "bootleg" "Block (bootleg */
	DRIVER( arkcrsn5 ) /* 2023 "Abyss leader of the 80's" "Cruisin'5 Demo (Arkanoid Hw) By Abyss (2023 */
	DRIVER( arknid2j ) /* 1987 "Taito Corporation" "Arkanoid - Revenge of DOH (Japan)" */
	DRIVER( arknid2u ) /* 1987 "Taito America Corporation (Romstar license)" "Arkanoid - Revenge of DOH (US)" */
	DRIVER( arknoid2 ) /* 1987 "Taito Corporation Japan" "Arkanoid - Revenge of DOH (World)" */
	DRIVER( arknoidj ) /* 1986 "Taito Corporation" "Arkanoid (Japan)" */
	DRIVER( arknoidu ) /* 1986 "Taito America Corporation (Romstar license)" "Arkanoid (US)" */
	DRIVER( arknoiuo ) /* 1986 "Taito America Corporation (Romstar license)" "Arkanoid (US */
	DRIVER( arkretrn ) /* 1997 "Taito Corporation" "Arkanoid Returns (Ver 2.02J 1997/02/10)" */
	DRIVER( ashnojoe ) /* 1990 "WAVE / Taito Corporation" "Ashita no Joe (Japan)" */
	DRIVER( ashura ) /* 1990 "Taito Corporation" "Ashura Blaster (Japan)" */
	DRIVER( ashurau ) /* 1990 "Taito America Corporation" "Ashura Blaster (US)" */
	DRIVER( asuka ) /* 1988 "Taito Corporation" "Asuka & Asuka (Japan)" */
	DRIVER( ballbros ) /* 1992 "East Technology" "Balloon Brothers" */
	DRIVER( benberob ) /* 1984 "Taito" "Ben Bero Beh (Japan)" */
	DRIVER( bigevglf ) /* 1986 "Taito America Corporation" "Big Event Golf" */
	DRIVER( bioatack ) /* 1983 "Taito Corporation (Fox Video Games license)" "Bio Attack" */
	DRIVER( bking ) /* 1982 "Taito Corporation" "Birdie King" */
	DRIVER( bking2 ) /* 1983 "Taito Corporation" "Birdie King 2" */
	DRIVER( bking3 ) /* 1984 "Taito Corporation" "Birdie King 3" */
	DRIVER( boblbobl ) /* 1986 "bootleg" "Bobble Bobble" */
	DRIVER( bonzeado ) /* 1988 "Taito Corporation Japan" "Bonze Adventure (World */
	DRIVER( bonzeadu ) /* 1988 "Taito America Corporation" "Bonze Adventure (US)" */
	DRIVER( bonzeadv ) /* 1988 "Taito Corporation Japan" "Bonze Adventure (World */
	DRIVER( bronx ) /* 1986 "bootleg" "Bronx" */
	DRIVER( bshark ) /* 1989 "Taito America Corporation" "Battle Shark (US)" */
	DRIVER( bsharkj ) /* 1989 "Taito Corporation" "Battle Shark (Japan)" */
	DRIVER( bubblem ) /* 1995 "Taito Corporation Japan" "Bubble Memories: The Story Of Bubble Bobble III (Ver 2.4O 1996/02/15)" */
	DRIVER( bubblemj ) /* 1995 "Taito Corporation" "Bubble Memories: The Story Of Bubble Bobble III (Ver 2.3J 1996/02/07)" */
	DRIVER( bubbobr1 ) /* 1986 "Taito America Corporation (Romstar license)" "Bubble Bobble (US)" */
	DRIVER( bublbob1 ) /* 1986 "Taito Corporation" "Bubble Bobble (older)" */
	DRIVER( bublbob2 ) /* 1994 "Taito Corporation Japan" "Bubble Bobble II (Ver 2.5O 1994/10/05)" */
	DRIVER( bublbobl ) /* 1986 "Taito Corporation" "Bubble Bobble" */
	DRIVER( bublbobr ) /* 1986 "Taito America Corporation (Romstar license)" "Bubble Bobble (US with mode select)" */
	DRIVER( bubsympe ) /* 1994 "Taito Corporation Japan" "Bubble Symphony (Ver 2.5O 1994/10/05)" */
	DRIVER( bubsymph ) /* 1994 "Taito Corporation" "Bubble Symphony (Ver 2.5J 1994/10/05)" */
	DRIVER( bubsympu ) /* 1994 "Taito America Corporation" "Bubble Symphony (Ver 2.5A 1994/10/05)" */
	DRIVER( buggychl ) /* 1984 "Taito Corporation" "Buggy Challenge" */
	DRIVER( buggycht ) /* 1984 "Taito Corporation (Tecfri license)" "Buggy Challenge (Tecfri)" */
	DRIVER( cachat ) /* 1993 "Taito Corporation" "Cachat (Japan)" */
	DRIVER( cadash ) /* 1989 "Taito Corporation Japan" "Cadash (World)" */
	DRIVER( cadashf ) /* 1989 "Taito Corporation Japan" "Cadash (France)" */
	DRIVER( cadashi ) /* 1989 "Taito Corporation Japan" "Cadash (Italy)" */
	DRIVER( cadashj ) /* 1989 "Taito Corporation" "Cadash (Japan)" */
	DRIVER( cadashu ) /* 1989 "Taito America Corporation" "Cadash (US)" */
	DRIVER( cameltrj ) /* 1989 "Taito Corporation" "Cameltry (Japan)" */
	DRIVER( cameltry ) /* 1989 "Taito America Corporation" "Cameltry (US)" */
	DRIVER( camltrua ) /* 1989 "Taito America Corporation" "Cameltry (US */
	DRIVER( chaknpop ) /* 1983 "Taito Corporation" "Chack'n Pop" */
	DRIVER( champbwl ) /* 1989 "Seta / Romstar Inc." "Championship Bowling" */
	DRIVER( champwr ) /* 1989 "Taito Corporation Japan" "Champion Wrestler (World)" */
	DRIVER( champwrj ) /* 1989 "Taito Corporation" "Champion Wrestler (Japan)" */
	DRIVER( champwru ) /* 1989 "Taito America Corporation" "Champion Wrestler (US)" */
	DRIVER( chasehq ) /* 1988 "Taito Corporation Japan" "Chase H.Q. (World)" */
	DRIVER( chasehqj ) /* 1988 "Taito Corporation" "Chase H.Q. (Japan)" */
	DRIVER( chukatai ) /* 1988 "Taito Corporation Japan" "Chuka Taisen (World)" */
	DRIVER( chukataj ) /* 1988 "Taito Corporation" "Chuka Taisen (Japan)" */
	DRIVER( chukatau ) /* 1988 "Taito America Corporation" "Chuka Taisen (US)" */
	DRIVER( cleopatr ) /* 1996 "Taito Corporation" "Cleopatra Fortune (Ver 2.1J 1996/09/05)" */
	DRIVER( colt ) /* 1986 "bootleg" "Colt" */
	DRIVER( commandw ) /* 1992 "Taito Corporation" "Command War - Super Special Battle & War Game (Ver 0.0J) (Prototype)" */
	DRIVER( complexx ) /* 1984 "Taito America Corporation" "Complex X" */
	DRIVER( contcirc ) /* 1987 "Taito Corporation Japan" "Continental Circus (World)" */
	DRIVER( contcrcu ) /* 1987 "Taito America Corporation" "Continental Circus (US set 1)" */
	DRIVER( contcrua ) /* 1987 "Taito America Corporation" "Continental Circus (US set 2)" */
	DRIVER( crbalon2 ) /* 1980 "Taito Corporation" "Crazy Balloon (set 2)" */
	DRIVER( crbaloon ) /* 1980 "Taito Corporation" "Crazy Balloon (set 1)" */
	DRIVER( crimec ) /* 1989 "Taito Corporation Japan" "Crime City (World)" */
	DRIVER( crimecj ) /* 1989 "Taito Corporation" "Crime City (Japan)" */
	DRIVER( crimecu ) /* 1989 "Taito America Corporation" "Crime City (US)" */
	DRIVER( cubybop ) /* 199? "Taito Corporation" "Cuby Bop (Location Test)" */
	DRIVER( cupfinal ) /* 1993 "Taito Corporation Japan" "Taito Cup Finals (Ver 1.0O 1993/02/28)" */
	DRIVER( cyclshtg ) /* 1986 "Taito" "Cycle Shooting" */
	DRIVER( daisenpu ) /* 1989 "Taito Corporation" "Daisenpu (Japan)" */
	DRIVER( dankuga ) /* 1994 "Taito Corporation" "Dan-Ku-Ga (Ver 0.0J 1994/12/13) (Prototype)" */
	DRIVER( darius ) /* 1986 "Taito Corporation Japan" "Darius (World)" */
	DRIVER( darius2 ) /* 1989 "Taito Corporation" "Darius II (Japan)" */
	DRIVER( darius2d ) /* 1989 "Taito Corporation" "Darius II (dual screen) (Japan)" */
	DRIVER( dariuse ) /* 1986 "Taito Corporation" "Darius (Extra) (Japan)" */
	DRIVER( dariusg ) /* 1994 "Taito Corporation Japan" "Darius Gaiden - Silver Hawk (Ver 2.5O 1994/09/19)" */
	DRIVER( dariusgj ) /* 1994 "Taito Corporation" "Darius Gaiden - Silver Hawk (Ver 2.5J 1994/09/19)" */
	DRIVER( dariusgu ) /* 1994 "Taito America Corporation" "Darius Gaiden - Silver Hawk (Ver 2.5A 1994/09/19)" */
	DRIVER( dariusgx ) /* 1994 "Taito Corporation" "Darius Gaiden - Silver Hawk Extra Version (Ver 2.7J 1995/03/06) (Official Hack)" */
	DRIVER( dariusj ) /* 1986 "Taito Corporation" "Darius (Japan)" */
	DRIVER( dariuso ) /* 1986 "Taito Corporation" "Darius (Japan old version)" */
	DRIVER( darkmist ) /* 1986 "Taito" "The Lost Castle In Darkmist" */
	DRIVER( dblaxle ) /* 1991 "Taito America Corporation" "Double Axle (US)" */
	DRIVER( deadconj ) /* 1992 "Taito Corporation" "Dead Connection (Japan)" */
	DRIVER( deadconx ) /* 1992 "Taito Corporation Japan" "Dead Connection (World)" */
	DRIVER( dinorex ) /* 1992 "Taito Corporation Japan" "Dino Rex (World)" */
	DRIVER( dinorexj ) /* 1992 "Taito Corporation" "Dino Rex (Japan)" */
	DRIVER( dinorexu ) /* 1992 "Taito America Corporation" "Dino Rex (US)" */
	DRIVER( dleague ) /* 1990 "Taito Corporation" "Dynamite League (Japan)" */
	DRIVER( dondokdj ) /* 1989 "Taito Corporation" "Don Doko Don (Japan)" */
	DRIVER( dondokdu ) /* 1989 "Taito America Corporation" "Don Doko Don (US)" */
	DRIVER( dondokod ) /* 1989 "Taito Corporation Japan" "Don Doko Don (World)" */
	DRIVER( driftout ) /* 1991 "Visco" "Drift Out (Japan)" */
	DRIVER( drius2do ) /* 1989 "Taito Corporation" "Darius II (dual screen) (Japan old version)" */
	DRIVER( driveout ) /* 1991 "bootleg" "Drive Out" */
	DRIVER( drtoppel ) /* 1987 "Taito Corporation Japan" "Dr. Toppel's Adventure (World)" */
	DRIVER( drtopplj ) /* 1987 "Taito Corporation" "Dr. Toppel's Tankentai (Japan)" */
	DRIVER( drtopplu ) /* 1987 "Taito America Corporation" "Dr. Toppel's Adventure (US)" */
	DRIVER( dungenmu ) /* 1993 "Taito America Corporation" "Dungeon Magic (Ver 2.1A 1994/02/18)" */
	DRIVER( dungeonm ) /* 1993 "Taito Corporation Japan" "Dungeon Magic (Ver 2.1O 1994/02/18)" */
	DRIVER( earthjkr ) /* 1993 "Visco" "U.N. Defense Force: Earth Joker (Japan)" */
	DRIVER( elecyoy2 ) /* 1982 "Taito America Corporation" "The Electric Yo-Yo (set 2)" */
	DRIVER( elecyoyo ) /* 1982 "Taito America Corporation" "The Electric Yo-Yo (set 1)" */
	DRIVER( elevatob ) /* 1983 "bootleg" "Elevator Action (bootleg)" */
	DRIVER( elevator ) /* 1983 "Taito Corporation" "Elevator Action" */
	DRIVER( elvact2u ) /* 1994 "Taito America Corporation" "Elevator Action II (Ver 2.2A 1995/02/20)" */
	DRIVER( elvactr ) /* 1994 "Taito Corporation Japan" "Elevator Action Returns (Ver 2.2O 1995/02/20)" */
	DRIVER( elvactrj ) /* 1994 "Taito Corporation" "Elevator Action Returns (Ver 2.2J 1995/02/20)" */
	DRIVER( enforce ) /* 1988 "Taito Corporation" "Enforce (Japan)" */
	DRIVER( eto ) /* 1994 "Visco" "Kokontouzai Eto Monogatari (Japan)" */
	DRIVER( euroch92 ) /* 1992 "Taito Corporation Japan" "Euro Champ '92 (World)" */
	DRIVER( evilston ) /* 1990 "Spacy Industrial Ltd." */
	DRIVER( extrmatn ) /* 1987 "[Taito] World Games" "Extermination (US)" */
	DRIVER( exzisus ) /* 1987 "Taito Corporation" "Exzisus (Japan)" */
	DRIVER( fgoal ) /* 1979 "Taito" "Field Goal" */
	DRIVER( fgoala ) /* 1979 "Taito" "Field Goal (different)" */
	DRIVER( fhawk ) /* 1988 "Taito Corporation Japan" "Fighting Hawk (World)" */
	DRIVER( fhawkj ) /* 1988 "Taito Corporation" "Fighting Hawk (Japan)" */
	DRIVER( fieldday ) /* 1984 "Taito Corporation" "Field Day" */
	DRIVER( finalb ) /* 1988 "Taito Corporation Japan" "Final Blow (World)" */
	DRIVER( finalbj ) /* 1988 "Taito Corporation" "Final Blow (Japan)" */
	DRIVER( finalbu ) /* 1988 "Taito America Corporation" "Final Blow (US)" */
	DRIVER( flstory ) /* 1985 "Taito" "The FairyLand Story" */
	DRIVER( flstoryj ) /* 1985 "Taito" "The FairyLand Story (Japan)" */
	DRIVER( footchmp ) /* 1990 "Taito Corporation Japan" "Football Champ (World)" */
	DRIVER( frontlin ) /* 1982 "Taito Corporation" "Front Line" */
	DRIVER( fullthrl ) /* 1987 "Taito Corporation" "Full Throttle (Japan)" */
	DRIVER( galmedes ) /* 1992 "Visco" "Galmedes (Japan)" */
	DRIVER( gblchmp ) /* 1994 "Taito America Corporation" "Global Champion (Ver 2.1A 1994/07/29)" */
	DRIVER( gekirido ) /* 1995 "Taito Corporation" "Gekirindan (Ver 2.3J 1995/09/21)" */
	DRIVER( gigandes ) /* 1989 "East Technology" "Gigandes" */
	DRIVER( gigandsj ) /* 1989 "East Technology" "Gigandes (Japan)" */
	DRIVER( gladiatr ) /* 1986 "Taito America Corporation" "Gladiator (US)" */
	DRIVER( grchamp ) /* 1981 "Taito" "Grand Champion" */
	DRIVER( greatgur ) /* 1986 "Taito Corporation" "Great Gurianos (Japan?)" */
	DRIVER( groundfx ) /* 1992 "Taito Corporation" "Ground Effects / Super Ground Effects (Japan)" */
	DRIVER( growl ) /* 1990 "Taito Corporation Japan" "Growl (World)" */
	DRIVER( growlu ) /* 1990 "Taito America Corporation" "Growl (US)" */
	DRIVER( gseeker ) /* 1992 "Taito Corporation Japan" "Grid Seeker: Project Storm Hammer (Ver 1.3O)" */
	DRIVER( gseekerj ) /* 1992 "Taito Corporation" "Grid Seeker: Project Storm Hammer (Ver 1.3J)" */
	DRIVER( gseekeru ) /* 1992 "Taito America Corporation" "Grid Seeker: Project Storm Hammer (Ver 1.3A)" */
	DRIVER( gsword ) /* 1984 "Taito Corporation" "Great Swordsman (World?)" */
	DRIVER( gsword2 ) /* 1984 "Taito Corporation" "Great Swordsman (Japan?)" */
	DRIVER( gunbustr ) /* 1992 "Taito Corporation" "Gunbuster (Japan)" */
	DRIVER( gunfronj ) /* 1990 "Taito Corporation" "Gun Frontier (Japan)" */
	DRIVER( gunfront ) /* 1990 "Taito Corporation Japan" "Gun & Frontier (World)" */
	DRIVER( gunlock ) /* 1993 "Taito Corporation Japan" "Gunlock (Ver 2.3O 1994/01/20)" */
	DRIVER( halleycj ) /* 1986 "Taito Corporation" "Halley's Comet (Japan set 2)" */
	DRIVER( halleys ) /* 1986 "Taito America Corporation (Coin-It license)" "Halley's Comet (US)" */
	DRIVER( halleysc ) /* 1986 "Taito Corporation" "Halley's Comet (Japan set 1)" */
	DRIVER( hitice ) /* 1990 "Williams" "Hit the Ice (US)" */
	DRIVER( horshoes ) /* 1990 "Taito America Corporation" "American Horseshoes (US)" */
	DRIVER( hotsmash ) /* 1987 "Taito" "Vs. Hot Smash" */
	DRIVER( hthero ) /* 1990 "Taito Corporation" "Hat Trick Hero (Japan)" */
	DRIVER( hthero93 ) /* 1993 "Taito Corporation" "Hat Trick Hero '93 (Ver 1.0J 1993/02/28)" */
	DRIVER( hthero94 ) /* 1994 "Taito America Corporation" "Hat Trick Hero '94 (Ver 2.2A 1994/05/26)" */
	DRIVER( hthero95 ) /* 1994 "Taito Corporation" "Hat Trick Hero '95 (Ver 2.5J 1994/11/03)" */
	DRIVER( hthro95u ) /* 1994 "Taito America Corporation" "Hat Trick Hero '95 (Ver 2.5A 1994/11/03)" */
	DRIVER( hwrace ) /* 1983 "Taito Corporation" "High Way Race" */
	DRIVER( insectx ) /* 1989 "Taito Corporation Japan" "Insector X (World)" */
	DRIVER( intcup94 ) /* 1994 "Taito Corporation Japan" "International Cup '94 (Ver 2.2O 1994/05/26)" */
	DRIVER( jigkmgri ) /* 1988 "Taito Corporation" "Jigoku Meguri (Japan)" */
	DRIVER( jollyjgr ) /* 1982 "Taito Corporation" "Jolly Jogger" */
	DRIVER( josvolly ) /* 1983 "Taito Corporation" "Joshi Volleyball" */
	DRIVER( jumping ) /* 1989 "bootleg" "Jumping" */
	DRIVER( jungleh ) /* 1982 "Taito America Corporation" "Jungle Hunt (US)" */
	DRIVER( junglek ) /* 1982 "Taito Corporation" "Jungle King (Japan)" */
	DRIVER( junglhbr ) /* 1983 "Taito do Brasil" "Jungle Hunt (Brazil)" */
	DRIVER( junglkj2 ) /* 1982 "Taito Corporation" "Jungle King (Japan */
	DRIVER( kabukiz ) /* 1988 "Taito Corporation Japan" "Kabuki-Z (World)" */
	DRIVER( kabukizj ) /* 1988 "Taito Corporation" "Kabuki-Z (Japan)" */
	DRIVER( kageki ) /* 1988 "Taito America Corporation (Romstar license)" "Kageki (US)" */
	DRIVER( kagekia ) /* 1992 "Taito Corporation" "Kageki (World? */
	DRIVER( kagekij ) /* 1988 "Taito Corporation" "Kageki (Japan)" */
	DRIVER( kaiserkj ) /* 1994 "Taito Corporation" "Kaiser Knuckle (Ver 2.1J 1994/07/29)" */
	DRIVER( kaiserkn ) /* 1994 "Taito Corporation Japan" "Kaiser Knuckle (Ver 2.1O 1994/07/29)" */
	DRIVER( kicknrun ) /* 1986 "Taito Corporation" "Kick and Run" */
	DRIVER( kikikai ) /* 1986 "Taito Corporation" "KiKi KaiKai" */
	DRIVER( kikstart ) /* 1984 "Taito Corporation" "Kick Start Wheelie King" */
	DRIVER( kirameki ) /* 1997 "Taito Corporation" "Kirameki Star Road (Ver 2.10J 1997/08/29)" */
	DRIVER( knightb ) /* 1986 "bootleg" "Knight Boy" */
	DRIVER( koshien ) /* 1990 "Taito Corporation" "Ah Eikou no Koshien (Japan)" */
	DRIVER( kram ) /* 1982 "Taito America Corporation" "Kram (set 1)" */
	DRIVER( kram2 ) /* 1982 "Taito America Corporation" "Kram (set 2)" */
	DRIVER( kram3 ) /* 1982 "Taito America Corporation" "Kram (encrypted)" */
	DRIVER( ksayakyu ) /* 1985 "Taito Corporation" "Kusayakyuu" */
	DRIVER( ktiger2 ) /* 1995 "Taito Corporation" "Kyukyoku Tiger II (Ver 2.1J 1995/11/30)" */
	DRIVER( kurikina ) /* 1988 "Taito Corporation Japan" "Kuri Kinton (World */
	DRIVER( kurikinj ) /* 1988 "Taito Corporation" "Kuri Kinton (Japan)" */
	DRIVER( kurikint ) /* 1988 "Taito Corporation Japan" "Kuri Kinton (World)" */
	DRIVER( kurikinu ) /* 1988 "Taito America Corporation" "Kuri Kinton (US)" */
	DRIVER( kyustrkr ) /* 1989 "East Technology" "Last Striker / Kyuukyoku no Striker" */
	DRIVER( landmakr ) /* 1998 "Taito Corporation" "Land Maker (Ver 2.01J 1998/06/01)" */
	DRIVER( landmkrp ) /* 1998 "Taito Corporation" "Land Maker (Ver 2.02O 1998/06/02) (Prototype)" */
	DRIVER( lightbr ) /* 1993 "Taito Corporation" "Light Bringer (Ver 2.1J 1994/02/18)" */
	DRIVER( liquidk ) /* 1990 "Taito Corporation Japan" "Liquid Kids (World)" */
	DRIVER( liquidku ) /* 1990 "Taito America Corporation" "Liquid Kids (US)" */
	DRIVER( lkage ) /* 1984 "Taito Corporation" "The Legend of Kage" */
	DRIVER( lkageb ) /* 1984 "bootleg" "The Legend of Kage (bootleg set 1)" */
	DRIVER( lkageb2 ) /* 1984 "bootleg" "The Legend of Kage (bootleg set 2)" */
	DRIVER( lkageb3 ) /* 1984 "bootleg" "The Legend of Kage (bootleg set 3)" */
	DRIVER( lsasquad ) /* 1986 "Taito" "Land Sea Air Squad / Riku Kai Kuu Saizensen" */
	DRIVER( m660 ) /* 1986 "[Woodplace Inc.] Taito America Corporation" "Mission 660 (US)" */
	DRIVER( m660b ) /* 1986 "bootleg" "Mission 660 (bootleg)" */
	DRIVER( m660j ) /* 1986 "[Woodplace Inc.] Taito Corporation" "Mission 660 (Japan)" */
	DRIVER( majest12 ) /* 1990 "Taito Corporation" "Majestic Twelve - The Space Invaders Part IV (Japan)" */
	DRIVER( marinedt ) /* 1981 "Taito" "Marine Date" */
	DRIVER( masterw ) /* 1989 "Taito Corporation Japan" "Master of Weapon (World)" */
	DRIVER( masterwu ) /* 1989 "Taito America Corporation" "Master of Weapon (US)" */
	DRIVER( megab ) /* 1989 "Taito Corporation Japan" "Mega Blast (World)" */
	DRIVER( megabj ) /* 1989 "Taito Corporation" "Mega Blast (Japan)" */
	DRIVER( metalb ) /* 1991 "Taito Corporation Japan" "Metal Black (World)" */
	DRIVER( metalbj ) /* 1991 "Taito Corporation" "Metal Black (Japan)" */
	DRIVER( mexico86 ) /* 1986 "bootleg" "Mexico 86" */
	DRIVER( minivadr ) /* 1990 "Taito Corporation" "Minivader" */
	DRIVER( missb2 ) /* 1996 "Alpha Co" "Miss Bubble 2" */
	DRIVER( mizubaku ) /* 1990 "Taito Corporation" "Mizubaku Daibouken (Japan)" */
	DRIVER( mjnquesb ) /* 1990 "Taito Corporation" "Mahjong Quest (No Nudity)" */
	DRIVER( mjnquest ) /* 1990 "Taito Corporation" "Mahjong Quest (Japan)" */
	DRIVER( mofflott ) /* 1989 "Taito Corporation" "Maze of Flott (Japan)" */
	DRIVER( msisaac ) /* 1985 "Taito Corporation" "Metal Soldier Isaac II" */
	DRIVER( nastar ) /* 1988 "Taito Corporation Japan" "Nastar (World)" */
	DRIVER( nastarw ) /* 1988 "Taito America Corporation" "Nastar Warrior (US)" */
	DRIVER( nightstr ) /* 1989 "Taito America Corporation" "Night Striker (US)" */
	DRIVER( ninjak ) /* 1990 "Taito Corporation Japan" "The Ninja Kids (World)" */
	DRIVER( ninjakj ) /* 1990 "Taito Corporation" "The Ninja Kids (Japan)" */
	DRIVER( ninjaku ) /* 1990 "Taito America Corporation" "The Ninja Kids (US)" */
	DRIVER( ninjaw ) /* 1987 "Taito Corporation Japan" "The Ninja Warriors (World)" */
	DRIVER( ninjawj ) /* 1987 "Taito Corporation" "The Ninja Warriors (Japan)" */
	DRIVER( nunchaku ) /* 1985 "Taito" "Nunchackun" */
	DRIVER( nycaptor ) /* 1985 "Taito" "N.Y. Captor" */
	DRIVER( ogonsiro ) /* 1986 "Taito Corporation" "Ohgon no Siro (Japan)" */
	DRIVER( onna34ra ) /* 1985 "Taito" "Onna Sansirou - Typhoon Gal (set 2)" */
	DRIVER( onna34ro ) /* 1985 "Taito" "Onna Sansirou - Typhoon Gal (set 1)" */
	DRIVER( opwolf ) /* 1987 "Taito Corporation Japan" "Operation Wolf (World)" */
	DRIVER( opwolf3 ) /* 1994 "Taito Corporation Japan" "Operation Wolf 3 (World)" */
	DRIVER( opwolf3u ) /* 1994 "Taito America Corporation" "Operation Wolf 3 (US)" */
	DRIVER( opwolfb ) /* 1987 "bootleg" "Operation Bear" */
	DRIVER( opwolfu ) /* 1987 "Taito America Corporation" "Operation Wolf (US)" */
	DRIVER( othunder ) /* 1988 "Taito Corporation Japan" "Operation Thunderbolt (World)" */
	DRIVER( othundrj ) /* 1988 "Taito Corporation" "Operation Thunderbolt (Japan)" */
	DRIVER( othundu ) /* 1988 "Taito America Corporation" "Operation Thunderbolt (US)" */
	DRIVER( othunduo ) /* 1988 "Taito America Corporation" "Operation Thunderbolt (US */
	DRIVER( paddle2 ) /* 1988 "bootleg" "Paddle 2" */
	DRIVER( palamed ) /* 1990 "Taito Corporation" "Palamedes (Japan)" */
	DRIVER( pbillian ) /* 1986 "Taito" "Prebillian" */
	DRIVER( pbobbl2j ) /* 1995 "Taito Corporation" "Puzzle Bobble 2 (Ver 2.2J 1995/07/20)" */
	DRIVER( pbobbl2u ) /* 1995 "Taito America Corporation" "Bust-A-Move Again (Ver 2.3A 1995/07/31)" */
	DRIVER( pbobbl2x ) /* 1995 "Taito Corporation" "Puzzle Bobble 2X (Ver 2.2J 1995/11/11)" */
	DRIVER( pbobbl3j ) /* 1996 "Taito Corporation" "Puzzle Bobble 3 (Ver 2.1J 1996/09/27)" */
	DRIVER( pbobbl3u ) /* 1996 "Taito Corporation" "Puzzle Bobble 3 (Ver 2.1A 1996/09/27)" */
	DRIVER( pbobbl4j ) /* 1997 "Taito Corporation" "Puzzle Bobble 4 (Ver 2.04J 1997/12/19)" */
	DRIVER( pbobbl4u ) /* 1997 "Taito Corporation" "Puzzle Bobble 4 (Ver 2.04A 1997/12/19)" */
	DRIVER( pbobble ) /* 1994 "Taito Corporation" "Puzzle Bobble (Japan */
	DRIVER( pbobble2 ) /* 1995 "Taito Corporation Japan" "Puzzle Bobble 2 (Ver 2.2O 1995/07/20)" */
	DRIVER( pbobble3 ) /* 1996 "Taito Corporation" "Puzzle Bobble 3 (Ver 2.1O 1996/09/27)" */
	DRIVER( pbobble4 ) /* 1997 "Taito Corporation" "Puzzle Bobble 4 (Ver 2.04O 1997/12/19)" */
	DRIVER( perestrf ) /* 1993 "Promat (Fuuki license)" "Perestroika Girls (Fuuki license)" */
	DRIVER( perestro ) /* 1994 "Promat" "Perestroika Girls" */
	DRIVER( piratpet ) /* 1982 "Taito America Corporation" "Pirate Pete" */
	DRIVER( pitnrun ) /* 1984 "Taito Corporation" "Pit & Run (set 1)" */
	DRIVER( pitnruna ) /* 1984 "Taito Corporation" "Pit & Run (set 2)" */
	DRIVER( plgirls ) /* 1992 "Hot-B." "Play Girls" */
	DRIVER( plgirls2 ) /* 1993 "Hot-B." "Play Girls 2" */
	DRIVER( plottina ) /* 1989 "Taito Corporation Japan" "Plotting (World set 2)" */
	DRIVER( plotting ) /* 1989 "Taito Corporation Japan" "Plotting (World set 1)" */
	DRIVER( plottinu ) /* 1989 "Taito Corporation Japan" "Plotting (US)" */
	DRIVER( plumppop ) /* 1987 "Taito Corporation" "Plump Pop (Japan)" */
	DRIVER( popnpop ) /* 1997 "Taito Corporation" "Pop'n Pop (Ver 2.07O 1998/02/09)" */
	DRIVER( popnpopj ) /* 1997 "Taito Corporation" "Pop'n Pop (Ver 2.07J 1998/02/09)" */
	DRIVER( popnpopu ) /* 1997 "Taito Corporation" "Pop'n Pop (Ver 2.07A 1998/02/09)" */
	DRIVER( prmtmfgo ) /* 1993 "Taito America Corporation" "Prime Time Fighter (Ver 2.1A 1993/05/21) (Old Version)" */
	DRIVER( prmtmfgt ) /* 1993 "Taito America Corporation" "Prime Time Fighter (Ver 2.1A 1993/05/21) (New Version)" */
	DRIVER( puchicar ) /* 1997 "Taito Corporation" "Puchi Carat (Ver 2.02O 1997/10/29)" */
	DRIVER( puchicrj ) /* 1997 "Taito Corporation" "Puchi Carat (Ver 2.02J 1997/10/29)" */
	DRIVER( pulirula ) /* 1991 "Taito Corporation Japan" "PuLiRuLa (World)" */
	DRIVER( pulirulj ) /* 1991 "Taito Corporation" "PuLiRuLa (Japan)" */
	DRIVER( puzznic ) /* 1989 "Taito Corporation Japan" "Puzznic (World)" */
	DRIVER( puzznicj ) /* 1989 "Taito Corporation" "Puzznic (Japan)" */
	DRIVER( pwheelsj ) /* 1991 "Taito Corporation" "Power Wheels (Japan)" */
	DRIVER( pwrgoal ) /* 1994 "Taito Corporation Japan" "Taito Power Goal (Ver 2.5O 1994/11/03)" */
	DRIVER( qcrayon ) /* 1993 "Taito Corporation" "Quiz Crayon Shinchan (Japan)" */
	DRIVER( qcrayon2 ) /* 1993 "Taito Corporation" "Crayon Shinchan Orato Asobo (Japan)" */
	DRIVER( qix ) /* 1981 "Taito America Corporation" "Qix (set 1)" */
	DRIVER( qix2 ) /* 1981 "Taito America Corporation" "Qix II (Tournament)" */
	DRIVER( qixa ) /* 1981 "Taito America Corporation" "Qix (set 2)" */
	DRIVER( qixb ) /* 1981 "Taito America Corporation" "Qix (set 3)" */
	DRIVER( qjinsei ) /* 1992 "Taito Corporation" "Quiz Jinsei Gekijoh (Japan)" */
	DRIVER( qtheater ) /* 1994 "Taito Corporation" "Quiz Theater - 3tsu no Monogatari (Ver 2.3J 1994/11/10)" */
	DRIVER( qtorimon ) /* 1990 "Taito Corporation" "Quiz Torimonochou (Japan)" */
	DRIVER( quizhq ) /* 1990 "Taito Corporation" "Quiz H.Q. (Japan)" */
	DRIVER( quizhuhu ) /* 1995 "Taito Corporation" "Moriguchi Hiroko no Quiz de Hyuu!Hyuu! (Ver 2.2J 1995/05/25)" */
	DRIVER( qzchikyu ) /* 1991 "Taito Corporation" "Quiz Chikyu Bouei Gun (Japan)" */
	DRIVER( qzquest ) /* 1991 "Taito Corporation" "Quiz Quest - Hime to Yuusha no Monogatari (Japan)" */
	DRIVER( qzshowby ) /* 1993 "Taito Corporation" "Quiz Sekai wa SHOW by shobai (Japan)" */
	DRIVER( raimais ) /* 1988 "Taito Corporation Japan" "Raimais (World)" */
	DRIVER( raimaisj ) /* 1988 "Taito Corporation" "Raimais (Japan)" */
	DRIVER( rainbow ) /* 1987 "Taito Corporation" "Rainbow Islands (new version)" */
	DRIVER( rainbowe ) /* 1988 "Taito Corporation" "Rainbow Islands (Extra)" */
	DRIVER( rainbowo ) /* 1987 "Taito Corporation" "Rainbow Islands (old version)" */
	DRIVER( rambo3 ) /* 1989 "Taito Europe Corporation" "Rambo III (Europe set 1)" */
	DRIVER( rambo3a ) /* 1989 "Taito America Corporation" "Rambo III (US)" */
	DRIVER( rambo3ae ) /* 1989 "Taito Europe Corporation" "Rambo III (Europe set 2)" */
	DRIVER( rastan ) /* 1987 "Taito Corporation Japan" "Rastan (World)" */
	DRIVER( rastanu ) /* 1987 "Taito America Corporation" "Rastan (US set 1)" */
	DRIVER( rastanu2 ) /* 1987 "Taito America Corporation" "Rastan (US set 2)" */
	DRIVER( rastsag2 ) /* 1988 "Taito Corporation" "Rastan Saga 2 (Japan)" */
	DRIVER( rastsaga ) /* 1987 "Taito Corporation" "Rastan Saga (Japan)" */
	DRIVER( rayforce ) /* 1993 "Taito America Corporation" "Ray Force (Ver 2.3A 1994/01/20)" */
	DRIVER( rayforcj ) /* 1993 "Taito Corporation" "Ray Force (Ver 2.3J 1994/01/20)" */
	DRIVER( recalh ) /* 1994 "Taito Corporation" "Recalhorn (Ver 1.42J 1994/5/11) (Prototype)" */
	DRIVER( recordbr ) /* 1988 "Taito Corporation Japan" "Recordbreaker (World)" */
	DRIVER( retofin1 ) /* 1985 "bootleg" "Return of the Invaders (bootleg set 1)" */
	DRIVER( retofin2 ) /* 1985 "bootleg" "Return of the Invaders (bootleg set 2)" */
	DRIVER( retofinv ) /* 1985 "Taito Corporation" "Return of the Invaders" */
	DRIVER( ridefgtj ) /* 1992 "Taito Corporation" "Riding Fight (Ver 1.0J)" */
	DRIVER( ridefgtu ) /* 1992 "Taito America Corporation" "Riding Fight (Ver 1.0A)" */
	DRIVER( ridingf ) /* 1992 "Taito Corporation Japan" "Riding Fight (Ver 1.0O)" */
	DRIVER( ringfgt ) /* 1984 "Taito" "Ring Fighter (set 1)" */
	DRIVER( ringfgt2 ) /* 1984 "Taito" "Ring Fighter (set 2)" */
	DRIVER( ringrage ) /* 1992 "Taito Corporation Japan" "Ring Rage (Ver 2.3O 1992/08/09)" */
	DRIVER( ringragj ) /* 1992 "Taito Corporation" "Ring Rage (Ver 2.3J 1992/08/09)" */
	DRIVER( ringragu ) /* 1992 "Taito America Corporation" "Ring Rage (Ver 2.3A 1992/08/09)" */
	DRIVER( runark ) /* 1990 "Taito Corporation" "Runark (Japan)" */
	DRIVER( ryujin ) /* 1993 "Taito Corporation" "Ryu Jin (Japan)" */
	DRIVER( sbm ) /* 1990 "Taito Corporation" "Sonic Blast Man (Japan)" */
	DRIVER( sboblbob ) /* 1986 "bootleg" "Super Bobble Bobble" */
	DRIVER( sbowling ) /* 1982 "Taito Corporation" "Strike Bowling" */
	DRIVER( scfinals ) /* 1993 "Taito Corporation Japan" "Super Cup Finals (Ver 2.1O 1993/11/19)" */
	DRIVER( sci ) /* 1989 "Taito Corporation Japan" "Special Criminal Investigation (World set 1)" */
	DRIVER( scia ) /* 1989 "Taito Corporation Japan" "Special Criminal Investigation (World set 2)" */
	DRIVER( sciu ) /* 1989 "Taito America Corporation" "Special Criminal Investigation (US)" */
	DRIVER( sdungeon ) /* 1981 "Taito America Corporation" "Space Dungeon" */
	DRIVER( selfeena ) /* 1991 "East Technology" "Sel Feena" */
	DRIVER( sfposeid ) /* 1984 "Taito Corporation" "Sea Fighter Poseidon" */
	DRIVER( silentd ) /* 1992 "Taito Corporation Japan" "Silent Dragon (World)" */
	DRIVER( silentdj ) /* 1992 "Taito Corporation" "Silent Dragon (Japan)" */
	DRIVER( slapshot ) /* 1994 "Taito Corporation" "Slap Shot (Japan)" */
	DRIVER( slither ) /* 1982 "Century II" "Slither (set 1)" */
	DRIVER( slithera ) /* 1982 "Century II" "Slither (set 2)" */
	DRIVER( solfigtr ) /* 1991 "Taito Corporation Japan" "Solitary Fighter (World)" */
	DRIVER( spacecr ) /* 1981 "Taito Corporation" "Space Cruiser" */
	DRIVER( spacedx ) /* 1994 "Taito Corporation" "Space Invaders DX (US) v2.1" */
	DRIVER( spacedxj ) /* 1994 "Taito Corporation" "Space Invaders DX (Japan) v2.1" */
	DRIVER( spacedxo ) /* 1994 "Taito Corporation" "Space Invaders DX (Japan) v2.0" */
	DRIVER( spacegun ) /* 1990 "Taito Corporation Japan" "Space Gun (World)" */
	DRIVER( spaceskr ) /* 1981 "Taito Corporation" "Space Seeker" */
	DRIVER( spcinv95 ) /* 1995 "Taito Corporation Japan" "Space Invaders '95: The Attack Of Lunar Loonies (Ver 2.5O 1995/06/14)" */
	DRIVER( spcinvdj ) /* 1994 "Taito Corporation" "Space Invaders DX (Ver 2.6J 1994/09/14) (F3 Version)" */
	DRIVER( spcnv95u ) /* 1995 "Taito America Corporation" "Space Invaders '95: The Attack Of Lunar Loonies (Ver 2.5A 1995/06/14)" */
	DRIVER( sqix ) /* 1987 "Taito" "Super Qix (set 1)" */
	DRIVER( sqixa ) /* 1987 "Taito" "Super Qix (set 2)" */
	DRIVER( sqixbl ) /* 1987 "bootleg" "Super Qix (bootleg)" */
	DRIVER( ssi ) /* 1990 "Taito Corporation Japan" "Super Space Invaders '91 (World)" */
	DRIVER( ssrj ) /* 1985 "Taito Corporation" "Super Speed Race Junior (Japan)" */
	DRIVER( storming ) /* 1986 "Taito" "Storming Party / Riku Kai Kuu Saizensen" */
	DRIVER( superchs ) /* 1992 "Taito America Corporation" "Super Chase - Criminal Termination (US)" */
	DRIVER( superman ) /* 1988 "Taito Corporation" "Superman" */
	DRIVER( suprmanj ) /* 1988 "Taito Corporation" "Superman (Japan)" */
	DRIVER( syvalion ) /* 1988 "Taito Corporation" "Syvalion (Japan)" */
	DRIVER( tcobra2 ) /* 1995 "Taito Corporation Japan" "Twin Cobra II (Ver 2.1O 1995/11/30)" */
	DRIVER( tcobra2u ) /* 1995 "Taito America Corporation" "Twin Cobra II (Ver 2.1A 1995/11/30)" */
	DRIVER( tetrist ) /* 1989 "Sega" "Tetris (Japan */
	DRIVER( tetrista ) /* 1989 "Sega" "Tetris (Japan */
	DRIVER( tetrsark ) /* 19?? "D.R. Korea" "Tetris (D.R. Korea)" */
	DRIVER( thndfoxj ) /* 1990 "Taito Corporation" "Thunder Fox (Japan)" */
	DRIVER( thndfoxu ) /* 1990 "Taito America Corporation" "Thunder Fox (US)" */
	DRIVER( thundfox ) /* 1990 "Taito Corporation Japan" "Thunder Fox (World)" */
	DRIVER( timetunl ) /* 1982 "Taito Corporation" "Time Tunnel" */
	DRIVER( tinstar ) /* 1983 "Taito Corporation" "The Tin Star" */
	DRIVER( tnzs ) /* 1988 "Taito Corporation Japan" "The NewZealand Story (World */
	DRIVER( tnzsj ) /* 1988 "Taito Corporation" "The NewZealand Story (Japan)" */
	DRIVER( tnzso ) /* 1988 "Taito Corporation Japan" "The NewZealand Story (World */
	DRIVER( tokio ) /* 1986 "Taito Corporation" "Tokio / Scramble Formation" */
	DRIVER( tokiob ) /* 1986 "bootleg" "Tokio / Scramble Formation (bootleg)" */
	DRIVER( topspedu ) /* 1987 "Taito America Corporation (Romstar license)" "Top Speed (US)" */
	DRIVER( topspeed ) /* 1987 "Taito Corporation Japan" "Top Speed (World)" */
	DRIVER( trstar ) /* 1993 "Taito Corporation Japan" "Top Ranking Stars (Ver 2.1O 1993/05/21) (New Version)" */
	DRIVER( trstarj ) /* 1993 "Taito Corporation" "Top Ranking Stars (Ver 2.1J 1993/05/21) (New Version)" */
	DRIVER( trstaro ) /* 1993 "Taito Corporation Japan" "Top Ranking Stars (Ver 2.1O 1993/05/21) (Old Version)" */
	DRIVER( trstaroj ) /* 1993 "Taito Corporation" "Top Ranking Stars (Ver 2.1J 1993/05/21) (Old Version)" */
	DRIVER( tsamura2 ) /* 1985 "Taito" "Samurai Nihon-ichi (set 2)" */
	DRIVER( tsamurai ) /* 1985 "Taito" "Samurai Nihon-ichi (set 1)" */
	DRIVER( tubeit ) /* 1993 "Taito Corporation" "Tube-It" */
	DRIVER( twinhawk ) /* 1989 "Taito Corporation Japan" "Twin Hawk (World)" */
	DRIVER( twinhwku ) /* 1989 "Taito America Corporation" "Twin Hawk (US)" */
	DRIVER( twinqix ) /* 1995 "Taito America Corporation" "Twin Qix (Ver 1.0A 1995/01/17) (Prototype)" */
	DRIVER( undoukai ) /* 1984 "Taito Corporation" "The Undoukai (Japan)" */
	DRIVER( undrfire ) /* 1993 "Taito Corporation Japan" "Under Fire (World)" */
	DRIVER( undrfirj ) /* 1993 "Taito Corporation" "Under Fire (Japan)" */
	DRIVER( undrfiru ) /* 1993 "Taito America Corporation" "Under Fire (US)" */
	DRIVER( victnine ) /* 1984 "Taito" "Victorious Nine" */
	DRIVER( viofight ) /* 1989 "Taito Corporation Japan" "Violence Fight (World)" */
	DRIVER( viofighu ) /* 1989 "Taito America Corporation" "Violence Fight (US)" */
	DRIVER( volfied ) /* 1989 "Taito Corporation Japan" "Volfied (World)" */
	DRIVER( volfiedj ) /* 1989 "Taito Corporation" "Volfied (Japan)" */
	DRIVER( volfiedu ) /* 1989 "Taito America Corporation" "Volfied (US)" */
	DRIVER( vsgongf ) /* 1984 "Kaneko" "VS Gong Fight" */
	DRIVER( warriorb ) /* 1991 "Taito Corporation" "Warrior Blade - Rastan Saga Episode III (Japan)" */
	DRIVER( waterski ) /* 1983 "Taito Corporation" "Water Ski" */
	DRIVER( wgp ) /* 1989 "Taito America Corporation" "World Grand Prix (US)" */
	DRIVER( wgp2 ) /* 1990 "Taito Corporation" "World Grand Prix 2 (Japan)" */
	DRIVER( wgpj ) /* 1989 "Taito Corporation" "World Grand Prix (Japan)" */
	DRIVER( wgpjoy ) /* 1989 "Taito Corporation" "World Grand Prix (joystick version set 1) (Japan)" */
	DRIVER( wgpjoya ) /* 1989 "Taito Corporation" "World Grand Prix (joystick version set 2) (Japan)" */
	DRIVER( wwester1 ) /* 1982 "Taito Corporation" "Wild Western (set 2)" */
	DRIVER( wwestern ) /* 1982 "Taito Corporation" "Wild Western (set 1)" */
	DRIVER( yamagchi ) /* 1985 "Taito" "Go Go Mr. Yamaguchi / Yuke Yuke Yamaguchi-kun" */
	DRIVER( yesnoj ) /* 1992 "Taito Corporation" "Yes/No Sinri Tokimeki Chart" */
	DRIVER( yuyugogo ) /* 1990 "Taito Corporation" "Yuuyu no Quiz de GO!GO! (Japan)" */
	DRIVER( zookeep ) /* 1982 "Taito America Corporation" "Zoo Keeper (set 1)" */
	DRIVER( zookeep2 ) /* 1982 "Taito America Corporation" "Zoo Keeper (set 2)" */
	DRIVER( zookeep3 ) /* 1982 "Taito America Corporation" "Zoo Keeper (set 3)" */
#endif
#ifdef LINK_TATSUMI
	DRIVER( apache3 ) /* 1988 "Tatsumi" "Apache 3" */
	DRIVER( buggyb1 ) /* 1986 "Tatsumi" "Buggy Boy (Single Monitor)" */
	DRIVER( cyclwarr ) /* 1991 "Tatsumi" "Cycle Warriors" */
	DRIVER( roundup5 ) /* 1989 "Tatsumi" "Round Up 5 - Super Delta Force" */
#endif
#ifdef LINK_TCH
	DRIVER( actionhw ) /* 1995 "TCH" "Action Hollywood" */
	DRIVER( kickgoal ) /* 1995 "TCH" "Kick Goal" */
	DRIVER( speedspn ) /* 1994 "TCH" "Speed Spin" */
#endif
#ifdef LINK_TECFRI
	DRIVER( crzralla ) /* 1985 "Tecfri" "Crazy Rally (set 2)" */
	DRIVER( crzrallg ) /* 1985 "Tecfri (Gecas license)" "Crazy Rally (Gecas license)" */
	DRIVER( crzrally ) /* 1985 "Tecfri" "Crazy Rally (set 1)" */
	DRIVER( holeland ) /* 1984 "Tecfri" "Hole Land" */
	DRIVER( sauro ) /* 1987 "Tecfri" "Sauro" */
	DRIVER( speedbal ) /* 1987 "Tecfri" "Speed Ball" */
	DRIVER( trckydca ) /* 1987 "Tecfri" "Tricky Doc (Set 2)" */
	DRIVER( trckydoc ) /* 1987 "Tecfri" "Tricky Doc (Set 1)" */
#endif
#ifdef LINK_TECHNOS
	DRIVER( agress ) /* 1991 "Palco" "Agress" */
	DRIVER( agressb ) /* 2003 "Palco" "Agress (English bootleg)" */
	DRIVER( battlan2 ) /* 1986 "Technos (Taito license)" "Battle Lane! Vol. 5 (set 2)" */
	DRIVER( battlan3 ) /* 1986 "Technos (Taito license)" "Battle Lane! Vol. 5 (set 3)" */
	DRIVER( battlane ) /* 1986 "Technos (Taito license)" "Battle Lane! Vol. 5 (set 1)" */
	DRIVER( bigprowr ) /* 1983 "Technos" "The Big Pro Wrestling!" */
	DRIVER( blckout2 ) /* 1989 "Technos + California Dreams" "Block Out (set 2)" */
	DRIVER( blckoutj ) /* 1989 "Technos + California Dreams" "Block Out (Japan)" */
	DRIVER( blockout ) /* 1989 "Technos + California Dreams" "Block Out (set 1)" */
	DRIVER( bogeyman ) /* 1985? "Technos Japan" "Bogey Manor" */
	DRIVER( chinagat ) /* 1988 "[Technos] (Taito Romstar license)" "China Gate (US)" */
	DRIVER( ctribe ) /* 1990 "Technos" "The Combatribes (US)" */
	DRIVER( ctribeb ) /* 1990 "bootleg" "The Combatribes (bootleg)" */
	DRIVER( darktowr ) /* 1992 "Game Room" "Dark Tower" */
	DRIVER( ddragn2u ) /* 1988 "Technos" "Double Dragon II - The Revenge (US)" */
	DRIVER( ddragnw1 ) /* 1987 "[Technos] (Taito license)" "Double Dragon (World Set 2)" */
	DRIVER( ddrago3b ) /* 1990 "bootleg" "Double Dragon 3 - The Rosetta Stone (bootleg)" */
	DRIVER( ddragon ) /* 1987 "Technos" "Double Dragon (Japan)" */
	DRIVER( ddragon2 ) /* 1988 "Technos" "Double Dragon II - The Revenge (World)" */
	DRIVER( ddragon3 ) /* 1990 "Technos" "Double Dragon 3 - The Rosetta Stone (US)" */
	DRIVER( ddragonb ) /* 1987 "bootleg" "Double Dragon (bootleg)" */
	DRIVER( ddragonu ) /* 1987 "[Technos] (Taito America license)" "Double Dragon (US)" */
	DRIVER( ddragonw ) /* 1987 "[Technos] (Taito license)" "Double Dragon (World Set 1)" */
	DRIVER( ddungeon ) /* 1992 "East Coast Coin Company (Melbourne)" "Dangerous Dungeons" */
	DRIVER( dogfgt ) /* 1984 "Technos" "Acrobatic Dog-Fight" */
	DRIVER( dogfgtj ) /* 1984 "Technos" "Dog-Fight (Japan)" */
	DRIVER( excthour ) /* 1985 "Technos (Taito license)" "Exciting Hour" */
	DRIVER( kuniokub ) /* 1986 "bootleg" "Nekketsu Kouha Kunio-kun (Japan bootleg)" */
	DRIVER( kuniokun ) /* 1986 "Technos" "Nekketsu Kouha Kunio-kun (Japan)" */
	DRIVER( maniach ) /* 1986 "Technos (Taito America license)" "Mania Challenge (set 1)" */
	DRIVER( maniach2 ) /* 1986 "Technos (Taito America license)" "Mania Challenge (set 2)" */
	DRIVER( matmania ) /* 1985 "Technos (Taito America license)" "Mat Mania" */
	DRIVER( myststno ) /* 1984 "Technos" "Mysterious Stones - Dr. Kick in Adventure" */
	DRIVER( mystston ) /* 1984 "Technos" "Mysterious Stones - Dr. John's Adventure" */
	DRIVER( nkdodgeb ) /* 1987 "Technos" "Nekketsu Koukou Dodgeball Bu (Japan bootleg)" */
	DRIVER( renegade ) /* 1986 "Technos (Taito America license)" "Renegade (US)" */
	DRIVER( saiyugb1 ) /* 1988 "bootleg" "Sai Yu Gou Ma Roku (Japan bootleg 1)" */
	DRIVER( saiyugb2 ) /* 1988 "bootleg" "Sai Yu Gou Ma Roku (Japan bootleg 2)" */
	DRIVER( saiyugou ) /* 1988 "Technos" "Sai Yu Gou Ma Roku (Japan)" */
	DRIVER( shadfrce ) /* 1993 "Technos Japan" "Shadow Force (US Version 2)" */
	DRIVER( solarwar ) /* 1986 "[Technos] Taito (Memetron license)" "Solar-Warrior" */
	DRIVER( spdodgeb ) /* 1987 "Technos" "Super Dodge Ball (US)" */
	DRIVER( ssozumo ) /* 1984 "Technos" "Syusse Oozumou (Japan)" */
	DRIVER( stoffy ) /* 1994 "Midas (Unico license)" "Super Toffy" */
	DRIVER( tagteam ) /* 1983 "Technos (Data East license)" "Tag Team Wrestling" */
	DRIVER( toffy ) /* 1993 "Midas" "Toffy" */
	DRIVER( tstrike ) /* 1991 "East Coast Coin Company (Melbourne)" "Thunder Strike (Newer)" */
	DRIVER( tstrikea ) /* 1991 "East Coast Coin Company (Melbourne)" "Thunder Strike (Older)" */
	DRIVER( vball ) /* 1988 "Technos" "U.S. Championship V'ball (set 1)" */
	DRIVER( vball2pj ) /* 1988 "Technos" "U.S. Championship V'ball (Japan)" */
	DRIVER( wwfsstaa ) /* 1989 "Technos Japan" "WWF Superstars (US */
	DRIVER( wwfsstar ) /* 1989 "Technos Japan" "WWF Superstars (Europe)" */
	DRIVER( wwfsstau ) /* 1989 "Technos Japan" "WWF Superstars (US)" */
	DRIVER( wwfwfest ) /* 1991 "Technos Japan" "WWF WrestleFest (US set 1)" */
	DRIVER( wwfwfsta ) /* 1991 "Technos Japan (Tecmo license)" "WWF WrestleFest (US Tecmo)" */
	DRIVER( wwfwfstb ) /* 1991 "bootleg" "WWF WrestleFest (US bootleg)" */
	DRIVER( wwfwfstj ) /* 1991 "Technos Japan" "WWF WrestleFest (Japan)" */
	DRIVER( xsleena ) /* 1986 "Technos" "Xain'd Sleena" */
	DRIVER( xsleenab ) /* 1986 "bootleg" "Xain'd Sleena (bootleg)" */
#endif
#ifdef LINK_TEHKAN
	DRIVER( baluba ) /* 1986 "Able Corp Ltd." */
	DRIVER( bombjac2 ) /* 1984 "Tehkan" "Bomb Jack (set 2)" */
	DRIVER( bombjack ) /* 1984 "Tehkan" "Bomb Jack (set 1)" */
	DRIVER( drgnbowl ) /* 1992 "Nics" "Dragon Bowl" */
	DRIVER( fstarfrc ) /* 1992 "Tecmo" "Final Star Force (US)" */
	DRIVER( gaiden ) /* 1988 "Tecmo" "Ninja Gaiden (US)" */
	DRIVER( gemini ) /* 1987 "Tecmo" "Gemini Wing" */
	DRIVER( ginkun ) /* 1995 "Tecmo" "Ganbare Ginkun" */
	DRIVER( gridiron ) /* 1985 "Tehkan" "Gridiron Fight" */
	DRIVER( lvcards ) /* 1985 "Tehkan" "Lovely Cards" */
	DRIVER( lvpoker ) /* 1985 "Tehkan" "Lovely Poker [BET]" */
	DRIVER( megaforc ) /* 1985 "Tehkan (Video Ware license)" "Mega Force" */
	DRIVER( pbactio2 ) /* 1985 "Tehkan" "Pinball Action (set 2)" */
	DRIVER( pbactio3 ) /* 1985 "Tehkan" "Pinball Action (set 3 */
	DRIVER( pbaction ) /* 1985 "Tehkan" "Pinball Action (set 1)" */
	DRIVER( ponttehk ) /* 1985 "Tehkan" "Pontoon (Tehkan)" */
	DRIVER( raiga ) /* 1991 "Tecmo" "Raiga - Strato Fighter (Japan)" */
	DRIVER( riot ) /* 1992 "NMK" "Riot" */
	DRIVER( rygar ) /* 1986 "Tecmo" "Rygar (US set 1)" */
	DRIVER( rygar2 ) /* 1986 "Tecmo" "Rygar (US set 2)" */
	DRIVER( rygarj ) /* 1986 "Tecmo" "Argus no Senshi (Japan)" */
	DRIVER( ryukendn ) /* 1989 "Tecmo" "Ninja Ryukenden (Japan)" */
	DRIVER( senjyo ) /* 1983 "Tehkan" "Senjyo" */
	DRIVER( shadoww ) /* 1988 "Tecmo" "Shadow Warriors (World set 1)" */
	DRIVER( shadowwa ) /* 1988 "Tecmo" "Shadow Warriors (World set 2)" */
	DRIVER( silkworm ) /* 1988 "Tecmo" "Silk Worm (set 1)" */
	DRIVER( silkwrm2 ) /* 1988 "Tecmo" "Silk Worm (set 2)" */
	DRIVER( solomon ) /* 1986 "Tecmo" "Solomon's Key (Japan)" */
	DRIVER( spbactn ) /* 1991 "Tecmo" "Super Pinball Action (US)" */
	DRIVER( spbactnj ) /* 1991 "Tecmo" "Super Pinball Action (Japan)" */
	DRIVER( starforc ) /* 1984 "Tehkan" "Star Force" */
	DRIVER( starfore ) /* 1984 "Tehkan" "Star Force (encrypted)" */
	DRIVER( stratof ) /* 1991 "Tecmo" "Raiga - Strato Fighter (US)" */
	DRIVER( tbowl ) /* 1987 "Tecmo" "Tecmo Bowl (World?)" */
	DRIVER( tbowlj ) /* 1987 "Tecmo" "Tecmo Bowl (Japan)" */
	DRIVER( teedoff ) /* 1986 "Tecmo" "Tee'd Off (Japan)" */
	DRIVER( tehkanwc ) /* 1985 "Tehkan" "Tehkan World Cup" */
	DRIVER( tknight ) /* 1989 "Tecmo" "Tecmo Knight" */
	DRIVER( wc90 ) /* 1989 "Tecmo" "Tecmo World Cup '90 (set 1)" */
	DRIVER( wc90a ) /* 1989 "Tecmo" "Tecmo World Cup '90 (set 2)" */
	DRIVER( wc90b ) /* 1989 "bootleg" "Euro League" */
	DRIVER( wc90t ) /* 1989 "Tecmo" "Tecmo World Cup '90 (trackball)" */
	DRIVER( wildfang ) /* 1989 "Tecmo" "Wild Fang / Tecmo Knight" */
#endif
#ifdef LINK_THEPIT
	DRIVER( dockman ) /* 1982 "Taito Corporation" "Dock Man" */
	DRIVER( fitter ) /* 1981 "Taito Corporation" "Fitter" */
	DRIVER( funnymou ) /* 1982 "Chuo Co. Ltd" "Funny Mouse" */
	DRIVER( intrepi2 ) /* 1983 "Nova Games Ltd." "Intrepid (set 2)" */
	DRIVER( intrepid ) /* 1983 "Nova Games Ltd." "Intrepid (set 1)" */
	DRIVER( machomou ) /* 1982 "Techstar" "Macho Mouse" */
	DRIVER( portman ) /* 1982 "Nova Games Ltd." "Port Man" */
	DRIVER( progress ) /* 1984 "Chuo Co. Ltd" "Progress" */
	DRIVER( roundup ) /* 1981 "Amenip/Centuri" "Round-Up" */
	DRIVER( rtriv ) /* 198? "Romar" "Romar Triv" */
	DRIVER( suprmous ) /* 1982 "Taito Corporation" "Super Mouse" */
	DRIVER( thepit ) /* 1982 "Taito" "The Pit" */
	DRIVER( thepitb ) /* 1982 "Bootleg" "The Pit (Bootleg)" */
	DRIVER( thepitc ) /* 1982 "Centuri" "The Pit (Centuri)" */
	DRIVER( timelimt ) /* 1983 "Chuo Co. Ltd" "Time Limit" */
#endif
#ifdef LINK_TOAPLAN
	DRIVER( 3in1semi ) /* 1997 "SemiCom" "XESS - The New Revolution (SemiCom 3-in-1)" */
	DRIVER( 4in1boot ) /* 1999 "bootleg" "Puzzle King (bootleg)" */
	DRIVER( batrider ) /* 1998 "Raizing / Eighting" "Armed Police Batrider (Japan */
	DRIVER( batridra ) /* 1998 "Raizing / Eighting" "Armed Police Batrider (Japan */
	DRIVER( batridrk ) /* 1998 "Raizing / Eighting" "Armed Police Batrider (Korea */
	DRIVER( batsugna ) /* 1993 "Toaplan" "Batsugun (set 2)" */
	DRIVER( batsugun ) /* 1993 "Toaplan" "Batsugun (set 1)" */
	DRIVER( battleg ) /* 1996 "Raizing / Eighting" "Battle Garegga - Type 2 (Denmark / China) (Tue Apr 2 1996)" */
	DRIVER( battlega ) /* 1996 "Raizing / Eighting" "Battle Garegga (Europe / USA / Japan / Asia) (Sat Feb 3 1996)" */
	DRIVER( battlegb ) /* 1996 "Raizing / Eighting" "Battle Garegga (Austria / Hong Kong) (Sat Mar 2 1996)" */
	DRIVER( battlegc ) /* 1996 "Raizing / Eighting" "Battle Garegga (Austria / Hong Kong) (Sat Feb 3 1996)" */
	DRIVER( batugnsp ) /* 1993 "Toaplan" "Batsugun (Special Ver.)" */
	DRIVER( bbakradu ) /* 1999 "Eighting" "Battle Bakraid - Unlimited version (Japan) (Tue Jun 8 1999)" */
	DRIVER( bbakraid ) /* 1999 "Eighting" "Battle Bakraid (Japan) (Wed Apr 7 1999)" */
	DRIVER( cookbib2 ) /* 1996 "SemiCom" "Cookie & Bibi 2" */
	DRIVER( cookbib3 ) /* 1997 "SemiCom" "Cookie & Bibi 3" */
	DRIVER( demonwl1 ) /* 1989 "Toaplan (Taito license)" "Demon's World / Horror Story (Taito license)" */
	DRIVER( demonwl2 ) /* 1989 "Toaplan" "Demon's World / Horror Story (first edition)" */
	DRIVER( demonwld ) /* 1990 "Toaplan" "Demon's World / Horror Story" */
	DRIVER( dogyuun ) /* 1992 "Toaplan" "Dogyuun" */
	DRIVER( finalttr ) /* 1993 "Jeil Computer System" "Final Tetris" */
	DRIVER( fireshrk ) /* 1990 "Toaplan" "Fire Shark" */
	DRIVER( fixeighb ) /* 1992 "bootleg" "FixEight (bootleg)" */
	DRIVER( fixeight ) /* 1992 "Toaplan" "FixEight" */
	DRIVER( fshark ) /* 1987 "[Toaplan] Taito Corporation" "Flying Shark (World)" */
	DRIVER( fsharkbt ) /* 1987 "bootleg" "Flying Shark (bootleg)" */
	DRIVER( getstar ) /* 1986 "Taito" "Guardian" */
	DRIVER( getstarb ) /* 1986 "bootleg" "Get Star (bootleg */
	DRIVER( ghox ) /* 1991 "Toaplan" "Ghox" */
	DRIVER( grindsta ) /* 1992 "Toaplan" "Grind Stormer (older set)" */
	DRIVER( grindstm ) /* 1992 "Toaplan" "Grind Stormer" */
	DRIVER( gtstarba ) /* 1986 "bootleg" "Get Star (bootleg */
	DRIVER( gulfwar2 ) /* 1991 "Comad" "Gulf War II" */
	DRIVER( hellfir1 ) /* 1989 "Toaplan (Taito license)" "Hellfire (1P Ver.)" */
	DRIVER( hellfire ) /* 1989 "Toaplan (Taito license)" "Hellfire" */
	DRIVER( hishouza ) /* 1987 "[Toaplan] Taito Corporation" "Hishou Zame (Japan)" */
	DRIVER( honeydol ) /* 1995 "Barko Corp." "Honey Dolls" */
	DRIVER( hyperpac ) /* 1995 "SemiCom" "Hyper Pacman" */
	DRIVER( hyperpcb ) /* 1995 "bootleg" "Hyper Pacman (bootleg)" */
	DRIVER( kbash ) /* 1993 "Toaplan" "Knuckle Bash" */
	DRIVER( kbash2 ) /* 1999 "bootleg" "Knuckle Bash 2 (bootleg)" */
	DRIVER( kingdmgp ) /* 1994 "Raizing / Eighting" "Kingdom Grandprix (World)" */
	DRIVER( ktiger ) /* 1987 "[Toaplan] Taito Corporation" "Kyukyoku Tiger (Japan)" */
	DRIVER( mahoudai ) /* 1993 "Raizing (Able license)" "Mahou Daisakusen (Japan)" */
	DRIVER( mjsister ) /* 1986 "Toaplan" "Mahjong Sisters (Japan)" */
	DRIVER( moremore ) /* 1999 "SemiCom / Exit" "More More" */
	DRIVER( moremorp ) /* 1999 "SemiCom / Exit" "More More Plus" */
	DRIVER( outzone ) /* 1990 "Toaplan" "Out Zone (set 1)" */
	DRIVER( outzonea ) /* 1990 "Toaplan" "Out Zone (set 2)" */
	DRIVER( outzoneb ) /* 1990 "Toaplan" "Out Zone (set 3 */
	DRIVER( outzonec ) /* 1990 "Toaplan" "Out Zone (set 4)" */
	DRIVER( perfrman ) /* 1985 "[Toaplan] Data East Corporation" "Performan (Japan)" */
	DRIVER( perfrmau ) /* 1985 "[Toaplan] Data East USA" "Performan (US)" */
	DRIVER( pipibibi ) /* 1991 "[Toaplan] Ryouta Kikaku" "Pipi & Bibis / Whoopee!! (bootleg ?)" */
	DRIVER( pipibibs ) /* 1991 "Toaplan" "Pipi & Bibis / Whoopee!!" */
	DRIVER( pyros ) /* 1987 "[Toaplan] Taito America Corporation" "Pyros (US)" */
	DRIVER( rallybik ) /* 1988 "[Toaplan] Taito Corporation" "Rally Bike / Dash Yarou" */
	DRIVER( samesam2 ) /* 1989 "Toaplan" "Same! Same! Same! (2P Ver.)" */
	DRIVER( samesame ) /* 1989 "Toaplan" "Same! Same! Same!" */
	DRIVER( shippumd ) /* 1994 "Raizing / Eighting" "Shippu Mahou Daisakusen (Japan)" */
	DRIVER( skyshark ) /* 1987 "[Toaplan] Taito America Corporation (Romstar license)" "Sky Shark (US)" */
	DRIVER( slapbtjp ) /* 1986 "bootleg" "Slap Fight (Japan bootleg)" */
	DRIVER( slapbtuk ) /* 1986 "bootleg" "Slap Fight (English bootleg)" */
	DRIVER( slapfgtr ) /* 1986 "bootleg" "Slap Fight (bootleg)" */
	DRIVER( slapfigh ) /* 1986 "Taito" "Slap Fight (set 1)" */
	DRIVER( snowbro2 ) /* 1994 "[Toaplan] Hanafram" "Snow Bros. 2 - With New Elves / Otenki Paradise" */
	DRIVER( snowbro3 ) /* 2002 "bootleg" "Snow Brothers 3 - Magical Adventure" */
	DRIVER( snowbroa ) /* 1990 "Toaplan" "Snow Bros. - Nick & Tom (set 2)" */
	DRIVER( snowbrob ) /* 1990 "Toaplan" "Snow Bros. - Nick & Tom (set 3)" */
	DRIVER( snowbroc ) /* 1990 "Toaplan" "Snow Bros. - Nick & Tom (set 4)" */
	DRIVER( snowbroj ) /* 1990 "Toaplan" "Snow Bros. - Nick & Tom (Japan)" */
	DRIVER( snowbros ) /* 1990 "Toaplan" "Snow Bros. - Nick & Tom (set 1)" */
	DRIVER( sstriker ) /* 1993 "Raizing" "Sorcer Striker (World)" */
	DRIVER( sstrikra ) /* 1993 "Raizing" "Sorcer Striker (World */
	DRIVER( tekipaki ) /* 1991 "Toaplan" "Teki Paki" */
	DRIVER( tigerh ) /* 1985 "Taito America Corp." "Tiger Heli (US)" */
	DRIVER( tigerhb1 ) /* 1985 "bootleg" "Tiger Heli (bootleg set 1)" */
	DRIVER( tigerhb2 ) /* 1985 "bootleg" "Tiger Heli (bootleg set 2)" */
	DRIVER( tigerhj ) /* 1985 "Taito Corp." "Tiger Heli (Japan set 2)" */
	DRIVER( toppyrap ) /* 1996 "SemiCom" "Toppy & Rappy" */
	DRIVER( truxton ) /* 1988 "[Toaplan] Taito Corporation" "Truxton / Tatsujin" */
	DRIVER( truxton2 ) /* 1992 "Toaplan" "Truxton II / Tatsujin II / Tatsujin Oh (Japan)" */
	DRIVER( twincobr ) /* 1987 "[Toaplan] Taito Corporation" "Twin Cobra (World)" */
	DRIVER( twincobu ) /* 1987 "[Toaplan] Taito America Corporation (Romstar license)" "Twin Cobra (US)" */
	DRIVER( twinkle ) /* 1997 "SemiCom" "Twinkle" */
	DRIVER( vfive ) /* 1993 "Toaplan" "V-Five (Japan)" */
	DRIVER( vimana ) /* 1991 "Toaplan" "Vimana" */
	DRIVER( vimana1 ) /* 1991 "Toaplan" "Vimana (old set)" */
	DRIVER( vimanan ) /* 1991 "Toaplan (Nova Apparate GMBH & Co license)" "Vimana (Nova Apparate GMBH & Co)" */
	DRIVER( wardner ) /* 1987 "[Toaplan] Taito Corporation Japan" "Wardner (World)" */
	DRIVER( wardnerj ) /* 1987 "[Toaplan] Taito Corporation" "Wardner no Mori (Japan)" */
	DRIVER( whoopee ) /* 1991 "Toaplan" "Whoopee!! / Pipi & Bibis" */
	DRIVER( wintbob ) /* 1990 "bootleg" "The Winter Bobble" */
	DRIVER( zerowing ) /* 1989 "Toaplan" "Zero Wing" */
#endif
#ifdef LINK_TONG
	DRIVER( beezer ) /* 1982 "Tong Electronic" "Beezer (set 1)" */
	DRIVER( beezer1 ) /* 1982 "Tong Electronic" "Beezer (set 2)" */
	DRIVER( leprechn ) /* 1982 "Tong Electronic" "Leprechaun" */
	DRIVER( leprechp ) /* 1982 "Tong Electronic" "Leprechaun (Pacific Polytechnical license)" */
	DRIVER( piratetr ) /* 1982 "Tong Electronic" "Pirate Treasure" */
	DRIVER( potogold ) /* 1982 "GamePlan" "Pot of Gold" */
#endif
#ifdef LINK_UNICO
	DRIVER( burglarx ) /* 1997 "Unico" "Burglar X" */
	DRIVER( drgnmst ) /* 1994 "Unico" "Dragon Master" */
	DRIVER( silkroad ) /* 1999 "Unico" "The Legend of Silkroad" */
	DRIVER( zeropnt ) /* 1998 "Unico" "Zero Point (set 1)" */
	DRIVER( zeropnt2 ) /* 1999 "Unico" "Zero Point 2" */
	DRIVER( zeropnta ) /* 1998 "Unico" "Zero Point (set 2)" */
#endif
#ifdef LINK_UNIVERS
	DRIVER( cavenger ) /* 1981 "Universal" "Cosmic Avenger" */
	DRIVER( cheekyms ) /* 1980? "Universal" "Cheeky Mouse" */
	DRIVER( cosmica ) /* 1979 "Universal" "Cosmic Alien" */
	DRIVER( cosmica2 ) /* 1979 "Universal" "Cosmic Alien (older)" */
	DRIVER( cosmicg ) /* 1979 "Universal" "Cosmic Guerilla" */
	DRIVER( devzone ) /* 1980 "Universal" "Devil Zone" */
	DRIVER( devzone2 ) /* 1980 "Universal" "Devil Zone (easier)" */
	DRIVER( docastl2 ) /* 1983 "Universal" "Mr. Do's Castle (set 2)" */
	DRIVER( docastle ) /* 1983 "Universal" "Mr. Do's Castle (set 1)" */
	DRIVER( docastlo ) /* 1983 "Universal" "Mr. Do's Castle (older)" */
	DRIVER( dorodon ) /* 1982 "Falcon" "Dorodon (set 1)" */
	DRIVER( dorodon2 ) /* 1982 "Falcon" "Dorodon (set 2)" */
	DRIVER( dorunrca ) /* 1984 "Universal" "Do! Run Run (Do's Castle hardware */
	DRIVER( dorunru2 ) /* 1984 "Universal" "Do! Run Run (set 2)" */
	DRIVER( dorunruc ) /* 1984 "Universal" "Do! Run Run (Do's Castle hardware */
	DRIVER( dorunrun ) /* 1984 "Universal" "Do! Run Run (set 1)" */
	DRIVER( douni ) /* 1983 "Universal" "Mr. Do vs. Unicorns" */
	DRIVER( dowild ) /* 1984 "Universal" "Mr. Do's Wild Ride" */
	DRIVER( idsoccer ) /* 1985 "Universal" "Indoor Soccer" */
	DRIVER( jjack ) /* 1984 "Universal" "Jumping Jack" */
	DRIVER( kickridr ) /* 1984 "Universal" "Kick Rider" */
	DRIVER( ladybgb2 ) /* 1981 "bootleg" "Lady Bug (bootleg Set 2)" */
	DRIVER( ladybug ) /* 1981 "Universal" "Lady Bug" */
	DRIVER( ladybugb ) /* 1981 "bootleg" "Lady Bug (bootleg)" */
	DRIVER( magspot ) /* 1980 "Universal" "Magical Spot" */
	DRIVER( magspot2 ) /* 1980 "Universal" "Magical Spot II" */
	DRIVER( mrdo ) /* 1982 "Universal" "Mr. Do!" */
	DRIVER( mrdofix ) /* 1982 "Universal (Taito license)" "Mr. Do! (bugfixed)" */
	DRIVER( mrdot ) /* 1982 "Universal (Taito license)" "Mr. Do! (Taito)" */
	DRIVER( mrdoy ) /* 1982 "Universal" "Mr. Do! (prototype)" */
	DRIVER( mrdu ) /* 1982 "bootleg" "Mr. Du!" */
	DRIVER( mrlo ) /* 1982 "bootleg" "Mr. Lo!" */
	DRIVER( nomnlnd ) /* 1980 "Universal" "No Man's Land" */
	DRIVER( nomnlndg ) /* 1980 "Universal (Gottlieb license)" "No Man's Land (Gottlieb)" */
	DRIVER( panic ) /* 1980 "Universal" "Space Panic (version E)" */
	DRIVER( panic2 ) /* 1980 "Universal" "Space Panic (set 2)" */
	DRIVER( panic3 ) /* 1980 "Universal" "Space Panic (set 3)" */
	DRIVER( panicger ) /* 1980 "Universal (ADP Automaten license)" "Space Panic (German)" */
	DRIVER( panich ) /* 1980 "Universal" "Space Panic (harder)" */
	DRIVER( redclash ) /* 1981 "Tehkan" "Red Clash (set 1)" */
	DRIVER( redclask ) /* 1981 "Kaneko" "Red Clash (Kaneko)" */
	DRIVER( redclsha ) /* 1981 "Tehkan" "Red Clash (set 2)" */
	DRIVER( snapjack ) /* 1982 "Universal" "Snap Jack" */
	DRIVER( spiero ) /* 1987 "Universal" "Super Pierrot (Japan)" */
	DRIVER( sraider ) /* 1982 "Universal" "Space Raider" */
	DRIVER( yankeedo ) /* 1982 "hack" "Yankee DO!" */
	DRIVER( zerohour ) /* 1980 "Universal" "Zero Hour" */
#endif
#ifdef LINK_UPL
	DRIVER( arkarea ) /* 1988? "UPL" "Ark Area" */
	DRIVER( mnight ) /* 1987 "UPL (Kawakus license)" "Mutant Night" */
	DRIVER( mouser ) /* 1983 "UPL" "Mouser" */
	DRIVER( mouserc ) /* 1983 "[UPL] (Cosmos license)" "Mouser (Cosmos)" */
	DRIVER( ninjak2a ) /* 1987 "UPL" "Ninja-Kid II (set 2)" */
	DRIVER( ninjak2b ) /* 1987 "UPL" "Ninja-Kid II (set 3)" */
	DRIVER( ninjakd2 ) /* 1987 "UPL" "Ninja-Kid II (set 1)" */
	DRIVER( ninjakun ) /* 1984 "[UPL] (Taito license)" "Ninjakun Majou no Bouken" */
	DRIVER( nov2001u ) /* 1983 "UPL (Universal license)" "Nova 2001 (US)" */
	DRIVER( nova2001 ) /* 1983 "UPL" "Nova 2001 (Japan)" */
	DRIVER( omegaf ) /* 1989 "UPL" "Omega Fighter" */
	DRIVER( omegafs ) /* 1989 "UPL" "Omega Fighter Special" */
	DRIVER( pkunwar ) /* 1985? "UPL" "Penguin-Kun Wars (US)" */
	DRIVER( pkunwarj ) /* 1985? "UPL" "Penguin-Kun Wars (Japan)" */
	DRIVER( raiders5 ) /* 1985 "UPL" "Raiders5" */
	DRIVER( raidrs5t ) /* 1985 "UPL (Taito license)" "Raiders5 (Japan)" */
	DRIVER( rdaction ) /* 1987 "UPL (World Games license)" "Rad Action" */
	DRIVER( robokdj2 ) /* 1988 "UPL" "Atomic Robo-kid (Japan */
	DRIVER( robokid ) /* 1988 "UPL" "Atomic Robo-kid" */
	DRIVER( robokidj ) /* 1988 "UPL" "Atomic Robo-kid (Japan */
	DRIVER( xxmissio ) /* 1986 "UPL" "XX Mission" */
#endif
#ifdef LINK_VALADON
	DRIVER( bagman ) /* 1982 "Valadon Automation" "Bagman" */
	DRIVER( bagmans ) /* 1982 "Valadon Automation (Stern license)" "Bagman (Stern set 1)" */
	DRIVER( bagmans2 ) /* 1982 "Valadon Automation (Stern license)" "Bagman (Stern set 2)" */
	DRIVER( bagnard ) /* 1982 "Valadon Automation" "Le Bagnard (set 1)" */
	DRIVER( bagnarda ) /* 1982 "Valadon Automation" "Le Bagnard (set 2)" */
	DRIVER( botanic ) /* 1984 "Valadon Automation (Itisa license)" "Botanic" */
	DRIVER( pickin ) /* 1983 "Valadon Automation" "Pickin'" */
	DRIVER( sbagman ) /* 1984 "Valadon Automation" "Super Bagman" */
	DRIVER( sbagmans ) /* 1984 "Valadon Automation (Stern license)" "Super Bagman (Stern)" */
	DRIVER( tankbust ) /* 1985 "Valadon Automation" "Tank Busters" */
#endif
#ifdef LINK_VELTMJR
#endif
#ifdef LINK_VENTURE
	DRIVER( looping ) /* 1982 "Venture Line" "Looping (set 1)" */
	DRIVER( loopinga ) /* 1982 "Venture Line" "Looping (set 2)" */
	DRIVER( meteor ) /* 1981 "Venture Line" "Meteoroids" */
	DRIVER( skybump ) /* 1982 "Venture Line" "Sky Bumper" */
	DRIVER( spcforc2 ) /* 19?? "Elcon (bootleg?)" "Space Force (set 2)" */
	DRIVER( spcforce ) /* 1980 "Venture Line" "Space Force" */
	DRIVER( suprridr ) /* 1983 "Venture Line (Taito Corporation license)" "Super Rider" */
#endif
#ifdef LINK_VSYSTEM
	DRIVER( aerofgt ) /* 1992 "Video System Co." "Aero Fighters" */
	DRIVER( aerofgtb ) /* 1992 "Video System Co." "Aero Fighters (Turbo Force hardware set 1)" */
	DRIVER( aerofgtc ) /* 1992 "Video System Co." "Aero Fighters (Turbo Force hardware set 2)" */
	DRIVER( ccasino ) /* 1987 "V-System Co." "Chinese Casino [BET] (Japan)" */
	DRIVER( crshrac2 ) /* 1993 "Video System Co." "Lethal Crash Race (set 2)" */
	DRIVER( crshrace ) /* 1993 "Video System Co." "Lethal Crash Race (set 1)" */
	DRIVER( daiyogen ) /* 1990 "Video System Co." "Mahjong Daiyogen (Japan)" */
	DRIVER( f1gp ) /* 1991 "Video System Co." "F-1 Grand Prix" */
	DRIVER( f1gp2 ) /* 1992 "Video System Co." "F-1 Grand Prix Part II" */
	DRIVER( fromanc2 ) /* 1995 "Video System Co." "Taisen Idol-Mahjong Final Romance 2 (Japan)" */
	DRIVER( fromanc4 ) /* 1998 "Video System Co." "Taisen Mahjong FinalRomance 4 (Japan)" */
	DRIVER( fromance ) /* 1991 "Video System Co." "Idol-Mahjong Final Romance (Japan)" */
	DRIVER( fromancr ) /* 1995 "Video System Co." "Taisen Mahjong FinalRomance R (Japan)" */
	DRIVER( gstriker ) /* 1993 "Human" "Grand Striker" */
	DRIVER( hatris ) /* 1990 "Video System Co." "Hatris (Japan)" */
	DRIVER( idolmj ) /* 1988 "System Service" "Idol-Mahjong Housoukyoku (Japan)" */
	DRIVER( inufuku ) /* 1998 "Video System Co." "Quiz & Variety Sukusuku Inufuku (Japan)" */
	DRIVER( karatblj ) /* 1991 "Video System Co." "Karate Blazers (Japan)" */
	DRIVER( karatblu ) /* 1991 "Video System Co." "Karate Blazers (US)" */
	DRIVER( karatblz ) /* 1991 "Video System Co." "Karate Blazers (World?)" */
	DRIVER( mfunclub ) /* 1989 "Video System Co." "Mahjong Fun Club - Idol Saizensen (Japan)" */
	DRIVER( mjnatsu ) /* 1989 "Video System Co." "Mahjong Natsu Monogatari (Japan)" */
	DRIVER( natsuiro ) /* 1989 "Video System Co." "Natsuiro Mahjong (Japan)" */
	DRIVER( nekkyoku ) /* 1988 "Video System Co." "Rettou Juudan Nekkyoku Janshi - Higashi Nippon Hen (Japan)" */
	DRIVER( nmsengen ) /* 1991 "Video System Co." "Nekketsu Mahjong Sengen! AFTER 5 (Japan)" */
	DRIVER( ojanko2 ) /* 1987 "V-System Co." "Ojanko Yakata 2bankan (Japan)" */
	DRIVER( ojankoc ) /* 1986 "V-System Co." "Ojanko Club (Japan)" */
	DRIVER( ojankohs ) /* 1988 "V-System Co." "Ojanko High School (Japan)" */
	DRIVER( ojankoy ) /* 1986 "V-System Co." "Ojanko Yakata (Japan)" */
	DRIVER( pipedrm ) /* 1990 "Video System Co." "Pipe Dream (US)" */
	DRIVER( pipedrmj ) /* 1990 "Video System Co." "Pipe Dream (Japan)" */
	DRIVER( pspikes ) /* 1991 "Video System Co." "Power Spikes (World)" */
	DRIVER( pspikesb ) /* 1991 "bootleg" "Power Spikes (bootleg)" */
	DRIVER( pspikesk ) /* 1991 "Video System Co." "Power Spikes (Korea)" */
	DRIVER( quiz18k ) /* 1992 "EIM" "Miyasu Nonki no Quiz 18-Kin" */
	DRIVER( rabiolep ) /* 1987 "V-System Co." "Rabio Lepus (Japan)" */
	DRIVER( rpunch ) /* 1987 "V-System Co. (Bally/Midway/Sente license)" "Rabbit Punch (US)" */
	DRIVER( sformula ) /* 1989 "V-System Co." "Super Formula (Japan)" */
	DRIVER( sonicwi ) /* 1992 "Video System Co." "Sonic Wings (Japan)" */
	DRIVER( spinlbrj ) /* 1990 "V-System Co." "Spinal Breakers (Japan)" */
	DRIVER( spinlbrk ) /* 1990 "V-System Co." "Spinal Breakers (World)" */
	DRIVER( spinlbru ) /* 1990 "V-System Co." "Spinal Breakers (US)" */
	DRIVER( suprslam ) /* 1995 "Banpresto / Toei Animation" "Super Slams" */
	DRIVER( svolley ) /* 1989 "V-System Co." "Super Volleyball (Japan)" */
	DRIVER( svolleyk ) /* 1989 "V-System Co." "Super Volleyball (Korea)" */
	DRIVER( svolleyu ) /* 1989 "V-System Co. (Data East license)" "Super Volleyball (US)" */
	DRIVER( svolly91 ) /* 1991 "Video System Co." "Super Volley '91 (Japan)" */
	DRIVER( tail2nos ) /* 1989 "V-System Co." "Tail to Nose - Great Championship" */
	DRIVER( taotaida ) /* 1993 "Video System Co." "Tao Taido (set 2)" */
	DRIVER( taotaido ) /* 1993 "Video System Co." "Tao Taido (set 1)" */
	DRIVER( turbofrc ) /* 1991 "Video System Co." "Turbo Force" */
	DRIVER( wbbc97 ) /* 1997 "Comad" "Beach Festival World Championship 1997" */
	DRIVER( welltrij ) /* 1991 "Video System Co." "Welltris (Japan */
	DRIVER( welltris ) /* 1991 "Video System Co." "Welltris (World? */
#endif
#ifdef LINK_YUNSUNG
	DRIVER( bombkick ) /* 1998 "Yun Sung" "Bomb Kick" */
	DRIVER( cannball ) /* 1995 "Yun Sung / Soft Vision" "Cannon Ball" */
	DRIVER( garogun ) /* 2000 "Yun Sung" "Garogun Seroyang (Korea)" */
	DRIVER( madball ) /* 1998 "Yun Sung" "Mad Ball V2.0" */
	DRIVER( magicbua ) /* 19?? "Yun Sung" "Magic Bubble (Adult version)" */
	DRIVER( magicbub ) /* 19?? "Yun Sung" "Magic Bubble" */
	DRIVER( magix ) /* 1995 "Yun Sung" "Magix / Rock" */
	DRIVER( nmg5 ) /* 1998 "Yun Sung" "Multi 5 / New Multi Game 5" */
	DRIVER( paradise ) /* 1994+ "Yun Sung" "Paradise" */
	DRIVER( paradlx ) /* 1994+ "Yun Sung" "Paradise Deluxe" */
	DRIVER( pclubys ) /* 2000 "Yun Sung" "Puzzle Club (Yun Sung - set 1)" */
	DRIVER( pclubysa ) /* 2000 "Yun Sung" "Puzzle Club (Yun Sung - set 2)" */
	DRIVER( searchey ) /* 1999 "Yun Sung" "Search Eye" */
	DRIVER( shocking ) /* 1997 "Yun Sung" "Shocking" */
	DRIVER( tgtball ) /* 1995 "Yun Sung" "Target Ball (Nude)" */
	DRIVER( tgtballa ) /* 1995 "Yun Sung" "Target Ball" */
	DRIVER( torus ) /* 1996 "Yun Sung" "Torus" */
	DRIVER( wondstck ) /* ???? "Yun Sung" "Wonder Stick" */
#endif
#ifdef LINK_ZACCARIA
	DRIVER( catnmous ) /* 1982 "Zaccaria" "Cat and Mouse (set 1)" */
	DRIVER( dodgem ) /* 1979 "Zaccaria" "Dodgem" */
	DRIVER( jackrab2 ) /* 1984 "Zaccaria" "Jack Rabbit (set 2)" */
	DRIVER( jackrabs ) /* 1984 "Zaccaria" "Jack Rabbit (special)" */
	DRIVER( jackrabt ) /* 1984 "Zaccaria" "Jack Rabbit (set 1)" */
	DRIVER( laserbat ) /* 1981 "Zaccaria" "Laser Battle" */
	DRIVER( lazarian ) /* 1981 "Bally Midway (Zaccaria License)" "Lazarian" */
	DRIVER( monymony ) /* 1983 "Zaccaria" "Money Money" */
	DRIVER( sia2650 ) /* 1978 "Zaccaria/Zelco" "Super Invader Attack" */
	DRIVER( tinv2650 ) /* 1978 "Zaccaria/Zelco" "The Invaders" */
#endif
#endif	/* DRIVER_RECURSIVE */
