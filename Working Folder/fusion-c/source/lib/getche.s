; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                     V1.1 - 04 - 2019                      |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;------------------------------------------------
;	getche
;	Wait for console char and print it
;
;	Eric Boez 2019
;  char           Getche (void);
;-----------------------------------------------

	.area _CODE
_Getche::
   push ix
   push iy
   ld   ix,#0x009f    	; chget bios function 
   ld   iy,(0xFCC0)	 	; mainrom slotaddress
   call 0x001c		    ; interslotcall
   
   ld   ix,#0x00a2    	; chput bios function 
   call 0x001c		    ; interslotcall
   ei
   ld l,a			 ; Return to register L

   pop iy
   pop  ix
   ret