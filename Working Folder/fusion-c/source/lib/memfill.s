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
;    Memfill
;	Low level memory functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;
;----------------------------------------------------------

	.area _CODE


;--- proc 	MEMFILL
;
;	void	MemFill( char *adr, char c, int n )
;

_MemFill::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld c,7(ix)
	ld b,8(ix)
	pop ix
	
	ld	a,b
	or	c
	ret	z
	ld	(hl),e
	ld	e,l
	ld	d,h
	inc	de
	dec	bc
	
	ld	a,b
	or	c
	ret	z
	ldir
	ret

;--- end of proc 

