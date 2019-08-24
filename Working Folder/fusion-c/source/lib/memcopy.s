; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.2 - 08 2019                          |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;---------------------------------------------------------
;    Memcopy
;	Low level memory functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;
;----------------------------------------------------------

	.area _CODE


;--- proc	MEMCPY
;
;	void	MemCopy( char *dst, char *src, int n )
;

_MemCopy::
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
_imovmem:
	
	ld	a,b
	or	c
	ret	z
	ldir
	ret

;--- end of proc 

