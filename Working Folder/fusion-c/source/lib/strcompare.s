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
;	strcompare 
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRCMP
;
;	int	StrCompare( char *s1, char *s2 )
;
_StrCompare::
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	pop ix
lb_cp6:
	ld	a,(de)
	sub	(hl)
        jr	nz,lb_cp8
	inc	de
	inc	hl
	ld	a,(de)
	or	a
	jr	nz,lb_cp6
	ld	hl,#0
	ret
lb_cp8:
	ld	hl,#1
	ret	nc
	ld	hl,#-1
	ret 
	
;--- end of proc