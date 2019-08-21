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

SC5SPRITES_PATTERN		.equ	#0x7800
SC5SPRITES_ATTRIBS		.equ	#0x7600
SC5SPRITES_COLOURS		.equ	#0x7400

SC8SPRITES_PATTERN		.equ	#0xF000
SC8SPRITES_ATTRIBS		.equ	#0xFA00
SC8SPRITES_COLOURS		.equ	#0xF800

.area _CODE

; This prepares for "pixeling",  HL contains initial address
_VDPwrite:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0171	; NSTWRT Sets up the VDP for writing with full 16 bits VRAM address
	pop	ix
	ret         


;--------------------------------------------------------
; Define new pattern for sprites
;
;	Number of pattern 0..255	(2Kb for all pattern data)
;	Address of data to write to sprite VRAM pattern table (8 or 16 bytes)
;--------------------------------------------------------
_SC8SpritePattern::
	xor	a
	ld	(#_spr_bct),a
	ld	hl, #SC8SPRITES_PATTERN
	jr	SprWrTbBg

_SC5SpritePattern::
	xor	a
	ld	(#_spr_bct),a
	ld	hl, #SC5SPRITES_PATTERN
	jr	SprWrTbBg
	
;--------------------------------------------------------
; Define new colours for sprites
;
;	Number of sprite 0..31
;	Address of data to write to sprite VRAM colour table
;--------------------------------------------------------
_SC8SpriteColors::
	ld	a,#1
	ld	(#_spr_bct),a
	ld	hl, #SC8SPRITES_COLOURS
	jr SprWrTbBg

_SC5SpriteColors::
	ld	a,#1
	ld	(#_spr_bct),a
	ld	hl, #SC5SPRITES_COLOURS
	
SprWrTbBg:
	ld	(#_spr_thb),hl
	
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	pop ix

	push	de
	ld	e,l	
	ld	hl, (#_spr_thb)		
	call	Spr_locTb	; find position in table
	ld	b,c		; 8 or 32, depends on sprite size, or 8 or 16 if colour settings
	pop	de

Spr_wr_vram:	
		;write to VRAM table
	di
	call	_VDPwrite
	ex	de,hl
	ld	c,#0x98
	ld	a,(hl)
	otir
		; 8 dummies for colours of lines of 8x8 sprite
	ld	a,(#_spr_bct)
	or	a
	jr	z, lb_skpdm
	ld	a,(#_spr_16)
	or	a
	jr	nz, lb_skpdm
	ld	b,#8
lb_lpdm:
	out	(c),a
	djnz	lb_lpdm
	
lb_skpdm:
	xor	a
	ld	(#_spr_bct),a
	
	ld	hl,#0
	call	_VDPwrite
	ei
	ret
		
	; positions HL to sprite number in reg-e
Spr_locTb:
	ld	bc,#16
	ld	a,(#_spr_bct)
	or	a
	jr	nz,Spr_TbSpk		; 16 bytes colours
	
	ld	c,#8
	ld	a,(#_spr_16)
	or	a
	jr	z,Spr_TbSpk		; if 8x8 -> 8 
	ld	c,#32			; if 16x16 ->32
	ld	a,e
	and	#252
	rrca				; e/=4 palette number
	rrca
	ld	e,a
	
Spr_TbSpk:
	dec	e
	ret	m
	add	hl,bc
	jr	Spr_TbSpk

;-----------------------------------------------
; Set sprite attributes
;
;	Number of sprite
;	Pattern [0..255] created before
;	X [0..255]
;	Y [0..211]
;
_SC5SpriteAttribs::
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld a,6(ix)
	ld (#_spr_atrb+#2),a
	ld a,8(ix)
	ld (#_spr_atrb+#1),a
	ld a,10(ix)
	ld (#_spr_atrb+#0),a
	pop ix

	ld	hl, #SC5SPRITES_ATTRIBS
	ld	bc,#4
	call	Spr_TbSpk	; position HL
	ld	de, #_spr_atrb	
	ld	b,#4
	jr	Spr_wr_vram



;-----------------------------------------------
_Sprite32Bytes::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	inc	hl
	ld	de,#_spr_32b
	ld	c,#1
lb_spr0qrt:
	ld	b,#16
lb_spr1qrt:
	ld	a,(hl)
	ld	(de),a
	inc	hl
	inc	hl
	inc	de
	djnz	lb_spr1qrt
	dec	c
	jp	m,lb_sprQ4
	push	bc
	ld	bc,#33
	scf
	ccf
	sbc	hl,bc
	pop	bc
	jr	lb_spr0qrt
lb_sprQ4:
	ld	hl,#_spr_32b
	ret
	
		; .area	_XDATA

_spr_bct:	.db #0
_spr_thb:	.dw #0		
_spr_16:	.db #0		; keep value here, default 8x8
_spr_atrb:	.db #0,#0,#0,#0x0F
_spr_32b:	.ds #32