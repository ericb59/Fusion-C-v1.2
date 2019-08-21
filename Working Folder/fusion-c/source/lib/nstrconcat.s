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
;	nstrconcat 
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRNCAT
;
;	void	NStrConcat( char *dst, char *src, int n )
;
_NStrConcat::

	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	pop ix
	push bc
	ex	de,hl	
	xor	a
	ld	bc,#0
	cpir
	ex	de,hl
	dec	de
	pop bc
	
	ld	a,b
	or	c
	jr	z,lb_c5
	
lb_c2:	ld	a,(hl)
	ldi
	jp	pe,lb_c4
	jr	lb_c5
lb_c4:
	or	a
	jr	nz,lb_c2
lb_c5:
	xor	a
	ld	(de),a
	ret
	
;--- end of proc 