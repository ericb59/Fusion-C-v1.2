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
/* Setchannel =============================================================================
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
 SetChannel

 Function : Enable/disable Tone and Noise channels.
 Input    : [char] channel (0, 1 or 2)
            [boolean] tone state
            [boolean] noise state
 Output   : -
============================================================================= */
void SetChannel(char channel, boolean isTone, boolean isNoise)
{
channel;isTone;isNoise;
__asm
  push IX
  ld   IX,#0
  add  IX,SP

  ld   A,#7
  out  (#AY0index),A
  in   A,(#AY0read)   ;read register 7
  ld   B,A
      
  ld   A,4(IX) ;channel
  or   A
  jr   Z,MIX_A
  CP   #1
  jr   Z,MIX_B


MIX_C:
  ld   A,5(IX) ;isTone
  or   A
  call  Z,TONE_C_OFF 
  call NZ,TONE_C_ON 
  
  ld   A,6(IX) ;isNoise
  or   A
  call  Z,NOISE_C_OFF
  call NZ,NOISE_C_ON
  
  jr   SETMIX
  
  
MIX_A:
  ld   A,5(IX) ;isTone
  or   A
  call  Z,TONE_A_OFF 
  call NZ,TONE_A_ON 
  
  ld   A,6(IX) ;isNoise
  or   A
  call  Z,NOISE_A_OFF
  call NZ,NOISE_A_ON
  
  jr   SETMIX
  
MIX_B:  
  ld   A,5(IX) ;isTone
  or   A
  call  Z,TONE_B_OFF 
  call NZ,TONE_B_ON 
  
  ld   A,6(IX) ;isNoise
  or   A
  call  Z,NOISE_B_OFF
  call NZ,NOISE_B_ON 
   
                      
SETMIX:
  ld  A,#7
  out  (#AY0index),A
  ld   A,B
  out  (#AY0write),A
  
  pop  IX  
  ret


TONE_A_ON:
  res  0,B
  ret
TONE_A_OFF:
  set  0,B
  ret

  
TONE_B_ON:
  res  1,B
  ret
TONE_B_OFF:
  set  1,B
  ret

  
TONE_C_ON:
  res  2,B
  ret
TONE_C_OFF:
  set  2,B
  ret  
  
  
NOISE_A_ON:
  res  3,B
  ret
NOISE_A_OFF:
  set  3,B
  ret

  
NOISE_B_ON:
  res  4,B
  ret
NOISE_B_OFF:
  set  4,B
  ret

  
NOISE_C_ON:
  res  5,B
  ret
NOISE_C_OFF:
  set  5,B
  
__endasm;  
}

