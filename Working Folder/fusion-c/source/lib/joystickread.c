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
|                C   S O U R C E   C O D E                  |
|                                                           |
|                        JOYSTICK                           |
\___________________________________________________________/
*/
/*  
;------------------------------------------------
;       Joystickread
;       
;       Eric Boez 2019
;
;-----------------------------------------------*/

unsigned char JoystickRead(char joyNumber)  __naked
{
joyNumber;
  __asm
  push ix
  ld   ix,#0
  add  ix,sp
  ld   a,4(ix)
  push iy
  ld   ix,#0x00d5       ; Bios Joystick Read routine
  ld   iy,(0xFCC0)      ; mainrom slotaddress
  call 0x001c           ; interslotcall
  ei
  ld   l,a
  pop  iy
  pop  ix
  ret
  __endasm;
}
