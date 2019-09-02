; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.2 - August 2019                      |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|   G9klib is Fusion-C Library for GFX9000 Graphic Card     |
;|                                                           |
;|    This G9KLIB is a port of the ASM G9KLIB by Team Bomba  |
;|            https://www.teambomba.net/gfx9klib.html        |
;|                      By Sylvain Cregut                    |
;\___________________________________________________________/
;		 
;
;   This G9KLIB is currently in Beta Version. No manual Available at this time. 
;

	.include "g9kmacros.inc"
	.include "g9kbdos.inc"
	.include "g9klib.inc"

	.module G9k

	.area _CODE

;----------------------------------------------------------------------------;
; Math functions                                                             ;
;----------------------------------------------------------------------------;

; Function: BCMulDE32
; Purpose:  Multiply 16-bit values (with 32-bit result)
; In: Multiply BC with DE
; Result BC:HL
; Modfies AF,BC,HL

BCMulDE32:
	LD A,C
	LD C,B
	LD HL,#0
	LD 		B,#16
.BCMLoop:
	ADD		HL,HL
	RLA
	RL    C
	JR    NC,.BCMNoAdd
	ADD   HL,DE
	ADC   A,#0
	JP    NC,.BCMNoAdd
	INC   C
.BCMNoAdd:
	DJNZ .BCMLoop
	LD    B,C
	LD    C,A
	RET

;----------------------------------------------------------------------------;
; String functions                                                           ;
;----------------------------------------------------------------------------;

;  input  HL = String 1
;         DE = String 2
;          B = Length string
;  output  nz = not equal  z=equal
StringCompare:
	LD		A,(DE)
	CP      A,(HL)
    RET     NZ
    INC     DE
    INC     HL
    DJNZ    StringCompare
    RET

;----------------------------------------------------------------------------;
; Compare functions                                                          ;
;----------------------------------------------------------------------------;

; test if HL>=BC (DE)
; Input      HL, BC
; Output     CF=0 -> true (HL>=BC)
;            CF=1 -> false (HL<BC)
cmpgte:
	ld a,h
	xor b
	jp m,cmpgte2
	sbc hl,bc
	jr nc,cmpgte3
cmpgte1:
	scf             ;false
;	ld hl,#0
	ret
cmpgte2:
	bit #7,b
	jr z,cmpgte1
cmpgte3:
	or a            ;true
;	ld hl,#1
	ret

;----------------------------------------------------------------------------;
; File functions                                                             ;
;----------------------------------------------------------------------------;

ExplainError:
; input     :  A = error number
; output    :  ERROR_STRING filled with error txt
; Modifies  :  DE,C
	PUSH	AF
    PUSH    HL
    PUSH    BC
	LD	B,A
    LD      DE,#ERROR_STRING
    BdosCall #_EXPLAIN
    POP     BC
    POP     HL
    POP     AF
    RET

FileOpen:
; Function :   Open file
; input    :   DE = Pointer to file name
;          :    A = File open flags
; output   :    A = Error Number
;          :    B = File Handle
;          :   ERROR_STRING filled with error txt
;          :   nz = error
; Modifies :   All
	BdosCall #_OPEN
	JP	NZ,ExplainError
	RET

FileRead:
; Function :   Read from file
; input    :   DE = Pointer to buffer
;          :   HL = Length
;          :    B = File Handle
; output   :    A = Error Number
;          :   ERROR_STRING filled with error txt
;          :   nz = Error
;          :   HL=Bytes read
; Modifies :   AF,HL,DE
	PUSH	DE
	PUSH	BC
    BdosCall #_READ
	CALL	NZ,ExplainError
    POP     BC
    POP		DE
	RET

FileClose:
; Function :   close file
;          :   B = File Handle
; output   :   A = Error Number
;          :   ERROR_STRING filled with error txt
; Modifies :   All
	BdosCall #_CLOSE
	JP	NZ,ExplainError
	RET


;----------------------------------------------------------------------------;
; BitBuster functions                                                        ;
;----------------------------------------------------------------------------;

; FUNCTION:	depack_raw
;	Depack data that was packed with Bitbuster.
;	Decompresses the RAW data, i.e. the data that is storead after the block
;	count and block size!
;
; ENTRY:
;	HL - Address of packed data
;	DE - Address to depack to
;
; EXIT:
;	HL - Address of first byte after compressed data
;	DE - Address of first byte after decompressed data
; MODIFIES:
;	#AF, BC, BC', DE, DE', HL, HL'#
;
depack_raw:

    ld	a,#128

depack_loop:
    GET_BIT_FROM_BITSTREAM		; get compression type bit
	jp	c,output_compressed	; if set, we got lz77 compression
	ldi				; copy byte from compressed data to destination (literal byte)

;	IFDEF	BITBUSTER_OPTIMIZE_SPEED
	GET_BIT_FROM_BITSTREAM		; get compression type bit
	jp	c,output_compressed	; if set, we got lz77 compression
	ldi				; copy byte from compressed data to destination (literal byte)
	GET_BIT_FROM_BITSTREAM		; get compression type bit
	jp	c,output_compressed	; if set, we got lz77 compression
	ldi				; copy byte from compressed data to destination (literal byte)
;	ENDIF
	jp	depack_loop

;handle compressed data
output_compressed:
; calculate length value
	ld	bc,#1
get_gamma_value:
  	GET_BIT_FROM_BITSTREAM   	; get more bits
  	jr 	nc,get_gamma_value_end
  	GET_BIT_FROM_BITSTREAM   	; get next bit of value from bitstream
  	rl	c
  	rl	b
  	jp 	nc,get_gamma_value  	; repeat unless overflow occurred (=end of block marker)
  	ret

get_gamma_value_end:
  	inc 	bc  			; length was stored as length-2 so correct this

	ld	(source_length),bc

	ld	c,(hl)			; get lowest 7 bits of offset, plus offset extension bit
	inc	hl			; to next byte in compressed data

	ld	b,#0
	bit	7,c
	jr	z,output_match1		; no need to get extra bits if carry not set

	GET_BIT_FROM_BITSTREAM		; get offset bit 10
	rl	b
	GET_BIT_FROM_BITSTREAM		; get offset bit 9
	rl	b
	GET_BIT_FROM_BITSTREAM		; get offset bit 8
	rl	b
	GET_BIT_FROM_BITSTREAM		; get offset bit 7

	jp	c,output_match2		; since extension mark already makes bit 7 set
	res	7,c			; only clear it if the bit should be cleared
output_match1:
	scf
output_match2:
	push	hl			; address compressed data on stack

	ld	h,d
	ld	l,e			; destination address in HL...
	sbc	hl,bc			; calculate source address

	ld	bc,(source_length)

	ldir				; transfer data

	pop	hl			; address compressed data back from stack

;		IFDEF	BITBUSTER_OPTIMIZE_SPEED
	GET_BIT_FROM_BITSTREAM		; get compression type bit
	jp	c,output_compressed	; if set, we got lz77 compression
	ldi				; copy byte from compressed data to destination (literal byte)
	GET_BIT_FROM_BITSTREAM		; get compression type bit
	jp	c,output_compressed	; if set, we got lz77 compression
	ldi				; copy byte from compressed data to destination (literal byte)
;		ENDIF

	jp	depack_loop

;----------------------------------------------------------------------------;
; Gfx9000 general functions                                                  ;
;----------------------------------------------------------------------------;

_G9SetVramWrite::
; Function :    Sets Vram write address
; Input    : 	E:HL VRAM address
; Output   : 	none
; Modifies : 	A,C
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld l,5(ix)
	ld h,6(ix)
	pop ix

SetVramWrite:
	LD      A,#G9K_WRITE_ADDR
	OUT     (G9K_REG_SELECT),A
    LD      C,#G9K_REG_DATA
    OUT     (C),L
    OUT     (C),H
    OUT     (C),E
    RET

_G9SetVramRead::
; Function :    Sets Vram read address
; Input    : 	E:HL VRAM address
; Output   : 	none
; Modifies : 	A,C
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld l,5(ix)
	ld h,6(ix)
	pop ix

SetVramRead:
	LD      A,#G9K_READ_ADDR
	OUT     (G9K_REG_SELECT),A
	LD      C,#G9K_REG_DATA
	OUT     (C),L
	OUT     (C),H
	OUT     (C),E
	RET

;;	unsigned char g9k_Detect(void);
;	return 0 if GFX9000 is detected
_G9Detect::
; Input    :    none
; Output   :    A=0 detected / A<>0 not detected
; Modifies :  A,C,E,H,L,F
; Works but could be saver
	LD	E,#0
	LD	H,E
	LD	L,E
	CALL	SetVramWrite
	LD	A,#0xA2
	OUT (G9K_VRAM),A
	LD      E,#0
	LD	  H,E
	LD      L,E
	CALL    SetVramRead
	IN      A,(G9K_VRAM)
	SUB		#0xA2
	LD L,A
	RET

_G9WriteReg::
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	ld b,5(ix)
	pop ix

	OUT	(G9K_REG_SELECT),A
	LD	A,B
	OUT	(G9K_REG_DATA),A
	RET

_G9DisplayEnable::
; Modifies A
	G9kReadReg G9K_CTRL+G9K_DIS_INC_READ
	OR      A,#G9K_CTRL_DISP
	OUT     (G9K_REG_DATA),A
	RET

