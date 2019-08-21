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
;	vram to Ram Memory block transfert & Ram to Vram Memory Block Transfert
;
;	Port of the MSX-C ' Ascii routine
;
;	Eric Boez 2019
;

 .area _CODE
  

VDPIO   = 0x98 			;VRAM Data (Read/Write)
VDPSTATUS = 0x99 		;VDP Status Registers 
EXBRSA	=	0xFAF8
SCRMOD	=	0xFCAF
ACPAGE	=	0xFAF6
MODE	=	0xFAFC

;---------------------------
;	Return the VRAM Size of the MSX Computer 16/64/128 KB

_GetVramSize::
	push ix
	ld	a,(MODE)
	and	#0b00000110		; extract VRAM size
	ld	hl,#16			; assume 16K (MSX1)
	ret	z			; good assumption
	cp	#0b00000010		; 64K?
	ld	l,#64			; assume so
	ret	z			; good assumption
	add	hl,hl			; 128K
	pop ix
	ret

;---------------------------


;	MODULE	LDIRVM
;
;	LDIRVM	moves block from memory to the VRAM.
;
;   efinition : CopyRamToVram(void *SrcRamAddress, unsigned int DestVramAddress, unsigned int Length);
;
_CopyRamToVram::
	push ix
 
    ld   ix,#0
    add  ix,sp
	ld   E,4(ix) ; SRC RAM address  
  	ld   D,5(ix)
  	ld   L,6(ix) ; DEST VRAM address
  	ld   H,7(ix)

  	ld   C,8(ix) ; Block length
  	ld   B,9(ix)


	push	bc		; save length
	call	SETWRT
	ex	de,hl		; HL = pointer to source
	pop	de			; DE = length
					; C  = VDPIO
	ld	b,e			; set lower byte of length
	ld	a,d			; set higher byte of length
	inc	e			; is lower byte of length 0?
	dec	e
	jr	z,ldirvm_1	; yes
	inc	a
ldirvm_1:
	outi			; OTIR cannot be used (VDP may fail)
	jp	nz,ldirvm_1
	dec	a
	jp	nz,ldirvm_1
	pop  ix
	ret

;------------------------------
; MODULE	LDIRMV
;
;	LDIRMV	moves block from VRAM to memory.
;
; Definition : CopyVramToRam(unsigned int SrcVramAddress, void *DestRamAddress, unsigned int Length);
;

_CopyVramToRam::
	push ix
 
    ld   ix,#0
    add  ix,sp
	ld   E,4(ix) ; SRC VRAM address
  	ld   D,5(ix)
  	ld   L,6(ix) ; DEST RAM address 
  	ld   H,7(ix)

  	ld   C,8(ix) ; Block length
  	ld   B,9(ix)

	push	bc		; save length
	ex	de,hl		; HL = pointer to source
	call	SETRD
	ex	de,hl		; HL = pointer to destination
	pop	de			; DE = length
					; C  = VDPIO
	ld	b,e			; set lower byte of length
	ld	a,d			; set higher byte of length
	inc	e			; is lower byte of length 0?
	dec	e
	jr	z,ldirmv_1	; yes
	inc	a
ldirmv_1:
	ini				; INIR cannot be used (VDP may fail)
	jp	nz,ldirmv_1
	dec	a
	jp	nz,ldirmv_1
	pop ix
	ret
	
;------------------------------
;	MODULE	SETRD
;
;	SETRD	sets up the VDP for read.
;	Entry:	HL.
;	Exit:	None.
;
;

SETRD:
	call	SETADR
	ei
	out	(c),a
	dec	c
	ret

;-------------------------------
;	MODULE	SETWRT
;
;	SETWRT	sets up the VDP for write.
;
;
SETWRT:
	call	SETADR
	or	#0b01000000		;set the low address with write bit
	ei
	out	(c),a
	dec	c
	ret

;----------------------------
;   MODULE	SETADR
;
;	low level routine for SETRD and SETWRT
;
;	entry:		HL = read/write address
;
;	Used internally
;

SETADR:
	push	de
	ld	a,#VDPIO
	ld	c,a
	inc	c
	ld	a,(EXBRSA)			; MSX1?
	and	a
	jr	z,setaddress_1		; yes
	ld	a,h					; extract the high address
	rlca
	rlca
	and	#0b00000011
	ld	d,a					; save a15,a14
	ld	a,(SCRMOD)
	cp	#7
	ld	a,(ACPAGE)
	jr	c,set_high			; page size is 32k
	add	a,a
set_high:
	add	a,a
	add	a,d
	di
	out	(c),a				; set data to be written
	ld	a,#0x80+#14			; set it to register #14
	out	(c),a
setaddress_1:
	di
	out	(c),l				; set the low address
	ld	a,h				; prepare to set the middle address
	and	#0b00111111
	pop	de
	ret