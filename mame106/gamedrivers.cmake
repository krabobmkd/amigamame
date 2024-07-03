option(OPT_ALBA "" OFF)
option(OPT_ALLIEDL "" OFF)
option(OPT_ALPHA "" OFF)
option(OPT_AMIGA "" OFF)
option(OPT_ATARI "" OFF)
option(OPT_ATLUS "" OFF)
option(OPT_BFM "" OFF)
option(OPT_CAPCOM "" ON)
option(OPT_CINEMAT "" OFF)
option(OPT_COMAD "" OFF)
option(OPT_CVS "" OFF)
option(OPT_DATAEAST "" OFF)
option(OPT_DOOYONG "" OFF)
option(OPT_DYNAX "" OFF)
option(OPT_EDEVICES "" OFF)
option(OPT_EOLITH "" OFF)
option(OPT_EXCELENT "" OFF)
option(OPT_EXIDY "" OFF)
option(OPT_F32 "" OFF)
option(OPT_FUUKI "" OFF)
option(OPT_GAELCO "" OFF)
option(OPT_GAMEPLAN "" OFF)
option(OPT_GAMETRON "" OFF)
option(OPT_GOTTLIEB "" OFF)
option(OPT_GREYHND "" OFF)
option(OPT_IGS "" OFF)
option(OPT_IREM "" OFF)
option(OPT_ITECH "" OFF)
option(OPT_JALECO "" OFF)
option(OPT_KANEKO "" OFF)
option(OPT_KONAMI "" OFF)
option(OPT_MEADOWS "" OFF)
option(OPT_MERIT "" OFF)
option(OPT_METRO "" OFF)
option(OPT_MIDCOIN "" OFF)
option(OPT_MIDW8080 "" OFF)
option(OPT_MIDWAY "" OFF)
option(OPT_MIDWAYTUNIT "" OFF)
option(OPT_MINIKONAMI "" OFF)
option(OPT_MISC "" OFF)
option(OPT_NAMCO "" OFF)
option(OPT_NASCO "" OFF)
option(OPT_NEOGEO "" OFF)
option(OPT_NICHIBUT "" OFF)
option(OPT_NINTENDO "" OFF)
option(OPT_NIX "" OFF)
option(OPT_NMK "" OFF)
option(OPT_OLYMPIA "" OFF)
option(OPT_OMORI "" OFF)
option(OPT_ORCA "" OFF)
option(OPT_PACIFIC "" OFF)
option(OPT_PACMAN "" OFF)
option(OPT_PHOENIX "" OFF)
option(OPT_PLAYMARK "" OFF)
option(OPT_PSIKYO "" OFF)
option(OPT_RAMTEK "" OFF)
option(OPT_RARE "" OFF)
option(OPT_SANRITSU "" OFF)
option(OPT_SEGA "" ON)
option(OPT_SEIBU "" OFF)
option(OPT_SETA "" OFF)
option(OPT_SHARED "" ON)
option(OPT_SIGMA "" OFF)
option(OPT_SNK "" OFF)
option(OPT_STERN "" OFF)
option(OPT_SUN "" OFF)
option(OPT_SUNA "" OFF)
option(OPT_TAD "" OFF)
option(OPT_TAITO "" OFF)
option(OPT_TATSUMI "" OFF)
option(OPT_TCH "" OFF)
option(OPT_TECFRI "" OFF)
option(OPT_TECHNOS "" OFF)
option(OPT_TEHKAN "" OFF)
option(OPT_THEPIT "" OFF)
option(OPT_TOAPLAN "" OFF)
option(OPT_TONG "" OFF)
option(OPT_UNICO "" OFF)
option(OPT_UNIVERS "" OFF)
option(OPT_UPL "" OFF)
option(OPT_VALADON "" OFF)
option(OPT_VELTMJR "" OFF)
option(OPT_VENTURE "" OFF)
option(OPT_VSYSTEM "" OFF)
option(OPT_YUNSUNG "" OFF)
option(OPT_ZACCARIA "" OFF)
if(OPT_ALBA)
	add_compile_definitions(LINK_ALBA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/hanaroku.c drivers/rmhaihai.c drivers/yumefuda.c 	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 	)
endif()
if(OPT_ALLIEDL)
	add_compile_definitions(LINK_ALLIEDL=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/ace.c drivers/clayshoo.c machine/clayshoo.c vidhrdw/clayshoo.c 
	)
endif()
if(OPT_ALPHA)
	add_compile_definitions(LINK_ALPHA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/alpha68k.c vidhrdw/alpha68k.c drivers/champbas.c vidhrdw/champbas.c 
		drivers/equites.c machine/equites.c vidhrdw/equites.c drivers/exctsccr.c 
		machine/exctsccr.c vidhrdw/exctsccr.c drivers/meijinsn.c drivers/shougi.c 
		drivers/talbot.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_MSM5232 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM3812 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_MSM5232=1 HAS_YM2203=1 
		HAS_YM2413=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_AMIGA)
	add_compile_definitions(LINK_AMIGA=1)
	list(APPEND MAME_DRIVERS_SRC
		machine/amiga.c sndhrdw/amiga.c vidhrdw/amiga.c drivers/arcadia.c 
		drivers/mquake.c drivers/upscope.c 	)
	set(MSND_CUSTOM ON)
	set(MSND_ES5503 ON)
	set(MCPU_M68000 ON)
	list(APPEND CPU_DEFS
		HAS_CUSTOM=1 HAS_ES5503=1 	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 	)
endif()
if(OPT_ATARI)
	add_compile_definitions(LINK_ATARI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/atarigx2.c vidhrdw/atarigx2.c drivers/arcadecl.c vidhrdw/arcadecl.c 
		drivers/asteroid.c machine/asteroid.c sndhrdw/asteroid.c sndhrdw/llander.c 
		drivers/atarifb.c machine/atarifb.c sndhrdw/atarifb.c vidhrdw/atarifb.c 
		drivers/atarig1.c vidhrdw/atarig1.c drivers/atarig42.c vidhrdw/atarig42.c 
		drivers/atarigt.c vidhrdw/atarigt.c drivers/atarisy1.c vidhrdw/atarisy1.c 
		drivers/atarisy2.c vidhrdw/atarisy2.c drivers/atetris.c vidhrdw/atetris.c 
		drivers/avalnche.c machine/avalnche.c sndhrdw/avalnche.c vidhrdw/avalnche.c 
		drivers/badlands.c vidhrdw/badlands.c drivers/batman.c vidhrdw/batman.c 
		drivers/beathead.c vidhrdw/beathead.c drivers/blstroid.c vidhrdw/blstroid.c 
		drivers/boxer.c vidhrdw/boxer.c drivers/bsktball.c machine/bsktball.c 
		sndhrdw/bsktball.c vidhrdw/bsktball.c drivers/bwidow.c drivers/bzone.c 
		sndhrdw/bzone.c drivers/canyon.c sndhrdw/canyon.c vidhrdw/canyon.c 
		drivers/cball.c drivers/ccastles.c vidhrdw/ccastles.c drivers/centiped.c 
		vidhrdw/centiped.c drivers/cloak.c vidhrdw/cloak.c drivers/cloud9.c 
		vidhrdw/cloud9.c drivers/cojag.c sndhrdw/jaguar.c vidhrdw/jaguar.c 
		drivers/copsnrob.c machine/copsnrob.c vidhrdw/copsnrob.c drivers/cyberbal.c 
		sndhrdw/cyberbal.c vidhrdw/cyberbal.c drivers/destroyr.c vidhrdw/destroyr.c 
		drivers/dragrace.c sndhrdw/dragrace.c vidhrdw/dragrace.c drivers/eprom.c 
		vidhrdw/eprom.c drivers/firetrk.c sndhrdw/firetrk.c vidhrdw/firetrk.c 
		drivers/flyball.c vidhrdw/flyball.c drivers/foodf.c vidhrdw/foodf.c 
		drivers/gauntlet.c vidhrdw/gauntlet.c drivers/harddriv.c machine/harddriv.c 
		sndhrdw/harddriv.c vidhrdw/harddriv.c drivers/irobot.c machine/irobot.c 
		vidhrdw/irobot.c drivers/jedi.c vidhrdw/jedi.c drivers/klax.c 
		vidhrdw/klax.c drivers/liberatr.c vidhrdw/liberatr.c drivers/mediagx.c 
		drivers/mgolf.c drivers/mhavoc.c machine/mhavoc.c drivers/missile.c 
		vidhrdw/missile.c drivers/nitedrvr.c machine/nitedrvr.c sndhrdw/nitedrvr.c 
		vidhrdw/nitedrvr.c drivers/offtwall.c vidhrdw/offtwall.c drivers/orbit.c 
		sndhrdw/orbit.c vidhrdw/orbit.c drivers/poolshrk.c sndhrdw/poolshrk.c 
		vidhrdw/poolshrk.c drivers/quantum.c drivers/rampart.c vidhrdw/rampart.c 
		drivers/relief.c vidhrdw/relief.c drivers/runaway.c vidhrdw/runaway.c 
		drivers/sbrkout.c machine/sbrkout.c vidhrdw/sbrkout.c drivers/shuuz.c 
		vidhrdw/shuuz.c drivers/skullxbo.c vidhrdw/skullxbo.c drivers/skydiver.c 
		sndhrdw/skydiver.c vidhrdw/skydiver.c drivers/skyraid.c vidhrdw/skyraid.c 
		drivers/sprint2.c sndhrdw/sprint2.c vidhrdw/sprint2.c drivers/sprint4.c 
		sndhrdw/sprint4.c vidhrdw/sprint4.c drivers/sprint8.c vidhrdw/sprint8.c 
		drivers/starshp1.c vidhrdw/starshp1.c drivers/starwars.c machine/starwars.c 
		sndhrdw/starwars.c drivers/subs.c machine/subs.c sndhrdw/subs.c 
		vidhrdw/subs.c drivers/tank8.c sndhrdw/tank8.c vidhrdw/tank8.c 
		drivers/tempest.c drivers/thunderj.c vidhrdw/thunderj.c drivers/toobin.c 
		vidhrdw/toobin.c drivers/tourtabl.c vidhrdw/tia.c drivers/triplhnt.c 
		sndhrdw/triplhnt.c vidhrdw/triplhnt.c drivers/tunhunt.c vidhrdw/tunhunt.c 
		drivers/videopin.c sndhrdw/videopin.c vidhrdw/videopin.c drivers/vindictr.c 
		vidhrdw/vindictr.c drivers/wolfpack.c vidhrdw/wolfpack.c drivers/xybots.c 
		vidhrdw/xybots.c machine/asic65.c machine/atari_vg.c machine/atarigen.c 
		machine/mathbox.c machine/slapstic.c sndhrdw/atarijsa.c sndhrdw/cage.c 
		sndhrdw/redbaron.c vidhrdw/atarimo.c vidhrdw/atarirle.c 	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_DISCRETE ON)
	set(MSND_DMADAC ON)
	set(MSND_OKIM6295 ON)
	set(MSND_POKEY ON)
	set(MSND_SN76496 ON)
	set(MSND_TIA ON)
	set(MSND_TMS5220 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2413 ON)
	set(MCPU_ADSP2100 ON)
	set(MCPU_ASAP ON)
	set(MCPU_I386 ON)
	set(MCPU_JAGUAR ON)
	set(MCPU_M6502 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_S2650 ON)
	set(MCPU_T11 ON)
	set(MCPU_TMS32010 ON)
	set(MCPU_TMS32031 ON)
	set(MCPU_TMS34010 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_DISCRETE=1 
		HAS_DMADAC=1 HAS_OKIM6295=1 HAS_POKEY=1 HAS_SN76496=1 
		HAS_TIA=1 HAS_TMS5220=1 HAS_YM2151=1 HAS_YM2413=1 
	)
	list(APPEND CPU_DEFS
		HAS_ADSP2100=1 HAS_ASAP=1 HAS_I386=1 HAS_JAGUAR=1 
		HAS_M6502=1 HAS_M68000=1 HAS_M6809=1 HAS_S2650=1 
		HAS_T11=1 HAS_TMS32010=1 HAS_TMS32031=1 HAS_TMS34010=1 
	)
endif()
if(OPT_ATLUS)
	add_compile_definitions(LINK_ATLUS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/blmbycar.c vidhrdw/blmbycar.c drivers/ohmygod.c vidhrdw/ohmygod.c 
		drivers/powerins.c vidhrdw/powerins.c 	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2203=1 	)
endif()
if(OPT_BFM)
	add_compile_definitions(LINK_BFM=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/bfm_sc2.c vidhrdw/bfm_adr2.c drivers/mpu4.c machine/lamps.c 
		machine/mmtr.c machine/steppers.c machine/vacfdisp.c 	)
	set(MSND_AY8910 ON)
	set(MSND_SAA1099 ON)
	set(MSND_UPD7759 ON)
	set(MSND_YM2413 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_SAA1099=1 HAS_UPD7759=1 HAS_YM2413=1 
	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_M6809=1 	)
