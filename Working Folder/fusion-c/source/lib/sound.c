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
|          C   &   A S M    S O U R C E   C O D E           |
|                                                           |
|                                                           |
\___________________________________________________________/
*/
/* Sound =============================================================================
   SDCC AY-3-8910 PSG Functions Library (object type)
   Version: 1.0
   Date: 16/06/2018
   Author: mvac7/303bcn
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C
   WEB: 
   mail: mvac7303b@gmail.com

   Description:                                                           

============================================================================= */
#include "../../header/psg.h"

//intern MSX AY
#define AY0index 0xA0
#define AY0write 0xA1
#define AY0read  0xA2

//AY from MEGAFLASHROM-SCC+
//#define AY1index 0x10
//#define AY1write 0x11
//#define AY1read  0x12


/* =============================================================================
 SOUND

 Function : Write into a register of PSG
 Input    : [char] register number (0 to 13)
            [char] value
 Output   : -
============================================================================= */
void Sound(char reg, char value)
{
reg;value;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   C,4(IX) ;reg
  ld   B,5(IX) ;value
 
  ld   A,C
  cp   #7   ;IF reg=7
  jr   NZ,writeAY      ;NO 

  ;the last two bits are for joystick port control 
  ;they do not have to be modified  
  ld   A,B
  AND  #0b00111111   ;erase the last two bits of the value
  ld   B,A
      
  ;YES
  ld   A,#7
  out  (#AY0index),A
  in   A,(#AY0read)  ;read register 7
  and	 #0b11000000	 ;Mask to catch two bits for the joysticks. 
	or	 B		        
  ld   B,A

writeAY:
  ld   A,C    
  out  (#AY0index),A
  ld   A,B
  out  (#AY0write),A

  pop  IX
__endasm;  
}


