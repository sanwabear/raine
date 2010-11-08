ifdef NEO

OBJS += \
	$(SC000) \
	$(MZ80) \
	$(2610) \
	$(AY8910) \
	$(NEOCD) 
else
OBJS += \
	$(OBJDIR)/games/ainferno.o \
	$(OBJDIR)/games/aquajack.o \
	$(OBJDIR)/games/arabianm.o \
	$(OBJDIR)/games/arkanoid.o \
	$(OBJDIR)/games/arkretrn.o \
	$(OBJDIR)/games/armedf.o \
	$(OBJDIR)/games/ashura.o \
	$(OBJDIR)/games/asuka.o \
	$(OBJDIR)/games/biomtoy.o \
	$(OBJDIR)/games/bloodbro.o \
	$(OBJDIR)/games/bonzeadv.o \
	$(OBJDIR)/games/bshark.o \
	$(OBJDIR)/games/bsystem.o \
	$(OBJDIR)/games/bubblem.o \
	$(OBJDIR)/games/bublbobl.o \
	$(OBJDIR)/games/bubl_mcu.o \
	$(OBJDIR)/games/bubsymph.o \
	$(OBJDIR)/games/cabal.o \
	$(OBJDIR)/games/cadash.o \
	$(OBJDIR)/games/cave.o \
	$(OBJDIR)/games/chaknpop.o \
	$(OBJDIR)/games/chasehq2.o \
	$(OBJDIR)/games/chasehq.o \
	$(OBJDIR)/games/cleofort.o \
	$(OBJDIR)/games/contcirc.o \
	$(OBJDIR)/games/cookbib.o \
	$(OBJDIR)/games/cps1drv.o \
	$(OBJDIR)/games/cps2drv.o \
	$(OBJDIR)/games/crimec.o \
	$(OBJDIR)/games/crospang.o \
	$(OBJDIR)/games/ctribebl.o \
	$(OBJDIR)/games/cupfinal.o \
	$(OBJDIR)/games/dangarb.o \
	$(OBJDIR)/games/darius2d.o \
	$(OBJDIR)/games/darius.o \
	$(OBJDIR)/games/dariusg.o \
	$(OBJDIR)/games/deadconx.o \
	$(OBJDIR)/games/diverboy.o \
	$(OBJDIR)/games/dkong.o \
	$(OBJDIR)/games/dleague.o \
	$(OBJDIR)/games/doubled3.o \
	$(OBJDIR)/games/eaction2.o \
	$(OBJDIR)/games/earthjkr.o \
	$(OBJDIR)/games/esd16.o \
	$(OBJDIR)/games/exzisus.o \
	$(OBJDIR)/games/f3demo.o \
	$(OBJDIR)/games/fchamp.o \
	$(OBJDIR)/games/flstory.o \
	$(OBJDIR)/games/frogger.o \
	$(OBJDIR)/games/galpanic.o \
	$(OBJDIR)/games/gcpball.o \
	$(OBJDIR)/games/gekridan.o \
	$(OBJDIR)/games/gridseek.o \
	$(OBJDIR)/games/gunbird.o \
	$(OBJDIR)/games/gunlock.o \
	$(OBJDIR)/games/heavyunt.o \
	$(OBJDIR)/games/hitice.o \
	$(OBJDIR)/games/hthero95.o \
	$(OBJDIR)/games/hyperpcb.o \
	$(OBJDIR)/games/intcup94.o \
	$(OBJDIR)/games/kaiserkn.o \
	$(OBJDIR)/games/konamigx.o \
	$(OBJDIR)/games/koshien.o \
	$(OBJDIR)/games/ksystem.o \
	$(OBJDIR)/games/ktiger2.o \
	$(OBJDIR)/games/landmakr.o \
	$(OBJDIR)/games/lightbr.o \
	$(OBJDIR)/games/lsystem.o \
	$(OBJDIR)/games/macrossp.o \
	$(OBJDIR)/games/masterw.o \
	$(OBJDIR)/games/matmania.o \
	$(OBJDIR)/games/mcatadv.o \
	$(OBJDIR)/games/megasys1.o \
	$(OBJDIR)/games/metalb.o \
	$(OBJDIR)/games/mitchell.o \
	$(OBJDIR)/games/mugsmash.o \
	$(OBJDIR)/games/ninjak.o \
	$(OBJDIR)/games/ninjaw.o \
	$(OBJDIR)/games/nmk.o \
	$(OBJDIR)/games/nmk004.o \
	$(OBJDIR)/games/opwolf3.o \
	$(OBJDIR)/games/opwolf.o \
	$(OBJDIR)/games/othunder.o \
	$(OBJDIR)/games/pbobble2.o \
	$(OBJDIR)/games/pbobble3.o \
	$(OBJDIR)/games/pbobble4.o \
	$(OBJDIR)/games/pengo.o \
	$(OBJDIR)/games/pirates.o \
	$(OBJDIR)/games/popnpop.o \
	$(OBJDIR)/games/psyforce.o \
	$(OBJDIR)/games/puchicar.o \
	$(OBJDIR)/games/quizgame.o \
	$(OBJDIR)/games/rainbow.o \
	$(OBJDIR)/games/rambo3.o \
	$(OBJDIR)/games/rastan2.o \
	$(OBJDIR)/games/rastan.o \
	$(OBJDIR)/games/recordbr.o \
	$(OBJDIR)/games/ridefght.o \
	$(OBJDIR)/games/ringrage.o \
	$(OBJDIR)/games/robokid.o \
	$(OBJDIR)/games/seta.o \
	$(OBJDIR)/games/silentd.o \
	$(OBJDIR)/games/silkworm.o \
	$(OBJDIR)/games/spacegun.o \
	$(OBJDIR)/games/spcinv95.o \
	$(OBJDIR)/games/superchs.o \
	$(OBJDIR)/games/syvalion.o \
	$(OBJDIR)/games/taito_f2.o \
	$(OBJDIR)/games/tecmosys.o \
	$(OBJDIR)/games/terrac.o \
	$(OBJDIR)/games/terraf.o \
	$(OBJDIR)/games/tetrisp2.o \
	$(OBJDIR)/games/tetrist.o \
	$(OBJDIR)/games/toaplan1.o \
	$(OBJDIR)/games/toaplan2.o \
	$(OBJDIR)/games/toki.o \
	$(OBJDIR)/games/topland.o \
	$(OBJDIR)/games/topspeed.o \
	$(OBJDIR)/games/trstars.o \
	$(OBJDIR)/games/twinqix.o \
	$(OBJDIR)/games/undrfire.o \
	$(OBJDIR)/games/upl.o \
	$(OBJDIR)/games/viofight.o \
	$(OBJDIR)/games/volfied.o \
	$(OBJDIR)/games/warriorb.o \
	$(OBJDIR)/games/wgp.o \
	$(OBJDIR)/games/wrestlef.o \
	$(OBJDIR)/games/wwfsstar.o \
	$(OBJDIR)/games/xsystem1.o \
	$(OBJDIR)/games/xsystem2.o \
	$(OBJDIR)/games/zerozone.o \
	$(OBJDIR)/games/f3system.o \
	$(OBJDIR)/games/nichisnd.o \
	$(OBJDIR)/games/taitosnd.o \
	$(OBJDIR)/games/tchnosnd.o \
	$(OBJDIR)/games/kick_mcu.o \
	$(OBJDIR)/games/tc200obj.o \
	$(OBJDIR)/games/tc220ioc.o \
	$(OBJDIR)/games/tc110pcr.o \
	$(OBJDIR)/games/kiki_mcu.o \
	$(OBJDIR)/games/tc100scn.o \
	$(OBJDIR)/games/tc180vcu.o \
	$(OBJDIR)/games/tc005rot.o \
	$(OBJDIR)/games/tc140syt.o \
	$(OBJDIR)/games/setax1.o \
	$(OBJDIR)/games/tc003vcu.o \
	$(OBJDIR)/games/tc150rod.o \
	$(OBJDIR)/games/tc101scn.o \
	$(OBJDIR)/games/tc002obj.o \
	$(OBJDIR)/games/taito_z.o \
	$(OBJDIR)/games/tc004vcu.o \
	$(OBJDIR)/games/cps1.o \
	$(OBJDIR)/games/tc006vcu.o \
	$(OBJDIR)/games/gun.o \

OBJS += \
	$(SC000) \
	$(ASM020) \
	$(MZ80) \
	$(M6502) \
	$(M68705) \
	$(2151) \
	$(2203) \
	$(2413) \
	$(2610) \
	$(OBJDIR)/emudx.o \
	$(OBJDIR)/cat93c46.o \
	$(OBJDIR)/decode.o \
	$(OBJDIR)/cps2crpt.o \
	$(3812) \
	$(ADPCM) \
	$(AY8910) \
	$(DAC) \
	$(DXSMP) \
	$(ENSONIQ) \
	$(M6585) \
	$(MSM5205) \
	$(NAMCO) \
	$(QSOUND) \
	$(SMP16BIT) \
	$(YMZ280B) \
	$(YMF278B) \
	$(X1_010) \
	$(TOAPLAN2) \

endif