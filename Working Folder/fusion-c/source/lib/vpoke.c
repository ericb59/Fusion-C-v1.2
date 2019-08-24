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
/*    vpoke
|     2006/11/25    t.hara  
|     2018 Eric Boez
*/
#include "../../header/msx_fusion.h"

#define SCRMOD  0xFCAF      /* current screen mode */
#define ACPAGE  0xFAF6      /* active page number */
#define DPPAGE  0xFAF5      /* display page number */
#define ATRBAS  0xF928      /* sprite attribute table base address */
#define PATBAS  0xF926      /* sprite pattern generator table base address */


/* --------------------------------------------------------- */
/*  VRAM access                                              */
void VpokeFirst( unsigned int address ) {

    if( Peek( 0xFCAF ) >= 7 ) {
        VDPwriteNi( 14, (address >> 14) | (Peek( ACPAGE ) << 2) );
    }
    else {
        VDPwriteNi( 14, (address >> 14) | (Peek( ACPAGE ) << 1) );
    }
    OutPort( 0x99, (unsigned char)address );
    OutPort( 0x99, ((unsigned char)(address >> 8) & 0x7F) | 0x40 );
}



/* --------------------------------------------------------- */
void Vpoke( unsigned int address, unsigned char data ) {

    DisableInterrupt();
    VpokeFirst( address );
    OutPort( 0x98, data );
    EnableInterrupt();
}