endif()
if(OPT_CAPCOM)
	add_compile_definitions(LINK_CAPCOM=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/1942.c vidhrdw/1942.c drivers/1943.c vidhrdw/1943.c 
		drivers/bionicc.c vidhrdw/bionicc.c drivers/blktiger.c vidhrdw/blktiger.c 
		drivers/cbasebal.c vidhrdw/cbasebal.c drivers/commando.c vidhrdw/commando.c 
		drivers/cps1.c vidhrdw/cps1.c drivers/cps2.c drivers/cps3.c 
		drivers/egghunt.c drivers/fcrash.c drivers/gng.c vidhrdw/gng.c 
		drivers/gunsmoke.c vidhrdw/gunsmoke.c drivers/exedexes.c vidhrdw/exedexes.c 
		drivers/higemaru.c vidhrdw/higemaru.c drivers/lastduel.c vidhrdw/lastduel.c 
		drivers/lwings.c vidhrdw/lwings.c drivers/mitchell.c vidhrdw/mitchell.c 
		drivers/sf.c vidhrdw/sf.c drivers/sidearms.c vidhrdw/sidearms.c 
		drivers/sonson.c vidhrdw/sonson.c drivers/srumbler.c vidhrdw/srumbler.c 
		drivers/vulgus.c vidhrdw/vulgus.c drivers/tigeroad.c vidhrdw/tigeroad.c 
		drivers/zn.c machine/znsec.c machine/at28c16.c machine/mb3773.c 
		machine/kabuki.c machine/psx.c vidhrdw/psx.c sndhrdw/taitosnd.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_PSXSPU ON)
	set(MSND_QSOUND ON)
	set(MSND_SN76496 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM2610 ON)
	set(MSND_YM2610B ON)
	set(MSND_YM3812 ON)
	set(MSND_YMF271 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_PSXCPU ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 HAS_OKIM6295=1 HAS_PSXSPU=1 
		HAS_QSOUND=1 HAS_SN76496=1 HAS_YM2151=1 HAS_YM2203=1 
		HAS_YM2413=1 HAS_YM2610=1 HAS_YM2610B=1 HAS_YM3812=1 
		HAS_YMF271=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_M6809=1 HAS_PSXCPU=1 HAS_Z80=1 
	)
endif()
if(OPT_CINEMAT)
	add_compile_definitions(LINK_CINEMAT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/ataxx.c drivers/cinemat.c sndhrdw/cinemat.c vidhrdw/cinemat.c 
		drivers/cchasm.c machine/cchasm.c sndhrdw/cchasm.c vidhrdw/cchasm.c 
		drivers/dlair.c drivers/embargo.c drivers/jack.c vidhrdw/jack.c 
		drivers/leland.c machine/leland.c sndhrdw/leland.c vidhrdw/leland.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2151 ON)
	set(MCPU_CCPU ON)
	set(MCPU_I86 ON)
	set(MCPU_S2650 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_SAMPLES=1 
		HAS_YM2151=1 	)
	list(APPEND CPU_DEFS
		HAS_CCPU=1 HAS_I86=1 HAS_S2650=1 HAS_Z80=1 
	)
endif()
if(OPT_COMAD)
	add_compile_definitions(LINK_COMAD=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/funybubl.c vidhrdw/funybubl.c drivers/galspnbl.c vidhrdw/galspnbl.c 
		drivers/pushman.c vidhrdw/pushman.c drivers/zerozone.c vidhrdw/zerozone.c 
	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M6805 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2203=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_M6805=1 HAS_Z80=1 	)
endif()
if(OPT_CVS)
	add_compile_definitions(LINK_CVS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/cvs.c vidhrdw/cvs.c vidhrdw/s2636.c drivers/quasar.c 
		vidhrdw/quasar.c 	)
	set(MSND_DAC ON)
	set(MSND_TMS5110 ON)
	set(MCPU_I8039 ON)
	set(MCPU_S2650 ON)
	list(APPEND CPU_DEFS
		HAS_DAC=1 HAS_TMS5110=1 	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 HAS_S2650=1 	)
endif()
if(OPT_DATAEAST)
	add_compile_definitions(LINK_DATAEAST=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/actfancr.c vidhrdw/actfancr.c drivers/astrof.c sndhrdw/astrof.c 
		vidhrdw/astrof.c drivers/backfire.c drivers/battlera.c vidhrdw/battlera.c 
		drivers/boogwing.c vidhrdw/boogwing.c drivers/brkthru.c vidhrdw/brkthru.c 
		drivers/btime.c machine/btime.c vidhrdw/btime.c drivers/bwing.c 
		vidhrdw/bwing.c drivers/cbuster.c vidhrdw/cbuster.c drivers/cninja.c 
		vidhrdw/cninja.c drivers/cntsteer.c drivers/compgolf.c vidhrdw/compgolf.c 
		drivers/darkseal.c vidhrdw/darkseal.c drivers/dassault.c vidhrdw/dassault.c 
		drivers/dblewing.c drivers/dec0.c machine/dec0.c vidhrdw/dec0.c 
		drivers/dec8.c vidhrdw/dec8.c drivers/deco_mlc.c vidhrdw/deco_mlc.c 
		drivers/deco156.c machine/deco156.c drivers/deco32.c vidhrdw/deco32.c 
		drivers/decocass.c machine/decocass.c vidhrdw/decocass.c drivers/dietgo.c 
		vidhrdw/dietgo.c drivers/exprraid.c vidhrdw/exprraid.c drivers/firetrap.c 
		vidhrdw/firetrap.c drivers/funkyjet.c vidhrdw/funkyjet.c drivers/karnov.c 
		vidhrdw/karnov.c drivers/kchamp.c vidhrdw/kchamp.c drivers/kingobox.c 
		vidhrdw/kingobox.c drivers/lemmings.c vidhrdw/lemmings.c drivers/liberate.c 
		vidhrdw/liberate.c drivers/madalien.c drivers/madmotor.c vidhrdw/madmotor.c 
		drivers/metlclsh.c vidhrdw/metlclsh.c drivers/pcktgal.c vidhrdw/pcktgal.c 
		drivers/pktgaldx.c vidhrdw/pktgaldx.c drivers/rohga.c vidhrdw/rohga.c 
		drivers/shootout.c vidhrdw/shootout.c drivers/sidepckt.c vidhrdw/sidepckt.c 
		drivers/simpl156.c vidhrdw/simpl156.c drivers/sshangha.c vidhrdw/sshangha.c 
		drivers/stadhero.c vidhrdw/stadhero.c drivers/supbtime.c vidhrdw/supbtime.c 
		drivers/tryout.c vidhrdw/tryout.c drivers/tumbleb.c vidhrdw/tumbleb.c 
		drivers/tumbleb.c vidhrdw/tumbleb.c drivers/tumblep.c vidhrdw/tumblep.c 
		drivers/vaportra.c vidhrdw/vaportra.c machine/deco102.c machine/decocrpt.c 
		machine/decoprot.c vidhrdw/deco16ic.c 	)
	set(MSND_AY8910 ON)
	set(MSND_BSMT2000 ON)
	set(MSND_C6280 ON)
	set(MSND_DAC ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_ARM ON)
	set(MCPU_DECO16 ON)
	set(MCPU_H6280 ON)
	set(MCPU_I8X41 ON)
	set(MCPU_M6502 ON)
	set(MCPU_M65C02 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_BSMT2000=1 HAS_C6280=1 HAS_DAC=1 
		HAS_MSM5205=1 HAS_OKIM6295=1 HAS_SAMPLES=1 HAS_YM2151=1 
		HAS_YM2203=1 HAS_YM3812=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_ARM=1 HAS_DECO16=1 HAS_H6280=1 HAS_I8X41=1 
		HAS_M6502=1 HAS_M65C02=1 HAS_M68000=1 HAS_M6809=1 
		HAS_Z80=1 	)
endif()
if(OPT_DOOYONG)
	add_compile_definitions(LINK_DOOYONG=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/dooyong.c vidhrdw/dooyong.c drivers/gundealr.c vidhrdw/gundealr.c 
	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 	)
endif()
if(OPT_DYNAX)
	add_compile_definitions(LINK_DYNAX=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/cherrym2.c drivers/ddenlovr.c drivers/dynax.c vidhrdw/dynax.c 
		drivers/hnayayoi.c vidhrdw/hnayayoi.c drivers/rcasino.c drivers/realbrk.c 
		vidhrdw/realbrk.c drivers/royalmah.c 	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 HAS_OKIM6295=1 HAS_YM2203=1 
		HAS_YM2413=1 HAS_YM3812=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_EDEVICES)
	add_compile_definitions(LINK_EDEVICES=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/diverboy.c vidhrdw/diverboy.c drivers/fantland.c vidhrdw/fantland.c 
		drivers/mwarr.c drivers/mugsmash.c vidhrdw/mugsmash.c drivers/stlforce.c 
		vidhrdw/stlforce.c drivers/ppmast93.c drivers/twins.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM3812 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_MSM5205=1 HAS_OKIM6295=1 
		HAS_YM2151=1 HAS_YM2413=1 HAS_YM3812=1 	)
endif()
if(OPT_EOLITH)
	add_compile_definitions(LINK_EOLITH=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/eolith.c vidhrdw/eolith.c drivers/eolith16.c 	)
	set(MSND_OKIM6295 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 	)
endif()
if(OPT_EXCELENT)
	add_compile_definitions(LINK_EXCELENT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/aquarium.c vidhrdw/aquarium.c drivers/gcpinbal.c vidhrdw/gcpinbal.c 
		drivers/vmetal.c 	)
	set(MSND_ES8712 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MCPU_M68000 ON)
	list(APPEND CPU_DEFS
		HAS_ES8712=1 HAS_MSM5205=1 HAS_OKIM6295=1 HAS_YM2151=1 
	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 	)
endif()
if(OPT_EXIDY)
	add_compile_definitions(LINK_EXIDY=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/carpolo.c machine/carpolo.c vidhrdw/carpolo.c drivers/circus.c 
		sndhrdw/circus.c vidhrdw/circus.c drivers/exidy.c sndhrdw/exidy.c 
		vidhrdw/exidy.c drivers/exidy440.c sndhrdw/exidy440.c vidhrdw/exidy440.c 
		drivers/maxaflex.c machine/atari.c vidhrdw/atari.c vidhrdw/antic.c 
		vidhrdw/gtia.c drivers/starfire.c vidhrdw/starfire.c drivers/vertigo.c 
		machine/vertigo.c vidhrdw/vertigo.c drivers/victory.c vidhrdw/victory.c 
		sndhrdw/targ.c 	)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_HC55516 ON)
	set(MSND_POKEY ON)
	set(MSND_SAMPLES ON)
	set(MSND_TMS5220 ON)
	set(MCPU_M6502 ON)
	set(MCPU_M6805 ON)
	list(APPEND CPU_DEFS
		HAS_CUSTOM=1 HAS_DAC=1 HAS_HC55516=1 HAS_POKEY=1 
		HAS_SAMPLES=1 HAS_TMS5220=1 	)
	list(APPEND CPU_DEFS
		HAS_M6502=1 HAS_M6805=1 	)
endif()
if(OPT_F32)
	add_compile_definitions(LINK_F32=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/crospang.c vidhrdw/crospang.c drivers/f-32.c 	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM3812 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM3812=1 	)
endif()
if(OPT_FUUKI)
	add_compile_definitions(LINK_FUUKI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/fuukifg2.c vidhrdw/fuukifg2.c drivers/fuukifg3.c vidhrdw/fuukifg3.c 
	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMF262 ON)
	set(MSND_YMF278B ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2203=1 HAS_YM3812=1 HAS_YMF262=1 
		HAS_YMF278B=1 	)
endif()
if(OPT_GAELCO)
	add_compile_definitions(LINK_GAELCO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/gaelco.c vidhrdw/gaelco.c drivers/gaelco2.c machine/gaelco2.c 
		vidhrdw/gaelco2.c drivers/gaelco3d.c vidhrdw/gaelco3d.c drivers/glass.c 
		vidhrdw/glass.c drivers/mastboy.c drivers/splash.c vidhrdw/splash.c 
		drivers/targeth.c vidhrdw/targeth.c drivers/thoop2.c vidhrdw/thoop2.c 
		drivers/xorworld.c vidhrdw/xorworld.c drivers/wrally.c machine/wrally.c 
		vidhrdw/wrally.c 	)
	set(MSND_DMADAC ON)
	set(MSND_GAELCO_GAE1 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAA1099 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_ADSP2100 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_TMS32031 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_DMADAC=1 HAS_GAELCO_GAE1=1 HAS_MSM5205=1 HAS_OKIM6295=1 
		HAS_SAA1099=1 HAS_YM2203=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_ADSP2100=1 HAS_M68000=1 HAS_M6809=1 HAS_TMS32031=1 
		HAS_Z80=1 	)
endif()
if(OPT_GAMEPLAN)
	add_compile_definitions(LINK_GAMEPLAN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/enigma2.c drivers/gameplan.c drivers/toratora.c 	)
	set(MSND_AY8910 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 	)
endif()
if(OPT_GAMETRON)
	add_compile_definitions(LINK_GAMETRON=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/gotya.c sndhrdw/gotya.c vidhrdw/gotya.c drivers/sbugger.c 
		vidhrdw/sbugger.c 	)
	set(MSND_SAMPLES ON)
	list(APPEND CPU_DEFS
		HAS_SAMPLES=1 	)
