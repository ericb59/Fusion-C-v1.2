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
/*    spritecollision
|     Detect  Sprite collision
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"


char SpriteCollision(void)
{
	return((VDPstatus(0) >> 5)  & 0x01);
	
}
char SpriteCollisionX(void)
{
	return(VDPstatus(3));
	
}
char SpriteCollisionY(void)
{
	return(VDPstatus(5));
	
}