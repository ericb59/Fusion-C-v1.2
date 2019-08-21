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
;	IO.H
;	Disk operations
;	
;	(c) 1995, SOLID MSX C
;
;	SDCC port 2015
;
;	MSXDOS1, MSXDOS2 disk operations
;

	.area _CODE
	
;--- proc FCBs
;
;  INIT for IO operations
;	Returns pointer to FCB list
;
	
_FCBs::
	call	_OS_version
	call	_prep_fcbs
	ld	hl, #__buf8_fcbs
	ret
	
;--- end of proc

;--- proc  GET_OS_VERSION
;
;	int	GetOSVersion( )
;
;
_GetOSVersion::
	call	_OS_version
	ld	h, #0
	ld	a,(#__os_ver)
	ld	l, a
	ret
	
_OS_version:	
	ld	c,#0xC
	call	#5
	cp	#5
	jr	z,lb_v1
lb_va:
	ld	bc,#0x6F
	call	#5
	ld	(#__mx_ver),de	;MSXDOS2.SYS version number
	ld	a,b		;MSX-DOS kernel version
	cp	#2
	jr	c,lb_v1
	ld	a,#2
lb_vb:
	ld	(#__os_ver),a
	ret
lb_v1:
	ld	a,#1
	jr	lb_vb
	
	;.area _DATA
	
__os_ver:: .db #0	;1 -> MSXDOS 1.X, 2-> MSXDOS2
__mx_ver:: .db #0

;--- end of proc 

	.area _CODE
	
;-
; Variables and temporary FCBs for DOS1 functions
;-

	;.area _DATA
	
__buf8_fcbs::			;allocate 8 FCBs for DOS1
	.ds	#38*#8		;304 bytes = 0xff
	
__io_errno::
	.db	#0,#0
_xfcb:
	.ds	#38
_ffcb:
	.ds	#38
	
lb_con:
	.db	#0
	.ascii "CON        "
	.db	#0

	
	.area _CODE

	
;--- proc prep_fcbs

_prep_fcbs:
	xor	a
	ld	(de),a
	
	;allocate 8 FCBs for DOS1
	ld	hl, #__buf8_fcbs
	
	;set 2 FCBs to CON
	call	_opencon
	ld	de,#38
	add	hl,de
	call	_opencon
	ret

_opencon:
	push	hl
	ex	de,hl
	push	de
	ld	hl,#lb_con
	ld	bc,#13
	ldir
	pop	de
	push	de
	ld	c,#0xF
	call	#5
	pop	ix
	ld	14(ix),#1
	ld	15(ix),#0
	pop	hl
	ret
;
;--- end of proc 
	
;--- proc 	setfcb

;	setup FCB, used for DOS1 file IO functions

__setupfcb::
	
	push	hl
	push	de
	ld	l,e
	ld	h,d
	inc	de
	ld	bc,#16
	ld	(hl),#0
	ldir
	pop	de
	pop	hl
;
	xor	a
	ld	(de),a
	inc	hl
	ld	a,(hl)
	cp	#':'
	dec	hl
	jr	nz,lb_setu5
	ld	a,(hl)
	and	#0xF
	ld	(de),a
	inc	hl
	inc	hl
lb_setu5:
	inc	de
lb_setu6:
	ld	b,#8
	call	lb_setu7
	ld	a,(hl)
	cp	#'.'
	jr	nz,lb_setu8
	inc	hl
lb_setu8:
	ld	b,#3
	call	lb_setu7
	xor	a
	ret
;
lb_setu7:
	ld	a,(hl)
	cp	#'.'
	jr	z,lb_setu9
	cp	#'*'
	jr	z,lb_setu12
	cp	#'!'
	jr	c,lb_setu9
	ld	(de),a
	inc	hl
	inc	de
	djnz	lb_setu7
	ret
lb_setu12:
	inc	hl
	ld	a,#'?'
	jr	lb_setu10
;
lb_setu9:
	ld	a,#' '
lb_setu10:
	ld	(de),a
	inc	de
	djnz	lb_setu10	
	ret
;--- end of proc 


;--- proc 	dos1fd

;	convert FD to FCB address for DOS1

_get_fd::
	
	ld	h,#0
	add	hl,hl
	ld	d,h
	ld	e,l
	add	hl,hl
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,hl
	add	hl,hl
	add	hl,de
	add	hl,bc
	ld	de, #__buf8_fcbs
	add	hl,de
	ret

;--- end of proc 

;--- proc  open
;
;	FD Open(char *name, int mode)			-- DOS1 (attributes ignored), DOS2
;	FD OpenAttrib(char *name, int mode, int attrib)	-- DOS2
;
;	Attributes are for MSXDOS2 only
;
_Open::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld bc,#0
	pop ix
	jr _iopen
	
_OpenAttrib::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	pop ix
	
_iopen::
	push	ix
	ld	a,(#__os_ver)
	or	a
	jr	nz,lb_opC
				; maybe not initiated
	push	hl
	push	de
	push	bc
	call	_OS_version
	call	_prep_fcbs
	pop	bc
	pop	de
	pop	hl
	ld	a,(#__os_ver)
	or	a
	jr	z, lb_op15	; if can not manage
lb_opC:	
	dec	a
	dec	a
	jr	z,lb_op2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_op1		; MSXDOS1

lb_op25:
	pop	ix
	or	a
	ld	(#__io_errno),a
	ld	h,#0
	ret	z
lb_op14:
	ld	hl,#-1
	ret
;
lb_op2:
	xor	a
	ld	b,c	;attributes
	bit	#0,e	;O_WRITE
	jr	nz,lb_op21
	or	#1
lb_op21:
	bit	#2,e	;O_EXCL?
	jr	z,lb_op22
	set	#7,b
lb_op22:
	bit	#5,e
	ld	c,#0x43
	jr	z,lb_op23
	bit	#4,e
	jr	z,lb_op23
	inc	c
lb_op23:
	ex	de,hl
	push	hl
	call	#5
	ld	(#__io_errno),a
	pop	de
	bit	#6,e
	jr	z,lb_op24
	push	bc
	ld	hl,#0	;O_APPEND
	ld	d,h
	ld	e,l
	ld	a,#2
	ld	c,#0x4A	; seek to end
	call	#5
	pop	bc
lb_op24:
	ld	h,#0
	ld	l,b
	jr	lb_op25
;
lb_op1:
	push	de
	push	hl		;attributes ignored in DOS-1
	ld	hl, #__buf8_fcbs
	ld	de,#38
	ld	b,#8	; 8-fcbs
lb_op11:
	ld	a,(hl)
	inc	a
	jr	z,lb_op12
	add	hl,de
	djnz	lb_op11
	pop	de
	pop	hl
lb_op15:
	pop	ix
	jr	lb_op14
;
lb_op12:
	ld	a,#8		; 8-fcbs
	sub	b
	ld	(#lb_fdd),a
	ex	de,hl
	pop	hl
	push	de
	call	__setupfcb
	pop	de
	pop	bc
	bit	#5,c	;CREATE?
	push	bc
	ld	c,#0xF
	jr	z,lb_op13
	ld	c,#0x16
lb_op13:
	push	de
	call	#5
	ld	(#__io_errno),a
	pop	de
	pop	bc
	or	a
	jr	nz,lb_op15
	ld	hl,#14
	add	hl,de
	ld	(hl),#1	;set reclen = 1
	inc	hl
	ld	(hl),#0
	push	hl
	ld	de,#18
	add	hl,de	;hl -> RANDOMREC
	ex	de,hl
	pop	hl
	inc	hl
	bit	#6,c
	jr	nz,lb_op16
	ld	hl,#lb_po0
lb_op16:
	ld	bc,#4
	ldir
	ld	hl,(#lb_fdd)
	jp	lb_op25
;
	;.area _DATA
	
lb_po0:
	.db	#0,#0,#0,#0	;pointer position for OPEN
	
;--- end of proc

	.area _CODE
	
;--- proc  close
;
;	int Close(FD fd);	-- DOS1, DOS2
;
_Close::
	
	push	ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop	ix
_iclose::
	ld	h,#0
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_cl2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_cl1		; MSXDOS1
lb_cl0:
	push	ix
	call	#5
lb_cl00:
	pop	ix
	or	a
	ld	hl,#0
	ld	(#__io_errno),a
	ret	z
	dec	hl
	ret
;
lb_cl1:
	call	_get_fd
	ex	de,hl
	ld	c,#0x10
	push	ix
	push	de
	call	#5
	pop	hl
	ld	(hl),#255
	jr	lb_cl00
;
lb_cl2:
	ld	b,l
	ld	c,#0x45
	jr	lb_cl0
;
	;.area _DATA
lb_fdd:
	.dw	#0

;--- end of proc 

	.area _CODE

;--- proc  create
;
;	FD Create(char *name)			-- DOS1(attributes ignored), DOS2
;	FD CreateAttrib(char *name, int attr)	-- DOS2
;
_Create::
	
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	ld de,#0
	jr lb_crt

_CreateAttrib::
	
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	pop ix
lb_crt:	
	ld	c,e
	ld	b,d
	ld	e,#0x39
	jp	_iopen
	
;--- end of proc 


;--- proc  read
;
;	int Read(FD fd, void *buf, int nbytes);  -- DOS1, DOS2
;
_Read::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	pop ix
	
_iread::	
	push	ix
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_rd2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_rd1		; MSXDOS1	
lb_rd3:
	call	#5
lb_rd3a:
	pop	ix
	cp	#2
	ret	c
	ld	(#__io_errno),a
	ld	hl,#0
	ret
;
lb_rd1:
	push	bc
	push	hl
	ld	c,#0x1A
	call	#5
	pop	hl
	call	_get_fd
	ex	de,hl	
	pop	hl		;byte cnt
	ld	c,#0x27
	push	de
	call	#5
	pop	ix
	bit	#7,24(ix)
	jr	z,lb_rd3a
	set	#6,24(ix)
	jr	lb_rd3a
;
lb_rd2:
	ld	a,l
	ld	l,c
	ld	h,b
	ld	b,a
	ld	c,#0x48
	jr	lb_rd3

;--- end of proc 


;--- proc  write
;
;	int Write(FD fd,void *buf,int nbytes);  -- DOS1, DOS2
;
_Write::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	pop ix
	
_iwrite::	
	push	ix
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_wr2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_wr1		; MSXDOS1

lb_wr3:
	call	#5
	pop	ix
	cp	#2
	ret	c
	ld	(#__io_errno),a
	ld	hl,#0
	ret
;
lb_wr1:
	push	bc
	push	hl
	ld	c,#0x1A
	call	#5
	pop	hl
	call	_get_fd
	ex	de,hl	
	pop	hl		;byte cnt
	ld	c,#0x26
	jr	lb_wr3
;
lb_wr2:
	ld	a,l
	ld	l,c
	ld	h,b
	ld	b,a
	ld	c,#0x49
	jr	lb_wr3

;--- end of proc 
	
;--- proc  lseek
;
;	char Lseek(FD fd,long *where, char ot);  -- DOS1, DOS2
;
;

_Lseek::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	pop ix
	call _ilseek
	ld	l,a
	ld	h,#0
	ret
	
_ilseek::	
	push	ix
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_sk2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_sk1		; MSXDOS1

lb_sk0:
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret
	
;
lb_sk2:

	ld	a,c
	ld	b,l
	push	de
	pop	ix
	ld	l,0(ix)
	ld	h,1(ix)
	ld	e,2(ix)
	ld	d,3(ix)
	ld	c,#0x4A
	jr	lb_sk0	
;
lb_sk1:
	push	de
	push	bc
	call	_get_fd
	ex	de,hl		;where to put
	pop	bc
	ld	a,c
	or	a
	ld	hl,#lb_p00
	jr	z,lb_sk11
	ld	hl,#33
	add	hl,de
	dec	a
	jr	z,lb_sk11
	ld	hl,#16
	add	hl,de
lb_sk11:
	ld	b,h
	ld	c,l		;where to get
	ld	hl,#33
	add	hl,de
	ex	de,hl		;where to put
	pop	hl		;pop offset pointer
	ld	a,(bc)
	add	a,(hl)
	ld	(de),a
	inc	bc
	inc	de
	inc	hl	
	ld	a,(bc)
	adc	a,(hl)
	ld	(de),a
	inc	bc
	inc	de
	inc	hl	
	ld	a,(bc)
	adc	a,(hl)
	ld	(de),a
	inc	bc
	inc	de
	inc	hl	
	ld	a,(bc)
	adc	a,(hl)
	ld	(de),a
	xor	a
	pop	ix
	ret

	;.area _DATA
lb_p00:
	.db	#0,#0,#0,#0

;--- end of proc 

	.area _CODE
	
	
;--- proc  ltell
;
;	char _Ltell(FD fd, long *where);  -- DOS1, DOS2
;
_Ltell::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	pop ix
	call _iltell
	ld	l,a
	ld	h,#0
	ret
_iltell::	
	push	ix
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_tl2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_tl1		; MSXDOS1

lb_tl2a:
	pop	ix
	ld	(#__io_errno),a
	ret
;
lb_tl2:
	push	de
	ld	a,#1
	ld	b,l
	ld	hl,#0
	ld	de,#0
	ld	c,#0x4A
	call	#5
	pop	ix
	ld	0(ix),l
	ld	1(ix),h
	ld	2(ix),e
	ld	3(ix),d
	jr	lb_tl2a
;
lb_tl1:
	push	de
	call	_get_fd
	ld	de,#33
	add	hl,de
	pop	de
	ld	bc,#4
	ldir
	xor	a
	pop	ix
	ret

;--- end of proc 


;--- proc  getcwd
;
;	char GetCWD(char *buf, int size);  -- DOS1, DOS2
;
_GetCWD::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	pop ix
	call	_igetcwd
	ld	l,a
	ld	h,#0
	ret

_igetcwd::	
	push	ix
	push	hl
	push	bc
	ld	c,#0x19
	call	#5
	ld	l,a
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_gc2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_gc1		; MSXDOS1

lb_gc3:
	pop	bc
	pop	de
	ld	hl,(#0xF34D)
	push	de
	ldir
	dec	de
lb_gc0:
	xor	a
	ld	(de),a
	pop	hl
	pop	ix
	ret
;
lb_gc1:
	pop	bc
	pop	de
	push	de
	ld	a,#'A'
	add	a,l
	ld	(de),a
	inc	de
	ld	a,#':'
	ld	(de),a
	inc	de
	jr	lb_gc0
;
lb_gc2:
	ld	b,l
	ld	de,(#0xF34D)
	ld	c,#0x59
	call	#5
	jr	lb_gc3

;--- end of proc 


;--- proc 	REMOVE
;
;	char	Remove(char *filename)	-- DOS1, DOS2
;
	
_Remove::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	call	_iremove
	ld	l,a
	ld	h,#0
	ret
_iremove::	
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_re2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_re1		; MSXDOS1

lb_re3:
	push	ix
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret
lb_re2:
	ex	de,hl
	ld	c,#0x4D
	ld	hl,#0
	jr	lb_re3
lb_re1:
	ld	de,#_xfcb
	call	__setupfcb
	ld	de,#_xfcb
	ld	c,#0x13
	jr	lb_re3
	
;--- end of proc 


;--- proc 	RENAME
;
;	char	Rename(char *old_name, char *new_name);	-- DOS1, DOS2
;
_Rename::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	pop ix
	call	_irename
	ld	l,a
	ld	h,#0
	ret
_irename::
	push	ix
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_ren2	; MSXDOS2 can do the same as MSXDOS1
	xor	a
	jr	lb_ren1		; MSXDOS1

lb_ren0:
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret
;
lb_ren2:
	push	hl
	push	de
	ld	c,#0x4D
	call	#5		;delete new
	pop	hl
	pop	de
	ld	c,#0x4E
	jr	lb_ren0
;
lb_ren1:
	push	de
	ld	de,#_xfcb
	call	__setupfcb
	pop	hl
	ld	de,#_xfcb+#16
	call	__setupfcb
	ld	de,#_xfcb+#16
	ld	c,#0x13
	call	#5
	ld	de,#_xfcb
	ld	c,#0x17
	jr	lb_ren0

;--- end of proc 
	
;--- proc  CHDIR
;
;	char ChangeDir(char *path);	-- DOS2
;
_ChangeDir::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	call	_ichdir
	ld	l,a
	ld	h,#0
	ret
	
_ichdir::
	ld	a,(#__os_ver)
	dec	a
	dec	a
	ret	m
	ex	de,hl
	ld	c,#0x5A
	push	ix
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret

;--- end of proc 


;--- proc 	MKDIR
;
;	char MakeDir(char *name);	-- DOS2
;
_MakeDir::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	call	_imkdir
	ld	l,a
	ld	h,#0
	ret
	
_imkdir::
	ld	a,(#__os_ver)
	dec	a
	dec	a
	ret	m
	push	ix
	ex	de,hl
	jr	z,lb_mkd1
	ld	c,#0x41
lb_mkd0:
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret
;
lb_mkd1:
	ld	bc,#0x9044
	xor	a
	jr	lb_mkd0

;--- end of proc 


;--- proc 	RMDIR
;
;	char Removedir(char *name);	-- DOS2
;
_RemoveDir::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	pop ix
	call	_irmdir
	ld	l,a
	ld	h,#0
	ret
	
_irmdir::
	ex	de,hl
	ld	a,(#__os_ver)
	dec	a
	dec	a
	ret	m
	push	ix
	ld	c,#0x4D
	jr	z,lb_rmd0
	ld	c,#0x42
lb_rmd0:
	call	#5
	pop	ix
	ld	(#__io_errno),a
	ret

;--- end of proc 
 
;--- proc 	FINDFIRST
;
;	char FindFirst(char *willcard, char *result, int attrib);	-- DOS1, DOS2
;
;	For MSXDOS2 attrib:
;		0  - only non-hidden, non-system files 
;
_FindFirst::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld e,6(ix)
	ld d,7(ix)
	ld c,8(ix)
	ld b,9(ix)
	call	_ifindfirst
	pop ix
	ld	l,a
	ld	h,#0
	ret
	
_ifindfirst:

	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_ff2		; MSXDOS2 can do the same as MSXDOS1
	xor	a

	; other use FCB way to get filename, attribute is ignored
	push	de
	
	ld	de,#_ffcb
	push	de
	call	__setupfcb
	pop	de

	push	de
	ld	de,#_xfcb
	ld	c,#0x1A
	call	#5
	pop	de
	
	ld	c,#0x11

lb_ff1c::
	
	call	#5
	ld	(#__io_errno),a
	
	pop	de
	
	or	a
	ret	nz

	ld	c,#12
	ld	hl,#_xfcb+1
lb_ffo:
	ld	a,(hl)
	
	cp	a, #32
	jr	z, lb_ff0	
	ld	(de),a
	inc	de
lb_ff0:
	inc	hl
	dec	c
	jr	z, lb_ffe

	ld	a,#4
	cp	c
	jr	nz, lb_ffo
	ld	a, #'.'
	ld	(de),a
	inc	de
	jr	lb_ffo

lb_ffe:
	dec	de
	ld	a,(de)
	cp	#'.'
	jr	nz, lb_ffe2
	inc	de
lb_ffe2:
	xor	a
	ld	(de),a
	ret
	
lb_ff2:
	push	de
	ld	c,#0x40
	

	ld	b,c
	ld	d,h
	ld	e,l

lb_ff2c::
	xor	a
	ld	h,a
	ld	l,a
	ld	ix, #_xfcb

	call	#5
	
	ld	(#__io_errno),a
	pop	de
	
	or	a
	ret	nz
	
	ld	hl, #_xfcb+1
lb_ff2a:
	ld	a, (hl)
	ld	(de),a
	inc	hl
	inc	de
	or	a
	jr	nz, lb_ff2a
	ret
	
;--- end of proc

;--- proc 	FINDNEXT
;
;	char FindNext(char *result);	-- DOS1, DOS2
;
;
_FindNext::
	push ix
	ld ix,#0
	add ix,sp
	ld l,4(ix)
	ld h,5(ix)
	ld c,6(ix)
	ld b,7(ix)
	call	_ifindnext
	pop ix
	ld	l,a
	ld	h,#0
	ret
	
_ifindnext:
	push	hl
	
	ld	a,(#__os_ver)
	dec	a
	dec	a
	jr	z,lb_fn2		; MSXDOS2 can do the same as MSXDOS1
	xor	a
	
	ld	c,#0x12
	jr	lb_ff1c
lb_fn2:
	ld	c,#0x41
	jr	lb_ff2c

;--- end of proc

;--- proc 	setdisk
;
;	void	SetDisk(int diskno)
;
_SetDisk::
	push	ix
	ld ix,#0
	add ix,sp
	ld	a,4(ix)
	ld	e,a
	ld	c,#0xE
	call	#5
	pop	ix
	ret

;--- end of proc 

;--- proc 	getdisk
;
;	int	GetDisk();
;
_GetDisk::
	push	ix
	ld	c,#0x19
	call	#5
	ld	h,#0
	ld	l,a
	pop	ix
	ret
	
;--- end of proc 

;
;	Diskload - load binary file from disk to RAM.
;
;	Compile on SDCC for MSX 
;

		.area _CODE
		
_DiskLoad::
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	ld	e,6(ix)
	ld	d,7(ix)
	ld	c,8(ix)
	ld	b,9(ix)
	
	push	bc
	push	de
	
	ld	a,#1
	ld	(#loadflag),a

		; prepare FCB
	push	hl
	ld	hl,#f_fcb
	ld	de,#f_fn
	push	de
	ld	bc,#36
	xor	a
	ld	(hl),a
	ldir
	pop	de
	pop	hl

		; copy filename into FCB
	ld	bc,#11
	ldir

		; open file for reading
	ld	de,#f_fcb
	ld	c, #0xF
	call	#5

	ld	hl,#1
	ld	(#f_groot),hl
	dec	hl
	ld	(#f_blok),hl
	ld	(#f_blok+#2),hl
	
	ld	hl,(#f_bleng)	; obtain file size
	pop	de
	push	hl
		; set writing to RAM address
	ld	c,#0x1A
	call	#5
	pop	hl
	
		; read from file
	ld	de,#f_fcb
	ld	c,#0x27
	call	#5
	ld      (#loadflag),a  ;sets 0 if ok, 1 if can not load 
	
	ld	de,#f_fcb
	ld	c,#0x10
	call	#5

	pop	bc
	ld	(#lb_calladdr),bc
	
	pop	ix
	
	xor	a
	or	b
	or	c
	jr	z, lb_exit_
			
	.db	#0xCD	; call to address
lb_calladdr:
	.db	#0
	.db	#0
	
lb_exit_:
	ld	a,(#loadflag)
	ld	l,a
	ld	h,#0
	ret
	
;	.area _DATA

loadflag:	.db #0

f_fcb:		.db	#0
f_fn:		.ascii	"???????????"   ;11 chars          
		.dw	#0
f_groot:	.dw	#0
f_bleng:	.ds	#17
		
f_blok:		.dw	#0
		.dw	#0
	.db	#0
	
	.area _CODE
;


