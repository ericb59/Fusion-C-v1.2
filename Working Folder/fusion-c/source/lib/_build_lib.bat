CLS
@echo off
echo " "    
echo "  F U S I O N - C  V 1 . 2 "
echo " T h e   U l t i m a t e   SDCC   L i b r a r y   f o r   M S X "
echo "  Eric Boez & Fernando Garcia 2018-2019 : Made for coders !"
echo "_________________________________________________________________"
echo " "    
echo " "     
echo " "    
echo "------------------------------------ FUSION-C" > log.txt
echo %date% %time% > log.txt
echo "------------------------------------" > log.txt
@echo "Now Building FUSION-C Library..."
@del .\fusion.lib

@echo  "... Compiling ASM Functions"
sdasz80 -o fcb_find.s > log.txt
sdasz80 -o fcb_open.s > log.txt
sdasz80 -o fcb_read.s > log.txt
sdasz80 -o inport.s > log.txt
sdasz80 -o outport.s > log.txt
sdasz80 -o outports.s > log.txt
sdasz80 -o putchar_msxdos.s > log.txt
sdasz80 -o getchar_msxdos.s > log.txt
sdasz80 -o screen.s > log.txt
sdasz80 -o vdpstatus.s > log.txt
sdasz80 -o vdpstatusni.s > log.txt
sdasz80 -o vdpwrite.s > log.txt
sdasz80 -o vdpwriteni.s > log.txt
sdasz80 -o exit.s > log.txt
sdasz80 -o doscls.s > log.txt
sdasz80 -o intdosbios.s > log.txt
sdasz80 -o settime.s > log.txt
sdasz80 -o setdate.s > log.txt
sdasz80 -o gettime.s > log.txt
sdasz80 -o getdate.s > log.txt
sdasz80 -o suspend.s > log.txt
sdasz80 -o memchr.s > log.txt
sdasz80 -o memfill.s > log.txt
sdasz80 -o memcopy.s > log.txt
sdasz80 -o memcopyreverse.s > log.txt
sdasz80 -o memcompare.s > log.txt
sdasz80 -o waitkey.s > log.txt
sdasz80 -o keyboardread.s > log.txt
sdasz80 -o printdec.s > log.txt
sdasz80 -o inputchar.s > log.txt
sdasz80 -o printchar.s > log.txt
sdasz80 -o inputstring.s > log.txt
sdasz80 -o getche.s > log.txt
sdasz80 -o inkey.s > log.txt
sdasz80 -o setbordercolor.s > log.txt
sdasz80 -o puttext.s > log.txt
sdasz80 -o setpalette.s > log.txt
sdasz80 -o spritemore.s > log.txt
sdasz80 -o readwritescr.s > log.txt
sdasz80 -o scblock.s > log.txt
sdasz80 -o vdp_graph2.s > log.txt
sdasz80 -o vdp_graph1.s > log.txt
sdasz80 -o ctype.s > log.txt
sdasz80 -o io.s > log.txt
sdasz80 -o rammapper.s > log.txt
sdasz80 -o pt3replayer.s > log.txt
sdasz80 -o strcopy.s > log.txt
sdasz80 -o nstrcopy.s > log.txt
sdasz80 -o strlen.s > log.txt
sdasz80 -o strconcat.s > log.txt
sdasz80 -o nstrconcat.s > log.txt
sdasz80 -o strcompare.s > log.txt
sdasz80 -o nstrcompare.s > log.txt
sdasz80 -o strchr.s > log.txt
sdasz80 -o strsearch.s > log.txt
sdasz80 -o strposchr.s > log.txt
sdasz80 -o chartolower.s > log.txt
sdasz80 -o chartoupper.s > log.txt
sdasz80 -o strposstr.s > log.txt
sdasz80 -o strlefttrim.s > log.txt
sdasz80 -o strrighttrim.s > log.txt
sdasz80 -o setdisktradress.s > log.txt
sdasz80 -o getdisktradress.s > log.txt
sdasz80 -o sectorread.s > log.txt
sdasz80 -o sectorwrite.s > log.txt
sdasz80 -o getdiskparam.s > log.txt
sdasz80 -o readsp.s > log.txt
sdasz80 -o vram.s > log.txt
sdasz80 -o interrupt.s > log.txt
sdasz80 -o printhex.s > log.txt



