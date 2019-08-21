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
;------------------------------------------------
;	inputstring
;	Low level console functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;	Uses BIOS only. Very basis, small code.
;-----------------------------------------------
.area _CODE

;--- proc   InputString

_InputString::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	pop ix

	ld  (#lb_18),hl
	ld  hl,#0xFEFA	; buffer 0x106 in stack
	add hl,sp
	ld  sp,hl
	ld  hl,(#lb_18)
	push hl
	ld  (#lb_23),hl
	ld  a,e
	dec a
	dec a
	ld  hl,#0x2
	add hl,sp
	ld  (hl),a
	ld  hl,#0x2
	add hl,sp
	ex  de,hl
	ld  c,#10
	call    #5
	ld  e,#10
	ld  c,#2
	call    #5
	ld  hl,#0x4
	add hl,sp
	ld  a,(hl)
	pop hl
	cp  #0xA
	jr  nz,lb_5
	xor a
	ld (de),a
	ld  b,#0
	jr  lb_6

lb_5:
	push    hl
	ld  hl,#0x3
	add hl,sp
	ld  c,(hl)
	ld b,#0
	ld  hl,#0x4
	add hl,sp
	ex  de,hl
	pop hl
lb_8:
	dec c
	ld  a,c
	inc a
	jr  z,lb_7
	ld  a,(de)
	inc de
	inc b
	ld  (hl),a
	inc hl
	jr  lb_8
lb_7:
	xor a
	ld  (hl),a
	ld  hl,(#lb_23)
lb_6:
	ex  de,hl
	ld  hl,#0x106	;0x10000-0xFEFA
	add hl,sp
	ld  sp,hl
	ex  de,hl
	ld l,b
	ld h,#0
	ret

	;.area _DATA
	
lb_18:  .db #0,#0
lb_23:  .db #0,#0

	
;--- end of proc 



	
