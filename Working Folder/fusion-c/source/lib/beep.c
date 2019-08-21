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
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  beep
|    
|     2018 Eric Boez
*/

/* =============================================================================
 Beep()
 
 Description: 
         Emit a beep sound
 Input:   -
 Output:  -
============================================================================= */
void Beep(void)
{
 __asm

  push IX
  push IY

  ld iy, (0xFCC0)	; mainrom slotaddress (reference)
  ld ix, # 0x00C0	; Beep

  call 0x001c		; interslotcall
  ei
  pop IY 			; restore register
  pop IX

__endasm;
}
