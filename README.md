# amigamame

 This is a Port of Mame106 (2006) to Amiga classic 68060, using bebbo gcc6.5 crosscompiler and cmake.
 would only work on very fast Amigas with more than 64Mo. For the moment it also needs Cybergraphics or Picasso, and >=OS3.1. This is hugely based on Mame060 from triumph for the MUI part. All these are under GPL licences.
 
 Build instructions:
 
 If not done, Install a working bebbo gcc6.5 on linux windows or macos.
 follow instructions at : https://github.com/bebbo/amiga-gcc
 
 ...Then in some directory do:
 
 git clone https://github.com/krabobmkd/amigacommonlibs
 git clone https://github.com/krabobmkd/amigamame
 cd amigamame
 git switch beta106
 
 ... then download mame106 source at
   https://github.com/mamedev/historic-mame/releases/download/mame0106/mame0106s.zip
 unzip and do a non-overwriting copy in amigamame/mame106 to add the missing files.
 
 then:
 mkdir amiga-mame-build
 cd amiga-mame-build
 
 cmake ../
 
 The cmake platform will search gcc compiler in /opt/amiga on linux and macos, or search it from path. Windows will only look C:\cygwin64\opt\amiga , if it's elsewhere edit the platform file.
 
 For development, you may also configure a "amiga gcc platform" with an IDE like QtCreator or VSCode, using the cmake platform in amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake.
 
   Warning : BETA in development.
 
 Too slow, this is for experimental purpose at this level.
 Efforts are focused on translating original makefiles to more configurable cmake files in order to enable or disable drivers at linking. Only most popular drivers are there.
 Also focus on having 4 players with lowlevel.library and parralel port pads.

 Everything is still slow on a pistorm 3, only "dino" is likely playable,
 audio disabled, neogeo non working.
 
  