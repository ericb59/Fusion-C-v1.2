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
;
;	printdec 
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE



;--- proc   PrintDec
;
;   void   printDec(int num)
;
;--- proc   PUTDEC
;
;   void   putdec(int num)
;

_PrintDec::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)  
	pop ix
_iputdec:
    
	bit #7,h
	jr  z,lb_1
	call    lb_neghl
	ld  a,#'-'
	call    _iputch
lb_1:
	ld  de,#0
	ld  c,e
	ld  b,#16
lb_2:
	add hl,hl
	ld  a,e
	adc a,a
	daa
	ld  e,a
	ld  a,d
	adc a,a
	daa
	ld  d,a
	rl  c
	djnz    lb_2
	ld  hl, #lb_mystr+2
	call    lb_fhexw
	ld  (hl),#0
	ld  hl, #lb_mystr
	push hl
	ld  e,c
	call    lb_fhexb
	pop hl

	ld b, #5
lb_omL:
	ld a,(hl)
	cp #'0'
	jr nz, lb_omEx
	inc hl
	djnz lb_omL
lb_omEx:
;	pop bc
;	pop de
	jp	_icputs

lb_fhexw:
	push    de
	ld  e,d
	call    lb_fhexb
	pop de
lb_fhexb:
	ld  a,e
	push    af
	rrca
	rrca
	rrca
	rrca
	call    lb_3
	pop af
lb_3:
	and #0xF
	add a,#'0'
	cp  #0x3A
	jr  c,lb_4
	add a,#7
lb_4:
	ld  (hl),a
	inc hl
	ret
    
lb_neghl:
    
	dec hl
	ld  a,l
	cpl
	ld  l,a
	ld  a,h
	cpl
	ld  h,a
	ret

_iputch:

	cp  #10
	jr   nz,lb_ptch1


	ld  a,#13
	call	lb_ptch1

	ld  a,#10

lb_ptch1:

;
; BIOS call (not DOS)
;	
;	CALLF (RST 30h)
;	DB Destination slot.
;       ExxxSSPP
;       |     || Primary  slotnumber  (00-11)
;       |     - Secundary slotnumber (00-11)
;       +----------- Expanded slot (0 = no, 1 = yes (0x80))
;
;	DW  Destination address.  0x00A2 = CHPUT
;
	push ix
	.db #0xF7,#0x80,#0xA2,#0
	pop ix
	
	ret

	_icputs:

	ld  a,(hl)
	or  a
	ret z
	call    _iputch
	inc hl
	jr  _icputs
;
	.area _DATA
    
lb_mystr:  .db #0,#0,#0,#0,#0,#0,#0;