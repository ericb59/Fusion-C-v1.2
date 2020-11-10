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
/*    spriteOff
|     2018 Eric Boez Updated November 2020
*/

void SpriteOff(void) __naked
{
	__asm

	RG1SAV = 0xF3E0 
	DPPAGE = 0xFAF5 
	ATRBAS = 0xF928 
	RG25SAV = 0xFFFA
	RG2SAV = 0xF3E1
	RG8SAV = 0xFFE7
	RG9SAV = 0xFFE8 
	
  push ix
  ld a,(RG8SAV)
  or #0b00000010
  ld (RG8SAV),a
  ld b,a
  ld c,#8
  ld ix,#0x0047   ; Write VDP Bios
  ld iy, (0xFCC0)	; mainrom slotaddress 
  call 0x001c		; interslotcall
  ei
  pop ix
  ret
	__endasm;
}
