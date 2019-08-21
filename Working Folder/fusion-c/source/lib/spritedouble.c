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
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/ 
/*    spriteDouble
|     Activate Double Sprite size
|     2006/11/25    t.hara  
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"

#define RG1SAV  0xF3E0     /* System saves here the byte written to the register R#01, Used by VDP(1) in Basic */

void SpriteDouble(void)
{
	unsigned char val;
	val=Peek(RG1SAV) | 0b00000001;
	VDPwriteNi(0x01,val);  /// Write to VDP Register 1
	Poke(RG1SAV,val);
}
