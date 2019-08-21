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
;	setdate 
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE

;--- proc 	SetDate
;
;	int	SetDate(DATE *date)
;
_SetDate::
	push	ix
	ld ix,#0
	add ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	ld	d,6(ix)
	ld	e,7(ix)
	ld	c,#0x2B
	call	#5
	pop	ix
	ld	l,a
	ld	h,#0
	ret

;--- end of proc 
