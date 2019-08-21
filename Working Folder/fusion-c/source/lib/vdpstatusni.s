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
; VDPstatusNi
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	vdp_status.s for MSX													;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	2006/11/25	t.hara														;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.area _CODE

;;	unsigned char VDPstatusNi( unsigned char vdpreg );
_VDPstatusNi::
	ld		hl, #2
	add		hl, sp

	ld		a, (hl)			;;	port
	and		#0x0F

	ld		c, #0x99		;;	VDP port #1 (unsupport "MSX1 adapter")
	out		(c), a			;;	status register number
	ld		a, #0x8F		;;	VDP register R#15
	out		(c), a			;;	out VDP register number
	in		l, (c)			;;	read VDP S#n
	xor		a
	out		(c), a
	ld		a, #0x8F		;;	VDP register R#15
	out		(c), a			;;	out VDP register number
	ret