endif()
if(OPT_GOTTLIEB)
	add_compile_definitions(LINK_GOTTLIEB=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/exterm.c vidhrdw/exterm.c drivers/gottlieb.c sndhrdw/gottlieb.c 
		vidhrdw/gottlieb.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SAMPLES ON)
	set(MSND_SP0250 ON)
	set(MSND_YM2151 ON)
	set(MCPU_M6502 ON)
	set(MCPU_TMS34010 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SAMPLES=1 HAS_SP0250=1 
		HAS_YM2151=1 	)
	list(APPEND CPU_DEFS
		HAS_M6502=1 HAS_TMS34010=1 	)
endif()
if(OPT_GREYHND)
	add_compile_definitions(LINK_GREYHND=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/findout.c drivers/getrivia.c 	)
	set(MSND_DAC ON)
	list(APPEND CPU_DEFS
		HAS_DAC=1 	)
endif()
if(OPT_IGS)
	add_compile_definitions(LINK_IGS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/csk.c vidhrdw/csk.c drivers/goldstar.c vidhrdw/goldstar.c 
		drivers/igs_blit.c drivers/iqblock.c vidhrdw/iqblock.c drivers/lordgun.c 
		vidhrdw/lordgun.c drivers/pgm.c vidhrdw/pgm.c drivers/tarzan.c 
		machine/pgmcrypt.c machine/pgmprot.c machine/pgmy2ks.c 	)
	set(MSND_AY8910 ON)
	set(MSND_ICS2115 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM3812 ON)
	set(MCPU_ARM7 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_ICS2115=1 HAS_OKIM6295=1 HAS_YM2413=1 
		HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_ARM7=1 HAS_Z80=1 	)
endif()
if(OPT_IREM)
	add_compile_definitions(LINK_IREM=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/m62.c vidhrdw/m62.c drivers/m72.c sndhrdw/m72.c 
		vidhrdw/m72.c drivers/m90.c vidhrdw/m90.c drivers/m92.c 
		vidhrdw/m92.c drivers/m107.c vidhrdw/m107.c drivers/mpatrol.c 
		vidhrdw/mpatrol.c drivers/olibochu.c drivers/redalert.c sndhrdw/redalert.c 
		vidhrdw/redalert.c drivers/shisen.c vidhrdw/shisen.c drivers/skychut.c 
		vidhrdw/skychut.c drivers/travrusa.c vidhrdw/travrusa.c drivers/troangel.c 
		vidhrdw/troangel.c drivers/vigilant.c vidhrdw/vigilant.c drivers/wilytowr.c 
		drivers/yard.c vidhrdw/yard.c machine/irem_cpu.c sndhrdw/fghtbskt.c 
		sndhrdw/irem.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_IREMGA20 ON)
	set(MSND_MSM5205 ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MCPU_I8039 ON)
	set(MCPU_M6502 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M6803 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_IREMGA20=1 HAS_MSM5205=1 
		HAS_SAMPLES=1 HAS_YM2151=1 HAS_YM2203=1 	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 HAS_M6502=1 HAS_M6800=1 HAS_M6803=1 
		HAS_Z80=1 	)
endif()
if(OPT_ITECH)
	add_compile_definitions(LINK_ITECH=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/capbowl.c vidhrdw/capbowl.c drivers/itech8.c machine/slikshot.c 
		vidhrdw/itech8.c drivers/itech32.c vidhrdw/itech32.c 	)
	set(MSND_DAC ON)
	set(MSND_ES5506 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_DAC=1 HAS_ES5506=1 HAS_OKIM6295=1 HAS_YM2203=1 
		HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_M6809=1 HAS_Z80=1 	)
endif()
if(OPT_JALECO)
	add_compile_definitions(LINK_JALECO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/aeroboto.c vidhrdw/aeroboto.c drivers/argus.c vidhrdw/argus.c 
		drivers/bestleag.c drivers/bigstrkb.c vidhrdw/bigstrkb.c drivers/blueprnt.c 
		vidhrdw/blueprnt.c drivers/cischeat.c vidhrdw/cischeat.c drivers/citycon.c 
		vidhrdw/citycon.c drivers/ddayjlc.c drivers/exerion.c vidhrdw/exerion.c 
		drivers/fcombat.c vidhrdw/fcombat.c drivers/ginganin.c vidhrdw/ginganin.c 
		drivers/homerun.c vidhrdw/homerun.c drivers/megasys1.c vidhrdw/megasys1.c 
		drivers/momoko.c vidhrdw/momoko.c drivers/ms32.c vidhrdw/ms32.c 
		drivers/psychic5.c vidhrdw/psychic5.c drivers/pturn.c drivers/skyfox.c 
		vidhrdw/skyfox.c drivers/stepstag.c drivers/tetrisp2.c vidhrdw/tetrisp2.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMF271 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 
		HAS_YM3812=1 HAS_YMF271=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_KANEKO)
	add_compile_definitions(LINK_KANEKO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/airbustr.c vidhrdw/airbustr.c drivers/djboy.c vidhrdw/djboy.c 
		drivers/galpanic.c vidhrdw/galpanic.c drivers/galpani2.c vidhrdw/galpani2.c 
		drivers/galpani3.c drivers/jchan.c drivers/kaneko16.c machine/kaneko16.c 
		vidhrdw/kaneko16.c drivers/suprnova.c vidhrdw/suprnova.c 	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 HAS_YMZ280B=1 
	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_KONAMI)
	add_compile_definitions(LINK_KONAMI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/88games.c vidhrdw/88games.c drivers/ajax.c machine/ajax.c 
		vidhrdw/ajax.c drivers/aliens.c vidhrdw/aliens.c drivers/amidar.c 
		drivers/asterix.c vidhrdw/asterix.c drivers/battlnts.c vidhrdw/battlnts.c 
		drivers/bishi.c vidhrdw/bishi.c drivers/bladestl.c vidhrdw/bladestl.c 
		drivers/blockhl.c vidhrdw/blockhl.c drivers/bottom9.c vidhrdw/bottom9.c 
		drivers/chqflag.c vidhrdw/chqflag.c drivers/circusc.c vidhrdw/circusc.c 
		drivers/combatsc.c vidhrdw/combatsc.c drivers/contra.c vidhrdw/contra.c 
		drivers/crimfght.c vidhrdw/crimfght.c drivers/dbz.c vidhrdw/dbz.c 
		drivers/ddrible.c vidhrdw/ddrible.c drivers/djmain.c vidhrdw/djmain.c 
		drivers/fastfred.c vidhrdw/fastfred.c drivers/fastlane.c vidhrdw/fastlane.c 
		drivers/finalizr.c vidhrdw/finalizr.c drivers/flkatck.c vidhrdw/flkatck.c 
		drivers/frogger.c drivers/gberet.c vidhrdw/gberet.c drivers/gbusters.c 
		vidhrdw/gbusters.c drivers/gijoe.c vidhrdw/gijoe.c drivers/gradius3.c 
		vidhrdw/gradius3.c drivers/gticlub.c drivers/gyruss.c sndhrdw/gyruss.c 
		vidhrdw/gyruss.c drivers/hcastle.c vidhrdw/hcastle.c drivers/hexion.c 
		vidhrdw/hexion.c drivers/hornet.c machine/konppc.c drivers/hyperspt.c 
		vidhrdw/hyperspt.c drivers/ironhors.c vidhrdw/ironhors.c drivers/jackal.c 
		machine/jackal.c vidhrdw/jackal.c drivers/jailbrek.c vidhrdw/jailbrek.c 
		drivers/junofrst.c drivers/konamigq.c drivers/konamigv.c drivers/konamigx.c 
		machine/konamigx.c vidhrdw/konamigx.c drivers/konamim2.c drivers/labyrunr.c 
		vidhrdw/labyrunr.c drivers/lethal.c vidhrdw/lethal.c drivers/mainevt.c 
		vidhrdw/mainevt.c drivers/megazone.c vidhrdw/megazone.c drivers/mikie.c 
		vidhrdw/mikie.c drivers/mogura.c drivers/moo.c vidhrdw/moo.c 
		drivers/mystwarr.c vidhrdw/mystwarr.c drivers/nemesis.c vidhrdw/nemesis.c 
		drivers/nwk-tr.c drivers/overdriv.c vidhrdw/overdriv.c drivers/pandoras.c 
		vidhrdw/pandoras.c drivers/parodius.c vidhrdw/parodius.c drivers/pingpong.c 
		vidhrdw/pingpong.c drivers/plygonet.c vidhrdw/plygonet.c drivers/pooyan.c 
		vidhrdw/pooyan.c drivers/qdrmfgp.c vidhrdw/qdrmfgp.c drivers/rockrage.c 
		vidhrdw/rockrage.c drivers/rocnrope.c vidhrdw/rocnrope.c drivers/rollerg.c 
		vidhrdw/rollerg.c drivers/rungun.c vidhrdw/rungun.c drivers/sbasketb.c 
		vidhrdw/sbasketb.c drivers/scobra.c drivers/scotrsht.c vidhrdw/scotrsht.c 
		drivers/scramble.c machine/scramble.c sndhrdw/scramble.c drivers/shaolins.c 
		vidhrdw/shaolins.c drivers/simpsons.c machine/simpsons.c vidhrdw/simpsons.c 
		drivers/spy.c vidhrdw/spy.c drivers/surpratk.c vidhrdw/surpratk.c 
		drivers/thunderx.c vidhrdw/thunderx.c drivers/timeplt.c sndhrdw/timeplt.c 
		vidhrdw/timeplt.c drivers/tmnt.c vidhrdw/tmnt.c drivers/tp84.c 
		vidhrdw/tp84.c drivers/trackfld.c machine/konami.c sndhrdw/trackfld.c 
		vidhrdw/trackfld.c drivers/tutankhm.c vidhrdw/tutankhm.c drivers/twin16.c 
		vidhrdw/twin16.c drivers/ultraman.c vidhrdw/ultraman.c drivers/vendetta.c 
		vidhrdw/vendetta.c drivers/wecleman.c vidhrdw/wecleman.c drivers/xexex.c 
		vidhrdw/xexex.c drivers/xmen.c vidhrdw/xmen.c drivers/yiear.c 
		vidhrdw/yiear.c drivers/zr107.c vidhrdw/konamiic.c 	)
	set(MSND_AY8910 ON)
	set(MSND_CDDA ON)
	set(MSND_DAC ON)
	set(MSND_K005289 ON)
	set(MSND_K007232 ON)
	set(MSND_K051649 ON)
	set(MSND_K053260 ON)
	set(MSND_K054539 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_PSXSPU ON)
	set(MSND_RF5C400 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76496 ON)
	set(MSND_TMS5110 ON)
	set(MSND_UPD7759 ON)
	set(MSND_VLM5030 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_HD6309 ON)
	set(MCPU_I8039 ON)
	set(MCPU_KONAMI ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_S2650 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CDDA=1 HAS_DAC=1 HAS_K005289=1 
		HAS_K007232=1 HAS_K051649=1 HAS_K053260=1 HAS_K054539=1 
		HAS_MSM5205=1 HAS_OKIM6295=1 HAS_PSXSPU=1 HAS_RF5C400=1 
		HAS_SAMPLES=1 HAS_SN76496=1 HAS_TMS5110=1 HAS_UPD7759=1 
		HAS_VLM5030=1 HAS_YM2151=1 HAS_YM2203=1 HAS_YM3812=1 
		HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_HD6309=1 HAS_I8039=1 HAS_KONAMI=1 HAS_M68000=1 
		HAS_M6809=1 HAS_S2650=1 HAS_Z80=1 	)
endif()
if(OPT_MEADOWS)
	add_compile_definitions(LINK_MEADOWS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/lazercmd.c vidhrdw/lazercmd.c drivers/meadows.c sndhrdw/meadows.c 
		vidhrdw/meadows.c 	)
	set(MSND_DAC ON)
	set(MSND_SAMPLES ON)
	set(MCPU_S2650 ON)
	list(APPEND CPU_DEFS
		HAS_DAC=1 HAS_SAMPLES=1 	)
	list(APPEND CPU_DEFS
		HAS_S2650=1 	)
endif()
if(OPT_MERIT)
	add_compile_definitions(LINK_MERIT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/couple.c drivers/merit.c 	)
	set(MSND_AY8910 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 	)
endif()
if(OPT_METRO)
	add_compile_definitions(LINK_METRO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/hyprduel.c vidhrdw/hyprduel.c drivers/metro.c vidhrdw/metro.c 
		drivers/rabbit.c 	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM2610 ON)
	set(MSND_YMF278B ON)
	set(MCPU_UPD7810 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2413=1 HAS_YM2610=1 
		HAS_YMF278B=1 	)
	list(APPEND CPU_DEFS
		HAS_UPD7810=1 	)
endif()
if(OPT_MIDCOIN)
	add_compile_definitions(LINK_MIDCOIN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/wallc.c drivers/wink.c 	)
	set(MSND_AY8910 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 	)
