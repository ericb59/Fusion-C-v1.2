; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                     V1.1b - July 2019                     |
;|                       V1 by sylvain                       |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;
;	VDPgraph2.s	- MSX2 graphics VDP functions For MSX2
;
;	Compile on SDCC for MSX
;
;	This works on MSXDOS, not BASIC ROM
;	Copy pixel data from memory RAM to video memory VRAM by using chip built in functions.
;	No slow BASIC graphics, real game must load data, manage rectangles of pixels in memory.
;
;
;
	.area _CODE

ACPAGE = 0xFAF6
DPPAGE = 0xFAF5
FORCLR	.equ  #0xF3E9         ; foreground color

	.area	_CODE

;---------------------------------------- 2 pixel writing/reading mode
;
; VDP write/read at address
;
_pos_byXY:		; Procedure calculates hl offset and sets VDP for writing
	di
	ld	a,d	; d=x[0..255]
	and	#254
	rra
	ld	l,a	;2px per byte
	ld	a,e	; e=y[0..211]
	ld	de,#0
	bit	#0,a
	jp	z,lb_l2b
	ld	e,#0x80
lb_l2b:
	rra
	ld	h,a
	add	hl,de
	ei
	ret

; This prepares for "pixeling",  HL contains initial address
_VDPwrite:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0171	; NSTWRT Sets up the VDP for writing with full 16 bits VRAM address
	pop	ix
	ret


