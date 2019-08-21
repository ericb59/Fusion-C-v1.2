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
	jr	z,lb_l2b
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
	jr	nz, lb_vrYm
				; MSX1
	ld	de, #_MSX1_err_
	ld	c, #9
	call	#5	; display "MSX1 not supported"
	ld	a,#1
	jr	lb_vrEx

lb_vrYm:
	ld	a,#2		; MSX2 and above
lb_vrEx:
	ld	l,a
	ld	h,#0
	ret

;-----------------------------------------------
; Save and restore VDP internal registers
;
_SaveVDP::
	; let's have initial 0
	ld	hl,#_VDP+#0
	ld	de,#_VDP_0+#0
	ld	bc,#8
	ldir
	ld	hl,#_VDP+#8
	ld	de,#_VDP_0+#8
	ld	bc,#17
	ldir
	ret
;-----------------------------------------------
_RestoreVDP::
	ld	bc,#0x0800
	ld	hl,#_VDP_0
	call    lb_rstVdp
	ld      bc,#0x1008
	ld      hl,#_VDP_0+#8
	call    lb_rstVdp

	; not sure, hangs sometimes on R#19h
	; Register 19h: 9958 ONLY -- Horizontal Scroll Control
	;ld      bc,#0x0319
	;ld      hl,#_Vdp000
	;call    lb_rstVdp
	ret
