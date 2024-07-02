# amigamame

 This is a Port of Mame106 (2006) to Amiga classic 68060, using bebbo gcc6.5 crosscompiler and cmake.
 would only work on very fast Amigas with more than 64Mo. For the moment it also needs Cybergraphics or Picasso, and >=OS3.1. This is hugely based on Mame060 from triumph for the MUI part. All these are under GPL licences.
 
 Build instructions:
 
 If not done, Install a working bebbo gcc6.5 on linux, windows or macos.
 follow instructions at : https://github.com/bebbo/amiga-gcc
 also install git and cmake.
 
 ...Then in some directory do:
 
 git clone https://github.com/krabobmkd/amigacommonlibs
 git clone https://github.com/krabobmkd/amigamame
 cd amigamame
 git switch beta106
 
 ... then download mame106 source at
   https://github.com/mamedev/historic-mame/releases/download/mame0106/mame0106s.zip
 unzip and do a non-overwriting copy in amigamame/mame106 to add the missing files.
 
 then still in amigamame:
 mkdir amiga-mame-build
 cd amiga-mame-build
 cmake ../amigamame106/ -DCMAKE_TOOLCHAIN_FILE=../../amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake -DCMAKE_BUILD_TYPE=Release

 .. then build with:
 cmake --build .
 
 The cmake platform will search gcc compiler in /opt/amiga on linux and macos, or search it from path. Windows will only look C:\cygwin64\opt\amiga , if it's elsewhere edit the platform file. Yes, the Windows version is also working, and this cmake platform also allow .s old syntax assembler sources to be automatically assembled with vasm and linked.
 
 For development, you may also configure a "amiga gcc platform" with an IDE like QtCreator or VSCode, using this cmake platform in amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake.
 
   Warning : BETA in development.
 Bug reports, requests, ideas, love letters and maybe pull requests have one natural place:
 https://github.com/krabobmkd/amigamame/issues
 
 Too slow, for experimental purpose, at this level. (well, does great with UAE JIT)
 Efforts are focused on translating original makefiles to more configurable cmake files in order to enable or disable drivers at linking , having no bugs or memory leak. Only most popular drivers are there. This project Also focus on having 4 players with lowlevel.library and parallel port pads.

 Everything is still slow ,(between 40% and 60% of native speed) on a pistorm 3, only "dino" is likely playable.
 - audio disabled (no use if per are not 100%.)
 - neogeo non working (because i dont know ) 
 - rotations and flip modes unmanaged on this version.
 
 You can switch on and off the linking of drivers with options on top of: mame106\gamedrivers.cmake,
 which makes things very handy. This .cmake is generated with buildtools/mameMakToCmake that allows that. Not all switch are tested and some sub-configurations may have some missing defines. In that case the correction should be made in mameMakToCmake, not the .cmake itself.
 
  Users doc:
  It's mame. It needs copyrighted .zip files in a "roms" directory. Read Mame106 documentation.
  
  P96 users are urged to create screen modes with adapted resolutions for their favorite games so it looks fullscreen. Do not use mode promotion, each games have different needs.
  
  If you have a CD32 pads plug it before launching Mame.
  Keyboard if very finely used and will not mix up your country keymap configuration.
  
  Player1 use port 2
  Player2 use mouse port  ... or reconfigure that (tab)
  Player 3 4 use parralel port extension but allow just one button. ... or reconfigure that (tab).
  Keys:
  
  p			pause.
  Tab		configure panel.
  Esc		exit panel or game.
  ShitF7	(+some pad number)save state
  F3		reset machine
  F4 		display machine palette
  F7		(+some apd number)load state
  Help		display/hide performance stats. (note:when in pause only video driver is emulated.)
  1-4		Player 1/2/3/4 start
  5-8		Player 1/2/3/4 Coins
  
 Don't try to use drivers that are marked "not working.", just look if a parent driver can work. 
  
   