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
/*    VDPLinesSwitch
|     Switch MSX2 VDP to 192 or 212 vertical lines
|     2019 Eric Boez
*/
#include "../../header/msx_fusion.h"

#define RG09SAV  0xFFE8      /* Register 9 Saved parameters */

void VDPLinesSwitch(void)    
{
	unsigned char val;

	if ((Peek(RG09SAV) >> 7) & 0x01 )
	{
		val=Peek(RG09SAV) & 0b0111111;
	} 
	else
	{
		val=Peek(RG09SAV) | 0b10000000;
	}
	VDPwriteNi(0x09,val);  /// Write to VDP Register 
	Poke(RG09SAV,val);
}
