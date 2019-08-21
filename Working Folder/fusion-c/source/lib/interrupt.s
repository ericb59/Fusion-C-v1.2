; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                     V1.1   -  05-2019                     |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;
;	Interrupt functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	2006/11/25	t.hara														;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;

 .area _CODE
  
; ---------------------------------------------------------
reset_vect	= 0x0000
bdos		= 0x0005
keyint		= 0x0038

; ---------------------------------------------------------
;	void InitInterruptHandler(void)
; ---------------------------------------------------------
_InitInterruptHandler::
	;	keyint 
	ld		hl, #keyint
	ld		de, #backup_keyint
	ld		bc, #3
	ldir
	
	di
	ld		a, #0xc3				;	jp intr_handler
	ld		(keyint), a
	ld		hl, #intr_handler
	ld		(keyint+1), hl
	ei
	ret

backup_keyint:
	.ds		3

; ---------------------------------------------------------
;	void EndInterruptHandler(void)
; ---------------------------------------------------------
_EndInterruptHandler::
	di
	ld		hl, #backup_keyint
	ld		de, #keyint
	ld		bc, #3
	ldir
	ei
	ret

; ---------------------------------------------------------
;	intr_handler
; ---------------------------------------------------------
intr_handler:
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy
	exx
	ex		af,af'
	push	af
	push	bc
	push	de
	push	hl
	push	ix
	push	iy

user_intr_handler:
	call	dummy_handler

	xor		a
	or		l
	call	z, backup_keyint

	pop		iy
	pop		ix
	pop		hl
	pop		de
	pop		bc
	pop		af
	ex		af,af'
	exx
	pop		iy
	pop		ix
	pop		hl
	pop		de
	pop		bc
	pop		af
	reti

dummy_handler:
	ret

; ---------------------------------------------------------
;	void SetInterruptHandler( char (*p_handler)( void ) )
; ---------------------------------------------------------
_SetInterruptHandler::
	ld		hl, #2
	add		hl, sp

	ld		c, (hl)
	inc		hl
	ld		b, (hl)
	di
	ld		(user_intr_handler + 1), bc
	ei
	ret