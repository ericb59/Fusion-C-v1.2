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
;	StrPosStr
;	Definitions for dealing with MSXDOS
;
;	(C) 1995, SOLID MSX C
;
;	SDCC port 2015
;
	.area _CODE


;--- proc  STRposSTR
;
;	int	StrPosStr( char *s1, char *s2 )
;
_StrPosStr::
	
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld	(lb_qHL),hl
	pop ix

	ld	a,(hl)
	or	a
	ld	hl, #0
	ret	z
lb_q1:
	ld	a,(de)
	or	a
	jr	z, lb_q2
	push	hl
	ld	hl,(lb_qHL)
	push	de
	call	_striseq
	pop	de
	pop	hl
	ret	z
	inc	de
	inc	hl
	jr	lb_q1
lb_q2:
	ld	hl, #-1
	ret
	
lb_qHL:	.db #0,#0

_striseq:

lb_q4:
	ld	a,(de)
	sub	(hl)
        jr	nz, lb_q9
	inc	de
	inc	hl
	ld	a,(de)
	or	a
	jr	nz,lb_q4
	ld	a,(hl)
	or	a
	ret
lb_q9:
	ld	a,(hl)
	or	a
	ret

;--- end of proc 