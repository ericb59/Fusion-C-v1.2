; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                      V1.1 - 03-2019                       |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
;
;	WaitKey 
;	Calling Bios Function Chget 0x009f
;
;	Eric Boez 2019
;
	.area _CODE

;--- proc 	WaitKey
;
;	unsigned char	WaitKey(void)
;
_WaitKey::
   push ix
   push iy
   ld   ix,#0x009f    ; chget bios function 
   ld   iy,(0xFCC0)	 ; mainrom slotaddress
   call 0x001c		    ; interslotcall
   ei
   
   ld l,a			 ; Return to register L
   ld	h,#0
   pop iy
   pop  ix
   ret

;--- end of proc 