endif()
if(OPT_MIDW8080)
	add_compile_definitions(LINK_MIDW8080=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/8080bw.c machine/8080bw.c sndhrdw/8080bw.c vidhrdw/8080bw.c 
		drivers/m79amb.c vidhrdw/m79amb.c drivers/rotaryf.c drivers/sspeedr.c 
		vidhrdw/sspeedr.c 	)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_DISCRETE ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76477 ON)
	set(MCPU_I8039 ON)
	list(APPEND CPU_DEFS
		HAS_CUSTOM=1 HAS_DAC=1 HAS_DISCRETE=1 HAS_SAMPLES=1 
		HAS_SN76477=1 	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 	)
endif()
if(OPT_MIDWAY)
	add_compile_definitions(LINK_MIDWAY=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/astrocde.c machine/astrocde.c vidhrdw/astrocde.c drivers/balsente.c 
		machine/balsente.c vidhrdw/balsente.c drivers/gridlee.c sndhrdw/gridlee.c 
		vidhrdw/gridlee.c drivers/mcr.c machine/mcr.c sndhrdw/mcr.c 
		vidhrdw/mcr.c drivers/mcr3.c vidhrdw/mcr3.c drivers/mcr68.c 
		vidhrdw/mcr68.c drivers/midtunit.c machine/midtunit.c vidhrdw/midtunit.c 
		drivers/midvunit.c vidhrdw/midvunit.c drivers/midwunit.c machine/midwunit.c 
		drivers/midxunit.c drivers/midyunit.c machine/midyunit.c vidhrdw/midyunit.c 
		drivers/midzeus.c drivers/omegrace.c drivers/seattle.c drivers/vegas.c 
		drivers/williams.c machine/williams.c sndhrdw/williams.c vidhrdw/williams.c 
		machine/midwayic.c sndhrdw/dcs.c sndhrdw/gorf.c sndhrdw/wow.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CEM3394 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_DMADAC ON)
	set(MSND_HC55516 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_TMS5220 ON)
	set(MSND_YM2151 ON)
	set(MCPU_ADSP2100 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M6809 ON)
	set(MCPU_TMS32031 ON)
	set(MCPU_TMS34010 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CEM3394=1 HAS_CUSTOM=1 HAS_DAC=1 
		HAS_DMADAC=1 HAS_HC55516=1 HAS_OKIM6295=1 HAS_SAMPLES=1 
		HAS_TMS5220=1 HAS_YM2151=1 	)
	list(APPEND CPU_DEFS
		HAS_ADSP2100=1 HAS_M6800=1 HAS_M6809=1 HAS_TMS32031=1 
		HAS_TMS34010=1 HAS_Z80=1 	)
endif()
if(OPT_MIDWAYTUNIT)
	add_compile_definitions(LINK_MIDWAYTUNIT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/midtunit.c machine/midtunit.c vidhrdw/midtunit.c drivers/midyunit.c 
		machine/midyunit.c vidhrdw/midyunit.c machine/midwayic.c sndhrdw/williams.c 
		sndhrdw/dcs.c sndhrdw/cage.c 	)
	set(MSND_DMADAC ON)
	set(MSND_HC55516 ON)
	set(MCPU_ADSP2100 ON)
	set(MCPU_ADSP2101 ON)
	set(MCPU_ADSP2104 ON)
	set(MCPU_ADSP2105 ON)
	set(MCPU_ADSP2115 ON)
	set(MCPU_ADSP2181 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M6808 ON)
	set(MCPU_M6809 ON)
	set(MCPU_TMS34010 ON)
	list(APPEND CPU_DEFS
		HAS_DMADAC=1 HAS_HC55516=1 	)
	list(APPEND CPU_DEFS
		HAS_ADSP2100=1 HAS_ADSP2101=1 HAS_ADSP2104=1 HAS_ADSP2105=1 
		HAS_ADSP2115=1 HAS_ADSP2181=1 HAS_M6800=1 HAS_M6808=1 
		HAS_M6809=1 HAS_TMS34010=1 	)
endif()
if(OPT_MINIKONAMI)
	add_compile_definitions(LINK_MINIKONAMI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/tmnt.c vidhrdw/tmnt.c vidhrdw/konamiic.c drivers/simpsons.c 
		machine/simpsons.c vidhrdw/simpsons.c 	)
	set(MSND_K007232 ON)
	set(MSND_K053260 ON)
	set(MSND_K054539 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_UPD7759 ON)
	set(MSND_YM2151 ON)
	set(MCPU_KONAMI ON)
	set(MCPU_M68000 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_K007232=1 HAS_K053260=1 HAS_K054539=1 HAS_OKIM6295=1 
		HAS_SAMPLES=1 HAS_UPD7759=1 HAS_YM2151=1 	)
	list(APPEND CPU_DEFS
		HAS_KONAMI=1 HAS_M68000=1 HAS_Z80=1 	)
endif()
if(OPT_MISC)
	add_compile_definitions(LINK_MISC=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/1945kiii.c drivers/4enraya.c vidhrdw/4enraya.c drivers/afega.c 
		vidhrdw/afega.c drivers/ambush.c vidhrdw/ambush.c drivers/ampoker.c 
		drivers/amspdwy.c vidhrdw/amspdwy.c drivers/artmagic.c vidhrdw/artmagic.c 
		drivers/attckufo.c sndhrdw/attckufo.c vidhrdw/attckufo.c drivers/aztarac.c 
		sndhrdw/aztarac.c vidhrdw/aztarac.c drivers/beaminv.c vidhrdw/beaminv.c 
		drivers/bmcbowl.c drivers/cave.c vidhrdw/cave.c drivers/cherrym.c 
		drivers/coinmstr.c drivers/coolpool.c drivers/crystal.c machine/ds1302.c 
		vidhrdw/vrender0.c drivers/cybertnk.c drivers/dambustr.c drivers/dcheese.c 
		vidhrdw/dcheese.c drivers/dgpix.c drivers/dorachan.c vidhrdw/dorachan.c 
		drivers/dreamwld.c drivers/dribling.c vidhrdw/dribling.c drivers/dwarfd.c 
		drivers/dynadice.c drivers/epos.c vidhrdw/epos.c drivers/ertictac.c 
		drivers/esd16.c vidhrdw/esd16.c drivers/ettrivia.c drivers/flower.c 
		sndhrdw/flower.c vidhrdw/flower.c drivers/fortecar.c drivers/freekick.c 
		vidhrdw/freekick.c drivers/funworld.c drivers/go2000.c drivers/gotcha.c 
		vidhrdw/gotcha.c drivers/gumbo.c vidhrdw/gumbo.c drivers/gunpey.c 
		drivers/hexa.c vidhrdw/hexa.c drivers/homedata.c vidhrdw/homedata.c 
		drivers/hotblock.c drivers/intrscti.c drivers/jackpool.c drivers/kyugo.c 
		vidhrdw/kyugo.c drivers/ladyfrog.c vidhrdw/ladyfrog.c drivers/laserbas.c 
		drivers/lastfght.c drivers/lethalj.c vidhrdw/lethalj.c drivers/ltcasino.c 
		drivers/lucky8.c drivers/magic10.c drivers/malzak.c vidhrdw/malzak.c 
		drivers/mcatadv.c vidhrdw/mcatadv.c drivers/micro3d.c vidhrdw/micro3d.c 
		drivers/mirax.c drivers/mole.c vidhrdw/mole.c drivers/monzagp.c 
		drivers/mosaic.c vidhrdw/mosaic.c drivers/mrjong.c vidhrdw/mrjong.c 
		drivers/murogem.c drivers/news.c vidhrdw/news.c drivers/oneshot.c 
		vidhrdw/oneshot.c drivers/onetwo.c drivers/othldrby.c vidhrdw/othldrby.c 
		drivers/pass.c vidhrdw/pass.c drivers/pipeline.c drivers/pkscram.c 
		drivers/pntnpuzl.c drivers/policetr.c vidhrdw/policetr.c drivers/polyplay.c 
		sndhrdw/polyplay.c vidhrdw/polyplay.c drivers/rbmk.c drivers/shangkid.c 
		vidhrdw/shangkid.c drivers/skyarmy.c drivers/sliver.c drivers/sprcros2.c 
		vidhrdw/sprcros2.c drivers/ssfindo.c drivers/ssingles.c drivers/starspnr.c 
		drivers/statriv2.c drivers/supertnk.c drivers/tattack.c drivers/taxidrvr.c 
		vidhrdw/taxidrvr.c drivers/tcl.c drivers/thedeep.c vidhrdw/thedeep.c 
		drivers/tickee.c vidhrdw/tickee.c drivers/truco.c vidhrdw/truco.c 
		drivers/trucocl.c vidhrdw/trucocl.c drivers/trvquest.c drivers/ttchamp.c 
		drivers/tugboat.c drivers/turbosub.c drivers/usgames.c vidhrdw/usgames.c 
		drivers/vamphalf.c drivers/vp906iii.c drivers/vroulet.c drivers/wldarrow.c 
		drivers/xyonix.c vidhrdw/xyonix.c 	)
	set(MSND_AY8910 ON)
	set(MSND_BSMT2000 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_MSM5232 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76477 ON)
	set(MSND_SN76496 ON)
	set(MSND_UPD7759 ON)
	set(MSND_VRENDER0 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2610 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_ARM ON)
	set(MCPU_ARM7 ON)
	set(MCPU_H83002 ON)
	set(MCPU_I8051 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_S2650 ON)
	set(MCPU_SE3208 ON)
	set(MCPU_TMS32025 ON)
	set(MCPU_TMS34010 ON)
	set(MCPU_UPD7810 ON)
	set(MCPU_Z180 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_BSMT2000=1 HAS_CUSTOM=1 HAS_DAC=1 
		HAS_MSM5232=1 HAS_OKIM6295=1 HAS_SAMPLES=1 HAS_SN76477=1 
		HAS_SN76496=1 HAS_UPD7759=1 HAS_VRENDER0=1 HAS_YM2151=1 
		HAS_YM2203=1 HAS_YM2610=1 HAS_YM3812=1 HAS_YMZ280B=1 
	)
	list(APPEND CPU_DEFS
		HAS_ARM=1 HAS_ARM7=1 HAS_H83002=1 HAS_I8051=1 
		HAS_M68000=1 HAS_M6809=1 HAS_S2650=1 HAS_SE3208=1 
		HAS_TMS32025=1 HAS_TMS34010=1 HAS_UPD7810=1 HAS_Z180=1 
		HAS_Z80=1 	)
endif()
if(OPT_NAMCO)
	add_compile_definitions(LINK_NAMCO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/baraduke.c vidhrdw/baraduke.c drivers/galaga.c vidhrdw/galaga.c 
		drivers/galaxian.c sndhrdw/galaxian.c vidhrdw/galaxian.c drivers/gaplus.c 
		machine/gaplus.c vidhrdw/gaplus.c drivers/mappy.c vidhrdw/mappy.c 
		drivers/namcofl.c vidhrdw/namcofl.c drivers/namcoic.c drivers/namcona1.c 
		vidhrdw/namcona1.c drivers/namconb1.c vidhrdw/namconb1.c drivers/namcond1.c 
		machine/namcond1.c vidhrdw/ygv608.c drivers/namcos1.c machine/namcos1.c 
		vidhrdw/namcos1.c drivers/namcos10.c drivers/namcos11.c drivers/namcos12.c 
		drivers/namcos2.c machine/namcos2.c vidhrdw/namcos2.c drivers/namcos21.c 
		vidhrdw/namcos21.c drivers/namcos22.c vidhrdw/namcos22.c drivers/namcos23.c 
		drivers/namcos86.c vidhrdw/namcos86.c drivers/pacland.c vidhrdw/pacland.c 
		drivers/polepos.c sndhrdw/polepos.c vidhrdw/polepos.c drivers/rallyx.c 
		vidhrdw/rallyx.c drivers/skykid.c vidhrdw/skykid.c drivers/tankbatt.c 
		vidhrdw/tankbatt.c drivers/tceptor.c vidhrdw/tceptor.c drivers/toypop.c 
		vidhrdw/toypop.c drivers/warpwarp.c sndhrdw/warpwarp.c vidhrdw/warpwarp.c 
		machine/namcoio.c sndhrdw/geebee.c sndhrdw/namcoc7x.c vidhrdw/bosco.c 
		vidhrdw/digdug.c machine/psx.c vidhrdw/psx.c machine/xevious.c 
		vidhrdw/xevious.c 	)
	set(MSND_AY8910 ON)
	set(MSND_C140 ON)
	set(MSND_C352 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_NAMCO ON)
	set(MSND_NAMCONA ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76496 ON)
	set(MSND_YM2151 ON)
	set(MCPU_H83002 ON)
	set(MCPU_I960 ON)
	set(MCPU_M37710 ON)
	set(MCPU_M6502 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6805 ON)
	set(MCPU_M6809 ON)
	set(MCPU_S2650 ON)
	set(MCPU_TMS32025 ON)
	set(MCPU_Z80 ON)
	set(MCPU_Z8000 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_C140=1 HAS_C352=1 HAS_CUSTOM=1 
		HAS_DAC=1 HAS_NAMCO=1 HAS_NAMCONA=1 HAS_SAMPLES=1 
		HAS_SN76496=1 HAS_YM2151=1 	)
	list(APPEND CPU_DEFS
		HAS_H83002=1 HAS_I960=1 HAS_M37710=1 HAS_M6502=1 
		HAS_M6800=1 HAS_M68000=1 HAS_M6805=1 HAS_M6809=1 
		HAS_S2650=1 HAS_TMS32025=1 HAS_Z80=1 HAS_Z8000=1 
	)
