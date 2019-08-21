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
;	nstrcompare 
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRNCMP
;
;	int	NStrCompare( char *s1, char *s2, int n )
;

_NStrCompare::
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
lb_n1:
	ld	a,(de)
	sub	(hl)
        jr	nz,lb_n3
	ld	a,(de)
	or	a
	jr	z,lb_n2
	inc	de
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,lb_n1
lb_n2:
	ld	hl,#0
	ret
lb_n3:
	ld	hl,#1
	ret	nc
	ld	hl,#-1
	ret 
	
;--- end of proc  