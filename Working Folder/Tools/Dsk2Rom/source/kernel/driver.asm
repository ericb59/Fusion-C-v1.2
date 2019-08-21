; -----------------------------------------------------------------------------
; DSK2ROM - ASCII8/KonamiSCC megarom driver
; (C)2007 Vincent van Dam (vincentd@erg.verweg.com)
; -----------------------------------------------------------------------------
; based on the template by Arjen Zeilemaker (C)1992-2005 Ultrasoft.
; many thanks to Ramones for his help, support & testing (and getdpb code!)
; -----------------------------------------------------------------------------
; This driver requires a patched BDOS kernel; without the patched kernel it
; doesn't make much sense.
; -----------------------------------------------------------------------------

; symbols which can be used from the kernel

; GETSLT	get my slotid
; DIV16		divide
; GETWRK	get my workarea
; SETINT	install my interrupt handler
; PRVINT	call orginal interrupt handler
; PROMPT	prompt for phantom drive
; RAWFLG	verify flag
; $SECBUF	temporary sectorbuffer
; XFER		transfer to TPA
; DISINT	inform interrupts are being disabled
; ENAINT	inform interrupts are being enabled
; PROCNM	CALL statement name

 ; symbols which must be defined by the driver

; INIHRD	initialize diskdriver hardware
; DRIVES	how many drives are connected
; INIENV	initialize diskdriver workarea
; DSKIO		diskdriver sector i/o
; DSKCHG	diskdriver diskchange status
; GETDPB	build Drive Parameter Block
; CHOICE	get format choice string
; DSKFMT	format disk
; DSKSTP	stop diskmotor
; OEMSTA	diskdriver special call statements

; MYSIZE	size of diskdriver workarea
; SECLEN	size of biggest sector supported by the diskdriver
; DEFDPB	pointer to a default Drive Parameter Block

; errorcodes used by DSKIO, DSKCHG and GETDPB
;
; 0	write protect error
; 2	not ready error
; 4	data (crc) error
; 6	seek error
; 8	record not found error
; 10	write fault error
; 12	other error

; errorcodes used by DSKFMT
;
; 0	write protect error
; 2	not ready error
; 4	data (crc) error
; 6	seek error
; 8	record not found error
; 10	write fault error
; 12	bad parameter
; 14	insufficient memory
; 16	other error

MSXVER:		equ     $002D
WRTVDP:		equ     $0047
		
; -----------------------------------------------------------------------------
; some constants
; -----------------------------------------------------------------------------
	
MYSIZE:		equ	0		; Size of environment
SECLEN:		equ	512		; Size of biggest sector

; -----------------------------------------------------------------------------
; INIHRD
; -----------------------------------------------------------------------------
; Input:	None
; Output:	None
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

INIHRD:
	ld   a,(MSXVER)
	or   a
	ret  z
	call INIHRD_videomode
	call INIHRD_palette
	ret

; -----------------------------------------------------------------------------
; INIHRD_BASIC
; -----------------------------------------------------------------------------
; Input:	None
; Output:	None
; Changed:	No registers may be affected
; -----------------------------------------------------------------------------
	
INIHRD_BASIC:
	push hl
	push de
	push bc
	push af
	push ix
	push iy
	call INIHRD
	pop  iy
	pop  ix
	pop  af
	pop  bc
	pop  de
	pop  hl
	ret

; set ntsc/pal
	
INIHRD_videomode:	
	ld   a,(VIDEO_MODE)
	or   a
	ret  z
	dec  a
	jr   z,INIHRD_pal
INIHRD_ntsc:
	ld   bc,$0009
	jp   WRTVDP
INIHRD_pal:
	ld   bc,$0209
	jp   WRTVDP

; set msx1 palette

INIHRD_palette:	
	ld   a,(MSX1PALETTE)
	or   a
	ret  z
	ld   bc,$0010
	call WRTVDP
	ld   hl,INIHRD_palette0
	ld   bc,$209a
	otir
	ret
INIHRD_palette0:	
	dw $000,$000,$612,$634,$227,$337,$262,$637
	dw $271,$373,$562,$663,$512,$365,$666,$777

; -----------------------------------------------------------------------------
; PATCH_KERNEL_BANKSWITCH
; -----------------------------------------------------------------------------
; Input:	None
; Output:	None
; Changed:	No registers may be affected
; -----------------------------------------------------------------------------

