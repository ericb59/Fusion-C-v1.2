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
//
// Mouse control and button test function - return values to Structure
// 
// MousePort must be 1 or 2

#include "../../header/msx_fusion.h"

void MouseReadTo(unsigned char MousePort, MOUSE_DATA *md) __naked
{
MousePort;
md;
__asm
        ; Routine to read the mouse by direct accesses (works on MSX1/2/2+/turbo R)
        ;
        ; Input: MousePort = 1 or 2
        ;        md (MOUSE_DATA) including dx, dy, left button (1=OFF 0=ON), right button (1=OFF 0=ON)
        
        push ix
        ld ix,#0
        add ix,sp

        ld a,4(ix)
        ld l,5(ix)
        ld h,6(ix)

		ld de,#0x1310 ; DE = 01310h for mouse in port 1 (D = 00010011b, E = 00010000b)
		and #02
		jr z,GTMOUS
		ld de,#0x6C20 ; DE = 06C20h for mouse in port 2 (D = 01101100b, E = 00100000b)

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
			ld (hl),a
			inc hl
            call    GTOFST  ; Read bits 7-4 of the y-offset
            and #0x0F
            rlca
            rlca
            rlca
            rlca
            ld  c,a
            call    GTOFST  ; Read bits 3-0 of the y-offset
            ; can read mouse button bit4=left button / bit5=right button
            ld  b,a
            and #0x0F
            or c
			ld (hl),a
			ld a,b
			and #0x10
            rlca
            rlca
            rlca
            rlca
			inc hl
			ld (hl),a
			ld a,b
			and #0x20
            rlca
            rlca
            rlca
            rlca
            rlca
			inc hl
			ld (hl),a

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