lb_rstVdp:
	ld      a,(hl)
	inc     hl
	di
	out     (#0x99),a
	ld      a,c
	or      #0x128
	ei
	out     (#0x99),a
	inc     c
	djnz    lb_rstVdp
	ret

	.area	_XDATA


_VDP:		.dw #0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0
_VDP_0:		.dw #0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0,#0
_Vdp000:	.db #0,#0,#0

_curPage:	.db #0
_MSX1_err_:	.ascii "MSX1 not supported by VDPgraph2$\n"
_curHL:		.dw #0
	.area	_CODE

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




;-----------------------------------------------
;Set page [0..3] in screen 5. [0..1] in screen 8
;
_SetPage::

	ld  hl,#2
	add hl,sp
	ld a,(hl)
	ld (#_curPage),a
	ld (DPPAGE),a


_iSetPage:
        add     a,a ;x32
        add     a,a
        add     a,a
        add     a,a
        add     a,a
        add     a,#31
        ld      (#_VDP+#2),a
        di
	out     (#0x99),a
        ld      a,#128+#2
        ei
	out     (#0x99),a
        ret

;-----------------------------------------------
_SetDestPage::
	ld  hl,#2
	add hl,sp
	ld a,(hl)
	ld (#_curPage),a
	ld (ACPAGE),a
	ret




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
	jr	z, lb_ahl_0
	ld	h,#0x80
lb_ahl_0:
	ret

_bumpVDP::			;Resets current page.
	call	VDPready
	ld	a,(#_curPage)
	call	_iSetPage
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
	push	hl
	call	_getAHL

        rlc	h
        rla
        rlc	h
        rla
        srl	h
        srl	h
        di
	out     (#0x99),a
        ld	a,#14+#128
	out     (#0x99),a
        ld	a,l
        nop
	out     (#0x99),a
        ld	a,h
        or	#64
        ei
	out     (#0x99),a
	pop	hl
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
	push	hl
	ld	bc,#0x6A00	;256/2 * 212 lines
	ld	hl,#0
	call	_VDPwrite
	pop	hl
	ld	de,#0
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x005C	; LDIRVM 	Block transfer to VRAM from memory

;
;	ld	bc,#0x6A00	;256/2 * 212 lines heavy direct VDP access loop
;lb_wr0:
;	ld	a,(hl)
;	inc	hl
;	out	(#0x98),a
;	dec	bc
;	ld	a,b
;	or	c
;	jr	nz, lb_wr0

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
	jr	nz, VDPready	; wait

;	rra ; sct: instruction inutile
	xor	a
	out	(#0x99),a
	ld	a,#128+#15
	out (#0x99),a
;	ei
;	jr	c,VDPready    ;wait till previous VDP execution is over (CE)
	ret


;****************************************************************
;	PSET puts pixel
;		 to use, set H, L, E, D as follows
;		 pset (x: H, y: L), color: E, logi-OP: D
;****************************************************************
;

_Pset::
	push ix
	ld ix,#0
	add ix,sp
	ld h,4(ix)
	ld l,6(ix)
	ld e,8(ix)
	ld d,10(ix)
	pop ix
_ipset:
	di
	call	VDPready
;	call	WAITVDP

	ld	a,#36
	out	(#0x99),a ; sct: attention le port VDP peut ne pas etre 0x99!
	ld	a,#128+#17
	out	(#0x99),a

	xor	a
	ld	c,#0x9b
	out	(c),h ; x
	out	(c),a
	out	(c),l ; y
	out	(c),a

	ld	a,#44
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a

	out	(c),e ; couleurs
	xor	a
	out	(c),a

	ld	e,#0b01010000 ; sct: instruction Pset
	ld	a,d	; sct: prise en compte de l operation logique
	and #0x0f ; sct: pour eviter d'ecraser le quartet fort Pset
	or	e
	out	(c),a

;	call	VDPready ; sct: inutile d attendre encore la fin de la commande video si on la prend en compte au debut de chaque commande vdp

	ei
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
	jr	nz,lb_pst2
	and	#0x0F
	ld	c,a
	ld	a,b
	rla
	rla
	rla
	rla
	or	c
	jr lb_pstS
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
	jr	nz,lb_psg2
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
;****************************************************************

_Point::
	push ix
	ld ix,#0
	add ix,sp
	ld h,4(ix)
	ld l,6(ix)
	pop ix
_ipoint:
	di
	call	VDPready

	ld	a,#32
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a

	xor	a
	ld	c,#0x9b
	out	(c),h
	out	(c),a
	out	(c),l
	out	(c),a

	out	(#0x99),a
	ld	a,#128+#45
	out	(#0x99),a
	ld	a,#0b01000000
	out	(#0x99),a
	ld	a,#128+#46
	out	(#0x99),a

	call	VDPready

	ld	a,#7
	call	hmmc_Status
	push	af
	xor	a
	call	hmmc_Status
	pop	af

	ld	l,a
	ld	h,#0
	ei
	ret

;****************************************************************
; draws LINE
;        to use, set H, L, D, E, B, A and go
;        draw LINE (H,L)-(D,E) with color B, log-op A
; H,L,D,E absolute values
;****************************************************************

_Line::
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
;
_Rect::
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

	call	VDPready

	xor	a
lb_svfl:
	ld	(#_rect_fill),a
	ld	a,c
	cp	#0xff		; if fill case
	jr	nz, lb_rNfl
	xor	a
	ld	c,a
	inc	a
	jr	lb_svfl
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

	call	VDPready
	ld	a,(#_rect_fill)
	or	a
	ret	z
			; fill rectangle with colour
lb_rcLp:
	ld	a,h
	sub	d
	jr	z, lb_rcOK
	jr	nc,lb_rcm
	inc	h
	inc	h
lb_rcm:
	dec	h
	push	de
	ld	d,h
	call	lb_rctl
	pop	de
	jr	lb_rcLp
lb_rcOK:
	call	VDPready
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
	ret

	.area	_XDATA

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

	call	VDPready

		; this is way to limit stack depth, it is better then dumb crash
_ptAgain:
	ld	bc, #0x2000	; calculate depth, limit
	call	_paintRR

	ld	hl,(#_lpaintHL)
	ld	a,b
	or	c
	jr	z,_ptAgain	; if bc==0 then try paint at last position
	call	VDPready
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
	jr	z,lb_ptX0
	dec	l
	call	recIfPaint
	inc	l
lb_ptX0:
	ld	a,h
	or	a
	jr	z,lb_ptY2
	dec	h
	call	recIfPaint
	inc	h
lb_ptY2:
	ld	a,l
	cp	#211
	jr	z,lb_ptX2
	inc	l
	call	recIfPaint
	dec	l
lb_ptX2:
	ld	a,h
	cp	#255
	jr	z,lb_ptOk
	inc	h
	call	recIfPaint
	dec	h
lb_ptOk:
	ld	a,b
	or	c
	ret	z	; if bc==0 then just return
	jr	lb_rbc

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
	jr	z,lb_rbc
	cp	e
	jr	z,lb_rbc
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
	;xor	a
	call	hmmc_wait_VDP

	ld	a,#36	;command register R#36
	out	(#0x99),a
	ld	a,#128+#17	;VDP(17)<=36
	out	(#0x99),a
	;xor	a
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

	out	(c),a		;DIX and DIY = 0
	ld	a,#0b11110000
	out	(c),a		; command to do it
	ld	a,#128+#44
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	; VDP(17)<=44
hmmc_Loop:
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
	jr	z, hmmc_Loop
	bit	#0,a		; CE? is over?
	jr	z, hmmc_exit
	inc	ix
	ld	a,(ix)
	out	(#0x9b),a
	jr	hmmc_Loop
hmmc_exit:
	xor	a
	call	hmmc_Status
	ei
	pop	ix
	ret

hmmc_Status:
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

hmmc_wait_VDP:
	ld	a,#2
	call	hmmc_Status
	and	#1
	jr	nz,hmmc_wait_VDP
	xor	a
	call	hmmc_Status
	ret




;_HMMC::
;	push ix
;	ld ix,#0
;	add ix,sp
;	ld l,4(ix)
;	ld h,5(ix)
;	push hl
;	ld h,6(ix)
;	ld l,8(ix)
;	ld d,10(ix)
;	ld e,12(ix)
;	pop ix
;
;	di
;	xor	a
;	call	hmmc_wait_VDP
;
;	ld	a,#36	;command register R#36
;	out	(#0x99),a
;	ld	a,#128+#17	;VDP(17)<=36
;	out	(#0x99),a
;	xor	a
;	ld	c,#0x9b
;	out	(c),h		;X
;	out	(c),a
;	out	(c),l		;Y
;	out	(c),a
;	out	(c),d		;DX in dots
;	out	(c),a		;
;	out	(c),e		;DY in dots
;	out	(c),a		;
;
;	ld	h,(ix)		;first byte of data
;	out	(c),h
;
;	out	(c),a		;DIX and DIY = 0
;	ld	a,#0b11110000
;	out	(c),a		; command to do it
;	ld	a,#128+#44
;	out	(#0x99),a
;	ld	a,#128+#17
;	out	(#0x99),a	; VDP(17)<=44
;hmmc_Loop:
;	ld	a,#2
;	out	(#0x99),a
;	ld	a,#0x8f
;	out	(#0x99),a
;	in	a,(#0x99)
;	ld	b,a
;	xor	a
;	out	(#0x99),a
;	ld	a,#0x8f
;	out	(#0x99),a
;	ld	a,b
;	bit	#7,a		; TR? transferring?
;	jr	z, hmmc_Loop
;	bit	#0,a		; CE? is over?
;	jr	z, hmmc_exit
;	inc	ix
;	ld	a,(ix)
;	out	(#0x9b),a
;	jr	hmmc_Loop
;hmmc_exit:
;	xor	a
;	call	hmmc_Status
;	ei
;	pop	ix
;	ret

;hmmc_Status:
;	out	(#0x99),a
;	ld	a,#0x8f
;	out	(#0x99),a
;	in	a,(#0x99)
;	push	af
;	xor	a
;	out	(#0x99),a
;	ld	a,#0x8f
;	out	(#0x99),a
;	pop	af
;	ret
;
;hmmc_wait_VDP:
;	ld	a,#2
;	call	hmmc_Status
;	and	#1
;	jr	nz,hmmc_wait_VDP
;	xor	a
;	call	hmmc_Status
;	ret





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

	ld	a,#0b10100000	;LMCM command
	out	(c),a

hmcm_Loop:
	ld	a,#2
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	bit	#0,a		; CE? is over?
	jr	z, hmcm_exit
	bit	#7,a		; TR? transferring?
	jr	z, hmcm_Loop

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

	jr	hmcm_Loop
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
	jr	nz,hmcm_wait_VDP
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
	jr	nc,lb_dxy1
	ld	d,#0b00000000
	neg
lb_dxy1:
	ld	h,a		;H := NX , D := DIX
 	ld	a,l
	sub	e
	ld	e,#0b00001000
	jr	nc,lb_dxy2
	ld	e,#0b00000000
	neg
lb_dxy2:
	ld	l,a		;L := NY , E := DIY
	xor	a
	ret





;****************************************************************
; HMCM_SC8 (High speed move VRAM to CPU)

;****************************************************************
;	.area	_XDATA
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

	ld	a,#0b10100000	;LMCM command
	out	(c),a

hmcm_Loop_SC8:
	ld	a,#2
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	bit	#0,a		; CE? is over?
	jr	z, hmcm_exit_SC8
	bit	#7,a		; TR? transferring?
	jr	z, hmcm_Loop_SC8

	; read 1 pixel !!!
	ld	a,#7		; 1px
	out	(#0x99),a
	ld	a,#0x8f
	out	(#0x99),a
	in	a,(#0x99)
	ld	(ix),a
	inc	ix
	jr	hmcm_Loop_SC8

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
	jr		nc,lb_dxy1_SC8     ;if X1>=X2 then goto lb_dxy1
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
	jr	nc,lb_dxy2_SC8		;if Y1>=Y2 then goto lb_dxy2
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
;  LMMM (High speed Logical move VRAM to VRAM)
;        to use, set H, L, D, E, B, C, A-logical operation and go
;        VRAM (H,L)-(D,E) ---> VRAM (B,C)
;        VRAM (D,E)-(H,L) ---> VRAM (B,C)	;backwards scanned
; byte DIX,DIY=0, explained:
; The 0 copies the block starting from the upper left, the 1 from right/bottom.
; what's the difference? when copying overlapping source/destination
; (a scroller for example)
; when scrolling from right to left DIX/DIY can both be 0
;  but copying from left to right DIX must be 1. just figure it out...
; Then give coord.positive from right upper corner to left.
;****************************************************************

_LMMM::
	push ix
	ld ix,#0
	add ix,sp

	ld h,4(ix)
	ld l,6(ix)
	ld d,8(ix)
	ld e,10(ix)
	ld b,12(ix)
	ld c,14(ix)
	ld a,16(ix)

	di
	push	af
	call	VDPready
	ld	a,#32
	out	(#0x99),a
	ld	a,#128+#17
	out	(#0x99),a	;R#17 := 32

	push	de
	push	bc
	xor	a
	ld	c,#0x9b
	out	(c),h		;X from
	out	(c),a
	out	(c),l		;Y from
	out	(c),a
	pop	de		; de=bc
	out	(c),d		;X to
	out	(c),a
	out	(c),e 		;Y to
	out	(c),a
	pop	de		; de=de

	call	DIX_DIY

	out	(c),h		;DX in dots
	out	(c),a
	out	(c),l		;DY in dots
	out	(c),a
	out	(c),a		;dummy

	ld	a,d
	or	e
	out	(c),a		;DIX and DIY

	pop	af
	or	#0b10010000	;LMMM command or with LOGICAL

	out	(c),a		;do it
	call	VDPready
	ei
	pop	ix
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
	jr	nz, lb_flq
	or	#0b10010000	;LMMM command
lb_flq:
	out	(c),a		; do it, VDP!
	call	VDPready

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
	jr	nz,lb_drwMmv

	ld	de,(_drw_YX)
lb_drwMmv:
	ld	a,(hl)
	or	a
	jp	z, lb_drwEx
	cp	#97
	jr	c,lb_drwUpc
	sub	#32
lb_drwUpc:
lb_drw0:
	cp	#'U'
	jr	nz, lb_drw1
	call	_drw_dcY
	jr	flb_drwQ
lb_drw1:
	cp	#'D'
	jr	nz, lb_drw2
	call	_drw_icY
	jr	flb_drwQ
lb_drw2:
	cp	#'L'
	jr	nz, lb_drw3
	call	_drw_dcX
	jr	flb_drwQ
lb_drw3:
	cp	#'R'
	jr	nz, lb_drw4
	call	_drw_icX
	jr	flb_drwQ
lb_drw4:
	cp	#'E'
	jr	nz, lb_drw5
	call	_drw_icX
	call	_drw_dcY
	jr	flb_drwQ
lb_drw5:
	cp	#'F'
	jr	nz, lb_drw6
	call	_drw_icX
	call	_drw_icY
	jr	flb_drwQ
lb_drw6:
	cp	#'G'
	jr	nz, lb_drw7
	call	_drw_dcX
	call	_drw_icY
	jr	flb_drwQ
lb_drw7:
	cp	#'H'
	jr	nz, lb_drw8
	call	_drw_dcX
	call	_drw_dcY
	jr	flb_drwQ

flb_drwQ:	jp	lb_drwQ
flb_drwQp:	jp	lb_drwQp
flb_drwScnOp:	jp	lb_drwScnOp

lb_drw8:
	cp	#'B'
	jr	nz, lb_drw9
	ld	a,#1
	ld	(_drw_B),a
	jr	flb_drwQp
lb_drw9:
	cp	#'N'
	jr	nz, lb_drw10
	ld	a,#1
	ld	(_drw_N),a
	jr	flb_drwQp

lb_drw10:
	cp	#'S'
	jr	nz, lb_drw11
	call	_drw_gLnQ		; c=scale count
	ld	a,c
	and	#252
	rrca
	rrca				; scale/4 = pixels
	or	a
	jr	nz,lb_drwScs
	inc	a
lb_drwScs:
	ld	(_drw_Scale),a
	jr	flb_drwScnOp
lb_drw11:
	cp	#'C'
	jr	nz, lb_drw12
	call	_drw_gLn		; read colour number
	ld	a,(_drw_Ln)
	ld	(#FORCLR),a
	jr	flb_drwScnOp

lb_drw12:
	cp	#'M'
	jr	nz, lb_drw13
	ld	a,#1
	ld	(_drw_m),a
	call	_drw_gLn		; read X
	ld	a,(_drw_sg)
	or	a
	jr	nz,_drw_relX
	ld	d,#0			; absolute X, not +cnt
_drw_relX:
	cp	#2
	jr	z,_drw_relmX
	call	_drw_icX
	jr	_drw_releX
_drw_relmX:
	call	_drw_dcX
_drw_releX:
	jr	lb_drwScnOp
lb_drw13:
	cp	#','
	jr	nz, lb_drw14
	call	_drw_gLn		; read Y
	ld	a,(_drw_sg)
	or	a
	jr	nz,_drw_relY
	ld	e,#0			; absolute Y, not +cnt
_drw_relY:
	cp	#2
	jr	z,_drw_relmY
	call	_drw_icY
	jr	_drw_releY
_drw_relmY:
	call	_drw_dcY
_drw_releY:
	xor	a
	ld	(_drw_m),a
	jr	lb_drwQ
lb_drw14:

	jr	lb_drwScnOp


lb_drwQ:
	ld	a,(_drw_m)	; if M command, save till ','
	or	a
	jr	nz,lb_drwQp
	ld	a,#1
	ld	(_drw_f),a
lb_drwQp:
	push	hl
	ld	hl,(_drw_YX)
	ld	a,(_drw_B)
	or	a
	jr	nz,lb_drwSkB
	ld	a,(#FORCLR)
	ld	b,a			; set colour
	ld	c,#0			; logical = 0
	call	lb_rctl			; draw line
lb_drwSkB:
	ld	a,(_drw_N)
	or	a
	jr	nz,lb_drwSkN
	ld	(_drw_YX),de		; save new position
lb_drwSkN:
	pop	hl

lb_drwScnOp:
	ld	a,(_drw_f)
	or	a
	jr	z,lb_drwcl
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
	jr	_drw_dcylpc
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
	jr	z,_drw_icyEx
	djnz	_drw_icylp
	dec	c
	dec	c
	jp	m,_drw_icyEx
	inc	c
	jr	_drw_icylpc
_drw_icyEx:
	pop	de
	ret

_drw_ldb:
	push	af
	ld	a,(_drw_m)
	or	a
	jr	z,_drw_ldb2

	ld	a,(_drw_sg)
	or	a
	jr	nz,_drw_ldb2
	ld	a,#1
	jr	_drw_ldbE
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
	jr	nz,_drw_glnq
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
	jr	z,lb_drwLnP
	cp	#'-'
	jr	z,lb_drwLnM
	xor	a
	dec	hl
	jr	_drw_Lnc
lb_drwLnP:
	ld	a,#1
	jr	_drw_Lnc
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
	jr	z,lb_rex
	sub	#'0'
	jr	c,lb_rex
	cp	#10
	jr	nc,lb_rex

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
	jr	_drw_gllp
lb_rex:
	pop	bc
	pop	af
	pop	hl
	ret

	.area	_XDATA

_drw_f:		.db #0
_drw_B:		.db #0
_drw_N:		.db #0
_drw_sg:	.db #0	; 0 no sign, 1="+", 2="-"
_drw_Ln:	.db #0
_drw_m:		.db #0
_drw_YX:	.dw #0
_drw_Scale:	.db #1