endif()
if(OPT_NASCO)
	add_compile_definitions(LINK_NASCO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/crgolf.c vidhrdw/crgolf.c drivers/suprgolf.c 	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	set(MSND_YM2203 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 HAS_YM2203=1 	)
endif()
if(OPT_NEOGEO)
	add_compile_definitions(LINK_NEOGEO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/neogeo.c machine/neogeo.c vidhrdw/neogeo.c machine/neoboot.c 
		machine/neocrypt.c machine/neoprot.c 	)
	set(MSND_YM2610 ON)
	set(MSND_YM2610B ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_YM2610=1 HAS_YM2610B=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_NICHIBUT)
	add_compile_definitions(LINK_NICHIBUT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/armedf.c vidhrdw/armedf.c drivers/bigfghtr.c drivers/cclimber.c 
		sndhrdw/cclimber.c vidhrdw/cclimber.c drivers/clshroad.c vidhrdw/clshroad.c 
		drivers/cop01.c vidhrdw/cop01.c drivers/dacholer.c drivers/galivan.c 
		vidhrdw/galivan.c drivers/gomoku.c sndhrdw/gomoku.c vidhrdw/gomoku.c 
		drivers/hyhoo.c vidhrdw/hyhoo.c drivers/magmax.c vidhrdw/magmax.c 
		drivers/nbmj8688.c vidhrdw/nbmj8688.c drivers/nbmj8891.c vidhrdw/nbmj8891.c 
		drivers/nbmj8991.c vidhrdw/nbmj8991.c drivers/nbmj9195.c vidhrdw/nbmj9195.c 
		drivers/niyanpai.c machine/m68kfmly.c vidhrdw/niyanpai.c drivers/pastelg.c 
		vidhrdw/pastelg.c drivers/seicross.c vidhrdw/seicross.c drivers/terracre.c 
		vidhrdw/terracre.c drivers/tubep.c vidhrdw/tubep.c drivers/wiping.c 
		sndhrdw/wiping.c vidhrdw/wiping.c drivers/yamato.c machine/nb1413m3.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M68000 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_MSM5205=1 
		HAS_OKIM6295=1 HAS_SAMPLES=1 HAS_YM2203=1 HAS_YM3812=1 
	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_Z80=1 	)
endif()
if(OPT_NINTENDO)
	add_compile_definitions(LINK_NINTENDO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/dkong.c sndhrdw/dkong.c vidhrdw/dkong.c drivers/mario.c 
		sndhrdw/mario.c vidhrdw/mario.c drivers/n8080.c sndhrdw/n8080.c 
		vidhrdw/n8080.c drivers/nss.c machine/snes.c sndhrdw/snes.c 
		vidhrdw/snes.c drivers/playch10.c machine/playch10.c vidhrdw/playch10.c 
		drivers/popeye.c vidhrdw/popeye.c drivers/punchout.c vidhrdw/punchout.c 
		drivers/spacefb.c vidhrdw/spacefb.c drivers/vsnes.c machine/vsnes.c 
		vidhrdw/vsnes.c machine/drakton.c machine/rp5h01.c machine/strtheat.c 
		vidhrdw/ppu2c03b.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76477 ON)
	set(MSND_VLM5030 ON)
	set(MCPU_G65816 ON)
	set(MCPU_I8039 ON)
	set(MCPU_M6502 ON)
	set(MCPU_S2650 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SAMPLES=1 HAS_SN76477=1 
		HAS_VLM5030=1 	)
	list(APPEND CPU_DEFS
		HAS_G65816=1 HAS_I8039=1 HAS_M6502=1 HAS_S2650=1 
		HAS_Z80=1 	)
endif()
if(OPT_NIX)
	add_compile_definitions(LINK_NIX=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/fitfight.c vidhrdw/fitfight.c drivers/pirates.c vidhrdw/pirates.c 
	)
	set(MSND_OKIM6295 ON)
	set(MCPU_UPD7810 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 	)
	list(APPEND CPU_DEFS
		HAS_UPD7810=1 	)
endif()
if(OPT_NMK)
	add_compile_definitions(LINK_NMK=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/acommand.c drivers/jalmah.c drivers/macrossp.c vidhrdw/macrossp.c 
		drivers/nmk16.c machine/nmk004.c vidhrdw/nmk16.c drivers/quizdna.c 
		vidhrdw/quizdna.c drivers/quizpani.c vidhrdw/quizpani.c 	)
	set(MSND_ES5506 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M68000 ON)
	list(APPEND CPU_DEFS
		HAS_ES5506=1 HAS_OKIM6295=1 HAS_YM2203=1 HAS_YM3812=1 
	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 	)
endif()
if(OPT_OLYMPIA)
	add_compile_definitions(LINK_OLYMPIA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/dday.c vidhrdw/dday.c drivers/portrait.c vidhrdw/portrait.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_TMS5220 ON)
	set(MCPU_I8039 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_TMS5220=1 	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 	)
endif()
if(OPT_OMORI)
	add_compile_definitions(LINK_OMORI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/battlex.c vidhrdw/battlex.c drivers/carjmbre.c vidhrdw/carjmbre.c 
		drivers/popper.c vidhrdw/popper.c drivers/spaceg.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SN76496 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SN76496=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_ORCA)
	add_compile_definitions(LINK_ORCA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/espial.c vidhrdw/espial.c drivers/funkybee.c vidhrdw/funkybee.c 
		drivers/marineb.c vidhrdw/marineb.c drivers/vastar.c vidhrdw/vastar.c 
		drivers/zodiack.c vidhrdw/zodiack.c 	)
	set(MSND_AY8910 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_PACIFIC)
	add_compile_definitions(LINK_PACIFIC=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/mrflea.c vidhrdw/mrflea.c drivers/thief.c vidhrdw/thief.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_SAMPLES ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_SAMPLES=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_PACMAN)
	add_compile_definitions(LINK_PACMAN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/jrpacman.c drivers/pacman.c vidhrdw/pacman.c drivers/pengo.c 
		machine/acitya.c machine/jumpshot.c machine/mspacman.c machine/pacplus.c 
		machine/theglobp.c 	)
	set(MSND_AY8910 ON)
	set(MSND_NAMCO ON)
	set(MSND_SN76496 ON)
	set(MCPU_S2650 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_NAMCO=1 HAS_SN76496=1 	)
	list(APPEND CPU_DEFS
		HAS_S2650=1 	)
endif()
if(OPT_PHOENIX)
	add_compile_definitions(LINK_PHOENIX=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/naughtyb.c vidhrdw/naughtyb.c drivers/phoenix.c sndhrdw/phoenix.c 
		vidhrdw/phoenix.c drivers/safarir.c sndhrdw/pleiads.c 	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_SN76477 ON)
	set(MSND_TMS36XX ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_SN76477=1 HAS_TMS36XX=1 
	)
endif()
if(OPT_PLAYMARK)
	add_compile_definitions(LINK_PLAYMARK=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/drtomy.c drivers/playmark.c vidhrdw/playmark.c drivers/powerbal.c 
		drivers/sderby.c vidhrdw/sderby.c drivers/sslam.c vidhrdw/sslam.c 
	)
	set(MSND_OKIM6295 ON)
	set(MCPU_I8051 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 	)
	list(APPEND CPU_DEFS
		HAS_I8051=1 	)
endif()
if(OPT_PSIKYO)
	add_compile_definitions(LINK_PSIKYO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/psikyo.c vidhrdw/psikyo.c drivers/psikyo4.c vidhrdw/psikyo4.c 
		drivers/psikyosh.c vidhrdw/psikyosh.c 	)
	set(MSND_YM2610 ON)
	set(MSND_YMF278B ON)
	set(MCPU_SH2 ON)
	list(APPEND CPU_DEFS
		HAS_YM2610=1 HAS_YMF278B=1 	)
	list(APPEND CPU_DEFS
		HAS_SH2=1 	)
endif()
if(OPT_RAMTEK)
	add_compile_definitions(LINK_RAMTEK=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/hitme.c sndhrdw/hitme.c drivers/starcrus.c vidhrdw/starcrus.c 
	)
	set(MSND_DISCRETE ON)
	set(MSND_SAMPLES ON)
	list(APPEND CPU_DEFS
		HAS_DISCRETE=1 HAS_SAMPLES=1 	)
endif()
if(OPT_RARE)
	add_compile_definitions(LINK_RARE=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/btoads.c vidhrdw/btoads.c drivers/kinst.c drivers/xtheball.c 
	)
	set(MSND_BSMT2000 ON)
	set(MSND_DAC ON)
	set(MCPU_ADSP2100 ON)
	set(MCPU_TMS34010 ON)
	list(APPEND CPU_DEFS
		HAS_BSMT2000=1 HAS_DAC=1 	)
	list(APPEND CPU_DEFS
		HAS_ADSP2100=1 HAS_TMS34010=1 	)
endif()
if(OPT_SANRITSU)
	add_compile_definitions(LINK_SANRITSU=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/appoooh.c vidhrdw/appoooh.c drivers/bankp.c vidhrdw/bankp.c 
		drivers/drmicro.c vidhrdw/drmicro.c drivers/mayumi.c vidhrdw/mayumi.c 
		drivers/mermaid.c vidhrdw/mermaid.c drivers/mjkjidai.c vidhrdw/mjkjidai.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SN76496 ON)
	set(MSND_YM2203 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_MSM5205=1 HAS_OKIM6295=1 
		HAS_SN76496=1 HAS_YM2203=1 	)
endif()
if(OPT_SEGA)
	add_compile_definitions(LINK_SEGA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/angelkds.c vidhrdw/angelkds.c drivers/blockade.c sndhrdw/blockade.c 
		vidhrdw/blockade.c drivers/calorie.c drivers/coolridr.c drivers/deniam.c 
		vidhrdw/deniam.c drivers/dotrikun.c vidhrdw/dotrikun.c drivers/genesis.c 
		vidhrdw/genesis.c drivers/kopunch.c vidhrdw/kopunch.c drivers/megaplay.c 
		drivers/megatech.c drivers/model1.c machine/model1.c vidhrdw/model1.c 
		drivers/model2.c drivers/model3.c vidhrdw/model3.c machine/model3.c 
		drivers/puckpkmn.c drivers/sega.c sndhrdw/sega.c vidhrdw/sega.c 
		drivers/segac2.c drivers/segahang.c vidhrdw/segahang.c drivers/segaorun.c 
		vidhrdw/segaorun.c drivers/segar.c machine/segar.c sndhrdw/segar.c 
		vidhrdw/segar.c drivers/segas16a.c vidhrdw/segas16a.c drivers/segas16b.c 
		drivers/segas16b.c vidhrdw/segas16b.c drivers/segas18.c vidhrdw/segas18.c 
		drivers/segas32.c machine/segas32.c vidhrdw/segas32.c drivers/segasyse.c 
		vidhrdw/segasyse.c drivers/segaxbd.c vidhrdw/segaxbd.c drivers/segaybd.c 
		vidhrdw/segaybd.c drivers/sg1000a.c vidhrdw/tms9928a.c drivers/stactics.c 
		machine/stactics.c vidhrdw/stactics.c drivers/stv.c drivers/stvhacks.c 
		machine/stvcd.c drivers/suprloco.c vidhrdw/suprloco.c drivers/system1.c 
		vidhrdw/system1.c drivers/system16.c machine/system16.c sndhrdw/system16.c 
		vidhrdw/system16.c vidhrdw/sys16spr.c drivers/system18.c drivers/system24.c 
		machine/system24.c vidhrdw/system24.c drivers/topshoot.c drivers/turbo.c 
		machine/turbo.c sndhrdw/turbo.c vidhrdw/turbo.c drivers/vicdual.c 
		sndhrdw/vicdual.c vidhrdw/vicdual.c drivers/zaxxon.c sndhrdw/zaxxon.c 
		vidhrdw/zaxxon.c machine/fd1089.c machine/fd1094.c machine/mc8123.c 
		machine/s16fd.c machine/s24fd.c machine/scudsp.c machine/segaic16.c 
		sndhrdw/carnival.c sndhrdw/depthch.c sndhrdw/invinco.c sndhrdw/pulsar.c 
		sndhrdw/segasnd.c vidhrdw/segaic16.c vidhrdw/segaic24.c vidhrdw/stvvdp1.c 
		vidhrdw/stvvdp2.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_DISCRETE ON)
	set(MSND_MSM5205 ON)
	set(MSND_MULTIPCM ON)
	set(MSND_OKIM6295 ON)
	set(MSND_RF5C68 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SCSP ON)
	set(MSND_SEGAPCM ON)
	set(MSND_SN76496 ON)
	set(MSND_SP0250 ON)
	set(MSND_TMS36XX ON)
	set(MSND_UPD7759 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM3438 ON)
	set(MSND_YM3812 ON)
	set(MCPU_ADSP21062 ON)
	set(MCPU_I8039 ON)
	set(MCPU_I960 ON)
	set(MCPU_M68000 ON)
	set(MCPU_SH2 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_DISCRETE=1 HAS_MSM5205=1 
		HAS_MULTIPCM=1 HAS_OKIM6295=1 HAS_RF5C68=1 HAS_SAMPLES=1 
		HAS_SCSP=1 HAS_SEGAPCM=1 HAS_SN76496=1 HAS_SP0250=1 
		HAS_TMS36XX=1 HAS_UPD7759=1 HAS_YM2151=1 HAS_YM2203=1 
		HAS_YM2413=1 HAS_YM3438=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_ADSP21062=1 HAS_I8039=1 HAS_I960=1 HAS_M68000=1 
		HAS_SH2=1 HAS_Z80=1 	)
endif()
if(OPT_SEIBU)
	add_compile_definitions(LINK_SEIBU=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/cshooter.c drivers/dcon.c vidhrdw/dcon.c drivers/deadang.c 
		vidhrdw/deadang.c drivers/dynduke.c vidhrdw/dynduke.c drivers/kncljoe.c 
		vidhrdw/kncljoe.c drivers/mustache.c vidhrdw/mustache.c drivers/panicr.c 
		drivers/raiden.c vidhrdw/raiden.c drivers/raiden2.c drivers/seibuspi.c 
		machine/seibuspi.c vidhrdw/seibuspi.c drivers/sengokmj.c vidhrdw/sengokmj.c 
		drivers/stfight.c machine/stfight.c vidhrdw/stfight.c drivers/wiz.c 
		vidhrdw/wiz.c machine/spisprit.c sndhrdw/seibu.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DISCRETE ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMF271 ON)
	set(MCPU_M6800 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DISCRETE=1 HAS_MSM5205=1 HAS_OKIM6295=1 
		HAS_YM2151=1 HAS_YM2203=1 HAS_YM3812=1 HAS_YMF271=1 
	)
	list(APPEND CPU_DEFS
		HAS_M6800=1 HAS_Z80=1 	)
endif()
if(OPT_SETA)
	add_compile_definitions(LINK_SETA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/aleck64.c machine/n64.c vidhrdw/n64.c drivers/darkhors.c 
		drivers/hanaawas.c vidhrdw/hanaawas.c drivers/macs.c drivers/seta.c 
		vidhrdw/seta.c drivers/seta2.c vidhrdw/seta2.c drivers/speedatk.c 
		vidhrdw/speedatk.c drivers/srmp2.c vidhrdw/srmp2.c drivers/srmp6.c 
		drivers/ssv.c vidhrdw/ssv.c drivers/st0016.c vidhrdw/st0016.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_ES5506 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_ST0016 ON)
	set(MSND_X1_010 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_RSP ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_ES5506=1 HAS_MSM5205=1 
		HAS_OKIM6295=1 HAS_ST0016=1 HAS_X1_010=1 HAS_YM2203=1 
		HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_RSP=1 HAS_Z80=1 	)
endif()
if(OPT_SHARED)
	add_compile_definitions(LINK_SHARED=1)
	list(APPEND MAME_DRIVERS_SRC
		machine/53c810.c machine/6532riot.c machine/6522via.c machine/6526cia.c 
		machine/6821pia.c machine/6840ptm.c machine/6850acia.c machine/7474.c 
		machine/74123.c machine/74148.c machine/74153.c machine/74181.c 
		machine/8042kbdc.c machine/8237dma.c machine/8255ppi.c machine/adc083x.c 
		machine/am53cf96.c machine/ds2404.c machine/idectrl.c machine/intelfsh.c 
		machine/mc146818.c machine/nmk112.c machine/pci.c machine/pckeybrd.c 
		machine/pcshare.c machine/pd4990a.c machine/pic8259.c machine/pit8253.c 
		machine/scsicd.c machine/scsihd.c machine/segacrpt.c machine/smc91c9x.c 
		machine/ticket.c machine/timekpr.c machine/tmp68301.c machine/z80ctc.c 
		machine/z80pio.c machine/z80sio.c vidhrdw/crtc6845.c vidhrdw/avgdvg.c 
		vidhrdw/poly.c vidhrdw/res_net.c vidhrdw/tlc34076.c vidhrdw/tms34061.c 
		vidhrdw/voodoo.c 	)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_SIGMA)
	add_compile_definitions(LINK_SIGMA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/nyny.c vidhrdw/nyny.c drivers/r2dtank.c drivers/spiders.c 
		machine/spiders.c sndhrdw/spiders.c vidhrdw/spiders.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MCPU_M6800 ON)
	set(MCPU_M6809 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 	)
	list(APPEND CPU_DEFS
		HAS_M6800=1 HAS_M6809=1 	)
