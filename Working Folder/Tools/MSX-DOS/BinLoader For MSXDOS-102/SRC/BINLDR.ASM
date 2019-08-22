
; BINARY FILE LOADER  v1.02 By GDX

; Assembled with Zasm cross assembler
; http://sourceforge.net/projects/zasm/

;*S 16	; Remove the left semicolon of this line to assemble it with GEN80

BDOS	equ	0005h
RDSLT	equ	000Ch
CALSLT	equ	001Ch
ENASLT	equ	0024h
FCB1	equ	005Ch

SETT32	equ	007Bh
CHGCPU	equ	0180h

MTOFF	equ	04029h

NMBDRV	equ	0F347h
MASTERS	equ	0F348h
LINL40	equ	0F3AEh
LINLEN	equ	0F3B0h
BUF	equ	0F55Eh
CSRSW	equ	0FCA9h
EXPTBL	equ	0FCC1h
H.TIMI	equ	0FD9Fh
H.POSD	equ	0FEBCh
H.PHYD	equ	0FFA7h
H.FORM	equ	0FFACh
DEVICE	equ	0FD99h

	org	00100H

	jp	START

Title_TXT:
	db	"Object Binary File Loader",10,13
	db	"v1.02 by gdx",13,10,'$'
HELP_TXT:
	db	10,13,"Use: BINLOAD <FileName[.BIN]>",10,13,10
	db	"Key functions during loading:",10,13
	db	"[JIKOU] = R800 mode with ROM",10,13
	db	"[SHIFT] = Remove the drives",10,13 
	db	"[SELECT] = Drives keep on",10,13,'$' 
SELKEY:
	db	255
START:
	xor	a
	ld	(CSRSW),a	; Remove the cursor

	ld	de,Title_TXT
	ld	c,9
	call	BDOS

	ld	a,(FCB1+1)
	cp	020h
	ld	de,HELP_TXT
	jp	z,BCK2DOS	; Back to DOS if no file name

	ld	a,(FCB1+9)
	cp	020h
	jp	nz,LOAD
	ld	a,'B'
	ld	(FCB1+9),a
	ld	a,'I'
	ld	(FCB1+10),a
	ld	a,'N'
	ld	(FCB1+11),a
	
LOAD:	ld	de,Loading_TXT
	ld	c,9
	call	BDOS		;print"loading..."

	ld	bc,24
	ld	de,FCB1+13
	ld	hl,FCB1+12
	ld	(hl),b
	ldir			; Reset the second half of FCB

	ld	de,FCB1
	ld	c,0FH
	call	BDOS		; Open the binary file
	ld	hl,1
	ld	(FCB1+14),hl	; Record size = 1 byte
	ld	de,FileNotFound_TXT
	or	a
	jp	nz,BCK2DOS	; Jump if File not found

	ld	c,01Ah
	ld	de,BIN_File_Data
	call	BDOS		; DMA Buffer = BIN_File_Data

	ld	hl,(FCB1+16)	; Block length = File size
	ld	c,027h
	ld	de,FCB1
	call	BDOS		; Read the binary file
	cp	1
	ld	de,ReadError_TXT
	jp	z,BCK2DOS	; Jump if read error

	ld	hl,BIN_File_Data
	ld	a,(hl)
	cp	0FEh
	ld	de,Bad_Format_TXT
	jp	nz,BCK2DOS	; Back to DOS if not binary file

	ld	b,255
TIMI_Wait:
	push	bc
	call	H.TIMI
	pop	bc
	djnz	TIMI_Wait

	ld	a,39
	ld	(LINL40),a	; Width 39 when the SCREEN 0 is set

	ld	ix,SETT32
	ld	iy,(EXPTBL-1)
	call	CALSLT		; Set SCREEN1 mode

	di
	in	a,(0AAH)
	and	0F0H
	add	a,7		; Row 7
	out	(0AAH),A
	ei
	in	a,(0A9H)	; Read the keyboard matrix
	ld	(SELKEY),a
	bit	6,a
	jp	z,NO_SDOWN	; Jump if [SELECT] key is pressed
	di
	in	a,(0AAH)
	and	0F0H
	add	a,6		; Row 6
	out	(0AAH),A
	ei
	in	a,(0A9H)	; Read the keyboard matrix
	bit	0,a
	jp	nz,RD_ROW11	; Jump if [SHIFT] key is not pressed

; Shut Down Disk Drive

SDOWN:
	ld	a,(MASTERS)
	ld	hl,MTOFF
	push	hl
	call	RDSLT
	pop	ix
	and	a
	jp	z,NO_SDOWN

	ld	iy,(MASTERS-1)
	call	CALSLT