echo " ... Compiling C functions"
sdcc --use-stdout -mz80 -c  vpeek.c > log.txt
sdcc --use-stdout -mz80 -c  vpeekfirst.c > log.txt
sdcc --use-stdout -mz80 -c  vpoke.c > log.txt
sdcc --use-stdout -mz80 -c  bchput.c > log.txt
sdcc --use-stdout -mz80 -c  num2dec16.c > log.txt
sdcc --use-stdout -mz80 -c  printfnumber.c > log.txt
sdcc --use-stdout -mz80 -c  printnumber.c > log.txt
sdcc --use-stdout -mz80 -c  getkeymatrix.c > log.txt
sdcc --use-stdout -mz80 -c  readmsxtype.c > log.txt
sdcc --use-stdout -mz80 -c  initpsg.c > log.txt
sdcc --use-stdout -mz80 -c  killkeybuffer.c > log.txt
sdcc --use-stdout -mz80 -c  fillvram.c > log.txt
sdcc --use-stdout -mz80 -c  changecap.c > log.txt
sdcc --use-stdout -mz80 -c  beep.c > log.txt
sdcc --use-stdout -mz80 -c  showdisplay.c > log.txt
sdcc --use-stdout -mz80 -c  hidedisplay.c > log.txt
sdcc --use-stdout -mz80 -c  functionkeys.c > log.txt
sdcc --use-stdout -mz80 -c  width.c > log.txt
sdcc --use-stdout -mz80 -c  setscrollv.c > log.txt
sdcc --use-stdout -mz80 -c  setscrollh.c > log.txt
sdcc --use-stdout -mz80 -c  psgwrite.c > log.txt
sdcc --use-stdout -mz80 -c  psgread.c > log.txt
sdcc --use-stdout -mz80 -c  setactivepage.c > log.txt
sdcc --use-stdout -mz80 -c  setdisplaypage.c > log.txt
sdcc --use-stdout -mz80 -c  keysound.c > log.txt
sdcc --use-stdout -mz80 -c  circle.c > log.txt
sdcc --use-stdout -mz80 -c  circlefilled.c > log.txt
sdcc --use-stdout -mz80 -c  joystickread.c > log.txt
sdcc --use-stdout -mz80 -c  triggerread.c > log.txt
sdcc --use-stdout -mz80 -c  getcpu.c > log.txt
sdcc --use-stdout -mz80 -c  changecpu.c > log.txt
sdcc --use-stdout -mz80 -c  pcmplay.c > log.txt
sdcc --use-stdout -mz80 -c  silencepsg.c > log.txt
sdcc --use-stdout -mz80 -c  sound.c > log.txt
sdcc --use-stdout -mz80 -c  soundfx.c > log.txt
sdcc --use-stdout -mz80 -c  setchannel.c > log.txt
sdcc --use-stdout -mz80 -c  getsound.c > log.txt
sdcc --use-stdout -mz80 -c  settoneperiod.c > log.txt
sdcc --use-stdout -mz80 -c  setnoiseperiod.c > log.txt
sdcc --use-stdout -mz80 -c  setenvelopeperiod.c > log.txt
sdcc --use-stdout -mz80 -c  setvolume.c > log.txt
sdcc --use-stdout -mz80 -c  playenvelope.c > log.txt
sdcc --use-stdout -mz80 -c  locate.c > log.txt
sdcc --use-stdout -mz80 -c  cls.c > log.txt
sdcc --use-stdout -mz80 -c  print.c > log.txt
sdcc --use-stdout -mz80 -c  setspritepattern.c > log.txt
sdcc --use-stdout -mz80 -c  putsprite.c > log.txt
sdcc --use-stdout -mz80 -c  sprite8.c > log.txt
sdcc --use-stdout -mz80 -c  sprite16.c > log.txt
sdcc --use-stdout -mz80 -c  spriteon.c > log.txt
sdcc --use-stdout -mz80 -c  spriteoff.c > log.txt
sdcc --use-stdout -mz80 -c  spritesmall.c > log.txt
sdcc --use-stdout -mz80 -c  spritedouble.c > log.txt
sdcc --use-stdout -mz80 -c  spritereset.c > log.txt
sdcc --use-stdout -mz80 -c  spritecollision.c > log.txt
sdcc --use-stdout -mz80 -c  setcolors.c > log.txt
sdcc --use-stdout -mz80 -c  intswap.c > log.txt
sdcc --use-stdout -mz80 -c  inttofloat.c > log.txt
sdcc --use-stdout -mz80 -c  ispositive.c > log.txt
sdcc --use-stdout -mz80 -c  mmalloc.c > log.txt
sdcc --use-stdout -mz80 -c  vdp50hz.c > log.txt
sdcc --use-stdout -mz80 -c  vdp60hz.c > log.txt
sdcc --use-stdout -mz80 -c  readtpa.c > log.txt
sdcc --use-stdout -mz80 -c  setrealtimer.c > log.txt
sdcc --use-stdout -mz80 -c  realtimer.c > log.txt
sdcc --use-stdout -mz80 -c  mouseread.c > log.txt
sdcc --use-stdout -mz80 -c  mousereadto.c > log.txt
sdcc --use-stdout -mz80 -c  covoxplay.c > log.txt
sdcc --use-stdout -mz80 -c  sc2circle.c > log.txt
sdcc --use-stdout -mz80 -c  sc2circlefilled.c > log.txt
sdcc --use-stdout -mz80 -c  vdplinesswitch.c > log.txt
sdcc --use-stdout -mz80 -c  rlewbtovram.c > log.txt
sdcc --use-stdout -mz80 -c  rlewbtoram.c > log.txt
sdcc --use-stdout -mz80 -c  strreverse.c > log.txt
sdcc --use-stdout -mz80 -c  itoa.c > log.txt
sdcc --use-stdout -mz80 -c  vdp_graph2plus.c > log.txt



