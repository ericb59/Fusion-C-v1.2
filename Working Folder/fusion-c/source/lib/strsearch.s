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
;	strsearch
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;
;	int	StrSearch( char *s1, char *s2 )
;

_StrSearch::
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld (lb_bHL), hl
	pop ix
	
	ld	a,(hl)
	or	a
	ld	hl, #0
	ret	z
lb_b1:
	ld	a,(de)
	or	a
	jr	z, lb_b2
	push	hl
	ld	hl, (lb_bHL)
	ld	c,a
	call	_inran
	pop	hl
	ret	nz
	inc	de
	inc	hl
	jr	lb_b1
lb_b2:
	ld	hl, #-1
	ret

lb_bHL:	.db #0,#0

_inran:
	ld	a,(hl)
	or	a
	ret	z
	cp	c
	inc	hl
	jr	nz,_inran
	or	#0xff
	ret

;--- end of proc