NO_SDOWN:
	ld	a,(SELKEY)
	bit	6,a
	jp	z,RD_ROW11	; Jump if [SELECT] key is pressed

	xor	a
	ld	(NMBDRV),a
	ld	(MASTERS),a
	ld	a,255
	ld	(DEVICE),a

; Set R800 Mode

RD_ROW11:
	ld	a,(EXPTBL)
	ld	hl,02Dh
	call	RDSLT		; Read MSX version
	cp	3
	jr	c,NotTurboR	; Jump if not Turbo R

	di
	in	a,(0AAh)
	and	0F0h
	add	a,11		; Matrix row 11
	out	(0AAh),a
	ei
	in	a,(0A9h)
	bit	1,a
	ld	a,081H		; R800 mode with ROM
	jr	z,CPU		; Jump if [JIKOU] key is pressed
	dec	a		; Z80 mode
CPU:
	ld	ix,CHGCPU
	ld	iy,(EXPTBL-1)
	call	CALSLT		; CPU change

NotTurboR:
	di
	ld	a,(MASTERS)
	or	a
	jr	z,SDOWN2	; Jump if no disk
	ld	sp,(0F672h)	; Stack with disk(s)
	jp	MoveData
SDOWN2:
	ld	a,0C9h
	ld	(H.TIMI),a
	ld	(H.POSD),a
	ld	(H.PHYD),a
	ld	(H.FORM),a
	
	ld	sp,0F2FFh	; Stack without disk
	ld	bc,0F2FEh-08000h
	ld	hl,08001h
	ld	de,08002h
	ld	(hl),0FFh
	ldir			; Clear the RAM if disks are removed

MoveData:
	ld	hl,(FCB1+16)	; Block length = File size
	xor	a
	ld	bc,7
	sbc	hl,bc
	ld	b,h
	ld	c,l		; BC = Binary data size

	ld	hl,(BIN_File_Data+1)
	add	hl,bc
	ld	d,h
	ld	e,l		; DE = Last address of the binary data

	ld	hl,BIN_File_Data+7	; Start address of binary file
	add	hl,bc
	inc	bc
	lddr

BIN_OK:

	ld	hl,(BIN_File_Data+5)
	push	hl

	ld	hl,JUMP1
	ld	de,04001h
	ld	bc,JUMP2-JUMP1
	ldir

	ld	hl,JUMP2
	ld	de,BUF
	ld	bc,BCK2DOS-JUMP2
	ldir
	jp	04001h

; Select Main-ROM on bank 0000h~3FFFh & C000h~FFFFh then
; run the binary program

JUMP1:
	ld	a,(0FFFFh)
	cpl
	and	0F0h
	ld	e,a		; Store current secondary slot numbers for RAM on bank C000h~FFFFh

	in	a,(0A8h)
	and	0F0h
	ld	c,a		; Store current primary slot numbers for RAM on bank C000h~FFFFh
	ld	a,(EXPTBL)
	ld	(04000h),a	; Store Main-Rom slot number
	and	03h
	ld	b,a
	rrca
	rrca
	or	b
	ld	b,a
	in	a,(0A8h)
	and	03Ch
	or	b
	out	(0A8h),a	; Select primary slot of Main-ROM on banks 0000h~3FFFh & C000h~FFFFh

	ld	a,(04000h)	; restore Main-Rom slot number
	and	0Ch
	rrca
	rrca
	rrca
	rrca
	ld	b,a		; Store	secondary slot number of Main-ROM for bank 0000h~3FFFh
	ld	a,(0FFFFh)
	cpl
	and	03Fh
	or	b
	ld	(0FFFFh),a	; Select secondary slot of Main-ROM on bank 0000h~3FFFh

	in	a,(0A8h)
	and	00Fh
	or	c		; Restore primary slot numbers for RAM
	out	(0A8h),a	; Select primary slot of RAM

	ld	a,(0FFFFh)
	cpl
	and	00Fh
	or	e
	ld	(0FFFFh),a	; Select secondary slot of Main-ROM on bank 0000h~3FFFh

	jp	BUF
JUMP2:	
	ld	a,(EXPTBL)
	ld	h,40h
	jp	ENASLT

BCK2DOS:
	ld	c,9
	call	BDOS
	ret			; Back to MSX-DOS

Loading_TXT:
	db	10,13,"Loading...",13,'$'

FileNotFound_TXT:
	db	"File not found!!!",10,13,'$'
ReadError_TXT
	db	"Read error!!!",10,13,'$'
Bad_Format_TXT:
	db	"Bad format!!!",10,13,'$'

BIN_File_Data:

END: