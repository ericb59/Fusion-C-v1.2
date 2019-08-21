; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                     V1.1 -  03-2019                       |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;
;	GetDiskParam 
;	Definitions for dealing with MSXDOS2 (only)
;	Get the disk parameters 
;	Eric Boez 2019
;
;
	.area _CODE

;--- proc 	GetDiskParam
;
;	unsigned char GetDiskParam(DSKPARAMS *info, unsigned char Drive)
;
_GetDiskParam::
  push  ix
  ld ix,#0
  add ix,sp
  ld  e,4(ix)
  ld  d,5(ix)

  ld l,6(ix)
  ld c,#0x31
  call #5
  ld h,#0
  ld l,a
  pop ix
  ret

;--- end of proc 
