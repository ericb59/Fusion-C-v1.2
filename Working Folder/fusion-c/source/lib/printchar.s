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
;------------------------------------------------
;	printchar  
;	Eric Boez 2019
;-----------------------------------------------
	.area _CODE


;--- proc   PrintChar
;
;   void PrintChar(char c)
;
_PrintChar::
	push	ix
	ld ix,#0
	add ix,sp
	ld	a,4(ix)
	push iy
 	ld   ix,#0xA2	      ; Bios CHPUT
    ld   iy,(0xFCC0)      ; mainrom slotaddress
    call 0x001c           ; interslotcall
    ei
    pop iy
    pop ix
    ret 
