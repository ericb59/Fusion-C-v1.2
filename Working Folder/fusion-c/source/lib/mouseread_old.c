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
|                   V1.1 -   02-04  2019                    |
|                                                           |
|                Eric Boez &  Fernando Garcia               |
|                                                           |
|          C   &   A S M    S O U R C E   C O D E           |
|                                                           |
|                                                           |
\___________________________________________________________/
*/
// Done with the help of GDX
//
// Mouse control function
// 
// Return the x and y offset of mouse in a 16 bits value.
// Return 0xFFFF if no mouse detected
// Mouse port codes are 
// Port 1 : 0x9310
// Port 2 : 0xEC20
//

unsigned int MouseRead(unsigned int MousePort) __naked
{
MousePort;
       __asm
        ; Routine to read the mouse by direct accesses (works on MSX1/2/2+/turbo R)
        ;
        ; Input: DE = 09310h for mouse in port 1 (D = 10010011b, E = 00010000b)
        ;        DE = 0EC20h for mouse in port 2 (D = 11101100b, E = 00100000b)
        ; Output: H = X-offset, L = Y-offset (H = L = 255 if no mouse)

        
        
        push ix
        ld ix,#0
        add ix,sp
        ld e,4(ix)
        ld d,5(ix)
        pop ix
        

        GTMOUS:
            ld  b,#30       ; Long delay for first read
            call    GTOFS2  ; Read bits 7-4 of the x-offset
            and #0xF
            rlca
            rlca
            rlca
            rlca
            ld  c,a
            call    GTOFST  ; Read bits 3-0 of the x-offset
            and #0xF
            or  c
            ld  h,a ; Store combined x-offset
            call    GTOFST  ; Read bits 7-4 of the y-offset
            and #0xF
            rlca
            rlca
            rlca
            rlca
            ld  c,a
            call    GTOFST  ; Read bits 3-0 of the y-offset
            and #0xF
            or c
            ld l,a          ; Store combined y-offset

            ret
            
        GTOFST: ld b,#10
        GTOFS2: ld a,#15     ; Read PSG register 15 for mouse
            di               ; DI useless if the routine is used during an interrupt
            out (#0xA0),a
            ld a,d
            out (#0xA1),a
            xor e
            ld d,a
            
            call WAITMS        ; Extra delay because the mouse is slow
            
            ld a,#14
            out (#0xA0),a
            ei                 ; EI useless if the routine is used during an interrupt
            in a,(#0xA2)
            ret
        WAITMS:
            ld  a,b
        WTTR:
            djnz    WTTR
            .db  #0xED,#0x55   ; Back if Z80 (RETN on Z80, NOP on R800)
            rlca
            rlca
            ;add a,b
            ld  b,a
        WTTR2:
            djnz    WTTR2
            ld  b,a 
        WTTR3:
            djnz    WTTR3
            ret

__endasm;

}
