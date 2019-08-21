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
;---------------------------------------------------------
;    Memcopyreverse
;	Low level memory functions
;
;	Fernando Garcia
;
;
;
;----------------------------------------------------------

	.area _CODE

_MemCopyReverse::
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix) ;dst
	ld l,6(ix)
	ld h,7(ix) ;src
	ld c,8(ix)
	ld b,9(ix) ;counter
	pop ix
_imovmem2:
	
	ld	a,b
	or	c
	ret	z
	lddr
	ret

	