_G9DisplayDisable::
; Modifies A
	G9kReadReg G9K_CTRL+G9K_DIS_INC_READ
	AND A,#255-G9K_CTRL_DISP
	OUT     (G9K_REG_DATA),A
	RET

_G9SpritesEnable::
; Modifies A
	G9kReadReg G9K_CTRL+G9K_DIS_INC_READ
	AND     A,#(255-G9K_CTRL_DIS_SPD)
	OUT     (G9K_REG_DATA),A
	RET

_G9SpritesDisable::
; Modifies A
	G9kReadReg G9K_CTRL+G9K_DIS_INC_READ
	OR      A,#G9K_CTRL_DIS_SPD
	OUT     (G9K_REG_DATA),A
	RET

_G9WaitVsync::
; Wait for Vsync
wvsync1:
	IN      A,(G9K_STATUS)
	AND     A,#G9K_STATUS_VR
	JR	NZ,wvsync1
wvsync2:
	IN      A,(G9K_STATUS)
	AND     A,#G9K_STATUS_VR
	JR	Z,wvsync2
	RET

; ********************************************************************************
; less than 257 bytes
; HL: RAM address
; ADE: VRAM address
; B: #bytes
; modifies: A,HL,BC
; does not change the value of A in the VRAM address, beware!
; ********************************************************************************
rvshrt:
	push af

; setup vram address
	xor a
	out (G9K_REG_SELECT),a
	ld a,e
	out (G9K_REG_DATA),a
	ld a,d
	out (G9K_REG_DATA),a
	pop af
	out (G9K_REG_DATA),a

	ld c,#G9K_VRAM
	otir
	ret

; ********************************************************************************
; Disables all sprites by setting PR1 bit of the sprite attribute
; and resetting all the remaining bits
; modifies: A,BC,DE,HL
; ********************************************************************************
_G9DisableAllSprites::
	ld b,#125
	ld de,#0xfe00
ldas:
	push bc
	ld hl,#clear_sprite
	ld b,#4
	ld a,#3
	call rvshrt
	inc de
	inc de
	inc de
	inc de
	pop bc
	djnz ldas
	ret