_VDPdraw2pixels:		; Put 2 pixels by sending one byte with 2 colour Nr. (bits 0-3,4-7)
	out (#0x98),a		; send this sequentially
	ret

_VDPread:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x016E	; NSETRD Sets up the VDP for reading with full 16 bits VRAM address
	pop	ix
	ret

_VDPget2pixels:           	; Get 2 pixels, one byte with 2 colour Nr. (bits 0-3,4-7)
	in	a,(#0x98)	; read this sequentially
	ret



;========================================================
;
;	Simply check MSX1 or MSX2 without touching VDP
;

EXBRSA	.equ	0xFAF8			; the slot of SUB-ROM (0 for MSX1)

_vMSX::
	ld	a, (EXBRSA)	; do not touch TMS9918A of MSX1, just obtain technical slot presence
	or	a
	jp	nz, lb_vrYm
				; MSX1
	ld	de, #_MSX1_err_
	ld	c, #9
	call	#5	; display "MSX1 not supported"
	ld	a,#1
	jp	lb_vrEx

lb_vrYm:
	ld	a,#2		; MSX2 and above
lb_vrEx:
	ld	l,a
	ld	h,#0
	ret


_MSX1_err_:	.ascii "MSX1 not supported by VDPgraph2$\n"
_curHL:		.dw #0
	

;
; Coordinate system of VRAM
; pages and memory blocks (32kb)
;	(SCREEN 5)
; ------------------------------	  00000H
; | (0,0) 	     (255,0) |	    |
; |	    Page 0	     |	    |
; | (0,255)	   (255,255) |	    |
; ------------------------------	  08000H
; | (0,256)	   (255,256) |	    |
; |	    Page 1	     |	    |
; | (0,511)	   (255,511) |	    |
; ------------------------------	  10000H
; | (0,512)	   (255,512) |	    |
; |	    Page 2	     |	    |
; | (0,767)	   (255,767) |	    |
; ------------------------------	  18000H
; | (0,768)	   (255,768) |	    |
; |	    Page 3	     |	    |
; | (0,1023)	  (255,1023) |	    |
; ------------------------------	  1FFFFH


;
; AHL is 17 bit value of
; Left upper corner of each pages:
; Provide page in A to set A,HL
;
;   0 -> A=0,HL=$0000
;   1 -> A=0,HL=$8000
;   2 -> A=1,HL=$0000
;   3 -> A=1,HL=$8000
;
_getAHL:
	ld	hl,#0
	and	#1
	jp	z,lb_ahl_0
	ld	h,#0x80
lb_ahl_0:
	ret

;-----------------------------------------------
; Set VDP port $98 to start writing at address AHL (17-bit)
;	Page provided.
;
_SetVDPWrite::           ; A(1bit),HL(16bits) input

	ld  hl,#2
	add hl,sp
	ld a,(hl)

_isetvdpwrite:
	push hl
	call _getAHL

	rlc h
	rla
	rlc h
	rla
	srl h
	srl h
	di
	out (#0x99),a
	ld  a,#14+#128
	out (#0x99),a
	ld  a,l
	nop
	out (#0x99),a
	ld  a,h
	or  #64
	ei
	out (#0x99),a
	pop hl
	    ret

;-----------------------------------------------
; A dumb memory to screen write
;

_WriteScr::

	push ix
	ld ix,#0
	add ix,sp

	ld l,4(ix)
	ld h,5(ix)

;
;  It's better to use BIOS #005C Block transfer to VRAM from memory
;
	push hl
	ld   bc,#0x6A00	;256/2 * 212 lines
	ld   hl,#0
	call _VDPwrite
	pop  hl
	ld   de,#0
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x005C	; LDIRVM 	Block transfer to VRAM from memory

	pop ix
	ret

;-----------------------------------------------
; Set VDP port $98 to start reading at address AHL (17-bit)
;	Page provided.
;
_SetVDPRead::

	ld  hl,#2
	add hl,sp
	ld a,(hl)
	call	_getAHL

	rlc	h
	rla
	rlc	h
	rla
	srl	h
	srl	h
	di
	out     (#0x99),a       ;set bits 15-17
	ld	a,#128+#14
	out     (#0x99),a
	ld      a,l		;set bits 0-7
	nop
	out     (#0x99),a
	ld	a,h		;set bits 8-14
	ei			; + read access
	out     (#0x99),a
	ret

;-----------------------------------------------
; A dumb screen to memory read
;

_ReadScr::

	push ix
	ld ix,#0
	add ix,sp

	ld l,4(ix)
	ld h,5(ix)

;
;  It's better to use BIOS #0059 Block transfer to memory from VRAM
;
	push	hl
	ld	bc,#0x6A00	;256/2 * 212 lines
	ld	hl,#0
	call	_VDPread
	pop	de
	ld	hl,#0
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0059	; LDIRMV 	Block transfer to memory from VRAM

;
;	ld	bc,#0x6A00	;256/2 * 212 lines heavy direct VDP access loop
;lb_rd0:
;	in	a,(#0x98)
;	ld	(hl),a
;	inc	hl
;	dec	bc
;	ld	a,b
;	or	c
;	jr	nz, lb_rd0

	pop ix
	ret

ARDVDP .equ	#0x0006
AWRVDP .equ	#0x0007

GETSTATUS:
	push	bc
	ld		bc,(AWRVDP)
	inc		c
	out		(c),a
	ld		a,#128+#15
	out		(c),a
	ld		bc,(ARDVDP)
	inc		c
	in		a,(c)
	pop		bc
	ret

WAITVDP:
	ld		a,#2
	call	GETSTATUS
	and		#1
	jp		nz,WAITVDP
	xor		a
	call	GETSTATUS
	ret

; a supprimer si WAITVDP fonctionne
VDPready:
	ld	a,#2
;	di
	out	(#0x99),a          ;select status register 2
	ld	a,#128+#15
	out	(#0x99),a
	in	a,(#0x99)

;	bit	#0,a
	and #1
	jp	nz, VDPready	; wait

;	rra ; sct: instruction inutile
	xor	a
	out	(#0x99),a
	ld	a,#128+#15
	out (#0x99),a
;	ei
;	jr	c,VDPready    ;wait till previous VDP execution is over (CE)
	ret


;****************************************************************
; METHOD 2   Locate AHL, then set writing, then
;	put/get 2pixel byte
;
; Set colour of pixel.
;
; void psetXY(int X, int Y, int Colour);
;
_PsetXY::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld a,8(ix)

	call	_ipsetXY
	pop ix
	ret
_ipsetXY:
	ld	b,a
	push	de
	call	_pos_byXY
	ld	(_curHL),HL
	call	_VDPread
	call	_VDPget2pixels
	pop	de
	bit	#0,d
	jp	nz,lb_pst2
	and	#0x0F
	ld	c,a
	ld	a,b
	rla
	rla
	rla
	rla
	or	c
	jp lb_pstS
lb_pst2:
	and	#0xF0
	or	a,b
lb_pstS:
	ld	c,a
	ld	hl,(_curHL)
	call	_VDPwrite
	ld	a,c		; write new colour of 2 pixels
	call	_VDPdraw2pixels
	ret

;-----------------------------------------------
; Get colour of pixel.
;
; int pgetXY(int X, int Y);
;
_PgetXY::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)

	call	_ipgetXY
	pop ix
	ld	l,a
	ld	h,#0
	ret
_ipgetXY:
	push	de
	call	_pos_byXY
	call	_VDPread
	call	_VDPget2pixels
	pop	de

	bit	#0,d
	jp	nz,lb_psg2
	and	#0xF0
	rra
	rra
	rra
	rra
lb_psg2:
	and	#0x0F
	ret

;****************************************************************
;	POINT gets pixel
;		 to use, set H, L as follows
;		 POINT ( x:H, y:L )
;		 returns:   A := COLOR CODE
; 			Screen 7 fixed : Eric
;****************************************************************

_Point::
	push ix
	ld ix,#0
	add ix,sp
	
_ipoint:
	di
	call    VDPready
	ld    a,#32
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 32

	ld    c,#0x9b

	ld  a,4(ix)  ;
	out    (c),a    ; R32 DX low byte

	ld  a,5(ix)  ;
	out    (c),a    ; R33 DX high byte

	ld  a,6(ix)  ;
	out    (c),a    ; R34 DY low byte

	ld  a,7(ix)  ;
	out    (c),a    ; R35 DY high byte


	out	(#0x99),a
	ld	a,#128+#45
	out	(#0x99),a
	
	ld	a,#0b01000000
	out	(#0x99),a
	ld	a,#128+#46
	out	(#0x99),a

	call	VDPready

	ld	a,#7
	call	mmc_Status
	push	af
	xor	a
	call	mmc_Status
	pop	af

	ld	l,a
	ld	h,#0
	ei
	pop ix
	ret



;****************************************************************
; New LINE     !!! NOT WORKING !!!
;        
;        draw LINE 
; 
;****************************************************************

;; Nouveau LINE par ERIC , mais ca ne marche pas !! :-(
; Utilisation de valeurs 16bits pour pouvoir utiliser en screen 7 . Largeur de 512 pixels

_NewLine::
push ix
  ld ix,#0
  add ix,sp

  di
  call    VDPready
  ld    a,#36
  out    (#0x99),a
  ld    a,#128+#17
  out    (#0x99),a    ;R#17 := 36

  ld    c,#0x9b

nline:
  
  ld a,4(ix)
  out (c),a     ; R36 DX Origin low byte
  ld a,5(ix)    ;
  out (c),a     ; R37 DX Origin high byte

  ld a,6(ix)
  out (c),a     ; R38 DY Origin low byte
  ld a,7(ix)    
  out (c),a     ; R39 DY Origin high byte


;-----------------
  ld h,9(ix)    ; On place x2 dans HL
  ld l,8(ix)
  ld b,5(ix)    ; on place X1 dans DE
  ld c,4(ix)
  or a          ; reset C Flag 
  sbc hl,bc     ; hl=hl-BC.  ->  hl=x2-x1.    SI HL > BC c flag = 0. Dans ce cas DIX doit etre = à 0 (vers la droite)
  push hl
  pop iy
  ld  d,#0b00000000
  jp  nc,nLINE1
  ld  d,#0b00000100

nLINE1:
  
  ld h,11(ix)   ; On place Y2 dans HL
  ld l,10(ix)
  ld b,7(ix)    ; on place Y1 dans DE
  ld c,6(ix)
  or a
  sbc hl,bc     ; Si HL > BC alors c Flag = 0. Dans ce cas DIY doit etre égal à 0 (vers le bas )
  ld e,#0b00000000
  jr nc,nLINE2
  ld e,#0b00001000

nLINE2:
   
  push iy
  pop bc
  or a
  sbc hl,bc     ; Si HL > BC alors c Flag = 0. Dans ce cas NY > NX .  MAJ doit être = à 1 

  ld a,#0b00000001

  jr nc,nLINE3      ; si hl est plus petit (nx)
  ld a,#0b00000000

  ld    c,#0x9b
  out (c),c     ;R40  long side  low byte
  out (c),b     ;R41  long side  high byte
  out (c),l     ;R42  short side low byte
  out (c),h     ;R43  Short side High byte
 
  JP nLINE4
  
nLINE3:

  ld   c,#0x9b
  out (c),c     ;R40  long side  low byte
  out (c),b     ;R41  long side  high byte
  out (c),l     ;R42  short side low byte
  out (c),h     ;R43  Short side High byte

nLINE4:  
  ld l,12(ix)     ; Couleur
  out (c),l       ;R44 Couleur 
  or d
  or e
  out(c),a      ;R45 Parameters

  ld a,13(ix)  ; OP

  ld a,#0b01110000
  out (c),a
  
  ei
  pop ix
  ret


;****************************************************************
; draws LINE. (OLD FUNCTION no more used. Keep for comptability reasons)
;        to use, set H, L, D, E, B, A and go
;        draw LINE (H,L)-(D,E) with color B, log-op A
; H,L,D,E absolute values
;****************************************************************

_OLD_Line::
	push ix
	ld ix,#0
	add ix,sp
	ld h,4(ix)
	ld l,6(ix)
	ld d,8(ix)
	ld e,10(ix)
	ld b,12(ix)
	ld a,14(ix)
	pop ix
_iline:
	di
	push	af		;save LOGICAL OPERATION
	push	bc		;save COLOR
	call	VDPready

	ld	a,#36
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	;R#17 := 36
	xor	a
	ld	c,#0x9b
	out	(c),h		;X from
	out	(c),a
	out	(c),l		;Y from
	out	(c),a

	ld	a,h		;make DX and DIX
	sub	d
	ld	d,#0b00000100
	jr	nc,gLINE1
	ld	d,#0b00000000
	neg
gLINE1:
	ld	h,a 		;H := DX , D := DIX
	ld	a,l		;make DY and DIY
	sub	e
	ld	e,#0b00001000
	jr	nc,gLINE2
	ld	e,#0b00000000
	neg
gLINE2:
	ld	l,a		;L := DY , E := DIY
	cp	h		;make Maj and Min
	jr	c,gLINE3
	xor	a
	out	(c),l		;long side
	out	(c),a
	out	(c),h		;short side
	out	(c),a
	ld	a,#0b00000001	;MAJ := 1
	jr	gLINE4
gLINE3:
	xor	a
	out	(c),h		;NX
	out	(c),a
	out	(c),l		;NY
	out	(c),a
	ld	a,#0b00000000	;MAJ := 0
gLINE4:
	or	d
	or	e		;A := DIX , DIY , MAJ
	pop	hl		;H := COLOR
	out	(c),h
	out	(c),a
	pop	af         	;A := LOGICAL OPERATION
	or	#0b01110000
	out	(c),a
	ld	a,#0x8F
	out	(c),a

	call	VDPready
	ei
	ret



;-----------------------------------------------
;
;	Draws rectangle (H,L)-(D,E) with color B, log-op A
;  (OLD FUNCTION no more used. Keep for comptability reasons)
_OLD_Rect::
	push ix
	ld ix,#0
	add ix,sp

	ld h,4(ix)
	ld l,6(ix)
	ld d,8(ix)
	ld e,10(ix)
	ld b,12(ix)
	ld c,14(ix)
	pop ix
	di
	call	VDPready

	xor	a
lb_svfl:
	ld	(#_rect_fill),a
	ld	a,c
	cp	#0xff		; if fill case
	jp	nz, lb_rNfl
	xor	a
	ld	c,a
	inc	a
	jp	lb_svfl
lb_rNfl:
	push	de
	push	hl
	push	de
	ld	e,l
	call	lb_rctl
	pop	hl
	call	lb_rctl
	pop	de
	push	de
	ld	e,l
	call	lb_rctl
	pop	hl
	call	lb_rctl
	pop	de

;	call	VDPready
	ld	a,(#_rect_fill)
	or	a
	ret	z
			; fill rectangle with colour
lb_rcLp:
	ld	a,h
	sub	d
	jp	z, lb_rcOK
	jp	nc,lb_rcm
	inc	h
	inc	h
lb_rcm:
	dec	h
	push	de
	ld	d,h
	call	lb_rctl
	pop	de
	jp	lb_rcLp
lb_rcOK:
;	call	VDPready
	ret

lb_rctl:
	push	hl
	push	de
	push	bc
	ld	a,c
	call	_iline
	pop	bc
	pop	de
	pop	hl
	ei
	ret


_rect_fill:	.db #0
_lpaintHL:	.dw #0


	.area	_CODE
;---------------------------
; Paints from the point (x:H, y:L), color:E, border color:D
; Not 100% correct. Basic has better PAINT code.
; There is SRCH (colour code search) VDP algorithm that
; should be implemented here instead of this crap.
;
_Paint::

	push ix
	ld ix,#0
	add ix,sp

	ld h,4(ix)
	ld l,6(ix)
	ld e,8(ix)
	ld d,10(ix)
	pop ix
	di
	call	VDPready

		; this is way to limit stack depth, it is better then dumb crash
_ptAgain:
	ld	bc, #0x2000	; calculate depth, limit
	call	_paintRR

	ld	hl,(#_lpaintHL)
	ld	a,b
	or	c
	jp	z,_ptAgain	; if bc==0 then try paint at last position
	call	VDPready
	ei
	ret

_paintRR:
	ld	a,e
	push	hl
	push	de
	push	bc
	ex	de,hl		; de = X,Y
	call	_ipsetXY
	pop	bc
	pop	de
	pop	hl
	ld	(#_lpaintHL),hl	; will continue after all returns (Call SP>SP>SP...>..0x8000 crash at)
lb_ptY0:
	ld	a,l
	or	a
	jp	z,lb_ptX0
	dec	l
	call	recIfPaint
	inc	l
lb_ptX0:
	ld	a,h
	or	a
	jp	z,lb_ptY2
	dec	h
	call	recIfPaint
	inc	h
lb_ptY2:
	ld	a,l
	cp	#211
	jp	z,lb_ptX2
	inc	l
	call	recIfPaint
	dec	l
lb_ptX2:
	ld	a,h
	cp	#255
	jp	z,lb_ptOk
	inc	h
	call	recIfPaint
	dec	h
lb_ptOk:
	ld	a,b
	or	c
	ret	z	; if bc==0 then just return
	jp	lb_rbc

recIfPaint:
	ld	a,b
	or	c
	ret	z	; if bc==0 then just return

	dec	bc
	push	hl
	push	de
	push	bc
	ex	de,hl		; de = X,Y
	call	_ipgetXY
	pop	bc
	pop	de
	pop	hl
	cp	d
	jp	z,lb_rbc
	cp	e
	jp	z,lb_rbc
	jp	_paintRR
lb_rbc:
	inc	bc
	ret




;****************************************************************
; HMMC (High speed move CPU to VRAM)
; Screen size 256x212 dots, each byte is colour Nr. for 2-pixels
;
; Copies data from memory to block in VRAM
; The same as in basic COPY file.pic TO (x,y)-(x+D-1,y+E-1)
;
; RAM [IX] => VRAM (H,L)-(D,E)
;
; set ix = memory address of data to write to VRAM
; set h,l,d,e for rectangle to put in
; D,E mod 2 = 0 !
;****************************************************************


;updated by (BTV) Bitvision Software 2018
; Now Y & height could be greater than 255
_HMMC::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	push hl
	ld l,6(ix)
	ld h,7(ix)		;hl=X dst
	ld e,8(ix)
	ld d,9(ix)		;de=Y dst
	exx
	ld	c,#0x9b
	ld l,10(ix)
	ld h,11(ix)		;hl'=width
	ld e,12(ix)
	ld d,13(ix)		;de'=height
	exx

	pop ix

	di
	call	mmc_wait_VDP

	ld	a,#36	;command register R#36
	out	(#0x99),a
	ld	a,#128+#17	;VDP(17)<=36
	out	(#0x99),a
	ld	c,#0x9b
	out	(c),l		;X
	out	(c),h
	out	(c),e		;Y
	out	(c),d
	exx
	out	(c),l		;DX in dots
	out	(c),h		;
	out	(c),e		;DY in dots
	out	(c),d		;
	exx

	ld	h,(ix)		;first byte of data
	out	(c),h

	xor	a
	out	(c),a		;DIX and DIY = 0
	ld	a,#0b11110000
	out	(c),a		; command to do it
	ld	a,#128+#44
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	; VDP(17)<=44
mmc_Loop:
	ld	a,#2
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	ld	b,a
	xor	a
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	ld	a,b
	bit	#7,a		; TR? transferring?
	jp	z, mmc_Loop
	bit	#0,a		; CE? is over?
	jp	z, mmc_exit
	inc	ix
	ld	a,(ix)
	out	(#0x9b),a
	jp	mmc_Loop
mmc_exit:
	xor	a
	call	mmc_Status
	ei
	pop	ix
	ret

mmc_Status:
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	push	af
	xor	a
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	pop	af
	ret

mmc_wait_VDP:
	ld	a,#2
	call	mmc_Status
	and	#1
	jp	nz,mmc_wait_VDP
	xor	a
	call	mmc_Status
	ret

;****************************************************************
; LMMC (Logical move CPU to VRAM)
;
; Copies data from memory to block in VRAM with logical operation OP
;
; RAM [IX] (OP) => VRAM (H,L)-(D,E)
;
; set ix = memory address of data to write to VRAM
; set h,l,d,e for rectangle to put in
;****************************************************************

_LMMC::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	push hl
	ld l,6(ix)
	ld h,7(ix)		;hl=X dst
	ld e,8(ix)
	ld d,9(ix)		;de=Y dst

	exx
	ld c,#0x9b
	ld l,10(ix)
	ld h,11(ix)		;hl'=width
	ld e,12(ix)
	ld d,13(ix)		;de'=height
	exx

	ld a,14(ix)  ; OP
	and #0x0f
	or  #0b10110000    ;LMMC command with LOGICAL
	ld b,a

	pop ix

	di
	call	mmc_wait_VDP

	ld	a,#36	;command register R#36
	out	(#0x99),a
	ld	a,#128+#17	;VDP(17)<=36
	out	(#0x99),a
	ld	c,#0x9b
	out	(c),l		;X
	out	(c),h
	out	(c),e		;Y
	out	(c),d
	exx
	out	(c),l		;DX in dots
	out	(c),h		;
	out	(c),e		;DY in dots
	out	(c),d		;
	exx

	ld	h,(ix)		;first byte of data
	out	(c),h

	xor	a
	out	(c),a		;DIX and DIY = 0

	out	(c),b		; command to do it

	ld	a,#128+#44
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	; VDP(17)<=44
	jp	mmc_Loop


;****************************************************************
; HMCM (High speed move VRAM to CPU)
; Screen size 256x212 dots, each byte is colour Nr. for 2-pixels
;
; Copies data from memory block in VRAM to RAM memory address
; The same as in basic SAVE (H,L)-(D-1,E-1) TO file.pic
;
; VRAM (H,L)-(D,E) => RAM [IX]
; VRAM (D,E)-(H,L) => RAM [IX]	; backwards scanned
;
; set ix = memory address of data to write to RAM
; set h,l,d,e for rectangle
; D,E mod 2 = 0 !
;****************************************************************

_HMCM::
	push ix
	ld ix,#0
	add ix,sp
	ld l,12(ix)
	ld h,13(ix)
	push hl
	ld h,4(ix)
	ld l,6(ix)
	ld d,8(ix)
	ld e,10(ix)

	ld a,14(ix)  ; OP
	and #0x0f
	or  #0b10100000	;LMCM command with logical
	ld b,a

	pop ix

	di
	xor	a
	call	hmcm_wait_VDP

	ld	a,#32	;command register R#32
	out	(#0x99),a
	ld	a,#128+#17	;VDP(17)<=32
	out	(#0x99),a
	xor	a
	ld	c,#0x9b
	out	(c),h		;SX
	out	(c),a
	out	(c),l		;SY
	out	(c),a

	out	(c),a		;dummy
	out	(c),a		;dummy
	out	(c),a		;dummy
	out	(c),a		;dummy

	call	DIX_DIY

	out	(c),h		;NX in dots
	out	(c),a		;
	out	(c),l		;NY in dots
	out	(c),a		;

	ld	a,(ix)		;first byte of data
	out	(c),a		;dummy
	ld	a,d
	or	e
	out	(c),a		;DIX and DIY

	ld	a,#7		;clear TR status
	call	hmcm_Status

	out	(c),b ; do it

hmcm_Loop:
	ld	a,#2
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	bit	#0,a		; CE? is over?
	jp	z, hmcm_exit
	bit	#7,a		; TR? transferring?
	jp	z, hmcm_Loop

	; read 2 pixels
	ld	a,#7		; 1px
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	rla
	rla
	rla
	rla
	ld	b,a

	ld	a,#7		; 2nd px
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	or	b

	ld	(ix),a
	inc	ix

	jp	hmcm_Loop
hmcm_exit:
	xor	a
	call	hmcm_Status
	ei
	pop	ix
	ret

hmcm_Status:
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	ret

hmcm_wait_VDP:
	ld	a,#2
	call	hmcm_Status
	and	#1
	jp	nz,hmcm_wait_VDP
	xor	a
	call	hmcm_Status
	ret

;
;	Scanning from top to right down (0), or from bottom to left up
;
DIX_DIY:
	ld	a,h		;make NX and DIX
	sub	d
	ld	d,#0b00000100
	jp	nc,lb_dxy1
	ld	d,#0b00000000
	neg
lb_dxy1:
	ld	h,a		;H := NX , D := DIX
 	ld	a,l
	sub	e
	ld	e,#0b00001000
	jp	nc,lb_dxy2
	ld	e,#0b00000000
	neg
lb_dxy2:
	ld	l,a		;L := NY , E := DIY
	xor	a
	ret


;****************************************************************
; HMCM_SC8 (High speed move VRAM to CPU)

;****************************************************************
	
_DIX:	.db	0
_DIY:	.db	0
_NX:	.dw	0
_NY:	.dw	0

	.area	_CODE

;written by (BTV) Bitvision Software 2018
_HMCM_SC8::
	push ix
	ld ix,#0
	add ix,sp
	ld l,12(ix)
	ld h,13(ix)
	push hl

	ld l,4(ix)
	ld h,5(ix)			;hl = X1
	ld e,6(ix)
	ld d,7(ix)			;de = Y1
	exx
	ld l,8(ix)
	ld h,9(ix)			;hl' = X2
	ld e,10(ix)
	ld d,11(ix)			;de' = Y2
	exx

	ld a,14(ix)  ; OP
	and #0x0f
	or  #0b10100000	;LMCM command with logical

	pop ix
	push af

	di
	xor	a
	call	hmcm_wait_VDP

	ld	a,#32	;command register R#32
	out	(#0x99),a
	ld	a,#128+#17	;VDP(17)<=32
	out	(#0x99),a
	xor	a
	ld	c,#0x9b

	out	(c),l		;SX
	out	(c),h
	out	(c),e		;SY
	out	(c),d

	;a is 0
	out	(c),a		;dummy
	out	(c),a		;dummy
	out	(c),a		;dummy
	out	(c),a		;dummy

	call	DIX_DIY_SC8
	ld	c,#0x9b
	ld	hl,(#_NX)
	out	(c),l		;NX in dots
	out	(c),h		;
	ld	hl,(#_NY)
	out	(c),l		;NY in dots
	out	(c),h		;

	;ld	a,(ix)		;first byte of data
	xor	a
	out	(c),a		;dummy
	ld	a,(#_DIX)
	ld	hl,#_DIY
	or	(hl)
	out	(c),a		;DIX and DIY

	ld	a,#7		;clear TR status
	call	hmcm_Status

	pop  af
	out	(c),a

hmcm_Loop_SC8:
	ld	a,#2
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	bit	#0,a		; CE? is over?
	jp	z, hmcm_exit_SC8
	bit	#7,a		; TR? transferring?
	jp	z, hmcm_Loop_SC8

	; read 1 pixel !!!
	ld	a,#7		; 1px
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	ld	(ix),a
	inc	ix
	jp	hmcm_Loop_SC8

hmcm_exit_SC8:
	;Jeez ... you need to read one last time! :-( ... WTF!
	ld	a,#7		; 1px
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	ld	(ix),a

	xor	a
	call	hmcm_Status
	ei
	pop	ix
	ret


;input
;	hl - X1
;	de - Y1
;	hl' - X2
;	de' - Y2
;ret
;	DIX, DIY, NX, NY updated
DIX_DIY_SC8:
	exx
	push	hl
	exx
	pop		bc		;bc=X2
	or		a		;C is unset
	sbc		hl,bc	;X1-X2
	ld		a,#0b00000100
	ld		(#_DIX),a
	jp		nc,lb_dxy1_SC8     ;if X1>=X2 then goto lb_dxy1
	ld		a,#0b00000000  ;X1<X2
	ld		(#_DIX),a
	;neg				   	   ;X1-X2 is negative .. now it's making that positive
	xor a
	sub l
	ld l,a
	sbc a,a
	sub h
	ld h,a			;neg hl
lb_dxy1_SC8:
	ld		(#_NX),hl
				;right here [NX], [DIX] ok
	exx
	push	de
	exx
	pop		bc		;bc=Y2
	ex		de,hl	;hl=Y1
	or		a		;C is unset
	sbc		hl,bc	;X1-X2

	ld	a,#0b00001000
	ld	(#_DIY),a
	jp	nc,lb_dxy2_SC8		;if Y1>=Y2 then goto lb_dxy2
	ld	a,#0b00000000
	ld	(#_DIY),a
;	neg
	xor a
	sub l
	ld l,a
	sbc a,a
	sub h
	ld h,a			;neg hl
lb_dxy2_SC8:
	ld				(#_NY),hl
					;right here [NY], [DIY] ok
	ret

;****************************************************************
;  HMMM (High speed move VRAM to VRAM)
;  copy a rectangle from 'source' VRAM to 'destination' VRAM
;  XS X coordinate of pixel source (left side)
;  YS Y coordinate of pixel source (upper side)
;  XD X coordinate of pixel destination (left side)
;  YD Y coordinate of pixel destination (upper side)
;  DX number of pixels to the right
;  DY number of pixels to the bottom
;****************************************************************

_HMMM::
	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#32
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 32

	ld    c,#0x9b
	ld  a,4(ix)  ;
	out    (c),a    ; R32 X1 low byte
	ld  a,5(ix)  ;
	out    (c),a    ; R33 X1 high byte
	ld  a,6(ix)  ;
	out    (c),a    ; R34 Y1 low byte
	ld  a,7(ix)  ;
	out    (c),a    ; R35 Y1 high byte
	ld  a,8(ix)  ;
	out (c),a    ; R36 X2 low byte
	ld  a,9(ix)  ;
	out    (c),a    ; R37 X2 high byte
	ld  a,10(ix) ;
	out    (c),a    ; R38 Y2 low byte
	ld  a,11(ix) ;
	out    (c),a    ; R39 Y2 high byte
	ld  a,12(ix) ;
	out    (c),a     ; R40 DX low byte
	ld  a,13(ix) ;
	out    (c),a    ; R41 DX high byte
	ld  a,14(ix) ;
	out    (c),a     ; R42 DY low byte
	ld  a,15(ix) ;
	out    (c),a    ; R43 DY high byte
	xor a        ;
	out    (c),a     ; R44 dummy (color not used by LMMM)
	out    (c),a     ; R45 DIX and DIY ! DX and DY express in incremental direction ! internal VRAM
; byte DIX,DIY=0, explained:
; The 0 copies the block starting from the upper left, the 1 from right/bottom.
; what's the difference? when copying overlapping source/destination
; (a scroller for example)
; when scrolling from right to left DIX/DIY can both be 0
;  but copying from left to right DIX must be 1. just figure it out...
; Then give coord.positive from right upper corner to left.

	or    #0b11010000    ;HMMM command

	out    (c),a    ;do it
	ei
	pop    ix
	ret


;****************************************************************
; YMMM (high-speed transfer between VRAM in Y direction)
;
; Eric
;****************************************************************

_YMMM::
	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#34
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 34

	ld    c,#0x9b
	
	ld  a,6(ix)  ;
	out    (c),a    ; R34 SY low byte

	ld  a,7(ix)  ;
	out    (c),a    ; R35 SY high byte

	ld  a,4(ix)  ;
	out    (c),a    ; R36 XS low byte

	ld  a,5(ix)  ;
	out    (c),a    ; R37 XS high byte

	ld  a,8(ix)  ;
	out (c),a    	; R38 DY low byte

	ld  a,9(ix)  ;
	out    (c),a    ; R39 DY high byte

	ld  a,4(ix) ;
	out    (c),a    ; R40 -- low byte

	ld  a,5(ix) ;
	out    (c),a    ; R41 -- high byte

	ld  a,10(ix) ;
	out    (c),a     ; R42 NY low byte

	ld  a,11(ix) ;
	out    (c),a    ; R43 NY high byte

	out    (c),a     ; R44 dummy 

	ld  	a,12(ix) ;
	rla
	rla
	out    (c),a     ; R45 DIX and DIY ! 

	;or    #0b11100000   ;YMMM command
	ld 		a,#0b11100000
	out    (c),a     ;do it
	ei
	pop    ix
	ret


;****************************************************************
; HMMV painting the rectangle in high speed
;
; Eric
;****************************************************************
_HMMV::
	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#36
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 36

	ld    c,#0x9b
	ld  a,4(ix)  ;
	out    (c),a    ; R36 DX low byte

	ld  a,5(ix)  ;
	out    (c),a    ; R37 DX high byte

	ld  a,6(ix)  ;
	out    (c),a    ; R38 DY low byte

	ld  a,7(ix)  ;
	out    (c),a    ; R39 DY high byte

	ld  a,8(ix)  ;
	out (c),a    	; R40 NX low byte

	ld  a,9(ix)  ;
	out    (c),a    ; R41 NX high byte

	ld  a,10(ix) ;
	out    (c),a    ; R42 NY low byte

	ld  a,11(ix) ;
	out    (c),a    ; R43 NY high byte

	ld  a,12(ix) ;

	out    (c),a     ; R44 COL low byte

	xor a        ;

	out    (c),a     ; R45 DIX and DIY ! DX and DY express in incremental direction ! internal VRAM

	or    #0b11000000   ;HMMV command

	out    (c),a    ;do it
	ei
	pop    ix
	ret

;****************************************************************
; LMMV painting the rectangle in high speed with logical operation
;
; Eric
;****************************************************************
_LMMV::

	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#36
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 36

	ld    c,#0x9b
	ld  a,4(ix)  ;
	out    (c),a    ; R36 DX low byte

	ld  a,5(ix)  ;
	out    (c),a    ; R37 DX high byte

	ld  a,6(ix)  ;
	out    (c),a    ; R38 DY low byte

	ld  a,7(ix)  ;
	out    (c),a    ; R39 DY high byte

	ld  a,8(ix)  ;
	out (c),a    	; R40 NX low byte

	ld  a,9(ix)  ;
	out    (c),a    ; R41 NX high byte

	ld  a,10(ix) ;
	out    (c),a    ; R42 NY low byte

	ld  a,11(ix) ;
	out    (c),a    ; R43 NY high byte

	ld  a,12(ix) ;
	out    (c),a     ; R44 COL low byte

	
	xor a        ;
	out    (c),a     ; R45 DIX and DIY ! DX and DY express in incremental direction ! internal VRAM

	ld a,13(ix)  ; OP
	and #0x0f
	or    #0b10000000    ;LMMV command 

	out    (c),a    ;do it
;    call    VDPready
	ei
	pop    ix
	ret



;****************************************************************
; PSET VDP COMMAND
;
; Eric
;****************************************************************
_Pset::

	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#36
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 36

	ld    c,#0x9b

	ld  a,4(ix)  ;
	out    (c),a    ; R36 DX low byte

	ld  a,5(ix)  ;
	out    (c),a    ; R37 DX high byte

	ld  a,6(ix)  ;
	out    (c),a    ; R38 DY low byte

	ld  a,7(ix)  ;
	out    (c),a    ; R39 DY high byte

 	ld	a,#44
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a ; R#44

	ld  a,8(ix)  ;
	out (c),a    	; R44 Col low byte

	ld  a,#0b00000000
	out    (c),a    ; R45 ARG high byte


	ld a,9(ix)  ; OP
	or    #0b01010000    ;PSET command 

	out    (c),a    ;do it

	ei
	pop    ix
	ret


;****************************************************************
;  LMMM (High speed Logical move VRAM to VRAM)
;  copy a rectangle from 'source' VRAM to 'destination' VRAM
;  XS X coordinate of pixel source (left side)
;  YS Y coordinate of pixel source (upper side)
;  XD X coordinate of pixel destination (left side)
;  YD Y coordinate of pixel destination (upper side)
;  DX number of pixels to the right
;  DY number of pixels to the bottom
;  OP logical operation to apply if supported
;****************************************************************

_LMMM::
	push ix
	ld ix,#0
	add ix,sp

	di
	call    VDPready
	ld    a,#32
	out    (#0x99),a
	ld    a,#128+#17
	out    (#0x99),a    ;R#17 := 32

	ld    c,#0x9b
	ld  a,4(ix)  ;
	out    (c),a    ; R32 X1 low byte
	ld  a,5(ix)  ;
	out    (c),a    ; R33 X1 high byte
	ld  a,6(ix)  ;
	out    (c),a    ; R34 Y1 low byte
	ld  a,7(ix)  ;
	out    (c),a    ; R35 Y1 high byte
	ld  a,8(ix)  ;
	out (c),a    ; R36 X2 low byte
	ld  a,9(ix)  ;
	out    (c),a    ; R37 X2 high byte
	ld  a,10(ix) ;
	out    (c),a    ; R38 Y2 low byte
	ld  a,11(ix) ;
	out    (c),a    ; R39 Y2 high byte
	ld  a,12(ix) ;
	out    (c),a     ; R40 DX low byte
	ld  a,13(ix) ;
	out    (c),a    ; R41 DX high byte
	ld  a,14(ix) ;
	out    (c),a     ; R42 DY low byte
	ld  a,15(ix) ;
	out    (c),a    ; R43 DY high byte
	xor a        ;
	out    (c),a     ; R44 dummy (color not used by LMMM)
	out    (c),a     ; R45 DIX and DIY ! DX and DY express in incremental direction ! internal VRAM
; byte DIX,DIY=0, explained:
; The 0 copies the block starting from the upper left, the 1 from right/bottom.
; what's the difference? when copying overlapping source/destination
; (a scroller for example)
; when scrolling from right to left DIX/DIY can both be 0
;  but copying from left to right DIX must be 1. just figure it out...
; Then give coord.positive from right upper corner to left.

	ld a,16(ix)  ; OP
	and #0x0f
	or    #0b10010000    ;LMMM command or with LOGICAL

	out    (c),a    ;do it
;    call    VDPready
	ei
	pop    ix
	ret

;----------------------------------------------------------------------------;
; Compare function                                                           ;
;----------------------------------------------------------------------------;

; test if HL>=DE
; Input      HL, DE
; Output     A=0 -> true (HL>=DE)
;            A=4 -> false (HL<DE)
; modify     A, HL, DE
cmpgte:
	and a    ; RESET CARRY FLAG
	ld a,h
	xor d
	jp m,cmpgte2
	sbc hl,de
	jp nc,cmpgte3
cmpgte1:

	xor a
	sub l
	ld l,a
    sbc a,a
    sub h
    ld h,a

	ld a,#4 ; move to the left
	ret
cmpgte2: ; if here, one is negative, the other one is positive
	bit #7,d
	jp z,cmpgte1
cmpgte3:
	xor a  ; move to the right
	ret


;****************************************************************
;  fLMMM (Far high speed Logical move VRAM to VRAM)
;   for not simplest case, structure provided
;****************************************************************

_fLMMM::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	di
	call	VDPready
	ld	a,#32
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	;R#17 := 32

	ld	bc,#0x0E9b	; c=#0x9b
				; b=6x2bytes + dummy + DI...
	otir			; X,Y,X2,Y2,DX,DY,DIY,DIX

	ld	a,(hl)		; Logical, operation
	bit	#7,a
	jp	nz, lb_flq
	or	#0b10010000	;LMMM command
lb_flq:
	out	(c),a		; do it, VDP!
;	call	VDPready

	ei
	ret

.area	_CODE
;
; DRAW function with syntax
;
_Draw::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	;call	VDPready

lb_drwLp:
	ld	a,(_drw_m)
	or	a
	jp	nz,lb_drwMmv

	ld	de,(_drw_YX)
lb_drwMmv:
	ld	a,(hl)
	or	a
	jp	z, lb_drwEx
	cp	#97
	jp	c,lb_drwUpc
	sub	#32
lb_drwUpc:
lb_drw0:
	cp	#'U'
	jp	nz, lb_drw1
	call	_drw_dcY
	jp	flb_drwQ
lb_drw1:
	cp	#'D'
	jp	nz, lb_drw2
	call	_drw_icY
	jp	flb_drwQ
lb_drw2:
	cp	#'L'
	jp	nz, lb_drw3
	call	_drw_dcX
	jp	flb_drwQ
lb_drw3:
	cp	#'R'
	jp	nz, lb_drw4
	call	_drw_icX
	jp	flb_drwQ
lb_drw4:
	cp	#'E'
	jp	nz, lb_drw5
	call	_drw_icX
	call	_drw_dcY
	jp	flb_drwQ
lb_drw5:
	cp	#'F'
	jp	nz, lb_drw6
	call	_drw_icX
	call	_drw_icY
	jp	flb_drwQ
lb_drw6:
	cp	#'G'
	jp	nz, lb_drw7
	call	_drw_dcX
	call	_drw_icY
	jp	flb_drwQ
lb_drw7:
	cp	#'H'
	jp	nz, lb_drw8
	call	_drw_dcX
	call	_drw_dcY
	jp	flb_drwQ

flb_drwQ:	jp	lb_drwQ
flb_drwQp:	jp	lb_drwQp
flb_drwScnOp:	jp	lb_drwScnOp

lb_drw8:
	cp	#'B'
	jp	nz, lb_drw9
	ld	a,#1
	ld	(_drw_B),a
	jp	flb_drwQp
lb_drw9:
	cp	#'N'
	jp	nz, lb_drw10
	ld	a,#1
	ld	(_drw_N),a
	jp	flb_drwQp

lb_drw10:
	cp	#'S'
	jp	nz, lb_drw11
	call	_drw_gLnQ		; c=scale count
	ld	a,c
	and	#252
	rrca
	rrca				; scale/4 = pixels
	or	a
	jp	nz,lb_drwScs
	inc	a
lb_drwScs:
	ld	(_drw_Scale),a
	jp	flb_drwScnOp
lb_drw11:
	cp	#'C'
	jp	nz, lb_drw12
	call	_drw_gLn		; read colour number
	ld	a,(_drw_Ln)
	ld	(#FORCLR),a
	jp	flb_drwScnOp

lb_drw12:
	cp	#'M'
	jp	nz, lb_drw13
	ld	a,#1
	ld	(_drw_m),a
	call	_drw_gLn		; read X
	ld	a,(_drw_sg)
	or	a
	jp	nz,_drw_relX
	ld	d,#0			; absolute X, not +cnt
_drw_relX:
	cp	#2
	jp	z,_drw_relmX
	call	_drw_icX
	jp	_drw_releX
_drw_relmX:
	call	_drw_dcX
_drw_releX:
	jp	lb_drwScnOp
lb_drw13:
	cp	#','
	jp	nz, lb_drw14
	call	_drw_gLn		; read Y
	ld	a,(_drw_sg)
	or	a
	jp	nz,_drw_relY
	ld	e,#0			; absolute Y, not +cnt
_drw_relY:
	cp	#2
	jp	z,_drw_relmY
	call	_drw_icY
	jp	_drw_releY
_drw_relmY:
	call	_drw_dcY
_drw_releY:
	xor	a
	ld	(_drw_m),a
	jp	lb_drwQ
lb_drw14:
	jp	lb_drwScnOp

lb_drwQ:
	ld	a,(_drw_m)	; if M command, save till ','
	or	a
	jp	nz,lb_drwQp
	ld	a,#1
	ld	(_drw_f),a
lb_drwQp:
	push	hl
	ld	hl,(_drw_YX)
	ld	a,(_drw_B)
	or	a
	jp	nz,lb_drwSkB
	ld	a,(#FORCLR)
	ld	b,a			; set colour
	ld	c,#0			; logical = 0
	call	lb_rctl			; draw line
lb_drwSkB:
	ld	a,(_drw_N)
	or	a
	jp	nz,lb_drwSkN
	ld	(_drw_YX),de		; save new position
lb_drwSkN:
	pop	hl

lb_drwScnOp:
	ld	a,(_drw_f)
	or	a
	jp	z,lb_drwcl
	xor	a
	ld	(_drw_f),a
	ld	(_drw_B),a
	ld	(_drw_N),a
lb_drwcl:
	inc	hl
	jp	lb_drwLp		; loop

lb_drwEx:
	ret

		; new position of point inc,dec
_drw_dcX:
	ld	a,d
	call	_drw_dec
	ld	d,a
	ret
_drw_dcY:
	ld	a,e
	call	_drw_dec
	ld	e,a
	ret
_drw_icX:
	ld	a,d
	ld	c,#255
	call	_drw_inc
	ld	d,a
	ret
_drw_icY:
	ld	a,e
	ld	c,#211
	call	_drw_inc
	ld	e,a
	ret


_drw_dec:				; count x scale dec
	call	_drw_gLnQ		; c=count
_drw_dcylpc:
	call	_drw_ldb		; b=scale
_drw_dcylp:
	or	a
	ret	z
	dec	a
	djnz	_drw_dcylp
	dec	c
	dec	c
	jp	m,_drw_dcyEx
	inc	c
	jp	_drw_dcylpc
_drw_dcyEx:
	ret

_drw_inc:
	cp	c
	ret	z

	push	de
	ld	e,c
					; count x scale inc
	call	_drw_gLnQ		; c=count
_drw_icylpc:
	call	_drw_ldb		; b=scale
_drw_icylp:
	inc	a
	cp	e
	jp	z,_drw_icyEx
	djnz	_drw_icylp
	dec	c
	dec	c
	jp	m,_drw_icyEx
	inc	c
	jp	_drw_icylpc
_drw_icyEx:
	pop	de
	ret

_drw_ldb:
	push	af
	ld	a,(_drw_m)
	or	a
	jp	z,_drw_ldb2

	ld	a,(_drw_sg)
	or	a
	jp	nz,_drw_ldb2
	ld	a,#1
	jp	_drw_ldbE
_drw_ldb2:
	ld	a,(_drw_Scale)
_drw_ldbE:
	ld	b,a
	pop	af
	ret

_drw_gLnQ:			; number is 1 or more
	push	af
	call	_drw_gLn
	ld	a,(_drw_Ln)
	or	a
	jp	nz,_drw_glnq
	inc	a
	ld	(_drw_Ln),a
_drw_glnq:
	ld	c,a
	pop	af
	ret

_drw_gLn:			; get number if is
	push	hl
	push	af
	push	bc

	inc	hl
	ld	a,(hl)
	cp	#'+'
	jp	z,lb_drwLnP
	cp	#'-'
	jp	z,lb_drwLnM
	xor	a
	dec	hl
	jp	_drw_Lnc
lb_drwLnP:
	ld	a,#1
	jp	_drw_Lnc
lb_drwLnM:
	ld	a,#2
_drw_Lnc:
	ld	(_drw_sg),a
	xor	a
	ld	(_drw_Ln),a
_drw_gllp:
	inc	hl
	ld	a,(hl)
	or	a
	jp	z,lb_rex
	sub	#'0'
	jp	c,lb_rex
	cp	#10
	jp	nc,lb_rex

	ld	c,a
	ld	a,(_drw_Ln)
	ld	b,a
	scf
	ccf
	rlca		;x 8
	rlca
	rlca
	adc	b	;+2  = x 10
	adc	b
	adc	c
	ld	(_drw_Ln),a
	jp	_drw_gllp
lb_rex:
	pop	bc
	pop	af
	pop	hl
	ret

	

_drw_f:		.db #0
_drw_B:		.db #0
_drw_N:		.db #0
_drw_sg:	.db #0	; 0 no sign, 1="+", 2="-"
_drw_Ln:	.db #0
_drw_m:		.db #0
_drw_YX:	.dw #0
_drw_Scale:	.db #1


