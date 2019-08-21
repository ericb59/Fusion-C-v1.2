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
;	SetDiskTrAdress 
;	Definitions for dealing with MSXDOS
;
;	Eric Boez 2019
;
;
	.area _CODE

;--- proc 	SetDiskTrAdress
;
;	void	SetDiskTrAdress(int adress)
;
_SetDiskTrAdress::
	push	ix
	ld ix,#0
	add ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	c,#0x1A
	call	#5
	pop	ix
	ret

;--- end of proc 
