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
;	strposchr
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  strposchr
;
;	int	StrPosChr( char *adr, char c )
;
_StrPosChr::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	pop ix
	
	ld	de, #-1
	ld	bc, #0
lb_cA:
	ld	a,(hl)
	cp	6(ix)		; compare with char
	jr	nz, lb_cB
	ld	d,b
	ld	e,c
lb_cB:
	inc	hl
	inc	bc
	or	a
	jr	nz, lb_cA
	ex	de, hl
	ret
	
;--- end of proc 