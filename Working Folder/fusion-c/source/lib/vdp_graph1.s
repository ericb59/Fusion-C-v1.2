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
;
;	VDPgraph1.h	- MSX1 graphics VDP functions (also MSX2 compatible)
;
;	Compile on SDCC for MSX
;
;	This works on MSXDOS, not BASIC ROM
;	Copy pixel data from memory RAM to video memory VRAM by using chip built in functions.
;	No slow BASIC graphics, real game must load data, manage rectangles of pixels in memory.
;
;	SCREEN 5 mode resolution 256 pixels x 212 lines x 16 colours (32kB for screen)
;
	.area _CODE

	
GRPACX		.equ	#0xFCB7
GRPACY		.equ	#0xFCB9 
FORCLR		.equ  	#0xF3E9         ; foreground color 


.area	_CODE
;---------------------------------------- 2 pixel writing/reading mode	
;
; VDP write/read at address
;
_pos_XY:		; Procedure calculates hl offset and sets VDP for writing
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


_VDPread1:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0050		; SETRD Sets up the VDP for reading with HL address
	pop	ix
	ret

_VDPwrite1:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0053		; SETWRT Sets up the VDP for writing with HL address
	pop	ix
	ret   



;****************************************************************
; Get pattern at x,y.

;
; int Get8px(int X, int Y);
;
_Get8px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	call	_igB8px
	ld	l,a
	ld	h,#0
	pop ix
	ret
;
; int Get1px(int X, int Y);
;
_Get1px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	call	_iget1px
	pop ix
	ld	l,a
	ld	h,#0
	ret
_iget1px:
	ld	hl,#_px8bits
	ld	b,#0
	ld	a,d
	and	#7
	ld	c,a
	add	hl,bc
	ld	b,(hl)
	call	_igB8px
	and	b
	ret
	
;****************************************************************
; Set pattern at x,y.

;
; void set8px(int X, int Y);
;
_Set8px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld b,#0xFF
	call	_iset8px
	pop ix
	ret
;
; void set1px(int X, int Y);
;
_Set1px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	call	_iset1px
	pop ix
	ret
_iset1px:
	ld	hl,#_px8bits
	ld	b,#0
	ld	a,d
	and	#7
	ld	c,a
	add	hl,bc
	ld	b,(hl)
	call	_iset8px
	ret
	
_igB8px:		; get byte from VRAM
	push	bc
	call	_pos_XY
	call	_VDPread1
	call	_VDPget8pixels
	pop	bc
	ret
	
_iset8px:
	call	_igB8px
	or	b
	ld	c,a
	call	_VDPwrite1
	ld	a,c		; write new pattern of 8 pixels
	call	_VDPput8pixels
	ret