_G9SetScrollX::
; Set Horizontal scroll
; In:         HL =>        Left X-position
; Uses:       AF, HL
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	PUSH	HL
	LD	A,#G9K_SCROLL_LOW_X
	OUT	(#G9K_REG_SELECT),a
	LD	A,L
	AND	A,#7
	OUT     (#G9K_REG_DATA),A
	LD	A,L
	SRL	H
	RRA
	SRL	H
	RRA
	SRL	H
	RRA
	OUT     (#G9K_REG_DATA),A
	POP	HL
	RET

_G9SetScrollY::
; Function :    Sets Scroll Y
; Input    : 	HL = Y
; Output   : 	none
; Modifies : 	A
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_SCROLL_LOW_Y
	LD	A,(#SCROLL_MODE)
	OR	A,H
	OUT	(#G9K_REG_DATA),A
	RET


_G9SetScrollXB::
; Set Horizontal scroll
; In:         HL =>        Left X-position
; Uses:       AF, BC, HL
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	LD	A,#G9K_SCROLL_LOW_X_B
	OUT	(#G9K_REG_SELECT),a
	LD	A,L
	AND	A,#7
	OUT     (#G9K_REG_DATA),A
	LD	A,L
	SRL	H
	RRA
	SRL	H
	RRA
	SRL	H
	RRA
	OUT     (#G9K_REG_DATA),A
	RET


_G9SetScrollYB::
; Function :    Sets Scroll Y Layer B
; Input    : 	HL = Y
; Output   : 	none
; Modifies : 	A
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_SCROLL_LOW_Y_B
	LD	A,H
	OUT	(G9K_REG_DATA),A
	RET


_G9SetScrollMode::
; Vertical Scroll mode of Gfx9000
;
; In:         AF =>   A -> 0 = Roll by image space size
;                          1 = Roll by 256 lines
;                          2 = Roll by 512 lines
; Uses:       AF, B
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	pop ix

	AND	A,#3
	RRCA
	RRCA
	LD	(#SCROLL_MODE),A
	RET


_G9SetCmdWriteMask::
; HL = mask value
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_WRITE_MASK
	LD      A,H
	OUT     (G9K_REG_DATA),A
	RET

_G9SetBackDropColor::
; Change back drop color on Gfx9000
; In:         A =>   A -> Back drop color
; Uses:       AF
;
	LD	A,#G9K_BACK_DROP_COLOR
	OUT	(G9K_REG_SELECT),A
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	pop ix

	AND	A,#63
	OUT	(G9K_REG_DATA),A
	RET

_G9SetCmdColor::
; HL = color value
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_FC
	LD      A,H
	OUT     (G9K_REG_DATA),A
	RET

_G9SetCmdBackColor::
; HL = color value
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_BC
	LD      A,H
	OUT     (G9K_REG_DATA),A
	RET

_G9Reset::
; Function :    Resets v9990, Deletes Palette, Sprites off,stops current blit operation, puts V9990 in correct RAM config and disables display
; Input    :    None
; Output   :    None
; Notes    :    Doesn t change current adjust
; Modifies : 	A,B

	G9kReadReg G9K_DISPLAY_ADJUST+G9K_DIS_INC_READ
	PUSH    AF      ; Save adjust value

	; Set reset state
	LD      A,#G9K_SYS_CTRL_SRS
	OUT     (G9K_SYS_CTRL),A
	; Clear reset state
	XOR     A,A
	OUT     (G9K_SYS_CTRL),A

	POP     AF
	OUT     (G9K_REG_DATA),A        	; Restore adjust value

	G9kWriteReg G9K_OPCODE,G9K_OPCODE_STOP

	G9kWriteReg G9K_CTRL,G9K_CTRL_DIS_SPD+G9K_CTRL_VRAM512

	; Clear current palette
	G9kWriteReg G9K_PALETTE_PTR,#0       ; A becomes 0
	LD      B,#192 ; 64 x 3
__rstPalette:
	OUT     (G9K_PALETTE),A
	DJNZ    __rstPalette
	OUT     (G9K_OUTPUT_CTRL),A    	   ; Set output GFX9000
	RET

;----------------------------------------------------------------------------;
; Gfx9000 palette functions                                                  ;
;----------------------------------------------------------------------------;
_G9WritePalette::
 ;  HL=pointer to palette data
 ;   C=Palette pointer offset
 ;   B=Number of bytes to write
 ;  Modifies : AF,BC,HL
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix

WritePalette:
	G9kWriteRegC G9K_PALETTE_PTR
	LD	C,#G9K_PALETTE
	OTIR
	RET

_G9ReadPalette::
 ;  HL=pointer to buffer wherin palette data is stored
 ;   C=Palette pointer offset
 ;   B=Number of bytes to read
 ;
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix

ReadPalette:
	G9kWriteRegC G9K_PALETTE_PTR
	LD	C,#G9K_PALETTE
	INIR
	RET

;----------------------------------------------------------------------------;
; Gfx9000 blitter functions                                                  ;
;----------------------------------------------------------------------------;

_G9DrawFilledBox::
; HL = pointer to parameters (format: left,top,width,height)
; DE = Color

	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix) ; X1 => HL
	ld (_GDX),hl
	ld c,8(ix)
	ld b,9(ix) ; X2 => BC

	call cmpgte
; CF=0 -> true (HL>=BC) (BC>HL)
; CF=1 -> false (HL<BC) X1 < X2
	jr C,dfb1
; X1 >= X2
; DX = _GNX = X1-X2
	ld (_GNX),hl
; DIX = 1
	ld d,#G9K_ARG_DIX ; d will be used for ARG register (DIY DIX MAJ)
	jr dfb2
dfb1:
; X1 < X2
; DX = _GNX = X2-X1
	ld c,4(ix)
	ld b,5(ix)  ; X1 => BC
	ld l,8(ix)
	ld h,9(ix)  ; X2 => HL
	and a    ; RESET CARRY FLAG
	sbc hl,bc
	ld (_GNX),hl
; DIX = 0
	ld d,#0 ; rest DIY DIX MAJ
dfb2:
	ld l,6(ix)
	ld h,7(ix)  ; Y1 => HL
	ld (_GDY),hl
	ld c,10(ix)
	ld b,11(ix) ; Y2 => BC

	call cmpgte
; CF=0 -> true (HL>=BC) (BC>HL)
; CF=1 -> false (HL<BC) Y1 < Y2
	jr C,dfb3
; Y1 >= Y2
; DY = _GNY = Y1-Y2
	ld (_GNY),hl
; DIY = 1
	set 3,d ; set DIY
	jr dfb4
dfb3:
; Y1 < Y2
; DY = _GNY = Y2-Y1
	ld c,6(ix)
	ld b,7(ix)  ; Y1 => BC
	ld l,10(ix)
	ld h,11(ix) ; Y2 => HL
	sbc hl,bc
	inc hl ; to be verified
	ld (_GNY),hl
; DIY = 0

dfb4:
	ld bc,(_GNX)

; GNX must be greater than GNY
; compares HL versus BC
;
	call cmpgte
; CF=0 -> true (HL>=BC)
; CF=1 -> false (HL<BC) GNY < GNX
	JR C,dfbNdl
	set 0,d ; set MAJ
	ld hl,(_GNY)
	ld (_GNX),hl
	ld (_GNY),bc
dfbNdl:
	G9kWriteRegD G9K_ARG
	ld e,12(ix)
	ld d,13(ix) ; DE => couleur
	pop ix
DrawFilledBox:
	LD      A,#G9K_DS_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((8*256)+G9K_REG_DATA)
	LD		HL,#_GDATA
	G9kCmdWait
	OTIR
	LD      A,#G9K_FC
	OUT     (G9K_REG_SELECT),A
	OUT     (C),E
	OUT     (C),D   ; Set color

; LMMV (Logical Move to Memory from VDP)
; The VRAM rectangle area is painted out by using the color code.
; Set DX, DY, NX, NY, DIY, DIX, FC finally and OP.

	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LMMV
	RET

_G9CmdWait::
;
_wait0:
	IN	A,(G9K_STATUS)
	RRA
	JR	C,_wait0
	RET


_G9DrawLine::
; HL = pointer to parameters
; DE = Color
; X1,Y1,X2,Y2,couleur
	push ix
	ld ix,#0
	add ix,sp

	ld l,4(ix)
	ld h,5(ix) ; X1 => HL
	ld (_GDX),hl
	ld c,8(ix)
	ld b,9(ix) ; X2 => BC

	call cmpgte
; CF=0 -> true (HL>=BC) (BC>HL)
; CF=1 -> false (HL<BC) X1 < X2
	jr C,s1
; X1 >= X2
; DX = _GNX = X1-X2
	ld (_GNX),hl
; DIX = 1
	ld d,#G9K_ARG_DIX ; d will be used for ARG register (DIY DIX MAJ)
	jr s2
s1:
; X1 < X2
; DX = _GNX = X2-X1
	ld c,4(ix)
	ld b,5(ix)  ; X1 => BC
	ld l,8(ix)
	ld h,9(ix)  ; X2 => HL
	and a    ; RESET CARRY FLAG
	sbc hl,bc
	ld (_GNX),hl
; DIX = 0
	ld d,#0 ; rest DIY DIX MAJ
s2:
	ld l,6(ix)
	ld h,7(ix)  ; Y1 => HL
	ld (_GDY),hl
	ld c,10(ix)
	ld b,11(ix) ; Y2 => BC

	call cmpgte
; CF=0 -> true (HL>=BC) (BC>HL)
; CF=1 -> false (HL<BC) Y1 < Y2
	jr C,s3
; Y1 >= Y2
; DY = _GNY = Y1-Y2
	ld (_GNY),hl
; DIY = 1
	set 3,d ; set DIY
	jr s4
s3:
; Y1 < Y2
; DY = _GNY = Y2-Y1
	ld c,6(ix)
	ld b,7(ix)  ; Y1 => BC
	ld l,10(ix)
	ld h,11(ix) ; Y2 => HL
	sbc hl,bc
	ld (_GNY),hl
; DIY = 0

s4:
	ld bc,(_GNX)

; GNX must be greater than GNY
; compares HL versus BC
;
	call cmpgte
; CF=0 -> true (HL>=BC)
; CF=1 -> false (HL<BC) GNY < GNX
	JR C,Ndl
	set 0,d ; set MAJ
	ld hl,(_GNY)
	ld (_GNX),hl
	ld (_GNY),bc
Ndl:
	G9kWriteRegD G9K_ARG
	ld e,12(ix)
	ld d,13(ix) ; DE => couleur
	pop ix
DrawLine:
	LD      A,#G9K_DS_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((8*256)+G9K_REG_DATA)
	LD		HL,#_GDATA
	G9kCmdWait
	OTIR
	LD      A,#G9K_FC
	OUT     (G9K_REG_SELECT),A
	OUT     (C),E
	OUT     (C),D   ; Set color
; A straight line is drawn from the reference point on VRAM.
; Set DX, DY, MJ, MI, DIY, DIX, MAJ, FC and finally OP
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LINE
	RET


_G9SetPoint::
; HL = pointer to parameters
; DE = Color

	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld (_GDX),hl
	ld l,6(ix)
	ld h,7(ix)
	ld (_GDY),hl
	ld d,8(ix)
	ld e,9(ix)

	pop ix

	LD      A,#G9K_DS_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((4*256)+G9K_REG_DATA) ; 4 for G9K_DS_X  (2) and G9K_DS_Y (2)
	LD		HL,#_GDATA
	G9kCmdWait
	OTIR
	LD      A,#G9K_FC ; color set
	OUT     (G9K_REG_SELECT),A
	OUT     (C),E
	OUT     (C),D   ; Set color
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_PSET
	RET


_G9GetPoint::
; inputs : int X int Y
; output : color
; 2bpp   b7    b6	 b5 b4 b3 b2 b1 b0
;       color code	 unspecified
;
; 4bpp  b7 b6 b5 b4    b3 b2 b1 b0
;        color code    unspecified
;
; 8bpp   b7 b6 b5 b4 b3 b2 b1 b0
;              color code
;
; 16bpp     b7 b6 b5 b4 b3 b2 b1 b0
; 1st byte    color code low  byte
; 2nd byte    color code high byte

	push ix
	ld ix,#0
	add ix,sp

	ld l,4(ix)
	ld h,5(ix)
	ld (_GDX),hl
	ld l,6(ix)
	ld h,7(ix)
	ld (_GDY),hl

	pop ix

	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((4*256)+G9K_REG_DATA) ; 4 for G9K_SC_X  (2) and G9K_SC_Y (2)
	LD		HL,#_GDATA
	G9kCmdWait
	OTIR
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_POINT
	LD      C,#G9K_CMD_DATA
	IN 		H,(C)
	IN 		L,(C)
	RET


_G9SetupCopyRamToXY::
; Input   : HL = pointer to parameters (format: left,top,width,height)
; Modifies: A,BC,HL
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

SetupCopyRamToXY:
	LD      A,#G9K_DS_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((8*256)+G9K_REG_DATA)
	G9kCmdWait
	OTIR
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LMMC
	RET

_G9CopyRamToXY::
; HL = Pointer to data
; BC = Bytes to copy
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix

CopyRamToXY:
	LD      A,B
	LD      B,C
	INC     B
	DEC     B
	JR      Z,_crtxy1
    INC    A
_crtxy1:
    LD      C,#G9K_CMD_DATA
    DI
_crtxy2:
    OTIR
    DEC     A
    JP   	NZ,_crtxy2
    EI
    RET

_G9SetupCopyXYToRam::
; HL = pointer to parameters (format: left,top,width,height)
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

SetupCopyXYToRam:
	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((4*256)+G9K_REG_DATA)
	G9kCmdWait
	OTIR
	LD      B,#4
	LD      A,#G9K_NX
	OUT     (G9K_REG_SELECT),A
	OTIR
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LMCM
	RET


_G9CopyXYToXY::
; HL = Pointer to data  (format: SourceX,SourceY,DestX,DestY,Width,height) struct G9K_COPY_XY_XY
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

CopyXYToXY:
	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((12*256)+G9K_REG_DATA)
	G9kCmdWait
	OTIR
	; TODO : set DIY DIX
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LMMM
	RET

_G9CopyXYToRegisterXY::
; HL = Pointer to G9B_BOX struct
; IX = Dest X
; IY = Dest Y
	push ix
	push iy
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	ld e,8(ix)
	ld d,9(ix)

	ld ix,#0
	add ix,bc
	ld iy,#0
	add iy,de

	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      C,#G9K_REG_DATA
	G9kCmdWait

	OUTI
	OUTI	; SourceX
	OUTI
	OUTI	; SourceY
;	LD	A,IXL
	LD_A_IXL
	OUT	(G9K_REG_DATA),A
;	LD	A,IXH
	LD_A_IXH
	OUT	(G9K_REG_DATA),A; DestX
;	LD	A,IYL
	LD_A_IYL
	OUT	(G9K_REG_DATA),A
;	LD	A,IYH
	LD_A_IYH
	OUT	(G9K_REG_DATA),A; DestY
	OUTI
	OUTI	; Width
	OUTI
	OUTI	; Height
	; Set SX, SY, DX, DY, NX, NY, DIY, DIX and finally OP
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_LMMM

	pop iy
	pop ix
	RET

_G9CopyXYToVram::
; HL = Pointer to data  (format: SourceX,SourceY,DestAddress,Width,height)
; Modifies AF,HL,BC
	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((5*256)+G9K_REG_DATA) ; TODO : why 5 ? it has to be analyzed!
	G9kCmdWait
	OTIR
	OUT     (G9K_REG_DATA),A    ; dummy write
	LD      B,#6
	OTIR
	; Set SX, SY, DA, NX, NY, DIY, DIX and finally OP.
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_BMLX
	RET

_G9CopyVramToXY::
; HL = Pointer to data  (format: SourceAddress,DestX,DestY,Width,height)
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

_CopyVramToXY:
	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      BC,#((11*256)+G9K_REG_DATA) ; TODO : 11 ? to be analyzed!
	G9kCmdWait
	OUTI
	OUT     (G9K_REG_DATA),A    ; dummy write
	OTIR
	; Set SA, DX, DY, NX, NY, DIY, DIX and finally OP.
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_BMXL
	RET


_G9CopyXYToRam::
; HL = Pointer to data
; BC = Bytes to copy
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix

CopyXYToRam:
	LD      A,B
	LD      B,C
	INC     B
	DEC     B
	JR      Z,_cxytr1
	INC    A
_cxytr1:
	LD      C,#G9K_CMD_DATA
_cxytr2:
	INIR
	DEC     A
    JP   NZ,_cxytr2
   	RET

; unsigned char g9k_SetAdjust()
_G9SetAdjust::
; Function: Adjustment of Gfx9000 display
;                                                   L/T   C   R/B
; Input   :      BC =>   B -> Horizontal adjustment (-7 -> 0 -> 8)
;                        C -> Vertical adjustment   (-7 -> 0 -> 8)
; Modifies:      AF, BC
;
	push ix
	ld ix,#0
	add ix,sp
	ld c,4(ix)
	ld b,5(ix)
	pop ix

	LD	A,B
	ADD	A,#7
	CP	#16
	CCF
	JP C,SAerrorHA ; ERROR IN HORIZONTAL ADJUST
	SUB	#7
	NEG
	AND	#15
	LD	B,A
	LD	A,C
	ADD	A,#7
	CP	#16
	CCF
	JP C,SAerrorVA ; ERROR IN VERTICAL ADJUST
	SUB	#7
	NEG
	AND	#15
	RLCA
	RLCA
	RLCA
	RLCA
	OR	B
	LD	B,A
	G9kWriteRegB G9K_DISPLAY_ADJUST
	XOR A
	RET
SAerrorHA:
	LD A,#1
	RET
SAerrorVA:
	LD A,#2
	RET

;;	void g9k_SetScreenMode(char,char,char,char,char);
_G9SetScreenMode::
; A = Mode
; B = Bit per dot
; C = Image size
; D = Interlace
; E = Palette control register

; first : get parameters
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	ld b,5(ix)
	ld c,6(ix)
	ld d,7(ix)
	ld e,8(ix)
	pop ix

	LD      L,A
	ADD     A,A
	ADD     A,L     ; A  = A * 3
	LD      HL,#G9K_MODE_TABLE
	ADD_HL_A
	LD      A,#G9K_SCREEN_MODE0
	OUT     (G9K_REG_SELECT),A
	LD      A,(HL)  ; Get fixed settings for mode reg 6
	INC     HL
	OR      A,B     ; Set bits per dot
	OR      A,C     ; Set image size
	OUT     (G9K_REG_DATA),A
	LD      A,(HL)  ; Get fixed settings for mode reg 7
	INC     HL
	DEC     D
	INC     D       ; Is d 0?
	JR      Z,__jump1 ;$+4
	OR      A,#G9K_SCR1_EO+G9K_SCR1_IL
__jump1:
	OUT     (G9K_REG_DATA),A
	LD      A,(HL)
	OUT     (G9K_SYS_CTRL),A
	G9kWriteRegE G9K_PALETTE_CTRL
	RET

;----------------------------------------------------------------------------;
; Gfx9000 Tiles functions                                                    ;
;----------------------------------------------------------------------------;

_G9PrintTilesA::
; ********************************************************************************
; LAYER A
; Prints a number of tiles (with tile number indicated as a byte)
; pointed by HL, to destination DE (0-4095)
; Intended for printing small texts, scores...
; --------------------------------------------------------------------------------
; HL: ram address
; DE: destination tile
; B: number of tiles
; modifies: A,HL,BC
; ********************************************************************************
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld b,8(ix)
	pop ix

	push de
	; 2*de+c000
	ld a,d
	and #0x3f
	sla e
	rla
	add a,#0xc0
	ld d,a

	; setup vram address
	xor a
	out (G9K_REG_SELECT),a
	ld a,e
	out (G9K_REG_DATA),a
	ld a,d
	out (G9K_REG_DATA),a
	ld a,#7
	out (G9K_REG_DATA),a
lpta:
	ld a,(hl)
	out (G9K_VRAM),a
	inc hl
	xor a
	out (G9K_VRAM),a
	djnz lpta
	pop de
	ret

_G9PrintTilesB::
; ********************************************************************************
; LAYER B
; Prints a number of tiles (with tile number indicated as a byte)
; pointed by HL, to destination DE (0-4095)
; Intended for printing small texts, scores...
; --------------------------------------------------------------------------------
; HL: ram address
; DE: destination tile
; B: number of tiles
; modifies: A,HL,BC
; ********************************************************************************
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld b,8(ix)
	pop ix

	push de
	; 2*de+e000
	ld a,d
	and #0x3f
	sla e
	rla
	add a,#0xe0
	ld d,a

	; setup vram address
	xor a
	out (G9K_REG_SELECT),a
	ld a,e
	out (G9K_REG_DATA),a
	ld a,d
	out (G9K_REG_DATA),a
	ld a,#7
	out (G9K_REG_DATA),a
lptb:
	ld a,(hl)
	out (G9K_VRAM),a
	inc hl
	xor a
	out (G9K_VRAM),a
	djnz lptb
	pop de
	ret

_G9EnableLayerA::
	G9kReadReg G9K_SCROLL_HIGH_Y_B+G9K_DIS_INC_READ
	and #0x7f
	out	(G9K_REG_DATA),a
	ret

_G9DisableLayerA::
	G9kReadReg G9K_SCROLL_HIGH_Y_B+G9K_DIS_INC_READ
	or #0x80
	out	(G9K_REG_DATA),a
	ret

_G9EnableLayerB::
	G9kReadReg G9K_SCROLL_HIGH_Y_B+G9K_DIS_INC_READ
	and #0xBF
	out	(G9K_REG_DATA),a
	ret

_G9DisableLayerB::
	G9kReadReg G9K_SCROLL_HIGH_Y_B+G9K_DIS_INC_READ
	or #0x40
	out	(G9K_REG_DATA),a
	ret

;----------------------------------------------------------------------------;
; Gfx9000 Pattern mode functions                                             ;
;----------------------------------------------------------------------------;

_G9SetPatternData::
; DE = Ptr to pattern data
; HL = Pattern number
;  A = Layer number (0=A,1=B)
; Modifies: A,BC,HL
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld a,8(ix)

	PUSH	DE
	ADD		A,A
	LD		(PAT.LAYER_SEL_SC),A
	LD		(PAT.LAYER_SEL_DS),A
	ADD		HL,HL
	ADD		HL,HL
	ADD		HL,HL
	LD		A,L
	LD		(PAT.X),A
	LD		A,H
	ADD		A,A
	ADD		A,A
	ADD		A,A
	LD		(PAT.Y),A
	LD		HL,#PAT.X
	CALL 	SetupCopyRamToXY
	POP		HL
	LD		BC,#(8*8)
	CALL	CopyRamToXY
	pop ix
	RET

_G9GetPatternData::
; DE = Ptr to buffer to write pattern data in
; HL = Pattern number
;  A = Layer number (0=A,1=B)
; Modifies: A,BC,HL
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld a,8(ix)

	PUSH	DE
	ADD		A,A
	LD		(PAT.LAYER_SEL_SC),A
	LD		(PAT.LAYER_SEL_DS),A
	ADD		HL,HL
	ADD		HL,HL
	ADD		HL,HL
	LD		A,L
	LD		(PAT.X),A
	LD		A,H
	ADD		A,A
	ADD		A,A
	ADD		A,A
	LD		(PAT.Y),A
	LD		HL,#PAT.X
	CALL 	SetupCopyXYToRam
	POP		HL
	LD		BC,#(8*8)
	CALL	CopyXYToRam
	pop ix
	RET

_G9SetPattern::
; HL = Pattern number in generator table (on screen patterns 0 to A=8159,B=7679)
; DE = Pattern number in name table (on screen patterns 0 to 4095)
;  A = Layer number (0=A,1=B)
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld a,8(ix)

	PUSH	DE
	ADD		HL,HL
	LD		DE,#0xC000
	OR		A,A
	JR		Z,_sp1
	LD		DE,#0xE000
_sp1:
	ADD		HL,DE
	LD		E,#7
	CALL	SetVramWrite
	POP		DE
	LD		A,E
	OUT		(G9K_VRAM),A
	LD		A,D
	OUT		(G9K_VRAM),A
	pop ix
	RET

_G9GetPattern::
; Input  DE = Pattern number in name table (on screen patterns 0 to 4095)
; Output HL = Pattern number in generator table (on screen patterns 0 to A=8159,B=7679)
;  A = Layer number (0=A,1=B)
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)
	ld a,8(ix)

	PUSH	DE
	ADD		HL,HL
	LD		DE,#0xC000
	OR		A,A
	JR		Z,_gp1
	LD		DE,#0xE000
_gp1:
	ADD		HL,DE
	LD		E,#7
	CALL	SetVramRead
	POP		DE
	IN		A,(G9K_VRAM)
	LD		L,A
	IN		A,(G9K_VRAM)
	LD		H,A
	pop ix
	RET


; ********************************************************************************
; selects pattern mode 1
; modify: A
; ********************************************************************************
_G9InitPatternMode::
	xor a
	out (G9K_SYS_CTRL),a
	ld a,#6
	out (G9K_REG_SELECT),a
	ld a,#00
	out (G9K_REG_DATA),a
	ld a,#00
	out (G9K_REG_DATA),a
        ; this is needed in order to
        ; display vram contents
        ; instead of the backdrop color
    ld a,#0x82
    out (G9K_REG_DATA),a
    ret

; ********************************************************************************
; initializes the four palettes with the standard MSX colors
; palettes 1, 2, and 3 are dimmer versions of palette 0.
; modify: A,BC
; input: HL address of palette
; ********************************************************************************
_G9InitPalette::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld b,6(ix)

    ld a,#13
    out (G9K_REG_SELECT),a
    ld a,#0b00000100
    out (G9K_REG_DATA),a
    ;out (v9990+3),a

    ld c,#G9K_PALETTE
;        ld b,#192
    otir
	pop ix
    ret


_G9PCopyVramToVram::
; NOTE: Not finished yet
; HL = Pointer to data  (format: SrcAddress,DestAddress,NrBytes)
; Modifies AF,HL,BC
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

; not finished!

	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A
	LD      C,#G9K_REG_DATA
	G9kCmdWait
	; DestAddress address <40000h layer A
	; DestAddress address >40000h layer B
	; Addresses need to be shifted to the left
	LD	A,(HL)
	INC	HL
	ADD	A,A
	OUT     (G9K_REG_DATA),A     ; Source low
	OUT     (G9K_REG_DATA),A     ; dummy write
	LD	A,(HL)
	INC	HL
	RLA
	OUT     (G9K_REG_DATA),A     ; Source mid
	LD	A,(HL)
	INC	HL
	RLA
	OUT     (G9K_REG_DATA),A     ; Source high

	;OUTI			    ; Destination Low
	;OUT     (G9K_REG_DATA),A    ; dummy write
	;OUTI			    ; Destination mid
	;OUTI			    ; Destination High

	;OUTI			    ; Number of bytes Low
	;OUT     (G9K_REG_DATA),A    ; dummy write
	;OUTI			    ; Number of bytes mid
	;OUTI			    ; Number of bytes High

	G9kWriteReg G9K_OPCODE,G9K_OPCODE_BMLL

	RET


_G9PCopyVramToVramARegister::
; NOTE: Not finished yet
; Function: Copy vram to vram where destination is in layer A
; address value 00000h to 7FFFFh

; IYL:HL = start address
; IYH:DE = destination address
; IXL:BC = nr of bytes

; pas finie!

	LD      A,#G9K_SC_X
	OUT     (G9K_REG_SELECT),A

	; Prepare the addresses

	G9kCmdWait

	RET

;----------------------------------------------------------------------------;
; Gfx9000 Pattern mode sprite functions                                      ;
;----------------------------------------------------------------------------;

_G9SetSprite::
; B = Sprite Number (0 to 124)
	push ix
	ld ix,#0
	add ix,sp
	ld b,4(ix)
	pop ix

	; Set V9990 address = base 03FE00h + sprite number*4
	XOR	A,A
	OUT	(G9K_REG_SELECT),A
	LD	A,B
	LD	B,#0
	ADD	A,A
	ADD	A,A
	OUT	(G9K_REG_DATA),A  ; Low address
	RL	B
	LD	A,#0xFE
	ADD	A,B
	OUT	(G9K_REG_DATA),A  ; Mid
	LD	A,#0x03
	OUT	(G9K_REG_DATA),A  ; High
	RET

; ********************************************************************************
; sets the address for the sprite patterns
; reg 25: offset
; A = %000xyz0 not checked
; xyz: bits A17,A16,A15 of VRAM address
; of the sprite pattern table start
; usually A=4 (=> address is $10000)
; modify BC
; ********************************************************************************
_G9InitSpritePattern::
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	pop ix

	ld b,#G9K_PAT_GEN_TABLE
	ld c,#G9K_REG_SELECT
	out (c),b
	dec c
	out (c),a
	ret

;----------------------------------------------------------------------------;
; Gfx9000 Interrupt functions                                                ;
;----------------------------------------------------------------------------;

_G9SetIntLine::
; HL = int line
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix

	G9kWriteRegL G9K_INT_V_LINE_LO
	LD      A,H
	OUT     (G9K_REG_DATA),A
	RET

_G9CopyRamToVram::
; Function :    Data transfer from CPU to VRAM
; Input    :    HL = pointer to buffer, BC=number of bytes to send
; Output   : 	none
; Modifies :    A,B,C,HL
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix

CopyRamToVram:
	LD      A,B
	LD      B,C
	LD      C,#G9K_VRAM
	INC     B
	DEC     B
	JR      Z,_aCRTV
	INC     A
_aCRTV:
	OTIR
	DEC     A
	JP		NZ,_aCRTV
	RET

;----------------------------------------------------------------------------;
; Gfx9000 font functions                                                     ;
;----------------------------------------------------------------------------;
_G9OpenVff::
; Input  A = ram or vram.  0 = load data in vram, 1 = load font data in ram
;		 DE = Pointer to Font file name
;        IX = Pointer to VFF object
	push ix
	ld ix,#0
	add ix,sp
	ld a,4(ix)
	ld e,5(ix)
	ld d,6(ix)
	ld c,7(ix)
	ld b,8(ix)

	ld 		ix,#0
	ADD 	IX,BC
	LD		VFF_OBJECT.ramFont(IX),A
	PUSH	IX
	XOR		A,A
; Open File
; DE = Pointer to file name
; A = File open flags
	CALL	FileOpen			; Open given file
	POP		IX
	JP NZ,ovret ; Error opening

	PUSH	IX
	LD		HL,#3				; ID
	LD		DE,#VFF_DATA_ID
	CALL	FileRead			; Read the 3 first bytes of the file (header)
	POP		IX
	JP NZ,ovret ; Error reading

	PUSH	IX
	PUSH	BC					; Save file handle
	LD		B,#3
	LD		HL,#VFF_DATA_ID
	LD		DE,#VFF_ID
	CALL	StringCompare		; Compare file header with "VFF"
	LD		A,#_NOVFF
	POP		BC
	POP		IX
	JP NZ,ovret ; Not a VFF file

	LD		(IX),B				; Save file handle
;	LD 		DE,IX ; doesnt exists, replaced by PUSH / POP
	PUSH 	IX
	POP 	DE
	INC		DE
	INC		DE
	LD		HL,#21				; Load header
	CALL	FileRead
ovret:
	LD L,A
	pop ix
	RET


_G9LoadFont::
; Function	: Loads VFF font data
; Input     : IX = Pointer to VFF object
;           : IY = Font Offset Table
;  	        : DE = Pointer to font offset table, if it s zero it s not loaded
; 	        : HL = Pointer to buffer for loading, or if it s a ram font the destination address
;           : BC = Vram low word font data offset. If it s a ram font the pointer for buffer loading is used as font data offset
; Output    : Z = succeded NZ= not succeded, A= error number
	push ix
	ld ix,#0
	add ix,sp
	ld c,4(ix)
	ld b,5(ix) ; IX = Pointer to VFF object
	push bc
	ld e,6(ix)
	ld d,7(ix) ; DE = Pointer to font offset table, if it s zero it s not loaded
	ld l,8(ix)
	ld h,9(ix) ; HL = Pointer to buffer for loading, or if it s a ram font the destination address
;	ld c,10(ix)
;	ld b,11(ix) ; BC = Vram low word font data offset. If it s a ram font the pointer for buffer loading is used as font data offset

;	LD 		IX,#0
;	ADD 	IX,BC
	POP 	IX

	LD	A,VFF_OBJECT.ramFont(IX)
	OR	A,A
	JR	Z,_noRamFont
;	LD	BC,HL ; doesnt exists
	LD B,H
	LD C,L
_noRamFont:
	PUSH	IX
	PUSH	HL
	LD	VFF_OBJECT.vramOffset(IX),C		; Store Vram font offset
	LD	VFF_OBJECT.vramOffset+1(IX),B   ;
	LD	HL,#512                            ; Font offset table size
	LD	B,VFF_OBJECT.fileHandle(IX)	  ; File hanlde
	LD	VFF_OBJECT.ptrOffsetTable(IX),E  ; Store pointer to font character offset table
	LD	VFF_OBJECT.ptrOffsetTable+1(IX),D
	CALL	FileRead    		  ; Load font offset table
	POP	DE          			  ; Pointer to buffer
	POP	IX
	JP  NZ,lfret ; Return if loading failed

;	LD	HL,VFF_OBJECT.dataSize(IX)
	LD	L,VFF_OBJECT.dataSize(IX)	; yes
	LD	H,VFF_OBJECT.dataSize+1(IX)	; yes
	PUSH	HL
	PUSH	DE           ; Pointer to buffer
	CALL	FileRead
	POP	DE
	POP	HL
	JP NZ,lfret ; Loading failed

	; If it is ram font then copy to vram is skipped
	LD	A,VFF_OBJECT.ramFont(IX)
	DEC	A
	JP Z,lfret

	PUSH	DE           ; Pointer to buffer
	PUSH	HL           ; data size
;    LD	HL,VFF_OBJECT.vramOffset(IX)
    LD	L,VFF_OBJECT.vramOffset(IX)	; yes
    LD	H,VFF_OBJECT.vramOffset+1(IX)	; yes
    LD	E,#7	     ; High byte font address always 7
	CALL    SetVramWrite
	POP	BC           ; data size
	POP	HL	     ; Pointer to buffer
	CALL    CopyRamToVram
	XOR	A,A
lfret:
	LD L,A
	pop ix
	RET

_G9SetFont::
; Input IX = Pointer to VFF object
	push ix
	ld ix,#0
	add ix,sp
	ld c,4(ix)
	ld b,5(ix) ; BC = Pointer to VFF object

	PUSH	AF
	PUSH	HL
	PUSH	DE

	LD 		IX,#0
	ADD 	IX,BC

;	ld 		ix,#_VFF_OBJECT

	LD 	A,VFF_OBJECT.width(IX)
	LD 	(currentFont.width),A
	LD 	A,VFF_OBJECT.height(IX)
	LD 	(currentFont.height),A

	; Copy pointer to font offset table in print functions
;	LD	HL,VFF_OBJECT.ptrOffsetTable(IX)
	LD	L,VFF_OBJECT.ptrOffsetTable(IX)	; yes
	LD	H,VFF_OBJECT.ptrOffsetTable+1(IX)	; yes

;	.ifndef G9K_DISABLE_VFF_VRAM
	LD	(PrintStringVram.fontTablePtr+1),HL
	LD	(PutCharVram.fontTablePtr+1),HL
;	.endif

;.ifndef G9K_DISABLE_VFF_RAM
	LD	(PrintStringRam.fontTablePtr+1),HL
	LD	(PutCharRam.fontTablePtr+1),HL
;.endif

	INC	HL
	INC	HL
	LD	A,(HL)	; offset to second char is also the character size
	LD	(currentFont.charSize),A

	LD	A,VFF_OBJECT.vramOffset+#0(IX)
;.ifndef G9K_DISABLE_VFF_VRAM
	LD	(PrintStringVram.fontVramOffset+1),A
	LD	(PutCharVram.fontVramOffset+1),A
;.endif

;.ifndef G9K_DISABLE_VFF_RAM
	LD	(PrintStringRam.fontRamOffset+1),A
	LD	(PutCharRam.fontRamOffset+1),A
;.endif

	LD	A,VFF_OBJECT.vramOffset+1(IX)
;.ifndef G9K_DISABLE_VFF_VRAM
	LD	(PrintStringVram.fontVramOffset+2),A
	LD	(PutCharVram.fontVramOffset+2),A
;.endif

;.ifndef G9K_DISABLE_VFF_RAM
	LD	(PrintStringRam.fontRamOffset+2),A
	LD	(PutCharRam.fontRamOffset+2),A
;.endif

	LD	A,VFF_OBJECT.ramFont(IX)
;.ifndef G9K_DISABLE_VFF_VRAM
	LD	HL,#PrintStringVram
	LD	DE,#PutCharVram
;.endif

	OR	A,A
	JR	Z,_end

;.ifndef G9K_DISABLE_VFF_RAM
	LD	HL,#PrintStringRam
	LD	DE,#PutCharRam
;.endif
_end:
	LD	(PrintString+1),HL
	LD	(PutChar+1),DE

	POP	DE
	POP	HL
	POP	AF
	pop ix
	RET


_G9PrintStringVram::
; Function	: Print ASCIIZ String
; Input 	: DE = Pointer to txt
;			: BC = X
;  			: HL = Y
; Output        : DE = Pointer to byte after term char
;               : IX = modified to x after printed string
; Modifies      ; AF,BC,DE,HL,IX

	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	ld l,8(ix)
	ld h,9(ix)

PrintStringVram:

	push BC
	pop  IX
    LD	(currentFont.x),IX
    LD	(currentFont.y),HL

_loopPSVR:
	LD	A,#G9K_SC_X
	OUT	(#G9K_REG_SELECT),A
	LD	A,(DE)
	INC	DE
	OR	A,A
	JP Z,endPS ; End of ASCIIZ string

PrintStringVram.fontTablePtr:
	LD	BC,#0	;VFF.FONT_TABLE
	LD	L,A
	LD	H,#0
	ADD	HL,HL
	ADD	HL,BC

	LD	A,(currentFont.width)	; Next Char
	LD	C,A
	LD	B,#0
	ADD	IX,BC		; Precalculation

	LD	C,(HL)
	INC	HL
	LD	B,(HL)  ; vram offset of character
PrintStringVram.fontVramOffset:
	LD	HL,#0    ; vram font base address offset low word
	ADD	HL,BC

	LD	C,#G9K_REG_DATA
	G9kCmdWait

	OUT	(C),L   ; Vram address Low byte
	XOR A
	OUT	(C),A   ; Looks stuppid but else it doens t work on turbo r
	OUT	(C),H	; Vram addres Mid byte
	LD	HL,#currentFont.vramAddress
	OUTI            ; Vram high address
	OUTI            ; X low byte
	OUTI            ; X high byte
	OUTI            ; Y low byte
	OUTI            ; Y high byte
	OUTI            ; Width
	XOR A
	OUT	(C),A   ; Width High byte always 0
	OUTI            ; Height
	XOR A ; Height High byte always 0
	OUT	(C),A   ; Looks stuppid but else it doens t work in turbo r
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_CMMM
	LD	(currentFont.x),IX
	JP	_loopPSVR
endPS:
	pop ix
	RET

_G9PutCharVram::

PutCharVram:
; Function	: Print a character
; Input 	: E  = Character
; Output        ; IX = modified to x after printed character
; Modifies      ; AF,BC,DE,HL,IX
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)

; ! it modifies IX !
PutCharVram.fontTablePtr:
	LD	BC,#0
	LD	L,A
	LD	H,#0
	ADD	HL,HL
	ADD	HL,BC

	LD	A,#G9K_SC_X
	OUT	(#G9K_REG_SELECT),A

	LD	A,(currentFont.width)	; Next Char
	LD	C,A
	LD	B,#0
	ADD	IX,BC		; Precalculation

	LD	C,(HL)
	INC	HL
	LD	B,(HL)  ; vram offset of character
PutCharVram.fontVramOffset:
	LD	HL,#0    ; vram font base address offset low word
	ADD	HL,BC

	LD	C,#G9K_REG_DATA
   	G9kCmdWait

	OUT	(C),L    ; Vram address Low byte
;	LD	A,#0
	XOR A
	OUT	(C),A   ; Looks stuppid but else it doens t work on turbo r
	OUT	(C),H	; Vram addres Mid byte
	LD	HL,#currentFont.vramAddress
	OUTI            ; Vram high address
	OUTI            ; X low byte
	OUTI            ; X high byte
	OUTI            ; Y low byte
	OUTI            ; Y high byte
	OUTI            ; Width
	XOR A
	OUT	(C),A   ; Width High byte always 0
	OUTI            ; Height
	XOR A ; Height High byte always 0
	OUT	(C),A   ; Looks stuppid but else it doens t work in turbo r
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_CMMM
	LD	(currentFont.x),IX
	pop ix
	RET


PrintStringRam:
; Function	: Print ASCIIZ String
; Input 	: IX = X
;  		: IY = Y
;		; DE = Pointer to txt
; Output        : DE = Pointer to byte after term char
;               : IX = modified to x after printed string
; Modifies      ; AF,BC,DE,HL,IX

	LD	(currentFont.x),IX
	LD	(currentFont.y),IY

_loopPSR:
	LD	A,#G9K_DS_X
	OUT	(G9K_REG_SELECT),A
	LD	A,(DE)
	INC	DE
	OR	A,A	; End of ASCIIZ string
	RET	Z

	; Calculate the font data pointer address in ram
PrintStringRam.fontTablePtr:
	LD	BC,#0
	LD	L,A
	LD	H,#0
	ADD	HL,HL
	ADD	HL,BC

	; Precalculate next character offset
	; This is done here to minimize wasted time on waiting for the v9990 to finish
	LD	A,(currentFont.width)	; Next Char
	LD	C,A
	LD	B,#0
	ADD	IX,BC		; Precalculation

	; Get the address of the character data in ram
	LD	C,(HL)
	INC	HL
	LD	B,(HL)  ; ram offset of character
PrintStringRam.fontRamOffset:
	LD	HL,#0    ; ram font base address offset low word
	ADD	HL,BC
	PUSH	HL
	LD	C,#G9K_REG_DATA
	G9kCmdWait
	LD	HL,#currentFont.x
	OUTI            ; X low byte
	OUTI            ; X high byte
	OUTI            ; Y low byte
	OUTI            ; Y high byte
	OUTI            ; Width
	XOR A
	OUT	(C),A   ; Width High byte always 0
	OUTI            ; Height
	XOR A  ; Height High byte always 0
	OUT	(C),A   ; Looks stuppid but else it doens t work in turbo r
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_CMMC
	; Store addres of next X position
	LD	(currentFont.x),IX

	; Wait for v9990 to recieve data
	G9kWaitComReady

	; Send character data to vdp
	POP	HL	; Pointer to font data in ram
	LD	A,(currentFont.charSize)
	LD	B,A
	LD	C,#G9K_CMD_DATA
	OTIR
	JP	_loopPSR

PutCharRam:
; Function	: Print a character
; Input 	: A  = Character
; Output        ; IX = modified to x after printed character
; Modifies      ; AF,BC,DE,HL,IX
		; Calculate the font data pointer address in ram
PutCharRam.fontTablePtr:
	LD	BC,#0
	LD	L,A
	LD	H,#0
	ADD	HL,HL
	ADD	HL,BC

	LD	A,#G9K_DS_X
	OUT	(#G9K_REG_SELECT),A

	; Precalculate next character offset
	; This is done here to minimize wasted time on waiting for the v9990 to finish
	LD	A,(currentFont.width)	; Next Char
	LD	C,A
	LD	B,#0
	ADD	IX,BC		; Precalculation

	; Get the address of the character data in ram
	LD	C,(HL)
	INC	HL
	LD	B,(HL)  ; ram offset of character
PutCharRam.fontRamOffset:
	LD	HL,#0    ; ram font base address offset low word
    ADD	HL,BC
    PUSH	HL
    LD	C,#G9K_REG_DATA
   	G9kCmdWait
	LD	HL,#currentFont.x
	OUTI            ; X low byte
	OUTI            ; X high byte
	OUTI            ; Y low byte
	OUTI            ; Y high byte
	OUTI            ; Width
	XOR A
	OUT	(C),A   ; Width High byte always 0
	OUTI            ; Height
	XOR A     ; Height High byte always 0
	OUT	(C),A   ; Looks stuppid but else it doens t work in turbo r
	G9kWriteReg G9K_OPCODE,G9K_OPCODE_CMMC
	; Store addres of next X position
	LD	(currentFont.x),IX

	; Wait for v9990 to recieve data
	G9kWaitComReady

	; Send character data to vdp
	POP	HL	; Pointer to font data in ram
	LD	A,(currentFont.charSize)
	LD	B,A
	LD	C,#G9K_CMD_DATA
	OTIR
	RET

_G9Locate::
; Set X and Y position for putchar
; Value in IX is also needed for the putchar so don t overwrite it
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix
	LD	(currentFont.x),DE
	LD	(currentFont.y),BC
	RET


;----------------------------------------------------------------------------;
; Gfx9000 bitmap functions                                                   ;
;----------------------------------------------------------------------------;

_G9OpenG9B::
; Input  : DE = Pointer to file name asciiz
;        : HL = Pointer to G9B header data
; Output : NZ = error , Error code in A
; Modifies:
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix)
	ld l,6(ix)
	ld h,7(ix)

	PUSH	HL
	XOR	A,A
	CALL	FileOpen
	POP	HL
	JR NZ,endOG ; Error opening

	PUSH	HL
	LD	HL,#5		; ID + HEADER LENGTH
	LD	DE,#G9B_DATA_ID
	CALL	FileRead
	POP	HL
	JR NZ,endOG ; Error reading

	PUSH	HL
	PUSH	BC		; Save file handle
	LD	B,#3
	LD	HL,#G9B_DATA_ID
	LD	DE,#G9B_ID
	CALL	StringCompare
	LD	A,#_NOG9B
	POP	BC
	POP	HL
	JR NZ,endOG ; Not a G9B file

	LD	(HL),B		; Save handle in G9B struct
	INC	HL
	INC	HL
	EX	DE,HL
	LD	HL,(#G9B_HEADER_SIZE)
	CALL	FileRead
endOG:
	LD L,A
	pop ix

	RET

_G9ReadG9B::
; IX = Pointer to G9B object struct
; DE = Pointer to load buffer
; BC = Size of load buffer
; HL = Destination X
; IY = Destination Y
;  A = Palette offset in bytes. Only valid with palette modes. If a = 255 palette not loaded
	push ix
	ld ix,#0
	add ix,sp
	ld e,4(ix)
	ld d,5(ix) ; IX = Pointer to G9B object struct
	push de
	ld e,12(ix)
	ld d,13(ix) ; IY = Destination Y
	push de
	ld e,6(ix)
	ld d,7(ix) ; DE = Pointer to load buffer
	ld c,8(ix)
	ld b,9(ix) ; BC = Size of load buffer
	ld l,10(ix)
	ld h,11(ix) ; HL = Destination X
	ld a,14(ix) ; A = Palette offset

;	LD 		IX,#0
;	ADD 	IX,BC
	pop iy
	pop ix

	PUSH	AF	; Palette pointer
	LD	(G9B_COPY_PARMS),HL
	LD	(G9B_COPY_PARMS+2),IY

	LD	A,G9B_OBJECT.width+0(IX)
	LD	(G9B_COPY_PARMS+4),A
	LD	A,G9B_OBJECT.width+1(IX)
	LD	(G9B_COPY_PARMS+5),A		; Copy X

	LD	A,G9B_OBJECT.height+0(IX)
	LD	(G9B_COPY_PARMS+6),A
	LD	A,G9B_OBJECT.height+1(IX)
	LD	(G9B_COPY_PARMS+7),A		; Copy Y

	LD	HL,#G9B_COPY_PARMS
	POP	AF    ; Palette pointer
	PUSH	BC
	PUSH	DE

	PUSH 	AF     ; Palette pointer
	CALL	SetupCopyRamToXY
	POP	BC

	LD	C,B    ; C = palette pointer
	CALL	NZ,ReadG9BPalette

	POP	DE
	POP	BC
;	RET	NZ	      ; Error Loading palette
	JP NZ,endRG

;	LD	HL,#CopyRamToXY
;	LD	(ReadToG9K.copyMode+1),HL ; Set copy mode
	LD A,#TO_XY
	LD (CopyRamType),A

;		IFNDEF G9K_DISABLE_BITBUST
	LD	  A,G9B_OBJECT.compression(IX)
	AND	  A,#0x7F
	CP	  A,#G9B_COMPRESSION_BITBUST
	JP	  NZ,ReadG9B.Load
	CALL  G9kBitBustXY
	XOR 	A ; force no error
	JP 	  endRG
;		ENDIF

ReadG9B.Load:
	LD	A,G9B_OBJECT.dataSize+2(IX)
;	LD	HL,G9B_OBJECT.dataSize(IX)
	LD	L,G9B_OBJECT.dataSize(IX)	; yes
	LD	H,G9B_OBJECT.dataSize+1(IX)	; yes
ReadG9B.0:
	PUSH	HL
	POP	IY	       ; Save low word, because this is the last block
	OR	A,A
	SBC	HL,BC
	SBC	A,#0	       ; Calculate block size
	JR	C,ReadG9B.1    ; Jumps when last block is reached
	PUSH	AF 	       ; Size still to read high byte
	PUSH	HL     	   ; Size still to read low word
	PUSH	BC	       ; buffer size
;	LD		HL,BC	   ; Block size is buffer size
	LD H,B
	LD L,C

	LD	B,G9B_OBJECT.fileHandle(IX) ; Get file handle
	CALL	ReadToG9K
	POP	BC	       ; buffer size
	POP	HL         ; Size still to read low word
	POP	AF	       ; Size still to read high byte
	JP 	ReadG9B.0

ReadG9B.1:
	PUSH	IY
	POP	HL
	LD	B,(IX)	; Get file handle
	CALL	ReadToG9K
endRG:
	LD L,A
	pop ix
	RET


CalcG9BVramSize:
; Input IX    = Pointer to G9B header data
; Output E:HL = data size
;	LD	DE,G9B_OBJECT.width(IX)
	LD	E,G9B_OBJECT.width(IX)	; yes
	LD	D,G9B_OBJECT.width+1(IX)	; yes
;	LD	BC,G9B_OBJECT.height(IX)
	LD	C,G9B_OBJECT.height(IX)	; yes
	LD	B,G9B_OBJECT.height+1(IX)	; yes
	; Calculate number of pixels
	CALL	BCMulDE32 ; Result BC:HL
	LD	E,C
	LD	A,G9B_OBJECT.bitDepth(IX)
	JP	CalcDataSize

CalcDataSize:
;input E:HL=number of pixels
;	A=bits per dot
	CP	A,#16
	JR	Z,.calc16bit
	CP	A,#8
	RET	Z
	LD	B,#1
	CP	A,#4
	JR	Z,.shiftLoop
	LD	B,#2
.shiftLoop:
	SRL	E
	RR	H
	RR	L
	DJNZ	.shiftLoop
	RET
.calc16bit:
	ADD	HL,HL
	RL	E
	RET

ReadG9BLinear:
; IX   = Pointer to G9B header data
; DE   = Pointer to load buffer
; BC   = Size of load buffer
; IYL:HL = Vram destination address
;    A = Palette offset in bytes. Only valid with in palette modes, A = 255 palette not loaded

	PUSH	AF
	PUSH	BC
	PUSH	DE ; Pointer to buffer
;	LD	E,IYL
	LD_E_IYL
	CALL	SetVramWrite
	POP	DE
	POP	BC
	POP	AF

	PUSH	DE
	PUSH	BC
	LD	C,A    ; C = palette pointer
	CALL	NZ,ReadG9BPalette
	POP	BC
	POP	DE
	RET	NZ	      ; Error Loading palette

;	LD	HL,#CopyRamToVram
;	LD	(ReadToG9K.copyMode+1),HL
	LD A,#TO_VRAM
	LD (CopyRamType),A

;		IFNDEF G9K_DISABLE_BITBUST
	LD	  A,G9B_OBJECT.compression(IX)
	AND	  A,#0x7F
	CP	  A,#G9B_COMPRESSION_BITBUST
	JP	  Z,G9kBitBustLinear
;		ENDIF
	JP	ReadG9B.Load

ReadToG9K:
; Input B = FileHandle
;      DE = Pointer to buffer
;      HL = Bytes to read

	PUSH	DE
	PUSH	IX

	PUSH	DE  ; Pointer to buffer
	PUSH	HL  ; Bytes to read
	CALL	FileRead
	POP		BC  ; Bytes to read
	POP		HL  ; Pointer to buffer

ReadToG9K.copyMode:
	LD A,(CopyRamType)
	AND #1
	JR NZ,.toxy
	CALL	CopyRamToVram
	JP .rtgn
.toxy:
	CALL	CopyRamToXY
.rtgn:
	POP		IX
	POP		DE
	RET

ReadG9BPalette:
; IX = Pointer to G9B object struct
;  C = Palette pointer  (max 192), if 255 skip loading palette
; DE = Pointer to buffer

	INC	C
	RET	Z	; C = 255 skip loading
	DEC	C

	LD	A,G9B_OBJECT.nrColors(IX)
	OR	A,A
	RET	Z		; Nothing to load

	LD	B,A
	ADD	A,A
	ADD	A,B	; Number of colors * 3 = size on disk
	LD	H,#0
	LD	L,A
	PUSH	HL	; Bytes to read
	PUSH	DE      ; Pointer to buffer
	LD	B,G9B_OBJECT.fileHandle(IX)
	CALL	FileRead ; Saves BC
	POP	HL      ; Pointer to buffer
	POP	DE      ; Bytes to read
	RET	NZ	; Error reading palette
	LD	B,E
	JP	WritePalette


G9kBitBustXY:
;	LD	HL,#CopyRamToXY

	LD A,#TO_XY
	LD (CopyRamType),A
	JP	G9kBitBust

G9kBitBustLinear:
;	LD	HL,#CopyRamToVram
	LD A,#TO_VRAM
	LD (CopyRamType),A

G9kBitBust:
;	LD	(.copyFunction+1),HL ; code modification!

	PUSH	DE
	LD	B,G9B_OBJECT.fileHandle(IX) ; Get file handle
	LD	DE,#.BLOCKS
	LD	HL,#1
	CALL	FileRead
	POP	DE
	RET	NZ	; Read error
.BBloop:
	PUSH	DE
	LD	DE,#.BLOCK_SIZE
	LD	HL,#2
	CALL	FileRead
	POP	DE	; pointer to buffer
	RET	NZ	; Read error

	PUSH	DE      ; pointer to buffer
	LD	HL,(.BLOCK_SIZE)
	CALL	FileRead
	POP	DE      ; pointer to buffer
	RET	NZ      ; Read error

	PUSH	BC
	LD	HL,(.BLOCK_SIZE) ; bytes
	ADD	HL,DE    ; Calculate Address to depack to
	PUSH	DE       ; pointer to buffer

	PUSH	HL	 ; Pointer to decompress to
	EX	DE,HL    ; HL =  Address of packed data
	CALL	depack_raw
	; DE = Address of first byte after decompressed data
	POP	HL       ; Pointer to decompressed data

	PUSH	HL
	EX	DE,HL
	OR	A,A
	SBC	HL,DE    ; Calculate bytes to send to the vdp
;	LD	BC,HL	 ; Bytes to send
	LD	B,H
	LD  C,L
	POP	HL	 ; Pointer to decompressed data

.copyFunction:
	CALL	CopyRamToXY
	POP		DE       ; pointer to buffer
	POP		BC

	LD		A,(.BLOCKS)
	DEC		A
	LD		(.BLOCKS),A
	JP		NZ,.BBloop
	RET


_G9Close::
; IX = Pointer to G9B/VFF header object
; Modifies: B
	push ix
	ld ix,#0
	add ix,sp
	ld c,4(ix)
	ld b,5(ix) ; IX = Pointer to G9B object struct

	LD 		IX,#0
	ADD 	IX,BC
	LD		B,(IX)
	PUSH	IX
	CALL	FileClose
	POP		IX
	LD		(IX),#0 ; clear handle
	LD L,A
	pop ix
	RET


PrintString:
; JP @ set by g9k_SetFont
	JP 	PrintStringRam
PutChar:
; JP @ set by g9k_SetFont
	JP 	PutCharRam

;	.area	_XDATA

G9K_MODE_TABLE::
; Pattern mode 1      (P1)
  .db      G9K_SCR0_P1+G9K_SCR0_DTCLK4
  .db      0
  .db      G9K_SYS_CTRL_XTAL
  ; Pattern mode 2      (P2)
  .db      G9K_SCR0_P2+G9K_SCR0_DTCLK4
  .db      0
  .db      G9K_SYS_CTRL_XTAL
  ; Bitmap 256 * 212    (B1)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK4
  .db      0
  .db      G9K_SYS_CTRL_XTAL
  ; Bitmap 384 * 240    (B2)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK2
  .db      0
  .db      G9K_SYS_CTRL_MCKIN
  ; Bitmap 512 * 212    (B3)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK2
  .db      0
  .db      G9K_SYS_CTRL_XTAL
  ; Bitmap 768 * 212    (B4)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK
  .db      0
  .db      G9K_SYS_CTRL_MCKIN
  ; Bitmap 640 * 400    (B5)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK
  .db      G9K_SCR1_HSCN
  .db      G9K_SYS_CTRL_XTAL
  ; Bitmap 640 * 480    (B6)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK
  .db      G9K_SCR1_HSCN+G9K_SCR1_C25M
  .db      G9K_SYS_CTRL_XTAL
  ; Bitmap 1024 * 212   (B7) (undocumented mode)
  .db      G9K_SCR0_BITMAP+G9K_SCR0_DTCLK
  .db      0
  .db      G9K_SYS_CTRL_XTAL

clear_sprite:
	.db 0,0,0,0b00010000 ; disable sprite

CopyRamType:
	.db	TO_VRAM ; other value : TO_XY
PrintStringType:
	.db	TO_VRAM ; other value : TO_RAM
PutCharType:
	.db	TO_VRAM ; other value : TO_RAM

PAT.X::				.db	0
PAT.LAYER_SEL_SC::	.db	0
PAT.Y::				.db	0,0
PAT.WIDTH::			.db	8
PAT.LAYER_SEL_DS::	.db	0
PAT.HEIGHT::		.db	8,0

VFF_ID::			.str	"VFF"
VFF_DATA_ID::		.db		0,0,0

;currentFont
currentFont.vramAddress::	.db	7	; High Byte
currentFont.x::				.dw	0
currentFont.y::				.dw	0
currentFont.width::			.db	8	; Font Size
currentFont.height::		.db	8	;
currentFont.charSize::		.db	0	; Char size in bytes.

SCROLL_MODE:	.db	0

_GDATA::
_GDX::	.dw	0
_GDY::	.dw	0
_GNX::	.dw	0
_GNY::	.dw	0

source_length:	.dw	0		; length of source string

G9B_ID:				.str	"G9B"
G9B_DATA_ID:		.db	0,0,0
G9B_HEADER_SIZE: 	.dw	0
G9B_COPY_PARMS:		.dw	0,0,0,0

.BLOCKS:		.db	0
.BLOCK_SIZE:	.dw	0

_G9B_OBJECT:
_G9B_OBJECT.fileHandle:		.dw 0	; 2	; Dos2 file handle of the openend G9B file
_G9B_OBJECT.bitDepth:		.db 16	; 1	; 2,4,8 or 16 bit
_G9B_OBJECT.colorType:		.db 0	; 1	; 0=64 color palette mode,64=256 color fixed ,128=YJK and 192=YUV mode
_G9B_OBJECT.nrColors:		.db 16	; 1	; Number of colors in palette mode
_G9B_OBJECT.width:			.dw 0	; 2	; Width
_G9B_OBJECT.height:	    	.dw 0	; 2	; Height
_G9B_OBJECT.compression:	.db 0	; 1	; 0 = no compression, other value = compression used
_G9B_OBJECT.dataSize:		.db 0,0,0	; 3 ; 24 bit data size

_VFF_OBJECT:
_VFF_OBJECT.fileHandle: 	.dw	0	; Dos2 file handle of the openend VFF file
_VFF_OBJECT.name: 			.str "                "	; vff font name
_VFF_OBJECT.width: 			.db 0	; width
_VFF_OBJECT.height: 		.db 0	; height
_VFF_OBJECT.pitch: 			.db 0	; 0 = Fixed : 1 = Variable  (Still unsused)
_VFF_OBJECT.dataSize: 		.dw 0	; Data size
_VFF_OBJECT.ptrOffsetTable:	.dw 0	; Pointer to a font offset table
_VFF_OBJECT.vramOffset: 	.dw 0	; Pointer to base address of font in vram starting at #70000
_VFF_OBJECT.ramFont: 		.db 0	; 0 font data in vram, 1 font data in ram

ERROR_STRING::   	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
