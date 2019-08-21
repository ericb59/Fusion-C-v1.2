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
/*    sprite16
|     Activate  Sprite data size 16x16 pixels
|     2006/11/25    t.hara  
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"

#define RG08SAV  0xFFE7

void SpriteOn(void)
{
	unsigned char val;
	val=Peek(RG08SAV) & 0b11111101;
	VDPwriteNi(0x08,val);  /// Write to VDP Register 8
	Poke(RG08SAV,val);
}