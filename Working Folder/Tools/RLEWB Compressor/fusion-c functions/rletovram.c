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
/*  RleToVram
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
  ld ix,#0
  add ix,sp
      
  ld  L,4(ix) ; Data RAM address
  ld  H,5(ix)

  call unRLEWBVRAM
  pop IX
  ret

unRLEWBVRAM::
  
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
  ld   B,A            
  inc  HL
  ld   A,(HL)         ;get value
  
doRLE:
  out  (#0x98),A      ;write in VRAM
  nop
  nop
  djnz doRLE
  
  inc  HL
  jr   ANALYZE

WriteCONTROL:
  ld   A,#RLECONTROL  ;write CONTROL value
  
WriteByte:
  out  (#0x98),A      ;write in VRAM
  inc  HL
  jr   ANALYZE

__endasm;

}

void RleToVram(unsigned int *RamAddress, unsigned int VramAddress) 
{
 DisableInterupt();
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
  EnableInterupt();
}
