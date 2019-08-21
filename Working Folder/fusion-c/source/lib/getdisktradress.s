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
;	GetDiskTrAdress 
;	Definitions for dealing with MSXDOS
;
;	Eric Boez 2019
;
;
	.area _CODE

;--- proc 	GetDiskTrAdress
;
;	unsigned int  	GetDiskTrAdress(void); 
;
_GetDiskTrAdress::
  push  ix
  ld c,#0x57
  call  #5
  ld h,d
  ld l,e 
  pop ix
  ret

;--- end of proc 