_VDPput8pixels:		; Put 8 pixels by sending one byte
	out (#0x98),a		; send this sequentially
	ret

_VDPget8pixels:           	; Get 8 pixels
	in	a,(#0x98)	; read this sequentially
	ret
;****************************************************************
; Clear pattern at x,y.

;
; void Clear8px(int X, int Y);
;
_Clear8px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld b,#0x00
	call	_iset8px
	pop ix
	ret
	
;
; void Clear1px(int X, int Y);
;
_Clear1px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	call	_iclear1px
	pop ix
	ret
_iclear1px:
	ld	hl,#_px8bits
	ld	b,#0
	ld	a,d
	and	#7
	ld	c,a
	add	hl,bc
	
	ld	a,(hl)
	xor	#0xFF		; invert
	ld	b,a
	call	_igB8px
	and	b		; remove pixel
	ld	c,a
	call	_VDPwrite1
	ld	a,c		; write new pattern of 8 pixels
	call	_VDPput8pixels	
	ret
	
	; .area	_XDATA

_px8bits:	.db #0x80,#0x40,#0x20,#0x10,#0x08,#0x04,#0x02,#0x01

.area	_CODE

;****************************************************************
; Get colour at x,y.
;	void	GetCol8px( int x, int y, pxColor *C )
;
_GetCol8px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld l,8(ix)
	ld h,9(ix)
	push	hl
	pop	ix
	call	_igetCol8px
	ld	b,a
	xor	a
	ld	1(ix),#0
	ld	3(ix),#0
	ld	a,b
	and	#0x0F
	ld	2(ix),a
	ld	a,b
	and	#0xF0
	rra
	rra
	rra
	rra
	ld	0(ix),a
	pop ix
	ret
	
_igetCol8px:
	call	_pos_XY
	ld	bc,#0x2000	; colours are mapped +0x2000
	add	hl,bc
	call	_VDPread1
	call	_VDPget8pixels
	ret

;****************************************************************
; Set colour at x,y.
;	void	setCol8px( int x, int y, pxColor *C )
;
_SetCol8px::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld l,8(ix)
	ld h,9(ix)
	push	hl
	pop	ix
	ld	a,0(ix)
	rla
	rla
	rla
	rla
	or	2(ix)
	call	_isetCol8px
	pop ix
	ret
	
_isetCol8px:
	push	af
	call	_pos_XY
	ld	bc,#0x2000	; colours are mapped +0x2000
	add	hl,bc
	call	_VDPwrite1
	pop	af
	call	_VDPput8pixels
	ret	


	 
; This prepares for "pixeling",  HL contains initial address
_VDPwrite:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0053		; SETWRT Sets up the VDP for writing with HL address
	pop	ix
	ret         




_VDPread:
	xor	a
	push	ix
	.db	#0xF7	; RST 30h
	.db	#0x80
	.dw	#0x0050		; SETRD Sets up the VDP for reading with HL address
	pop	ix
	ret
	

	


;****************************************************************
;	Set coloured pixel at x,y
;	Ignore 8pt same colour scheme. 
;
;	void	PSET( int x, int y, int color )
;

_SC2Pset::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	ld b,8(ix)
	call	_ipset
	pop ix
	ret
_ipset:	
	push	bc
	ld	a,(#_ln_y8px)
	or	a
	jr	z,lb_lnpset
	xor	a
	ld	(#_ln_y8px),a
	ld b,#0xFF
	call	_iset8px
	jr	lb_lnpcol
lb_lnpset:	
	call	_iset1px
lb_lnpcol:
	call	_igetCol8px
	and	#0x0F
	ld	c,a
	pop	af
	and	#0x0F
	rla
	rla
	rla
	rla
	or	c
	ld	c,a
	call	_VDPwrite
	ld	a,c
	call	_VDPput8pixels
	ret

_ipsetsv:			; saves registers
	push	de
	push	hl
	push	bc
	call	_ipset
	pop	bc
	pop	hl
	pop	de
	ret
	
;****************************************************************
;	Get coloured pixel at x,y
;	Ignore 8pt same colour scheme. 
;
;	int	POINT( int x, int y )
;
		
_SC2Point::
	push ix
	ld ix,#0
	add ix,sp
	ld d,4(ix)
	ld e,6(ix)
	call	_ipoint
	pop ix
	ld	l,a
	ld	h,#0
	ret
_ipoint:	
	call	_iget1px
	or	a
	jr	z,lb_Pt0
	call	_igetCol8px
	rra
	rra
	rra
	rra
	jr	lb_PtExt
lb_Pt0:
	call	_igetCol8px
lb_PtExt:
	and	#0x0F
	ret
	
; ----------------------------------------------------------
; void LINE( int X,  int Y, int X2,  int Y2, int color )
; ----------------------------------------------------------

_SC2Line::
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-12
	add	hl,sp
	ld	sp,hl
;c#132: unsigned char xo = ( X2<X ? 1 : 0 );
	ld	a,8 (ix)
	sub	a, 4 (ix)
	ld	a,9 (ix)
	sbc	a, 5 (ix)
	jp	PO, lb_ln87
	xor	a, #0x80
lb_ln87:
	jp	P,lb_ln26
	ld	c,#0x01
	jr	lb_ln27
lb_ln26:
	ld	c,#0x00
lb_ln27:
;c#133: unsigned char yo = ( Y2<Y ? 1 : 0 );
	ld	a,10 (ix)
	sub	a, 6 (ix)
	ld	a,11 (ix)
	sbc	a, 7 (ix)
	jp	PO, lb_ln88
	xor	a, #0x80
lb_ln88:
	jp	P,lb_ln28
	ld	a,#0x01
	jr	lb_ln29
lb_ln28:
	ld	a,#0x00
lb_ln29:
	ld	-12 (ix),a
;c#135: xx = (xo==0 ? X2-X : X-X2);
	ld	a,c
	sub	a,#0x01
	ld	a,#0x00
	rla
	ld	d,a
	ld	h,8 (ix)
	ld	l,4 (ix)
	ld	a,d
	or	a, a
	jr	Z,lb_ln30
	ld	a,h
	sub	a, l
	jr	lb_ln31
lb_ln30:
	ld	a,l
	sub	a, h
lb_ln31:
	ld	-4 (ix),a
;c#136: yy = (yo==0 ? Y2-Y : Y-Y2);
	ld	a,-12 (ix)
	sub	a,#0x01
	ld	a,#0x00
	rla
	ld	d,a
	ld	h,10 (ix)
	ld	l,6 (ix)
	ld	a,d
	or	a, a
	jr	Z,lb_ln32
	ld	a,h
	sub	a, l
	jr	lb_ln33
lb_ln32:
	ld	a,l
	sub	a, h
lb_ln33:
	ld	h,a
;c#137: dx = xx; dy = yy;
	ld	a,-4 (ix)
	ld	-3 (ix),a
	ld	-2 (ix),#0x00
	ld	e,h
	ld	d,#0x00
;c#138: if(xx>yy)
	ld	a,h
	
	push	af
	; save flag if yy==0
	or	a
	jr	z, lb_lny0
	xor	a
	dec	a
lb_lny0:
	inc	a
	ld	(#_ln_yy0),a
	pop	af
	
	sub	a, -4 (ix)
	ld	a,#0x00
	rla
	ld	-1 (ix), a
	or	a, a
	jr	Z,lb_ln02
;c#140: n = xx;
	ld	a,-4 (ix)
	ld	-7 (ix),a
;c#141: dy<<=8;
	ld	h,e
	ld	l,#0x00
;c#142: k = dy/dx;
	push	bc
	ld	e,-3 (ix)
	ld	d,-2 (ix)
	
	ld	d,e
	call	Div_HL_D
	
	pop	bc
	ld	-9 (ix),l
	ld	-8 (ix),h
	jr	lb_ln46
lb_ln02:
;c#146: n = yy;
	ld	-7 (ix),h
;c#147: dx<<=8;
	ld	h,-3 (ix)
	ld	l,#0x00
;c#148: k = dx/dy;
	push	bc
	
	ld	a,e
	or	a
	jr	z,lb_lnDv0	; skip /0
	ld	d,e
	call	Div_HL_D
lb_lnDv0:
	
	pop	bc
	ld	-9 (ix),l
	ld	-8 (ix),h
;c#152: while(i<=n)
lb_ln46:
	ld	e,#0x00
lb_ln21:
	ld	a,-7 (ix)
	sub	a, e
	jp	C,lb_ln24
;c#154: r = i;
	ld	-11 (ix),e
	ld	-10 (ix),#0x00
;c#155: xp = X; yp = Y;
	ld	a,4 (ix)
	ld	-5 (ix),a
	ld	a,6 (ix)
	ld	-6 (ix),a
;c#156: if(n!=0)
	ld	a,-7 (ix)
	or	a, a
	jp	Z,lb_ln20
;c#160: j=(unsigned char)((r*k)>>8);
	push	bc
	push	de
	ld	e,-9 (ix)
	ld	d,-8 (ix)
	ld	a,-11 (ix)
	call	DE_Times_A
	pop	de
	pop	bc
;c#161: if(xx>yy)
	ld	a,-1 (ix)
	or	a, a
	jr	Z,lb_ln17
;c#163: if(xo==0) xp += i; else xp -= i;
	ld	a,c
	or	a, a
	jr	NZ,lb_ln05
	ld	a,-5 (ix)
	add	a, e	
	ld	-5 (ix),a

	; this makes faster when horizontal lines (for filled rectangles)
	
	ld	a,(#_ln_yy0)
	or	a
	jr	z,lb_lnNo0
	
	ld	a,-7 (ix)	; if(i+7<=n)
	sub	#7
	jp	C,lb_lnNo0	
	sub	e
	jp	C,lb_lnNo0

	ld	a,-5 (ix)	; if((xp&7)==0)
	and	#7
	jr	nz,lb_lnNo0
	
	ld	a,c		; if right direction
	or	a
	jr	nz,lb_lnNo0
	
	jr	lb_lnf1

	
lb_lnf1:
	ld	a, e		; then i+=7
	add	#7
	ld	e, a
	
	ld	a,#1
	ld	(#_ln_y8px),a
	
lb_lnNo0:
	; ----- 8px pset is much faster
	
	jr	lb_ln06
lb_ln05:
	ld	a,-5 (ix)
	sub	a, e
	ld	-5 (ix),a

	; this makes faster when horizontal lines (for filled rectangles)
	
	ld	a,(#_ln_yy0)
	or	a
	jr	z,lb_lnNo0
	
	ld	a,-7 (ix)	; if(i+7<=n)
	sub	#7
	jp	C,lb_lnNo0
	sub	e
	jp	C,lb_lnNo0

	ld	a,-5 (ix)	; if((xp&7)==0)
	and	#7
	cp	#7
	jr	nz,lb_lnNo0
	
	ld	a,c		; if left direction
	or	a
	jr	z,lb_lnNo0
	
	jr	lb_lnf1
	
lb_ln06:
;c#164: if(yo==0) yp += j; else yp -= j;
	ld	a,-12 (ix)
	or	a, a
	jr	NZ,lb_ln08
	ld	a,-6 (ix)
	add	a, h
	ld	-6 (ix),a
	jr	lb_ln20
lb_ln08:
	ld	a,-6 (ix)
	sub	a, h
	ld	-6 (ix),a
	jr	lb_ln20
lb_ln17:
;c#168: if(yo==0) yp += i; else yp -= i;
	ld	a,-12 (ix)
	or	a, a
	jr	NZ,lb_ln11
	ld	a,-6 (ix)
	add	a, e
	ld	-6 (ix),a
	jr	lb_ln12
lb_ln11:
	ld	a,-6 (ix)
	sub	a, e
	ld	-6 (ix),a
lb_ln12:
;c#169: if(xo==0) xp += j; else xp -= j;
	ld	a,c
	or	a, a
	jr	NZ,lb_ln14
	ld	a,-5 (ix)
	add	a, h
	ld	-5 (ix),a
	jr	lb_ln20
lb_ln14:
	ld	a,-5 (ix)
	sub	a, h
	ld	-5 (ix),a
lb_ln20:
;c#172: PSET( xp, yp, color );

	push	bc
	push	de

	ld	e,-6 (ix)
	ld	d,-5 (ix)
	ld	b,12 (ix)
	
	call	_ipset

	pop	de
	pop	bc
	
;c#173: i++;
	inc	e
	jp	lb_ln21
lb_ln24:
	ld	sp, ix
	pop	ix
	ret

_iline:
	ld	c,b
	ld	b,#0
	push	bc
	ld	c,e
	push	bc
	ld	c,d
	push	bc
	ld	c,l
	push	bc
	ld	c,h
	push	bc
	call	_SC2Line
	ld	hl,#10
	add	hl,sp
	ld	sp,hl
	ret


DE_Times_A:			; HL = DE * A
	ld	hl,#0		; Use HL to store the product
	ld	b,#8		; Eight bits to check
ml_loop:
	add	hl,hl
	rlca			; Check most-significant bit of accumulator
	jr	nc,ml_skip	; If zero, skip addition
	add	hl,de
ml_skip:
	djnz	ml_loop
	ret

Div_HL_D:			; HL = HL / D, A = remainder
	xor	a		; Clear upper eight bits of AHL
	ld	b, #16		; Sixteen bits in dividend
div_loop:
	add	hl,hl		; Do a SLA HL. If the upper bit was 1, the c flag is set
	rla			; This moves the upper bits of the dividend into A
	jr	c, div_overflow
	cp	d		; Check if we can subtract the divisor
	jr	c, div_skip	; Carry means D > A
div_overflow:
	sub	d		; Do subtraction for real this time
	inc	l		; Set the next bit of the quotient (currently bit 0)
div_skip:
	djnz	div_loop
	ret

;---------------------------
; 
;	Draws rectangle (H,L)-(D,E) with color B, log-op A
;
_SC2Rect::
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
	
	ld	a,(#_rect_fill)
	or	a
	ret	z
			; fill rectangle with colour
lb_rcLp:	
	ld	a,l
	sub	e
	jr	z, lb_rcOK
	jr	nc,lb_rcm
	inc	l
	inc	l
lb_rcm:
	dec	l
	push	de
	ld	e,l
	call	lb_rctl
	pop	de
	jr	lb_rcLp
lb_rcOK:
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


	;  .area	_XDATA
_rect_fill:	.db #0
_lpaintHL:	.dw #0
_ln_yy0:	.db #0
_ln_y8px:	.db #0
	.area	_CODE
	
;---------------------------
; Paints from the point (x:H, y:L), color:E
; Not 100% correct. Basic has better PAINT code.
; There is SRCH (colour code search) VDP algorithm that
; should be implemented here instead of this crap.
;
_SC2Paint::
	
	push ix
	ld ix,#0
	add ix,sp
	
	ld h,4(ix)
	ld l,6(ix)
	ld e,8(ix)
	pop ix
	
		; this is way to limit stack depth, it is better then dumb crash
_ptAgain:	
	ld	bc, #0x2000	; calculate depth, limit
	call	_paintRR
	
	ld	hl,(#_lpaintHL)
	ld	a,b
	or	c
	jr	z,_ptAgain	; if bc==0 then try paint at last position
	ret
	
_paintRR:
	push	hl
	push	de
	push	bc
	ld	b,e
	ex	de,hl		; de = X,Y
	call	_ipset
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
	cp	#191
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
	call	_iget1px
	pop	bc
	pop	de
	pop	hl
	or	a
	jr	nz,lb_rbc
	jp	_paintRR 
lb_rbc:
	inc	bc
	ret




		.area	_CODE
;
; DRAW function with syntax
;
_SC2Draw::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	
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
	ld	c,#191
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

	; .area	_XDATA

_drw_f:		.db #0
_drw_B:		.db #0
_drw_N:		.db #0
_drw_sg:	.db #0	; 0 no sign, 1="+", 2="-"
_drw_Ln:	.db #0
_drw_m:		.db #0
_drw_YX:	.dw #0
_drw_Scale:	.db #1

	