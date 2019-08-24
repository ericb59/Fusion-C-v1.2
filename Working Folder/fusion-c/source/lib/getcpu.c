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
|                        TURBO-R                            |
\___________________________________________________________/
*/
/* GetCPU */

/*  For MSX Turbo-R Only. Return CPU mode */
char GetCPU (void)  __naked
{
  // Return 0: Z80 mode
  //        1: R800 Rom Mode
  //        2: R800 DRAM Mode
  __asm
  push ix
  ld   ix,#0
  add  ix,sp
  ld   a,4(ix)
  push iy
  ld   ix,#0x0183       ; Bios CPU GET routine
  ld   iy,(0xFCC0)      ; mainrom slotaddress
  call 0x001c           ; interslotcall
  ei
  ld   l,a
  pop  iy
  pop  ix
  ret
  __endasm;
}
