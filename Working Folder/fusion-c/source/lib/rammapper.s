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
;	2018	Bitvision Software
;	Memory mapper routines for MSX on SDCC
;
;	info at
;	http://map.grauw.nl/resources/dos2_environment.php#c5
;



	.area	CODE
	
EXTBIO = 0xFFCA


;parm 
;	unsigned char deviceId
;								;devices list 
								;
								;00h  reserved for broadcasts
								;04h  DOS2 memory routines
								;08h  RS232
								;0Ah  MSX-AUDIO
								;11h  Kanji
								;22h  UNAPI (konamiman)
								;4Dh  Memman
								;FFh  System exclusive
								;		
;usually 04h
_InitRamMapperInfo::
	push 	ix
	ld 		ix,#0
	add 	ix,sp

	;get routines info table
	ld 		d,4(ix)				;deviceId
	xor		a
	ld		e,#1					;function
	call	EXTBIO
	ld		(#_rammapper_table),hl

	;populate ram mapper routines table
	xor		a
	ld		de,#0x0402
	call	EXTBIO
	;A = total number of memory mapper segments
	;B = slot number of primary mapper
	;C = number of free segments of primary mapper
	;DE = reserved
	;HL = start address of jump table	
	ld		(#_rammapper_routinestable),hl
	ld		de,#__Allocate
	ld		bc,#(3*16)
	ldir	
	
	pop ix
	ret





__GetRamMapperBaseTable::
	ld	hl,(#_rammapper_table)
	ret






;		   Parameters:   A=0 => allocate user segment
;                        A=1 => allocate system segment
;                        B=0 => allocate primary mapper
;                        B!=0 => allocate 
;                        FxxxSSPP slot address (primary mapper, if 0)
;                        xxx=000 allocate specified slot only
;                        xxx=001 allocate other slots than specified
;                        xxx=010 try to allocate specified slot and, if it failed, try another slot (if any)
;                        xxx=011 try to allocate other slots than specified and, if it failed, try specified slot
;		   Results:      Carry set => no free segments
;                        Carry clear => segment allocated
;                                       A=new segment number
;                                       B=slot address of mapper slot (0 if called as B=0)
_AllocateSegment::
	push 	ix
	ld 		ix,#0
	add 	ix,sp

	ld		hl,#_AllocateSegment_return
	push	hl
	ld 		a,4(ix)				;user(0)/system(1)
	ld 		b,5(ix)				;primary mapper(0)/allocate other mapper (see doc)
	jp		__Allocate

_AllocateSegment_return:
	push	af
	
	jr		c,_AllocateSegment_setCarry
	;no carry
	xor		a
	jr		_AllocateSegment_cont
_AllocateSegment_setCarry:
	ld		a,#1
_AllocateSegment_cont:
	ld		(#_segment_status+2),a		;carry flag
	
	pop		af			
	ld		(#_segment_status+0),a		;segment number
	ld		a,b						
	ld		(#_segment_status+1),a		;slot address
	ld		hl,#_segment_status			;ret address
		
	pop ix
	ret



;		   Parameters:   A=segment number to free
;                        B=0 primary mapper
;                        B!=0 mapper other than primary
;          Returns:      Carry set => error
;                        Carry clear => segment freed OK 
_FreeSegment::
	push 	ix
	ld 		ix,#0
	add 	ix,sp

	ld		hl,#_FreeSegment_return
	push	hl
	ld 		a,4(ix)				;segment number
	ld 		b,5(ix)				;primary mapper/mapper other than primary
	jp		__Free
_FreeSegment_return:

	jr		c,_FreeSegment_setCarry
	;no carry
	xor		a
	jr		_FreeSegment_cont
_FreeSegment_setCarry:
	ld		a,#1
_FreeSegment_cont:
	ld		(#_segment_status+2),a		;carry flag

	ld		hl,#_segment_status			;ret address

	pop ix
	ret







_Get_PN::
	push 	ix
	ld 		ix,#0
	add 	ix,sp

	ld		hl,#_Get_PN_return
	push	hl

	ld 		a,4(ix)	;page number
	or		a
	jp		z,__GetP0
	dec		a
	jp		z,__GetP1
	dec		a
	jp		z,__GetP2
	jp		__GetP3
	
_Get_PN_return:
	;return info on a
	ld		l,a
	
	pop ix
	ret






_Put_PN::
	push 	ix
	ld 		ix,#0
	add 	ix,sp

	ld		hl,#_Put_PN_return
	push	hl

	ld 		b,4(ix)	;page number
	inc		b		;trick to avoid 0 
	ld 		a,5(ix)	;segment number
	dec		b
	jp		z,__PutP0
	dec		b
	jp		z,__PutP1
	dec		b
	jp		z,__PutP2
	jp		__PutP3
	
_Put_PN_return:
	
	pop ix
	ret







	.area	DATA
_rammapper_table:			.dw	#0
_rammapper_routinestable:	.dw	#0

_segment_status:			.ds	3	;allocatedSegmentNumber, slotAddressOfMapper, carryFlag

__Allocate:
	.ds	3
__Free:
	.ds	3
__Read:
	.ds	3
__Write:
	.ds	3
__Call_Seg:
	.ds	3
__Calls:
	.ds	3
__PutPH:
	.ds	3
__GetPH:
	.ds	3
__PutP0:
	.ds	3
__GetP0:
	.ds	3
__PutP1:
	.ds	3
__GetP1:
	.ds	3
__PutP2:
	.ds	3
__GetP2:
	.ds	3
__PutP3:
	.ds	3
__GetP3:
	.ds	3


