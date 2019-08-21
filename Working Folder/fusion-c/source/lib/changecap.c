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
/*  changecap
|    
|     2018 Eric Boez
*/

// Change Capital Led of the keyboard On/off
void ChangeCap(char n) __naked
{
     __asm
        push ix
       
        ld   ix,#0
        add  ix,sp
        ld   a,4(ix)
        push iy
        ld   ix,#0x0132       ; Bios  routine
        ld   iy,(0xFCC0)      ; mainrom slotaddress
        call 0x001c           ; interslotcall
        ei
        pop IY
        pop IX
        ret 
        __endasm;
}
