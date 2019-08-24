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
|                                                           |
|                        TURBO-R                            |
\___________________________________________________________/
*/
/* pcmplay */


/*  For MSX Turbo-R Only. Play PCM from VRAM adress */
void PCMPlay(int start, int length)  __naked
{
start;
length;
        __asm
        push ix
       
        ld   ix,#0
        add  ix,sp
        ld   l,4(ix)
        ld   h,5(ix)   
        ld   c,6(ix)
        ld   b,7(ix)

        ld   a,#0x80           ; Input parameter: 0b100000000

        push iy
        ld   ix,#0x0186       ; Bios PCMPLAY Routine
        ld   iy,(0xFCC0)      ; mainrom slotaddress
        call 0x001c           ; interslotcall
        ei

        pop  iy
        pop  ix
        ret
        __endasm;
}