endif()
if(OPT_SNK)
	add_compile_definitions(LINK_SNK=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/bbusters.c vidhrdw/bbusters.c drivers/dmndrby.c drivers/hal21.c 
		drivers/hng64.c vidhrdw/hng64.c drivers/jcross.c vidhrdw/jcross.c 
		drivers/lasso.c vidhrdw/lasso.c drivers/mainsnk.c vidhrdw/mainsnk.c 
		drivers/marvins.c vidhrdw/marvins.c drivers/munchmo.c vidhrdw/munchmo.c 
		drivers/prehisle.c vidhrdw/prehisle.c drivers/rockola.c sndhrdw/rockola.c 
		vidhrdw/rockola.c drivers/sgladiat.c drivers/snk.c vidhrdw/snk.c 
		drivers/snk68.c vidhrdw/snk68.c 	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_NAMCO ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76477 ON)
	set(MSND_SN76496 ON)
	set(MSND_UPD7759 ON)
	set(MSND_YM2610 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M6502 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_NAMCO=1 
		HAS_SAMPLES=1 HAS_SN76477=1 HAS_SN76496=1 HAS_UPD7759=1 
		HAS_YM2610=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_M6502=1 HAS_Z80=1 	)
endif()
if(OPT_STERN)
	add_compile_definitions(LINK_STERN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/astinvad.c sndhrdw/astinvad.c vidhrdw/astinvad.c drivers/berzerk.c 
		machine/berzerk.c sndhrdw/berzerk.c vidhrdw/berzerk.c drivers/mazerbla.c 
		drivers/supdrapo.c 	)
	set(MSND_AY8910 ON)
	set(MSND_SAMPLES ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_SAMPLES=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_SUN)
	add_compile_definitions(LINK_SUN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/arabian.c vidhrdw/arabian.c drivers/ikki.c vidhrdw/ikki.c 
		drivers/kangaroo.c vidhrdw/kangaroo.c drivers/markham.c vidhrdw/markham.c 
		drivers/route16.c vidhrdw/route16.c drivers/shanghai.c drivers/shangha3.c 
		vidhrdw/shangha3.c drivers/strnskil.c vidhrdw/strnskil.c drivers/ttmahjng.c 
		vidhrdw/ttmahjng.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SN76477 ON)
	set(MSND_SN76496 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_OKIM6295=1 HAS_SN76477=1 
		HAS_SN76496=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_SUNA)
	add_compile_definitions(LINK_SUNA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/goindol.c vidhrdw/goindol.c drivers/suna8.c sndhrdw/suna8.c 
		vidhrdw/suna8.c drivers/suna16.c vidhrdw/suna16.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SAMPLES=1 HAS_YM2151=1 
		HAS_YM2203=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_TAD)
	add_compile_definitions(LINK_TAD=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/bloodbro.c vidhrdw/bloodbro.c drivers/cabal.c vidhrdw/cabal.c 
		drivers/goal92.c vidhrdw/goal92.c drivers/legionna.c vidhrdw/legionna.c 
		drivers/toki.c vidhrdw/toki.c 	)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3812 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_MSM5205=1 HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 
		HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_TAITO)
	add_compile_definitions(LINK_TAITO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/40love.c vidhrdw/40love.c drivers/arkanoid.c machine/arkanoid.c 
		vidhrdw/arkanoid.c drivers/ashnojoe.c vidhrdw/ashnojoe.c drivers/asuka.c 
		machine/bonzeadv.c vidhrdw/asuka.c drivers/bigevglf.c machine/bigevglf.c 
		vidhrdw/bigevglf.c drivers/bking2.c vidhrdw/bking2.c drivers/bublbobl.c 
		machine/bublbobl.c vidhrdw/bublbobl.c drivers/buggychl.c machine/buggychl.c 
		vidhrdw/buggychl.c drivers/chaknpop.c machine/chaknpop.c vidhrdw/chaknpop.c 
		drivers/champbwl.c drivers/changela.c drivers/crbaloon.c vidhrdw/crbaloon.c 
		sndhrdw/crbaloon.c drivers/darius.c vidhrdw/darius.c drivers/darkmist.c 
		vidhrdw/darkmist.c drivers/exzisus.c vidhrdw/exzisus.c drivers/fgoal.c 
		vidhrdw/fgoal.c drivers/flstory.c machine/flstory.c vidhrdw/flstory.c 
		drivers/gladiatr.c vidhrdw/gladiatr.c drivers/grchamp.c machine/grchamp.c 
		sndhrdw/grchamp.c vidhrdw/grchamp.c drivers/groundfx.c vidhrdw/groundfx.c 
		drivers/gsword.c machine/tait8741.c vidhrdw/gsword.c drivers/gunbustr.c 
		vidhrdw/gunbustr.c drivers/halleys.c drivers/jollyjgr.c drivers/ksayakyu.c 
		vidhrdw/ksayakyu.c drivers/lkage.c machine/lkage.c vidhrdw/lkage.c 
		drivers/lsasquad.c machine/lsasquad.c vidhrdw/lsasquad.c drivers/marinedt.c 
		drivers/mexico86.c machine/mexico86.c vidhrdw/mexico86.c drivers/minivadr.c 
		vidhrdw/minivadr.c drivers/missb2.c drivers/msisaac.c vidhrdw/msisaac.c 
		drivers/ninjaw.c vidhrdw/ninjaw.c drivers/nycaptor.c machine/nycaptor.c 
		vidhrdw/nycaptor.c drivers/opwolf.c machine/opwolf.c drivers/othunder.c 
		vidhrdw/othunder.c drivers/pitnrun.c machine/pitnrun.c vidhrdw/pitnrun.c 
		drivers/qix.c machine/qix.c sndhrdw/qix.c vidhrdw/qix.c 
		drivers/rainbow.c machine/rainbow.c drivers/rastan.c vidhrdw/rastan.c 
		drivers/retofinv.c machine/retofinv.c vidhrdw/retofinv.c drivers/sbowling.c 
		drivers/slapshot.c vidhrdw/slapshot.c drivers/ssrj.c vidhrdw/ssrj.c 
		drivers/superchs.c vidhrdw/superchs.c drivers/superqix.c vidhrdw/superqix.c 
		drivers/taito_b.c vidhrdw/taito_b.c drivers/taito_f2.c vidhrdw/taito_f2.c 
		drivers/taito_f3.c vidhrdw/taito_f3.c sndhrdw/taito_f3.c drivers/taito_h.c 
		vidhrdw/taito_h.c drivers/taito_l.c vidhrdw/taito_l.c drivers/taito_x.c 
		machine/cchip.c drivers/taito_z.c vidhrdw/taito_z.c drivers/taitoair.c 
		vidhrdw/taitoair.c drivers/taitojc.c drivers/taitosj.c machine/taitosj.c 
		vidhrdw/taitosj.c drivers/taitowlf.c drivers/tnzs.c machine/tnzs.c 
		vidhrdw/tnzs.c drivers/topspeed.c vidhrdw/topspeed.c drivers/tsamurai.c 
		vidhrdw/tsamurai.c drivers/undrfire.c vidhrdw/undrfire.c drivers/volfied.c 
		machine/volfied.c vidhrdw/volfied.c drivers/warriorb.c vidhrdw/warriorb.c 
		drivers/wgp.c vidhrdw/wgp.c machine/mb87078.c sndhrdw/taitosnd.c 
		vidhrdw/taitoic.c drivers/seta.c vidhrdw/seta.c 	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_DISCRETE ON)
	set(MSND_ES5506 ON)
	set(MSND_MSM5205 ON)
	set(MSND_MSM5232 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76477 ON)
	set(MSND_SN76496 ON)
	set(MSND_X1_010 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2610 ON)
	set(MSND_YM2610B ON)
	set(MSND_YM3438 ON)
	set(MSND_YM3526 ON)
	set(MSND_YM3812 ON)
	set(MCPU_I386 ON)
	set(MCPU_I8039 ON)
	set(MCPU_I8X41 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6802 ON)
	set(MCPU_M6805 ON)
	set(MCPU_M6809 ON)
	set(MCPU_TMS32025 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_DISCRETE=1 
		HAS_ES5506=1 HAS_MSM5205=1 HAS_MSM5232=1 HAS_OKIM6295=1 
		HAS_SAMPLES=1 HAS_SN76477=1 HAS_SN76496=1 HAS_X1_010=1 
		HAS_YM2151=1 HAS_YM2203=1 HAS_YM2610=1 HAS_YM2610B=1 
		HAS_YM3438=1 HAS_YM3526=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_I386=1 HAS_I8039=1 HAS_I8X41=1 HAS_M6800=1 
		HAS_M68000=1 HAS_M6802=1 HAS_M6805=1 HAS_M6809=1 
		HAS_TMS32025=1 HAS_Z80=1 	)
