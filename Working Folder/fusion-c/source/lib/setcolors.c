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
|                   V1.0 - 09-10-11 2018                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|                C   S O U R C E   C O D E                  |
|                                                           |
|                        TURBO-R                            |
\___________________________________________________________/
*/
/* Setcolors */


#include "../../header/msx_fusion.h"

#define FORCLR      0xF3E9       // foreground color 
#define BAKCLR      0xF3EA       // background color
#define BDRCLR      0xF3EB       // border color

void SetColors (unsigned char foreground, unsigned char background, unsigned char border )  
  {
        Poke(FORCLR,foreground);
        Poke(BAKCLR, background);
        Poke(BDRCLR, border);
      
__asm

  push IX
  push IY
;
  ld iy, (0xFCC0)     ; mainrom slotaddress (reference)
  ld ix, # 0x0062     ; bios (api) address
  call 0x001c         ; interslotcall
  ei

  pop IY              ; restore register
  pop IX

__endasm;

  }