PATCH_KERNEL_BANKSWITCH:
	push hl
	push de
	push bc
	push af
	push ix
	push iy

	ld   hl,(SAFE_BANKSWITCH)
	ld   a,h
	or   l			; no safe bankswitching
	jr   z,PATCH_KERNEL_BANKSWITCH_noswitch

	;; move routine
	push hl
	call GETWRK
	pop  de
	add  hl,de
	push hl
	ex   de,hl
	ld   bc,KERNEL_BANKSWITCH_SIZE
	ld   hl,KERNEL_BANKSWITCH
	ldir
	pop  hl
	ld   de,(XF368+1)
	ld   (XF368+1),hl
	;; 	ld   (XF36B+1),hl
	;; patch routine
	inc  hl
	ld   (hl),e
	inc  hl
	ld   (hl),d

PATCH_KERNEL_BANKSWITCH_noswitch:
	pop  iy
	pop  ix
	pop  af
	pop  bc
	pop  de
	pop  hl
	ret
	
KERNEL_BANKSWITCH:
	call ENASLT ; call original bankswitch code
	push af
	xor  a
ASCII8P1_1: equ $+1
	ld   ($5000),a
	inc  a
ASCII8P2_2: equ $+1
	ld   ($7000),a
	pop  af
	ret
KERNEL_BANKSWITCH_SIZE:	 equ $ - KERNEL_BANKSWITCH
	
; -----------------------------------------------------------------------------
; DRIVES
; -----------------------------------------------------------------------------
; Input: 	F	Zx set if to return physical drives
;			Zx reset if to return at least 2 drives, if only one
;			  physical drive it becomes a phantom drive
; Output:	L	number of drives
; Changed:	F,HL,IX,IY may be affected
;
; Remark:	DOS1 does not handle L=0 correctly
; -----------------------------------------------------------------------------

DRIVES:
	xor  a
	ld   l,1
	ret

; -----------------------------------------------------------------------------
; INIENV
; -----------------------------------------------------------------------------
; Input: 	None
; Output:	None
; Changed:	AF,BC,DE,HL,IX,IY may be affected
;
; Remark:	-
; -----------------------------------------------------------------------------

INIENV:
	ret
INTHAND:	
	jp   PRVINT

; -----------------------------------------------------------------------------
; DSKCHG
; -----------------------------------------------------------------------------
; Input: 	A	Drivenumber
; 		B	0
; 		C	Media descriptor
; 		HL	pointer to DPB
; Output:	F	Cx set for error
;			Cx reset for ok
;		A	if error, errorcode
;		B	if no error, disk change status
;			01 disk unchanged
;			00 unknown
;			FF disk changed
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; Remark:	DOS1 kernel expects the DPB updated when disk change status is
;               unknown or changed DOS2 kernel does not care if the DPB is
;               updated or not		
; -----------------------------------------------------------------------------

DSKCHG:
	or   a
	jr   nz,DSKCHG_error
	ld   b,1
	ret
DSKCHG_error:
	scf
	ld   a,12
	ret

; -----------------------------------------------------------------------------
; GETDPB
; -----------------------------------------------------------------------------
; Input: 	A	Drivenumber
; 		B	first byte of FAT
; 		C	Media descriptor
; 		HL	pointer to DPB
; Output:	[HL+1]
;		..
;		[HL+18]	updated
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

GETDPBERR:
	pop  bc
	pop  af
	ld   a,12
	scf
	ret
GETDPB:
	push af 
	push bc 
	push hl 
	ld   hl,(SECBUF)
	push hl
	ld   b,1
	ld   de,0 
	or   a 
	ld   c,0FFh 
	call DSKIO
	pop  iy 
	pop  hl 
	jr   c,GETDPBERR
	inc  hl
	push hl
	ex   de,hl
	ld   hl,DEFDPB+1
	ld   bc,18
	ldir
	pop  hl
	ld   a,(iy+21)
	cp   0F9h
	jr   z,GETDPBEND 
	ld   (hl),a
	inc  hl
	ld   a,(iy+11)
	ld   (hl),a
	inc  hl
	ld   a,(iy+12)
	ld   (hl),a
	inc  hl
	ld   (hl),0Fh
	inc  hl
	ld   (hl),04h
	inc  hl
	ld   a,(iy+0Dh)
	dec  a
	ld   (hl),a
	inc  hl
	add  a,1
	ld   b,0
