; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.0 - 09-10-11 2018                    |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;---------------------------------------------------------
;    set and restore screen 5 palette
;	Low level memory functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;
;----------------------------------------------------------

	.area _CODE

	
;-----------------------------------------------
;	Sets palette, provide table pointer
;
_SetSC5Palette::
        ; Sets colors by given RGB-table in HL-reg
	
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	
_isetpalette:	
	ld	b,#0x10		; 16 colours
SPcoLoop:
	di
	ld	a,(hl)
	inc	hl
	out	(#0x99),a	; colour Nr.
	ld	a, #128+#16
	out	(#0x99),a       
	ld	a,(hl)		; red
	inc	hl
	inc	hl
	sla	a
	sla	a
	sla	a
	sla	a		; bits 4-7
	ld	c,a
	ld	a,(hl)		; blue bits 0-3
	dec	hl
	or	c
	out	(#0x9A),a
	ld	a,(hl)		; green bits 0-3
	inc	hl
	inc	hl
	out	(#0x9A),a
	ei 
	djnz	SPcoLoop
	ret
	
;-----------------------------------------------
;	Sets palette, provide table pointer
;
_RestoreSC5Palette::
	ld	hl, #__msx_palette
	jr	_isetpalette

	;.area	_XDATA
	
;---------------------------------------------------
;        colour  R  G  B   bright 0..7   Name
;---------------------------------------------------
__msx_palette:
	.db #0,  #0,#0,#0		;transparent
	.db #1,  #0,#0,#0		;black
	.db #2,  #1,#6,#1		;bright green
	.db #3,  #3,#7,#3		;light green
	.db #4,  #1,#1,#7		;deep blue
	.db #5,  #2,#3,#7		;bright blue
	.db #6,  #5,#1,#1		;deep red
	.db #7,  #2,#6,#7		;light blue
	.db #8,  #7,#1,#1		;bright red
	.db #9,  #7,#3,#3		;light red
	.db #10, #6,#6,#1		;bright yellow
	.db #11, #6,#6,#3		;pale yellow
	.db #12, #1,#4,#1		;deep green
	.db #13, #6,#2,#5		;purple
	.db #14, #5,#5,#5		;grey
	.db #15, #7,#7,#7		;white
