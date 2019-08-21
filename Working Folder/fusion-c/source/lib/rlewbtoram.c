/*
 ___________________________________________________________
/               __           _                              \
|              / _|         (_)                             |
|             | |_ _   _ ___ _  ___  _ __                   |
|             |  _| | | / __| |/ _ \| '_ \                  |
|             | | | |_| \__ \ | (_) | | | |                 |
|             |_|  \__,_|___/_|\___/|_| |_| *               |
|                                                           |
|               The MSX C Library for SDCC                  |
|                   V1.1 - 02-04 - 2019                     |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  RleWBToRam
|    
|     2019 Eric Boez
|     ASM decompression code by Mvac7
|  Decompress to RAM RLEWB Data stored in RAM
*/


void RleWBToRam (unsigned int *RamSource, unsigned int *RamDest) __naked
{
RamSource;
RamDest;

__asm

RLECONTROL = 0x80

_unRLEWBtoRAM::
  push ix
  ld ix,#0
  add ix,sp
      
  ld  L,4(ix) ; Data RAM Source address
  ld  H,5(ix)
    
  ld E,6(ix) ; Data RAM Destination address
  ld D,7(ix)
   
  call unRLEWBRAM
    
  pop IX
  ret


;===========================================================================
; unRLEWBtoVR
; Function : RLE type 3 to VRAM unpacker
;     
; Input    : HL - source RAM RLE data address
;            DE - target RAM address            
;=========================================================================== 
unRLEWBRAM::

  
ANALYZE:
  ld   A,(HL)         ; get byte
  cp   #RLECONTROL                      
  jr   NZ,WriteByte   ; if raw
  
  inc  HL             ; get next byte 
  ld   A,(HL)
  or   A
  jr   Z,WriteCONTROL ;if A=0 then write one $80  ($80 $0)
  cp   #0xFF          ;if A=$FF ($80 $FF) 
  ret  Z              ;THEN exit
  
  ;$80 nn dd
  ;inc  A              ;2 to 255
  ld   B,A            
  inc  HL
  ld   A,(HL)         ;get value
  
doRLE:
  ld   (DE),A     ;write in DEST RAM
  inc  DE
  djnz doRLE
  
  inc  HL
  jr   ANALYZE

WriteCONTROL:
  ld   A,#RLECONTROL  ;write CONTROL value
  
WriteByte:
  ld   (DE),A     ;write in DEST RAM
  inc  DE
  inc  HL
  jr   ANALYZE

__endasm;
}