GETDPB0: 
	inc  b
	rra 
	jr   nc,GETDPB0
	ld   (hl),b
	inc  hl
	push bc
	ld   a,(iy+0Eh)
	ld   (hl),a
	inc  hl
	ld   d,(iy+0Fh)
	ld   (hl),d
	inc  hl
	ld   b,(iy+010h)
	ld   (hl),b
	inc  hl
GETDPB1: 
	add  a,(iy+016h)
	jr   nc,GETDPB2
	inc  d
GETDPB2: 
	djnz GETDPB1
	ld   c,a
	ld   b,d
	ld   e,(iy+011h)
	ld   d,(iy+012h)
	ld   a,d
	or   a
	ld   a,0FEh
	jr   nz,GETDPB3
	ld   a,e
GETDPB3: 
	ld   (hl),a
	inc  hl
	dec  de
	ld   a,4
GETDPB4: 
	srl  d
	rr   e
	dec  a
	jr   nz,GETDPB4
	inc  de
	ex   de,hl
	add  hl,bc
	ex   de,hl
	ld   (hl),e
	inc  hl
	ld   (hl),d
	inc  hl
	ld   a,(iy+013h)
	sub  e
	ld   e,a
	ld   a,(iy+014h)
	sbc  a,d
	ld   d,a
	pop  af
GETDPB5: 
	dec  a
	jr   z,GETDPB6
	srl  d
	rr   e
	jr   GETDPB5
GETDPB6:
	inc  de
	ld   (hl),e
	inc  hl
	ld   (hl),d
	inc  hl
	ld   a,(iy+016h)
	ld   (hl),a
	inc  hl
	ld   (hl),c
	inc  hl
	ld   (hl),b
GETDPBEND: 
	pop  bc
	pop  af 
	xor  a
	ret

DEFDPB:
	db   0
	;; default dpb
	db   0F9h		; Media F9
	dw   512		; 80 Tracks	
	db   0Fh		; 9 sectors
	db   04h		; 2 sides
	db   01h		; 3.5" 720 Kb
	db   02h
	dw   1
	db   2
	db   112
	dw   14
	dw   714
	db   3
	dw   7
	
; -----------------------------------------------------------------------------
; CHOICE
; -----------------------------------------------------------------------------
; Input: 	None
; Output:	HL	pointer to choice string, 0 if no choice
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

CHOICE:
	ld   hl,0
	ret

; -----------------------------------------------------------------------------
; DSKFMT
; -----------------------------------------------------------------------------
; Input: 	A	choicecode (1-9)
;		D	drivenumber
;		HL	begin of workarea
;		BC	length of workarea
; Output:	F	Cx set for error
;			Cx reset for ok
;		A	if error, errorcode
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

DSKFMT:
	ld   a,16
	scf
	ret

; -----------------------------------------------------------------------------
; OEMSTATEMENT
; -----------------------------------------------------------------------------
; Input:	HL	basicpointer
; Output:	F	Cx set if statement not recognized
;			Cx reset if statement is recognized
;		HL	basicpointer,	updated if recognized
;					unchanged if not recognized
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

OEMSTA:
	scf
	ret

; -----------------------------------------------------------------------------
; DSKSTP (Not the offical name)
; -----------------------------------------------------------------------------
; Input:	None
; Output:	None
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

DSKSTP:
	ret

; -----------------------------------------------------------------------------
; DSKIO
; -----------------------------------------------------------------------------
; Input: 	A	Drivenumber
;		F	Cx reset for read
;			Cx set for write
; 		B	number of sectors
; 		C	Media descriptor
;		DE	logical sectornumber
; 		HL	transferaddress
; Output:	F	Cx set for error
;			Cx reset for ok
;		A	if error, errorcode
;		B	if error, remaining sectors
; Changed:	AF,BC,DE,HL,IX,IY may be affected
; -----------------------------------------------------------------------------

DSKIO:
	jr   c,DISKIO_error
	or   a
	jr   nz,DISKIO_error
	;; todo? test sector within range... (rnf if not)
DSKIO_loop:
	push bc
	push de
	push hl
	call DSKIO_readsector
	pop  hl
	inc  h
	inc  h
	pop  de
	inc  de
	pop  bc
	djnz DSKIO_loop
	;; b=0, all sectors read.
	;; a=0, no error, clear carry.
	xor  a
	ret
DISKIO_error:	
	ld   a,12
	scf
	ret
