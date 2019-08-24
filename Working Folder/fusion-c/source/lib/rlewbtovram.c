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
|                   V1.2 - 08 2019                          |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  RleWBToVram
|    
|     2019 Eric Boez
|     ASM decompression code by Mvac7
|  Decompress to VRAM RLEWB Data stored in RAM
*/
#include "../../header/msx_fusion.h"

void RleVramDecompress(unsigned char *RamAddress) __naked
{
  RamAddress;

__asm
  
  RLECONTROL = 0x80     ; Control code used in RLEWB compressed data 


_unRLEWBtoVRAM::
  push ix
  ld   ix,#0
  add  ix,sp
      
  ld   l,4(ix) ; Data RAM address
  ld   h,5(ix)

  call unRLEWBVRAM
  pop  ix
  ret

unRLEWBVRAM::
  
ANALYZE:
  ld   a,(hl)         ; get byte
  cp   #RLECONTROL                      
  jr   nz,WriteByte   ; if raw
  
  inc  hl             ; get next byte 
  ld   a,(hl)
  or   a
  jr   z,WriteCONTROL ;if A=0 then write one $80  ($80 $0)
  cp   #0xFF          ;if A=$FF ($80 $FF) 
  ret  z              ;THEN exit
  
                      ;$80 nn dd
  ld   b,a            
  inc  hl
  ld   a,(hl)         ;get value
  
doRLE:
  out  (#0x98),A      ;write in VRAM
  nop
  nop
  djnz doRLE
  
  inc  hl
  jr   ANALYZE

WriteCONTROL:
  ld   a,#RLECONTROL  ;write CONTROL value
  
WriteByte:
  out  (#0x98),a      ;write in VRAM
  inc  hl
  jr   ANALYZE

__endasm;

}

void RleWBToVram(unsigned int *RamAddress, unsigned int VramAddress) 
{
 DisableInterrupt();
 if(Peek( 0xFCAF ) >= 7 ) {
       VDPwriteNi( 14, (VramAddress >> 14) | (Peek( 0xFAF6 ) << 2) );
  }
  else 
  {
      VDPwriteNi( 14, (VramAddress >> 14) | (Peek( 0xFAF6 ) << 1) );
  }
  OutPort( 0x99, (unsigned char)VramAddress );
  OutPort( 0x99, ((unsigned char)(VramAddress >> 8) & 0x7F) | 0x40 );

  RleVramDecompress(RamAddress);
  EnableInterrupt();
}
