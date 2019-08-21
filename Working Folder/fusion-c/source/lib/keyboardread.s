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
;------------------------------------------------
;	_keyboardRead
;	Low level console functions
;
;	(c) 1997, SOLID MSX C
;
;	SDCC port 2015
;	Uses BIOS only. Very basis, small code.
;-----------------------------------------------
;-----------------------------------------------
_KeyboardRead::
	push	ix
	in	a,(#0xAA)
	and	#0xF0		; only change bits 0-3
	or	#8			; reading row 8 of keyboard matrice
	out	(#0xAA),a
	in	a,(#0xA9)	; read row into A
	xor	#0xFF
	ld	l,a
	ld	h,#0
	pop	ix
	ret 