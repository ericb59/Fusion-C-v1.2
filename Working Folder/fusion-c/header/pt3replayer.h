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
|   pt3replayer.h : Specific Turbo-R Functions              |
|    it Use standard Bios routines                          |
|                                                           |
|                                                           |
\___________________________________________________________/
*                         (°□°)
*	Ported to SDCC by Bitvision Software 2018 (Fernando Garcia)	
*	
*/

#ifndef __PT3_REPLAYER_H__
#define __PT3_REPLAYER_H__


extern void InstallReplayer( void );
extern void PT3Init ( unsigned char *songAddress , unsigned char loop);	//loop==1=>no loop, ==0 loop
extern void PT3Play ( void );
extern void PT3Mute ( void );
extern void PT3Rout ( void );
extern void UninstallReplayer( void );


#endif
