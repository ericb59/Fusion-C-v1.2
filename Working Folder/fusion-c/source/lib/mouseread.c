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
// Port 1 : 0x1310
// Port 2 : 0x6C20
//

unsigned int MouseRead(unsigned int MousePort) __naked
{
MousePort;
__asm
        ; Routine to read the mouse by direct accesses (works on MSX1/2/2+/turbo R)
        ;
        ; Input: DE = 01310h for mouse in port 1 (D = 00010011b, E = 00010000b)
        ;        DE = 06C20h for mouse in port 2 (D = 01101100b, E = 00100000b)
        ; Output: H = X-offset, L = Y-offset (H = L = 255 if no mouse)
        
        push ix
        ld ix,#0
        add ix,sp
        ld e,4(ix)
        ld d,5(ix)

        GTMOUS:
            di               ; DI useless if the routine is used during an interrupt
            ld  b,#30 ; Long delay for first read
            call    GTOFS2  ; Read bits 7-4 of the x-offset
            and #0x0F
            rlca
            rlca
            rlca
            rlca
            ld  c,a
            call    GTOFST  ; Read bits 3-0 of the x-offset
            and #0x0F
            or  c
            ld  h,a ; Store combined x-offset
            call    GTOFST  ; Read bits 7-4 of the y-offset
            and #0x0F
            rlca
            rlca
            rlca
            rlca
            ld  c,a
            call    GTOFST  ; Read bits 3-0 of the y-offset
            and #0x0F
            or c
            ld l,a          ; Store combined y-offset
            ei                 ; EI useless if the routine is used during an interrupt

            ld  b,#40
            call WAITMS

            pop ix
            ret

        GTOFST: ld b,#10
        GTOFS2: ld a,#15     ; Read PSG register 15 for mouse
            out (#0xA0),a

            in  a,(#0xA1)    ; preserve LED code/Kana state
            and #0x80
            or  d            ; mouse1 x0010011b / mouse2 x1101100b
;            ld a,d
            out (#0xA1),a    ; first pin8=1, at second pass pin8=0 and so on
            xor e            ; reverse pin 8 state
            ld d,a           ;
            
            call WAITMS        ; Wait before read : 100<>150 us for first read
                               ;                    50<>150 us for next reads
            
            ld a,#14
            out (#0xA0),a
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
