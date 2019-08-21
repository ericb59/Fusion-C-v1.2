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
/*  showdisplay
|    
|     2018 Eric Boez
*/
// Show Video Display
void ShowDisplay(void)
{
        __asm
        push ix
        push iy
        ld   ix,#0x0044       ; Bios show display routine
        ld   iy,(0xFCC0)      ; mainrom slotaddress
        call 0x001c           ; interslotcall
        ei
        pop IY
        pop IX
        ret 
        __endasm;
}