DSKIO_readsector:
	ld   a,(COMPRESS_MODE)
	or   a
	jr   nz,DSKIO_readsector_unpack
	;; regular uncompressed sector
	ld   a,h
	cp   $40 - HIGH SECLEN		; $4000 - SECLEN
	jr   c,DSKIO_readsector0
	and  a
	jp   m,DSKIO_readsector0
	;; sector load page 1
	push hl
	ld   hl,(SECBUF)
	push hl
	call DSKIO_readsector0
	pop  hl
	pop  de
	ld   bc,SECLEN
	jp   XFER
DSKIO_readsector0:
	push hl
	push de
	srl  d
	rr   e
	srl  d
	rr   e
	srl  d
	rr   e
	srl  d
	rr   e			; sector/16 = page
	ld   a,e
	add  2
ASCII8P1_2: equ $+1
	ld   ($5000),a
	pop  de
	ld   a,e
	and  15			; sector%16 = sector within page
	add  a,a
	add  $40
	ld   h,a
	ld   l,0
	pop  de
	ld   bc,SECLEN
	ldir
	xor  a
ASCII8P1_3: equ $+1
	ld   ($5000),a
	ret
	;; compressed dsk image
DSKIO_readsector_unpack:
	push hl
	;; get page,address and length of sector
        ld   a,2
ASCII8P1_4: equ $+1
	ld   ($5000),a
	ex   de,hl
	push hl
	add  hl,hl              ; * 2
	add  hl,hl              ; * 4
	pop  de
	add  hl,de              ; * 5
	ld   de,(SECTOR_INDEX)
	add  hl,de
	;; get sector location
	ld   a,(hl)             ; a = page
	inc  hl
	ld   e,(hl)
	inc  hl
	ld   d,(hl)             ; de = source
	inc  hl
	ld   c,(hl)
	inc  hl
	ld   b,(hl)             ; bc = length
	push bc                 ; keep to check if sector is compressed
	;; get target unpack buffer and transfer (compressed) data
	push af
	push bc
	push de
	xor  a
ASCII8P1_5: equ $+1
	ld   ($5000),a
	ld   de,(SECBUF)
	pop  hl
        pop  bc
	pop  af
ASCII8P1_6: equ $+1
	ld   ($5000),a
	ex   af,af
	;; handle page overlapping
DSKIO_readsector_unpack_loop:
	ldi
 	ld   a,b
 	or   c
	jr   z,DSKIO_readsector_unpack_loop0
        ld   a,h
	cp   $60
	jr   c,DSKIO_readsector_unpack_loop
	ld   hl,(SECTOR_INDEX)
	ex   af,af
	inc  a
ASCII8P1_7: equ $+1
	ld   ($5000),a
	ex   af,af
	jr   DSKIO_readsector_unpack_loop
DSKIO_readsector_unpack_loop0:
        xor  a
ASCII8P1_8: equ $+1
	ld   ($5000),a
        ;; check if sector is compressed
	pop  de             ; de = length of stored sector 
	ld   hl,SECLEN      ; if equal to sector length, sector not compressed
	ld   a,h
	sub  d
	jr   nz,DSKIO_unpack0
	ld   a,l
	sub  e
DSKIO_unpack0:
	push af
	pop  af
        ;; uncompress/move sector data to secbuf
	ld   hl,(SECBUF)
	jr   z,DSKIO_unpack_transfer
	;; unpack using pletter decompression
	push hl
	call GETWRK
	ex   de,hl
	pop  hl
	push de
        call pakuit
	pop  hl
	;; transfer sector from secbuf to real destination
DSKIO_unpack_transfer:
	pop  de             ; de = destination adress
	ld   bc,SECLEN
	ld   a,d
	cp   $40 - HIGH SECLEN	    ; $4000 - SECLEN
	jr   c,DSKIO_unpack_transfer0
	and  a
	jp   p,XFER
DSKIO_unpack_transfer0:
	ldir
	ret

	;; pletter decompression code
	include "unpack.asm"

; -----------------------------------------------------------------------------
; compatible mode; this table can be mirrored on every page to ensure all
; entry points are available, whatever the page status of the mapper is.
; -----------------------------------------------------------------------------

PAGE0_PATCH_TABLE:
	dw   0,0,0,0,0,0,0,0
	push hl
	jr   PATCH4010
	push hl
	jr   PATCH4013
	push hl
	jr   PATCH4016
	push hl
	jr   PATCH4019
	push hl
	jr   PATCH401C
	push hl
	jr   PATCH401F
	push hl
	jr   PATCH4022
	push hl
	jr   PATCH4025
	nop
	push hl
	jr   PATCH4029
	nop
	push hl
	jr   PATCH402D
