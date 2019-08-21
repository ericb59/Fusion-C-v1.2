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
; A dumb screen to memory read
;
.area _CODE

_VDPread0:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0050		; SETRD Sets up the VDP for reading with HL address
	pop	ix
	ret

_VDPwrite0:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0053		; SETWRT Sets up the VDP for writing with HL address
	pop	ix
	ret   

_SC2ReadScr::

	push ix
	ld ix,#0
	add ix,sp

	ld l,6(ix)
	ld h,7(ix)
	push	hl
	
	ld l,4(ix)
	ld h,5(ix)
	push	hl
	
	di
	ld	hl,#0
	call	_VDPread0
	pop	de
	ld	hl,#0x0000	; patterns
	ld	bc,#0x1800	;32 x 192 lines
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0059	; LDIRMV 	Block transfer to memory from VRAM
	
	pop	de
	ld	hl,#0x2000	; colours
	ld	bc,#0x1800	;32 x 192 lines
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0059	; LDIRMV 	Block transfer to memory from VRAM
	ei
	
	pop ix
	ret

	.area	_CODE

;
; A dumb memory to screen write
;

_SC2WriteScr::

	push ix
	ld ix,#0
	add ix,sp

	ld l,6(ix)
	ld h,7(ix)
	push	hl
	
	ld l,4(ix)
	ld h,5(ix)
	push	hl

	di
	ld	hl,#0
	call	_VDPwrite0
	pop	hl
	ld	de,#0x0000	; patterns
	ld	bc,#0x1800	;32 x 192 lines
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x005C	; LDIRVM 	Block transfer to VRAM from memory
	
	pop	hl
	ld	de,#0x2000	; colours
	ld	bc,#0x1800	;32 x 192 lines
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x005C	; LDIRVM 	Block transfer to VRAM from memory 
	ei
	pop ix
	ret 