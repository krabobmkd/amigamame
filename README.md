# amigamame

Release 1.7

 Mame106Minimix is a fork of Mame106 (2006) to Amiga classic 68060, using bebbo gcc6.5 crosscompiler and cmake.
 would only work on very fast Amigas with more than 64Mo (likely a PiStorm + Rpi3 or 4). 
 Takes advantages of P96/CGX , MUI and AHI and AmigaOS3.1 and later. 
 This is hugely based on Mame060 from triumph for the MUI part. All these projects are under GPL licences.
 This is now largely forked from original 106 version.
 
  Users doc: https://github.com/krabobmkd/amigamame/wiki/MAME106-MiniMix-1.x-User-Documentation
 
 Developer notes:
 
 Build instructions:
 
 If not done, Install a working bebbo gcc6.5 on linux, windows or macos.
 follow instructions at : https://github.com/bebbo/amiga-gcc
 also install git and cmake.
 
 ...Then in some directory do:
 
 git clone https://github.com/krabobmkd/amigacommonlibs
 
 git clone https://github.com/krabobmkd/amigamame
 
 cd amigamame

 git switch main
 
 ... then download mame106 source at
   https://github.com/mamedev/historic-mame/releases/download/mame0106/mame0106s.zip
 unzip and do a non-overwriting copy in amigamame/mame106 to add the missing files.
 
 then still in amigamame:
 
 mkdir amiga-mame-build
 
 cd amiga-mame-build
 
 cmake ../amigamame106/ 
 -DCMAKE_TOOLCHAIN_FILE=../../amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake -DCMAKE_BUILD_TYPE=Release

 .. then build with:
 
 cmake --build .
 
 The cmake platform will search gcc compiler in /opt/amiga on linux and macos, or search it from path. Windows will only look C:\cygwin64\opt\amiga , if it's elsewhere edit the platform file. Yes, the Windows version is also working.
This cmake platform:
 - allows .s assembler files with old 68k syntax to be automatically assembled with vasm.
 - configures "-ansi, -D__AMIGA__ -unsigned-char" but let other options to be specified by projects. 
 
 For development, you may also configure a "amiga gcc platform" with an IDE like QtCreator or VSCode, using this cmake platform in amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake.
 
 Bug reports, requests, ideas, love letters and maybe pull requests have one natural place:
 https://github.com/krabobmkd/amigamame/issues
 
 Most games are now at 100% of the native speed on a RPi3. (Beta1 release with original source + O3 option
  gave around 40%.)
 
 You can switch on and off the linking of drivers with cmake options on top of: mame106\gamedrivers.cmake,
 which makes things very handy. This .cmake is generated with buildtools/mameMakToCmake that made it possible. Not all switch are tested and some sub-configurations may have some missing defines. In that case the correction should be made in mameMakToCmake, not the .cmake itself.
 
  
 
  
   