echo "... adding functions to the lib"
sdar -rc  fusion.lib printf.rel
sdar -rc  fusion.lib fcb_find.rel
sdar -rc  fusion.lib fcb_open.rel
sdar -rc  fusion.lib fcb_read.rel
sdar -rc  fusion.lib inport.rel
sdar -rc  fusion.lib outport.rel
sdar -rc  fusion.lib outports.rel
sdar -rc  fusion.lib putchar_msxdos.rel
sdar -rc  fusion.lib getchar_msxdos.rel
sdar -rc  fusion.lib screen.rel
sdar -rc  fusion.lib vdpstatus.rel
sdar -rc  fusion.lib vdpstatusni.rel
sdar -rc  fusion.lib vdpwrite.rel
sdar -rc  fusion.lib vdpwriteni.rel
sdar -rc  fusion.lib exit.rel
sdar -rc  fusion.lib doscls.rel
sdar -rc  fusion.lib intdosbios.rel
sdar -rc  fusion.lib settime.rel
sdar -rc  fusion.lib setdate.rel
sdar -rc  fusion.lib gettime.rel
sdar -rc  fusion.lib getdate.rel
sdar -rc  fusion.lib suspend.rel
sdar -rc  fusion.lib vpeek.rel
sdar -rc  fusion.lib vpeekfirst.rel
sdar -rc  fusion.lib vpoke.rel
sdar -rc  fusion.lib bchput.rel
sdar -rc  fusion.lib num2dec16.rel
sdar -rc  fusion.lib printfnumber.rel
sdar -rc  fusion.lib printnumber.rel
sdar -rc  fusion.lib getkeymatrix.rel
sdar -rc  fusion.lib readmsxtype.rel
sdar -rc  fusion.lib initpsg.rel
sdar -rc  fusion.lib killkeybuffer.rel
sdar -rc  fusion.lib fillvram.rel
sdar -rc  fusion.lib changecap.rel
sdar -rc  fusion.lib beep.rel
sdar -rc  fusion.lib showdisplay.rel
sdar -rc  fusion.lib hidedisplay.rel
sdar -rc  fusion.lib functionkeys.rel
sdar -rc  fusion.lib width.rel
sdar -rc  fusion.lib setscrollv.rel
sdar -rc  fusion.lib setscrollh.rel
sdar -rc  fusion.lib psgwrite.rel
sdar -rc  fusion.lib psgread.rel
sdar -rc  fusion.lib setactivepage.rel
sdar -rc  fusion.lib setdisplaypage.rel
sdar -rc  fusion.lib keysound.rel
sdar -rc  fusion.lib circle.rel
sdar -rc  fusion.lib circlefilled.rel
sdar -rc  fusion.lib joystickread.rel
sdar -rc  fusion.lib triggerread.rel
sdar -rc  fusion.lib getcpu.rel
sdar -rc  fusion.lib changecpu.rel
sdar -rc  fusion.lib pcmplay.rel
sdar -rc  fusion.lib silencepsg.rel
sdar -rc  fusion.lib sound.rel
sdar -rc  fusion.lib soundfx.rel
sdar -rc  fusion.lib setchannel.rel
sdar -rc  fusion.lib getsound.rel
sdar -rc  fusion.lib settoneperiod.rel
sdar -rc  fusion.lib setnoiseperiod.rel
sdar -rc  fusion.lib setenvelopeperiod.rel
sdar -rc  fusion.lib setvolume.rel
sdar -rc  fusion.lib playenvelope.rel
sdar -rc  fusion.lib memchr.rel
sdar -rc  fusion.lib memfill.rel
sdar -rc  fusion.lib memcopy.rel
sdar -rc  fusion.lib memcopyreverse.rel
sdar -rc  fusion.lib memcompare.rel
sdar -rc  fusion.lib waitkey.rel
sdar -rc  fusion.lib keyboardread.rel
sdar -rc  fusion.lib locate.rel
sdar -rc  fusion.lib cls.rel
sdar -rc  fusion.lib inputchar.rel
sdar -rc  fusion.lib printchar.rel
sdar -rc  fusion.lib inputstring.rel
sdar -rc  fusion.lib getche.rel
sdar -rc  fusion.lib inkey.rel
sdar -rc  fusion.lib print.rel
sdar -rc  fusion.lib setbordercolor.rel
sdar -rc  fusion.lib puttext.rel
sdar -rc  fusion.lib setpalette.rel
sdar -rc  fusion.lib setspritepattern.rel
sdar -rc  fusion.lib putsprite.rel
sdar -rc  fusion.lib sprite8.rel
sdar -rc  fusion.lib sprite16.rel
sdar -rc  fusion.lib spriteon.rel
sdar -rc  fusion.lib spriteoff.rel
sdar -rc  fusion.lib spritesmall.rel
sdar -rc  fusion.lib spritedouble.rel
sdar -rc  fusion.lib spritereset.rel
sdar -rc  fusion.lib spritemore.rel
sdar -rc  fusion.lib spritecollision.rel
sdar -rc  fusion.lib readwritescr.rel
sdar -rc  fusion.lib scblock.rel
sdar -rc  fusion.lib vdp_graph1.rel
sdar -rc  fusion.lib vdp_graph2.rel
sdar -rc  fusion.lib ctype.rel
sdar -rc  fusion.lib io.rel
sdar -rc  fusion.lib rammapper.rel
sdar -rc  fusion.lib pt3replayer.rel
sdar -rc  fusion.lib setcolors.rel
sdar -rc  fusion.lib strcopy.rel
sdar -rc  fusion.lib nstrcopy.rel
sdar -rc  fusion.lib strlen.rel
sdar -rc  fusion.lib strconcat.rel
sdar -rc  fusion.lib nstrconcat.rel
sdar -rc  fusion.lib strcompare.rel
sdar -rc  fusion.lib nstrcompare.rel
sdar -rc  fusion.lib strchr.rel
sdar -rc  fusion.lib strsearch.rel
sdar -rc  fusion.lib strposchr.rel
sdar -rc  fusion.lib chartolower.rel
sdar -rc  fusion.lib chartoupper.rel
sdar -rc  fusion.lib strposstr.rel
sdar -rc  fusion.lib strlefttrim.rel
sdar -rc  fusion.lib strrighttrim.rel
sdar -rc  fusion.lib printdec.rel
sdar -rc  fusion.lib intswap.rel
sdar -rc  fusion.lib inttofloat.rel
sdar -rc  fusion.lib ispositive.rel
sdar -rc  fusion.lib mmalloc.rel
sdar -rc  fusion.lib VDP50Hz.rel
sdar -rc  fusion.lib VDP60Hz.rel
sdar -rc  fusion.lib readsp.rel
sdar -rc  fusion.lib readtpa.rel
sdar -rc  fusion.lib setrealtimer.rel
sdar -rc  fusion.lib realtimer.rel
sdar -rc  fusion.lib mouseread.rel
sdar -rc  fusion.lib mousereadto.rel
sdar -rc  fusion.lib covoxplay.rel
sdar -rc  fusion.lib sc2circle.rel
sdar -rc  fusion.lib sc2circlefilled.rel
sdar -rc  fusion.lib vdplinesswitch.rel
sdar -rc  fusion.lib rlewbtovram.rel
sdar -rc  fusion.lib rlewbtoram.rel
sdar -rc  fusion.lib setdisktradress.rel
sdar -rc  fusion.lib getdisktradress.rel
sdar -rc  fusion.lib sectorread.rel
sdar -rc  fusion.lib sectorwrite.rel
sdar -rc  fusion.lib getdiskparam.rel
sdar -rc  fusion.lib vram.rel
sdar -rc  fusion.lib interrupt.rel
sdar -rc  fusion.lib strreverse.rel
sdar -rc  fusion.lib itoa.rel
sdar -rc  fusion.lib vdp_graph2plus.rel
sdar -rc  fusion.lib printhex.rel


echo "... cleaning folder"
del  .\*.rel
del .\*.asm
del .\*.lst
del .\*.sym


copy fusion.lib ..\..\lib\

echo " ... Have a good day sir"