endif()
if(OPT_TATSUMI)
	add_compile_definitions(LINK_TATSUMI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/lockon.c drivers/tatsumi.c machine/tatsumi.c vidhrdw/tatsumi.c 
		drivers/tx1.c machine/tx1.c vidhrdw/tx1.c 	)
	set(MSND_AY8910 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MCPU_I86 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 
	)
	list(APPEND CPU_DEFS
		HAS_I86=1 	)
endif()
if(OPT_TCH)
	add_compile_definitions(LINK_TCH=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/kickgoal.c vidhrdw/kickgoal.c drivers/littlerb.c drivers/speedspn.c 
		vidhrdw/speedspn.c drivers/wheelfir.c 	)
	set(MSND_OKIM6295 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 	)
endif()
if(OPT_TECFRI)
	add_compile_definitions(LINK_TECFRI=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/holeland.c vidhrdw/holeland.c drivers/sauro.c vidhrdw/sauro.c 
		drivers/speedbal.c vidhrdw/speedbal.c 	)
	set(MSND_AY8910 ON)
	set(MSND_YM3812 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_TECHNOS)
	add_compile_definitions(LINK_TECHNOS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/battlane.c vidhrdw/battlane.c drivers/blockout.c vidhrdw/blockout.c 
		drivers/bogeyman.c vidhrdw/bogeyman.c drivers/chinagat.c drivers/ddragon.c 
		vidhrdw/ddragon.c drivers/ddragon3.c vidhrdw/ddragon3.c drivers/dogfgt.c 
		vidhrdw/dogfgt.c drivers/matmania.c machine/maniach.c vidhrdw/matmania.c 
		drivers/mystston.c vidhrdw/mystston.c drivers/renegade.c vidhrdw/renegade.c 
		drivers/shadfrce.c vidhrdw/shadfrce.c drivers/spdodgeb.c vidhrdw/spdodgeb.c 
		drivers/ssozumo.c vidhrdw/ssozumo.c drivers/tagteam.c vidhrdw/tagteam.c 
		drivers/vball.c vidhrdw/vball.c drivers/wwfsstar.c vidhrdw/wwfsstar.c 
		drivers/wwfwfest.c vidhrdw/wwfwfest.c drivers/xain.c vidhrdw/xain.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_CUSTOM ON)
	set(MSND_DAC ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM3526 ON)
	set(MSND_YM3812 ON)
	set(MCPU_I8039 ON)
	set(MCPU_M6502 ON)
	set(MCPU_M6800 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_CUSTOM=1 HAS_DAC=1 HAS_MSM5205=1 
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM2203=1 HAS_YM3526=1 
		HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 HAS_M6502=1 HAS_M6800=1 HAS_M68000=1 
		HAS_M6809=1 HAS_Z80=1 	)
endif()
if(OPT_TEHKAN)
	add_compile_definitions(LINK_TEHKAN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/bombjack.c vidhrdw/bombjack.c drivers/gaiden.c vidhrdw/gaiden.c 
		drivers/lvcards.c vidhrdw/lvcards.c drivers/pbaction.c vidhrdw/pbaction.c 
		drivers/senjyo.c sndhrdw/senjyo.c vidhrdw/senjyo.c drivers/solomon.c 
		vidhrdw/solomon.c drivers/spbactn.c vidhrdw/spbactn.c drivers/tbowl.c 
		vidhrdw/tbowl.c drivers/tecmo.c vidhrdw/tecmo.c drivers/tecmo16.c 
		vidhrdw/tecmo16.c drivers/tecmosys.c drivers/tehkanwc.c vidhrdw/tehkanwc.c 
		drivers/wc90.c vidhrdw/wc90.c drivers/wc90b.c vidhrdw/wc90b.c 
	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76496 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2203 ON)
	set(MSND_YM2608 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMF262 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_M68000 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 HAS_OKIM6295=1 HAS_SAMPLES=1 
		HAS_SN76496=1 HAS_YM2151=1 HAS_YM2203=1 HAS_YM2608=1 
		HAS_YM3812=1 HAS_YMF262=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_Z80=1 	)
endif()
if(OPT_THEPIT)
	add_compile_definitions(LINK_THEPIT=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/thepit.c vidhrdw/thepit.c drivers/timelimt.c vidhrdw/timelimt.c 
	)
	set(MSND_AY8910 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 	)
endif()
if(OPT_TOAPLAN)
	add_compile_definitions(LINK_TOAPLAN=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/mjsister.c vidhrdw/mjsister.c drivers/slapfght.c machine/slapfght.c 
		vidhrdw/slapfght.c drivers/snowbros.c vidhrdw/snowbros.c drivers/toaplan1.c 
		machine/toaplan1.c vidhrdw/toaplan1.c drivers/toaplan2.c sndhrdw/toaplan2.c 
		vidhrdw/toaplan2.c drivers/twincobr.c machine/twincobr.c vidhrdw/twincobr.c 
		drivers/wardner.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM3812 ON)
	set(MSND_YMZ280B ON)
	set(MCPU_M68000 ON)
	set(MCPU_TMS32010 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_OKIM6295=1 HAS_YM2151=1 
		HAS_YM3812=1 HAS_YMZ280B=1 	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_TMS32010=1 HAS_Z80=1 	)
endif()
if(OPT_TONG)
	add_compile_definitions(LINK_TONG=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/leprechn.c machine/leprechn.c vidhrdw/leprechn.c drivers/beezer.c 
		machine/beezer.c vidhrdw/beezer.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MCPU_M6502 ON)
	set(MCPU_M6809 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 	)
	list(APPEND CPU_DEFS
		HAS_M6502=1 HAS_M6809=1 	)
endif()
if(OPT_UNICO)
	add_compile_definitions(LINK_UNICO=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/drgnmst.c vidhrdw/drgnmst.c drivers/silkroad.c vidhrdw/silkroad.c 
		drivers/unico.c vidhrdw/unico.c 	)
	set(MSND_OKIM6295 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM3812 ON)
	list(APPEND CPU_DEFS
		HAS_OKIM6295=1 HAS_YM2151=1 HAS_YM3812=1 	)
endif()
if(OPT_UNIVERS)
	add_compile_definitions(LINK_UNIVERS=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/cheekyms.c vidhrdw/cheekyms.c drivers/cosmic.c vidhrdw/cosmic.c 
		drivers/docastle.c machine/docastle.c vidhrdw/docastle.c drivers/ladybug.c 
		vidhrdw/ladybug.c drivers/mrdo.c vidhrdw/mrdo.c drivers/redclash.c 
		vidhrdw/redclash.c 	)
	set(MSND_DAC ON)
	set(MSND_MSM5205 ON)
	set(MSND_SAMPLES ON)
	set(MSND_SN76496 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_DAC=1 HAS_MSM5205=1 HAS_SAMPLES=1 HAS_SN76496=1 
	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_UPL)
	add_compile_definitions(LINK_UPL=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/mnight.c vidhrdw/mnight.c drivers/mouser.c vidhrdw/mouser.c 
		drivers/ninjakid.c vidhrdw/ninjakid.c drivers/ninjakd2.c vidhrdw/ninjakd2.c 
		drivers/nova2001.c vidhrdw/nova2001.c drivers/omegaf.c vidhrdw/omegaf.c 
		drivers/pkunwar.c vidhrdw/pkunwar.c drivers/raiders5.c vidhrdw/raiders5.c 
		drivers/xxmissio.c vidhrdw/xxmissio.c 	)
	set(MSND_AY8910 ON)
	set(MSND_SAMPLES ON)
	set(MSND_YM2203 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_SAMPLES=1 HAS_YM2203=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_VALADON)
	add_compile_definitions(LINK_VALADON=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/bagman.c machine/bagman.c vidhrdw/bagman.c drivers/tankbust.c 
		vidhrdw/tankbust.c 	)
	set(MSND_AY8910 ON)
	set(MSND_TMS5110 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_TMS5110=1 	)
endif()
if(OPT_VELTMJR)
	add_compile_definitions(LINK_VELTMJR=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/cardline.c drivers/witch.c 	)
	set(MSND_ES8712 ON)
	set(MSND_YM2203 ON)
	set(MCPU_I8051 ON)
	list(APPEND CPU_DEFS
		HAS_ES8712=1 HAS_YM2203=1 	)
	list(APPEND CPU_DEFS
		HAS_I8051=1 	)
endif()
if(OPT_VENTURE)
	add_compile_definitions(LINK_VENTURE=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/looping.c drivers/spcforce.c vidhrdw/spcforce.c drivers/suprridr.c 
		vidhrdw/suprridr.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SN76496 ON)
	set(MSND_TMS5220 ON)
	set(MCPU_I8039 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SN76496=1 HAS_TMS5220=1 
	)
	list(APPEND CPU_DEFS
		HAS_I8039=1 	)
endif()
if(OPT_VSYSTEM)
	add_compile_definitions(LINK_VSYSTEM=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/aerofgt.c vidhrdw/aerofgt.c drivers/crshrace.c vidhrdw/crshrace.c 
		drivers/f1gp.c vidhrdw/f1gp.c drivers/fromance.c vidhrdw/fromance.c 
		drivers/fromanc2.c vidhrdw/fromanc2.c drivers/gstriker.c vidhrdw/gstriker.c 
		drivers/inufuku.c vidhrdw/inufuku.c drivers/ojankohs.c vidhrdw/ojankohs.c 
		drivers/pipedrm.c drivers/rpunch.c vidhrdw/rpunch.c drivers/suprslam.c 
		vidhrdw/suprslam.c drivers/tail2nos.c vidhrdw/tail2nos.c drivers/taotaido.c 
		vidhrdw/taotaido.c drivers/welltris.c vidhrdw/welltris.c 	)
	set(MSND_AY8910 ON)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_UPD7759 ON)
	set(MSND_YM2151 ON)
	set(MSND_YM2413 ON)
	set(MSND_YM2610 ON)
	set(MSND_YM3812 ON)
	set(MCPU_M68000 ON)
	set(MCPU_M6809 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_MSM5205=1 HAS_OKIM6295=1 HAS_UPD7759=1 
		HAS_YM2151=1 HAS_YM2413=1 HAS_YM2610=1 HAS_YM3812=1 
	)
	list(APPEND CPU_DEFS
		HAS_M68000=1 HAS_M6809=1 HAS_Z80=1 	)
endif()
if(OPT_YUNSUNG)
	add_compile_definitions(LINK_YUNSUNG=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/nmg5.c drivers/paradise.c vidhrdw/paradise.c drivers/yunsung8.c 
		vidhrdw/yunsung8.c drivers/yunsun16.c vidhrdw/yunsun16.c 	)
	set(MSND_MSM5205 ON)
	set(MSND_OKIM6295 ON)
	set(MSND_YM3812 ON)
	set(MCPU_Z80 ON)
	list(APPEND CPU_DEFS
		HAS_MSM5205=1 HAS_OKIM6295=1 HAS_YM3812=1 	)
	list(APPEND CPU_DEFS
		HAS_Z80=1 	)
endif()
if(OPT_ZACCARIA)
	add_compile_definitions(LINK_ZACCARIA=1)
	list(APPEND MAME_DRIVERS_SRC
		drivers/galaxia.c drivers/laserbat.c drivers/zac2650.c vidhrdw/zac2650.c 
		drivers/zaccaria.c vidhrdw/zaccaria.c 	)
	set(MSND_AY8910 ON)
	set(MSND_DAC ON)
	set(MSND_SN76477 ON)
	set(MSND_TMS36XX ON)
	set(MSND_TMS5220 ON)
	set(MCPU_S2650 ON)
	list(APPEND CPU_DEFS
		HAS_AY8910=1 HAS_DAC=1 HAS_SN76477=1 HAS_TMS36XX=1 
		HAS_TMS5220=1 	)
	list(APPEND CPU_DEFS
		HAS_S2650=1 	)
endif()
if(MSND_AY8910)
	list(APPEND MAME_SOUND_SRC sound/ay8910.c )
endif()
if(MSND_BSMT2000)
	list(APPEND MAME_SOUND_SRC sound/bsmt2000.c )
endif()
if(MSND_C140)
	list(APPEND MAME_SOUND_SRC sound/c140.c )
endif()
if(MSND_C352)
	list(APPEND MAME_SOUND_SRC sound/c352.c )
endif()
if(MSND_C6280)
	list(APPEND MAME_SOUND_SRC sound/c6280.c )
endif()
if(MSND_CDDA)
	list(APPEND MAME_SOUND_SRC sound/cdda.c )
endif()
if(MSND_CEM3394)
	list(APPEND MAME_SOUND_SRC sound/cem3394.c )
endif()
if(MSND_CUSTOM)
	list(APPEND MAME_SOUND_SRC sound/custom.c )
