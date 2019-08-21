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
;	intdos & intbios
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE

;--- proc 	INTDOS
;
;	void IntDos()
;
_IntDos::
	ld	hl,#ibcladr
	ld	(hl),#5
_icallr:
	di
	push iy
	push ix
	ld	ix,#_wordregs
	ld	a,0(ix)	;AF
	ld	c,2(ix)	;BC
	ld	b,3(ix)
	ld	e,4(ix)	;DE
	ld	d,5(ix)
	ld	l,6(ix)	;HL
	ld	h,7(ix)
	push	hl
	ld	l,8(ix)	;IX
	ld	h,9(ix)
	push	hl
	ld	l,10(ix);IY
	ld	h,11(ix)
	push	hl
	pop	iy
	pop	ix
	
			; Clear
	push	bc
	push	af
	xor	a
	ld	b,#16
	ld	hl,#_wordregs
lb_idsLp:
	ld	(hl),a
	inc	hl
	djnz	lb_idsLp
	pop	af
	pop	bc

	pop	hl

	.db	#0xCD	; CALL
ibcladr:
	.dw	#0	; 5 (DOS) or 1Ch (BIOS)

	push	ix
	ld	ix,#_wordregs
	ld	0(ix),a		; AF
	ld	2(ix),c		; BC
	ld	3(ix),b
	ld	4(ix),e		; DE
	ld	5(ix),d
	ld	6(ix),l		; HL
	ld	7(ix),h
	
	pop	hl		; HL=IX
	ld	8(ix),l		; IX
	ld	9(ix),h

	push	iy
	pop	hl
	ld	10(ix),l	; IY
	ld	11(ix),h
	
	jr	nc, lb_idnC
	ld	b,#1		; CY=1
	jr	lb_idCs
lb_idnC:
	ld	b,#0		; CY=0
lb_idCs:	
	ld	14(ix),b

	jr	nz, lb_idnZ
	ld	b,#1		; Z=1
	jr	lb_idZs
lb_idnZ:
	ld	b,#0		; Z=0
lb_idZs:
	ld	12(ix),b

	pop	ix
	pop	iy
	ei
	ret

__REGs::
	ld	hl,#_wordregs
	ret

;--- proc 	INTBIOS
;
;	void IntBios()
;
_IntBios::
	ld	hl,#ibcladr
	ld	(hl),#0x1C
	jp	_icallr

	.area _DATA

_wordregs:	.dw #0,#0,#0,#0,#0,#0,#0,#0
	
	.area _CODE
	
;--- end of proc 
