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
|                   V1.2 08 2019                            |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|         compilation : > sdcc -mz80 -c  msx_misc.c         |
|                                                           |
\___________________________________________________________/
*/
/*  CovoxPlay
|    
|     2019 Eric Boez
|
|  Play Wav Sample stored in VRAM to Covox Module
*/
#include "../../header/msx_fusion.h"

void Covox(unsigned int length, char speed)
{
 length;
 speed;   
// Covox Play ASM routine
__asm

        push ix
        ld ix,#0
        add ix,sp
        ld e,4(ix)                  ; lenght value  Byte high
        ld d,5(ix)                  ; lenght value Byte low
        ld c,6(ix)                  ; wait value
      
    Loop:
        in      a, (#0x98)          ; read Vram adress, and increment for next read

        out     (#0x91), a          ; send data to covox 

        ld b,c
        wait:                       ; Wait loop 
            djnz wait

        dec de
        ld a,d
        or e
        jp nz,Loop                  ; Loop  'lenght' times

    pop ix                          ; end 
        
__endasm;

}

void CovoxPlay(char page, unsigned int start_adress, unsigned int length, char speed)
{
   
    // Prepare VDP for reading data from Start_adress, in current screen mode and  defined vram page
    DisableInterrupt();
    if( Peek( 0xFCAF ) >= 7 ) {
        VDPwriteNi( 14, (start_adress >> 14) | (page << 2) );
    }
    else {
        VDPwriteNi( 14, (start_adress >> 14) | (page << 1) );
    }
    OutPort( 0x99, (unsigned char)start_adress );
    OutPort( 0x99, (unsigned char)(start_adress >> 8) & 0x3F );
    
    // Send data to covox port #91 (Printer port)
    Covox(length, speed);
             
    EnableInterrupt();
}