endif()
if(MSND_DAC)
	list(APPEND MAME_SOUND_SRC sound/dac.c )
endif()
if(MSND_DISCRETE)
	list(APPEND MAME_SOUND_SRC sound/discrete.c )
endif()
if(MSND_DMADAC)
	list(APPEND MAME_SOUND_SRC sound/dmadac.c )
endif()
if(MSND_ES5503)
	list(APPEND MAME_SOUND_SRC sound/es5503.c )
endif()
if(MSND_ES5506)
	list(APPEND MAME_SOUND_SRC sound/es5506.c )
endif()
if(MSND_ES8712)
	list(APPEND MAME_SOUND_SRC sound/es8712.c )
endif()
if(MSND_GAELCO_GAE1)
	list(APPEND MAME_SOUND_SRC sound/gaelco.c )
endif()
if(MSND_HC55516)
	list(APPEND MAME_SOUND_SRC sound/hc55516.c )
endif()
if(MSND_ICS2115)
	list(APPEND MAME_SOUND_SRC sound/ics2115.c )
endif()
if(MSND_IREMGA20)
	list(APPEND MAME_SOUND_SRC sound/iremga20.c )
endif()
if(MSND_K005289)
	list(APPEND MAME_SOUND_SRC sound/k005289.c )
endif()
if(MSND_K007232)
	list(APPEND MAME_SOUND_SRC sound/k007232.c )
endif()
if(MSND_K051649)
	list(APPEND MAME_SOUND_SRC sound/k051649.c )
endif()
if(MSND_K053260)
	list(APPEND MAME_SOUND_SRC sound/k053260.c )
endif()
if(MSND_K054539)
	list(APPEND MAME_SOUND_SRC sound/k054539.c )
endif()
if(MSND_MSM5205)
	list(APPEND MAME_SOUND_SRC sound/msm5205.c )
endif()
if(MSND_MSM5232)
	list(APPEND MAME_SOUND_SRC sound/msm5232.c )
endif()
if(MSND_MULTIPCM)
	list(APPEND MAME_SOUND_SRC sound/multipcm.c )
endif()
if(MSND_NAMCO)
	list(APPEND MAME_SOUND_SRC sound/namco.c )
endif()
if(MSND_NAMCONA)
	list(APPEND MAME_SOUND_SRC sound/namcona.c )
endif()
if(MSND_OKIM6295)
	list(APPEND MAME_SOUND_SRC sound/okim6295.c )
endif()
if(MSND_POKEY)
	list(APPEND MAME_SOUND_SRC sound/pokey.c )
endif()
if(MSND_PSXSPU)
	list(APPEND MAME_SOUND_SRC sound/psx.c )
endif()
if(MSND_QSOUND)
	list(APPEND MAME_SOUND_SRC sound/qsound.c )
endif()
if(MSND_RF5C400)
	list(APPEND MAME_SOUND_SRC sound/rf5c400.c )
endif()
if(MSND_RF5C68)
	list(APPEND MAME_SOUND_SRC sound/rf5c68.c )
endif()
if(MSND_SAA1099)
	list(APPEND MAME_SOUND_SRC sound/saa1099.c )
endif()
if(MSND_SAMPLES)
	list(APPEND MAME_SOUND_SRC sound/samples.c )
endif()
if(MSND_SCSP)
	list(APPEND MAME_SOUND_SRC sound/scsp.c )
endif()
if(MSND_SEGAPCM)
	list(APPEND MAME_SOUND_SRC sound/segapcm.c )
endif()
if(MSND_SN76477)
	list(APPEND MAME_SOUND_SRC sound/sn76477.c )
endif()
if(MSND_SN76496)
	list(APPEND MAME_SOUND_SRC sound/sn76496.c )
endif()
if(MSND_SP0250)
	list(APPEND MAME_SOUND_SRC sound/sp0250.c )
endif()
if(MSND_ST0016)
	list(APPEND MAME_SOUND_SRC sound/st0016.c )
endif()
if(MSND_TIA)
	list(APPEND MAME_SOUND_SRC sound/tiasound.c sound/tiaintf.c )
endif()
if(MSND_TMS36XX)
	list(APPEND MAME_SOUND_SRC sound/tms36xx.c )
endif()
if(MSND_TMS5110)
	list(APPEND MAME_SOUND_SRC sound/tms5110.c sound/5110intf.c )
endif()
if(MSND_TMS5220)
	list(APPEND MAME_SOUND_SRC sound/tms5220.c sound/5220intf.c )
endif()
if(MSND_UPD7759)
	list(APPEND MAME_SOUND_SRC sound/upd7759.c )
endif()
if(MSND_VLM5030)
	list(APPEND MAME_SOUND_SRC sound/vlm5030.c )
endif()
if(MSND_VRENDER0)
	list(APPEND MAME_SOUND_SRC sound/vrender0.c )
endif()
if(MSND_X1_010)
	list(APPEND MAME_SOUND_SRC sound/x1_010.c )
endif()
if(MSND_YM2151)
	list(APPEND MAME_SOUND_SRC sound/2151intf.c sound/ym2151.c )
endif()
if(MSND_YM2203)
	list(APPEND MAME_SOUND_SRC sound/2203intf.c sound/ay8910.c sound/fm.c )
endif()
if(MSND_YM2413)
	list(APPEND MAME_SOUND_SRC sound/2413intf.c sound/ym2413.c )
endif()
if(MSND_YM2608)
	list(APPEND MAME_SOUND_SRC sound/2608intf.c sound/ay8910.c sound/fm.c sound/ymdeltat.c )
endif()
if(MSND_YM2610)
	list(APPEND MAME_SOUND_SRC sound/2610intf.c sound/ay8910.c sound/fm.c sound/ymdeltat.c )
endif()
if(MSND_YM2610B)
	list(APPEND MAME_SOUND_SRC sound/2610intf.c sound/ay8910.c sound/fm.c sound/ymdeltat.c )
endif()
if(MSND_YM3438)
	list(APPEND MAME_SOUND_SRC sound/2612intf.c sound/ay8910.c sound/fm.c )
endif()
if(MSND_YM3526)
	list(APPEND MAME_SOUND_SRC sound/3812intf.c sound/fmopl.c )
endif()
if(MSND_YM3812)
	list(APPEND MAME_SOUND_SRC sound/3812intf.c sound/fmopl.c )
endif()
if(MSND_YMF262)
	list(APPEND MAME_SOUND_SRC sound/ymf262.c sound/262intf.c )
endif()
if(MSND_YMF271)
	list(APPEND MAME_SOUND_SRC sound/ymf271.c )
endif()
if(MSND_YMF278B)
	list(APPEND MAME_SOUND_SRC sound/ymf278b.c )
endif()
if(MSND_YMZ280B)
	list(APPEND MAME_SOUND_SRC sound/ymz280b.c )
endif()
if(MCPU_ADSP2100)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ADSP2101)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ADSP2104)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ADSP2105)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ADSP21062)
	list(APPEND MAME_CPU_SRC cpu/sharc/sharc.c )
endif()
if(MCPU_ADSP2115)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ADSP2181)
	list(APPEND MAME_CPU_SRC cpu/adsp2100/adsp2100.c )
endif()
if(MCPU_ARM)
	list(APPEND MAME_CPU_SRC cpu/arm/arm.c )
endif()
if(MCPU_ARM7)
	list(APPEND MAME_CPU_SRC cpu/arm7/arm7.c )
endif()
if(MCPU_ASAP)
	list(APPEND MAME_CPU_SRC cpu/asap/asap.c )
endif()
if(MCPU_CCPU)
	list(APPEND MAME_CPU_SRC cpu/ccpu/ccpu.c )
endif()
if(MCPU_DECO16)
	list(APPEND MAME_CPU_SRC cpu/m6502/m6502.c )
endif()
if(MCPU_G65816)
	list(APPEND MAME_CPU_SRC cpu/g65816/g65816.c cpu/g65816/g65816o0.c cpu/g65816/g65816o1.c cpu/g65816/g65816o2.c cpu/g65816/g65816o3.c cpu/g65816/g65816o4.c )
endif()
if(MCPU_H6280)
	list(APPEND MAME_CPU_SRC cpu/h6280/h6280.c )
endif()
if(MCPU_H83002)
	list(APPEND MAME_CPU_SRC cpu/h83002/h83002.c cpu/h83002/h8periph.c )
endif()
if(MCPU_HD6309)
	list(APPEND MAME_CPU_SRC cpu/hd6309/hd6309.c )
endif()
if(MCPU_I386)
	list(APPEND MAME_CPU_SRC cpu/i386/i386.c )
endif()
if(MCPU_I8039)
	list(APPEND MAME_CPU_SRC cpu/i8039/i8039.c )
endif()
if(MCPU_I8051)
	list(APPEND MAME_CPU_SRC cpu/i8051/i8051.c )
endif()
if(MCPU_I86)
	list(APPEND MAME_CPU_SRC cpu/i86/i86.c )
endif()
if(MCPU_I8X41)
	list(APPEND MAME_CPU_SRC cpu/i8x41/i8x41.c )
endif()
if(MCPU_I960)
	list(APPEND MAME_CPU_SRC cpu/i960/i960.c )
endif()
if(MCPU_JAGUAR)
	list(APPEND MAME_CPU_SRC cpu/jaguar/jaguar.c )
endif()
if(MCPU_KONAMI)
	list(APPEND MAME_CPU_SRC cpu/konami/konami.c )
endif()
if(MCPU_M37710)
	list(APPEND MAME_CPU_SRC cpu/m37710/m37710.c cpu/m37710/m37710o0.c cpu/m37710/m37710o1.c cpu/m37710/m37710o2.c cpu/m37710/m37710o3.c cpu/m37710/m7700ds.c )
endif()
if(MCPU_M6502)
	list(APPEND MAME_CPU_SRC cpu/m6502/m6502.c )
endif()
if(MCPU_M65C02)
	list(APPEND MAME_CPU_SRC cpu/m6502/m6502.c )
endif()
if(MCPU_M6800)
	list(APPEND MAME_CPU_SRC cpu/m6800/m6800.c )
endif()
if(MCPU_M68000)
	list(APPEND MAME_CPU_SRC cpu/m68000/m68kcpu.c cpu/m68000/m68kmame.c cpu/m68000/m68kops.c cpu/m68000/m68kopac.c cpu/m68000/m68kopdm.c cpu/m68000/m68kopnz.c )
endif()
if(MCPU_M6802)
	list(APPEND MAME_CPU_SRC cpu/m6800/m6800.c )
endif()
if(MCPU_M6803)
	list(APPEND MAME_CPU_SRC cpu/m6800/m6800.c )
endif()
if(MCPU_M6805)
	list(APPEND MAME_CPU_SRC cpu/m6805/m6805.c )
endif()
if(MCPU_M6808)
	list(APPEND MAME_CPU_SRC cpu/m6800/m6800.c )
endif()
if(MCPU_M6809)
	list(APPEND MAME_CPU_SRC cpu/m6809/m6809.c )
endif()
if(MCPU_PSXCPU)
	list(APPEND MAME_CPU_SRC cpu/mips/psx.c )
endif()
if(MCPU_RSP)
	list(APPEND MAME_CPU_SRC cpu/rsp/rsp.c )
endif()
if(MCPU_S2650)
	list(APPEND MAME_CPU_SRC cpu/s2650/s2650.c )
endif()
if(MCPU_SE3208)
	list(APPEND MAME_CPU_SRC cpu/se3208/se3208.c )
endif()
if(MCPU_SH2)
	list(APPEND MAME_CPU_SRC cpu/sh2/sh2.c )
endif()
if(MCPU_T11)
	list(APPEND MAME_CPU_SRC cpu/t11/t11.c )
endif()
if(MCPU_TMS32010)
	list(APPEND MAME_CPU_SRC cpu/tms32010/tms32010.c )
endif()
if(MCPU_TMS32025)
	list(APPEND MAME_CPU_SRC cpu/tms32025/tms32025.c )
endif()
if(MCPU_TMS32031)
	list(APPEND MAME_CPU_SRC cpu/tms32031/tms32031.c )
endif()
if(MCPU_TMS34010)
	list(APPEND MAME_CPU_SRC cpu/tms34010/tms34010.c cpu/tms34010/34010fld.c )
endif()
if(MCPU_UPD7810)
	list(APPEND MAME_CPU_SRC cpu/upd7810/upd7810.c )
endif()
if(MCPU_Z180)
	list(APPEND MAME_CPU_SRC cpu/z180/z180.c cpu/z80/z80daisy.c )
endif()
if(MCPU_Z80)
	list(APPEND MAME_CPU_SRC cpu/z80/z80.c cpu/z80/z80daisy.c )
endif()
if(MCPU_Z8000)
	list(APPEND MAME_CPU_SRC cpu/z8000/z8000.c )
endif()
list(REMOVE_DUPLICATES MAME_DRIVERS_SRC)
list(REMOVE_DUPLICATES MAME_SOUND_SRC)
list(REMOVE_DUPLICATES MAME_CPU_SRC)
list(REMOVE_DUPLICATES CPU_DEFS)
