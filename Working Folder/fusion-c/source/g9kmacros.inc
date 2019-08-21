; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.2 - August 2019                      |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|      		Macro definition used by G9KLIB              |
;|                      By Sylvain G.                        |
;\___________________________________________________________/
;
;	.ifndef	MACROS_INC
;	.define	MACROS_INC

;add A to HL	
	.macro	ADD_HL_A
		add	a,l
		ld	l,a
		jr	nc,jpahl
		inc	h
jpahl:
	.endm

;	.endif


	; macro to get a bit from the bitstream
	; carry if bit is set, nocarry if bit is clear
	; must be entered with second registerset switched in!
	.macro	GET_BIT_FROM_BITSTREAM	?.done
		add	a,a		; shift out new bit
		jp	nz,.done	; if remaining value isn't zere, we're done

		ld	a,(hl)		; get 8 bits from bitstream
		inc	hl		; increase source data address

		rla                     ; (bit 0 will be set!!!!)
.done:
	.endm

	.macro LD_A_IXL
	.db 0xDD,0x7D ; LD	A,IXL
	.endm

	.macro LD_A_IXH
	.db 0xDD,0x7C ; LD	A,IXH
	.endm

	.macro LD_A_IYL
	.db 0xFD,0x7D ; LD	A,IYL
	.endm

	.macro LD_A_IYH
	.db 0xFD,0x7C ; LD	A,IXH
	.endm

	.macro LD_E_IYL
	.db 0xFD,0x5D ; LD	E,IYL
	.endm