PATCH4030:
	ld   hl,(YF34B)
	ret
	; custom patches
	push hl
	jr   PATCH4034
	push hl
	jr   PATCH4037
	push hl
	jr   PATCH403A
	push hl
	jr   PATCH403D
	push hl
	jr   PATCH4040
	push hl
	jr   PATCH4043
	push hl
	jr   PATCH4046
PATCH4010: ;dskio
	ld   l,$10
	jr   PATCH0_JUMP
PATCH4013: ;dskchg
	ld   l,$13
	jr   PATCH0_JUMP
PATCH4016: ;getdpb
	ld   l,$16
	jr   PATCH0_JUMP
PATCH4019: ;choice
	ld   l,$19
	jr   PATCH0_JUMP
PATCH401C: ;dskfmt
	ld   l,$1C
	jr   PATCH0_JUMP
PATCH401F: ;dskstp
	ld   l,$1F
	jr   PATCH0_JUMP
PATCH4022:
	ld   l,$22
	jr   PATCH0_JUMP
PATCH4025:
	ld   l,$25
	jr   PATCH0_JUMP
PATCH4029:
	ld   l,$29
	jr   PATCH0_JUMP
PATCH402D: ;getslt
	ld   l,$2D
	jr   PATCH0_JUMP
PATCH4034:
	ld   l,$34
	jr   PATCH0_JUMP
PATCH4037:
	ld   l,$37
	jr   PATCH0_JUMP
PATCH403A:
	ld   l,$3A
	jr   PATCH0_JUMP
PATCH403D:
	ld   l,$3D
	jr   PATCH0_JUMP
PATCH4040:
	ld   l,$40
	jr   PATCH0_JUMP
PATCH4043:
	ld   l,$43
	jr   PATCH0_JUMP
PATCH4046:
	ld   l,$46
	;; jr   PATCH0_JUMP
PATCH0_JUMP:
	ld   h,$40
	ex   (sp),hl
	push af
	ld   a,1
ASCII8P2_3: equ $+1
	ld   ($7000),a
	pop  af
	jp   PATCH1_JUMP
PAGE0_PATCH_TABLE_SIZE: equ $-PAGE0_PATCH_TABLE

PATCH1_JUMP:
	push af
	xor  a
ASCII8P1_9: equ $+1
	ld   ($5000),a
	pop  af
	ret

; -----------------------------------------------------------------------------
; configuration part
; -----------------------------------------------------------------------------

	defs 07F00H-$,0
COMPRESS_MODE:		db   0	
DECOMPRESS_BUFFER_SIZE:	dw   0
VIDEO_MODE:		db   0	  ; 0 = nothing, 1 = pal, 2 = ntsc.
MSX1PALETTE:		db   0	  ; 0 = normal, 1 = force msx1 palette
HOSTILE_TAKEOVER:	db   1	  ; 0 = no, 1 = make this an exclusive diskrom
SAFE_BANKSWITCH:	dw   0	  ; 0 = normal, other = offset bankswitch code
SECTOR_INDEX:		dw   $4000
TOTAL_SECTORS:		dw   0   
NO_OF_SIDES:		db   0

; this is the size and location of the high compatibility patch table.
	defs 07FA0H-$,0
PATCHTABLE_LOCATION:	dw   PAGE0_PATCH_TABLE
PATCHTABLE_SIZE:	dw   PAGE0_PATCH_TABLE_SIZE
SAFE_BANKSWITCH_SIZE:	dw   KERNEL_BANKSWITCH_SIZE
	
; these locations need to be patched in the kernel to switch from the default
; mapper scc to the ascii8 mapper.
	defs 07FB0H-$,0
ASCIIMAPPER_PATCHLIST:
        dw   ASCII8P1_1
        dw   ASCII8P1_2
        dw   ASCII8P1_3
        dw   ASCII8P1_4
        dw   ASCII8P1_5
	dw   ASCII8P1_6
	dw   ASCII8P1_7
	dw   ASCII8P1_8
	dw   ASCII8P1_9
	dw   0
	dw   ASCII8P2_1
	dw   ASCII8P2_2
	dw   ASCII8P2_3
	dw   0

; -----------------------------------------------------------------------------
; fill it up, leave signature.
; -----------------------------------------------------------------------------

	defs 07FE0H-$,0
	;;  0123456789abcdef
	db "DSK2ROM 0.80    "
	db "by joyrex 2007. "
