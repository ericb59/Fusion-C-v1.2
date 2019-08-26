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
;	PrintHex, PutCharHex, PrintString, CheckBreak
;   by Eugeny Brychkov 2019
;

INTFLG	.equ	#0xfc9b
CHPUT	.equ	#0x00a2
CHGET	.equ	#0x009f

;------------------------------------------------------------------------
; GET_POINTER_TO_VAR: get pointer to the variable in the stack
;------------------------------------------------------------------------
; in:	L=stack offset (forward)
; out:	HL=pointer to the variable
; regs:	AF
get_pointer_to_var:
	ld	h,#0			; always forward addressing
	add	hl,sp			; now points to variable pointer in the stack
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ret

_PrintString::

	ld	l,#4
	call	get_pointer_to_var

puts_loop:
	ld	a,(hl)
	or	a
	ret	z

	push	hl
	push	ix			; preserve IX
	rst	#0x30
	.db	#0
	.dw	CHPUT
	pop	ix			; restore IX
	pop	hl
	inc	hl
	jr	puts_loop
	ret

_PrintHex::
	ld	l,#4
	call	get_pointer_to_var
	push	hl
	ld	a,h
	call	prhex
	pop	hl
	ld	a,l
	call	prhex
	ret

;*************************************************************************
; PUTCHAR_HEX: print hex representation of argument
;*************************************************************************
_PutCharHex::
	ld	l,#4
	call	get_pointer_to_var
	ld	a,l

prhex:
	push	af
	rrca
	rrca
	rrca
	rrca
	call	putnib
	pop	af
	call	putnib
	ret

;*************************************************************************
; PUTNIB: print one nibble (0-F)
;*************************************************************************
; in:	A (bits 3-0) = value to print
putnib:
	and	#0xf			; remove unsignificant bits
	cp	#0xa
	jr	c,less10		; value is less than 10 (numerical)
	add	a,#0x7			; adjust for uppercase letter A-F
less10:
	push	ix			; preserve IX
	add	a,#0x30
	rst	#0x30
	.db	#0
	.dw	CHPUT
	pop	ix			; restore IX
	ret

;*************************************************************************
; CHECK_BREAK: Check for CTRL-STOP and suspend if STOP is pressed
;*************************************************************************
; in:	nothing
; out:	HL=0 if CTRL-STOP was not pressed, -1 otherwise
; note:	enables interrupts
_CheckBreak::
	ld	hl,#INTFLG
	di
	ld	a,(hl)			; sense STOP key pressed
	ei
	ld	(hl),#0
	cp	#3			; CTRL-STOP was pressed
	jr	z,crtl_stop_pressed
	cp	#4
	jr	nz,no_stop_condition

	; we are here if STOP key was pressed, suspending
	ld	(hl),#0			; reset stop press condition
sense_stop:
	di
	ld	a,(hl)
	ei
	ld	(hl),#0
	cp	#3
	jr	z,crtl_stop_pressed
	or	a
	jr	z,sense_stop

no_stop_condition:
	ld	hl,#0
	ret				; return 0 if CTRL-STOP was not pressed
crtl_stop_pressed:
	ld	hl,#0xffff
	ret				; return -1 if CTRL-STOP was pressed
