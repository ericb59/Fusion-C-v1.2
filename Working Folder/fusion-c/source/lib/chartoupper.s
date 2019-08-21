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
;	CharToUpper
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE



;--- proc  
;
;	void	CharToUpper( char *adr )
;
_CharToUpper::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
lb_sl3:
	ld	a,(hl)
	or	a
	jr	z,lb_sl4
	call	_to_upper
	ld	(hl),a
	inc	hl
	jr	lb_sl3
lb_sl4:
	ret

_to_upper:
	cp	#'a'
	ret	c
	cp	#'{'
	ret	nc
	sub	#' '
	ret
	
;--- end of proc 