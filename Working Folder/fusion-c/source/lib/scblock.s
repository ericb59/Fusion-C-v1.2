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

.area	_CODE
;---------------------------------------- 2 pixel writing/reading mode	
;
; VDP write/read at address
;
_pos_byXY:		; Procedure calculates hl offset and sets VDP for writing
	di
	ld	a,e	; e=y[0..191]
	and	#248	; /8
	rra
	rra
	rra
	ld	h,a	; each 8 lines = 256 bytes
	ld	a,d	; d=x[0..255]
	and	#248
	ld	l,a	; + 8*int(x/8)
	ld	a,e
	and	#7
	or	l	; + y mod 8
	ld	l,a
	ei
	ret


;
; BLOCK read to memory
;
	
_ReadBlock::
	push ix
	ld ix,#0
	add ix,sp

	call	lb_ld_cpyXY
	
	ld l,12(ix)
	ld h,13(ix)	; patterns
	push	ix
lb_bclp0:
	push	bc
	push	de
lb_bclp1:
	push	hl
	call	_pos_byXY
	di
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x004A ; RDVRM 	Reads the VRAM address by [HL].
	ei
	pop	hl
	ld	(hl),a
	inc	hl
	call	lb_d_add8
	djnz	lb_bclp1
	pop	de
	pop	bc
	inc	e
	dec	c
	jr	nz,lb_bclp0
	pop	ix
	
	call	lb_ld_cpyXY
	
	ld l,14(ix)
	ld h,15(ix)	;colors
lb_bclp0a:
	push	bc
	push	de
lb_bclp1a:
	push	hl
	call	_pos_byXY
	push	bc
	ld	bc,#0x2000	; colours are mapped +0x2000
	add	hl,bc
	pop	bc
	di
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x004A ; RDVRM 	Reads the VRAM address by [HL].
	ei
	pop	hl
	ld	(hl),a
	inc	hl
	call	lb_d_add8
	djnz	lb_bclp1a
	pop	de
	pop	bc
	inc	e
	dec	c
	jr	nz,lb_bclp0a
	
	pop ix
	ret

;
; BLOCK write from memory to screen
;
_WriteBlock::
	push ix
	ld ix,#0
	add ix,sp

	call	lb_ld_cpyXY
	
	ld l,12(ix)
	ld h,13(ix)	; patterns
	push	ix
lb_bclp4:
	push	bc
	push	de
lb_bclp5:
	ld	a,(hl)
	ld	(#_wr_by),a
	push	hl
	call	_pos_byXY
	ld	a,(#_wr_by)
	di
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x004D ; WRTVRM  	Write to the VRAM address by [HL].
	ei
	pop	hl
	inc	hl
	call	lb_d_add8
	djnz	lb_bclp5
	pop	de
	pop	bc
	inc	e
	dec	c
	jr	nz,lb_bclp4
	pop	ix
	
	call	lb_ld_cpyXY
	
	ld l,14(ix)
	ld h,15(ix)	;colors
lb_bclp4a:
	push	bc
	push	de
lb_bclp5a:
	ld	a,(hl)
	ld	(#_wr_by),a
	push	hl
	call	_pos_byXY
	push	bc
	ld	bc,#0x2000	; colours are mapped +0x2000
	add	hl,bc
	pop	bc
	ld	a,(#_wr_by)
	di
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x004D ; WRTVRM  	Write to the VRAM address by [HL].
	ei
	pop	hl
	inc	hl
	call	lb_d_add8
	djnz	lb_bclp5a
	pop	de
	pop	bc
	inc	e
	dec	c
	jr	nz,lb_bclp4a
	
	pop ix
	ret


lb_ld_cpyXY:

	ld b,8(ix)	;dx
	ld c,10(ix)	;dy
	
	ld d,4(ix)	; X,Y
	ld e,6(ix)
	
lb_b_div8:
	ld	a,b
	and	#248
	rrca
	rrca
	rrca		; /8
	ld	b,a
	ret
	
lb_d_add8:
	ld	a,d
	add	#8
	ld	d,a
	ret
	
	;.area	_XDATA

_wr_by:		.db	#0