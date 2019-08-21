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
;	strrighttrim
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRRTRIM
;
;	void	StrRightTrim( char *adr )
;
_StrRightTrim::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld	(lb_lHL),hl
	pop ix
		
	ld	a, #' '
lb_l3:
	cpi	
	jr	z,lb_l3
	dec	hl
	
	ld	a, (hl)
	or	a
	jr	nz, lb_l4
	
	ld	hl,(lb_lHL)
	xor	a
	ld	(hl),a
	ret
lb_l4:
	xor	a
	cpir
	dec	hl
lb_l5:	
	dec	hl
	ld	a,(hl)
	cp	#' '
	ret	nz
	xor	a
	ld	(hl),a
	jr	lb_l5

lb_lHL:	.db #0,#0

;--- end of proc