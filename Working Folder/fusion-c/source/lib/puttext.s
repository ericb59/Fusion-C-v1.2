; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.2 - 08 2019                          |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;---------------------------------------------------------
;    PutText
;	Low level memory functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;
;----------------------------------------------------------

	.area _CODE
;-----------------------------------------------	
;VRAM Data (Read/Write) port 0x98
;VDP Status Registers port 0x99

GRPACX		.equ	#0xFCB7
GRPACY		.equ	#0xFCB9 
LOGOPR		.equ	#0xFB02

; PutText( int X, int Y,  char *str, char LogOp );

;-----------------------------------------------
;	Puts string on graphics screen at position 
; 		(ignores \n\l\t and other text features)
_PutText::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld (GRPACX),hl ; X
	ld l,6(ix)
	ld h,7(ix)
	ld (GRPACY),hl ; Y
	ld a,10(ix)
	ld (LOGOPR),a  ; LogOp
	ld l,8(ix)
	ld h,9(ix)

	push iy
lb_ptlp:	
	ld   a,(hl)
	or   a
	jr   z, lb_ptEx

	ld   iy,(0xFCC0)		; mainrom slotaddress (reference)
	ld   ix,#0x008D		; bios (api) address
	call 0x001c	; interslotcall
	ei
	
	inc	hl
	jr	lb_ptlp
lb_ptEx:
	pop iy
	pop ix
	ret
