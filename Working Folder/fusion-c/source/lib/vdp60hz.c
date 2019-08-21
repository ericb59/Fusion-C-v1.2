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
|                   V1.1 -   02-04  2019                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|                C   S O U R C E   C O D E                  |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/ 
/*    VDP60Hz
|     Switch MSX2 VDP to 60Hz
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"

#define RG09SAV  0xFFE8      /* Register 9 Saved parameters */

void VDP60Hz(void)
{
	unsigned char val;
	val=Peek(RG09SAV) & 0b11111101;
	VDPwriteNi(0x09,val);  /// Write to VDP Register 1
	Poke(RG09SAV,val);
}
