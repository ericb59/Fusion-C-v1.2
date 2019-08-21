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
;	strlefttrim
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRLEFTRIM
;
;	void	StrLeftTrim( char *adr )
;
_StrLeftTrim::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	
	ld	a,(hl)
	or	a
	ret	z
	cp	#' '
	ret	nz
	push	hl
	pop	de
lb_l0:
	ld	a,(hl)
	or	a
	jr	z, lb_l1
	cp	#' '
	jr	nz, lb_l1
	inc	hl
	jr	lb_l0
lb_l1:
	ld	a,(hl)
	ldi
	or	a
	jr	nz, lb_l1
	ret
	
;--- end of proc