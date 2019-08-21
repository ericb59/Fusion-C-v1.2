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
; FCB_READ
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	fcb_access.s for MSX													;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	2006/12/02	t.hara														;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.area _CODE

;;	unsigned char fcb_read( FCB *p_fcb, void *p_buffer, int size );
_fcb_read::
	ld		hl, #2
	add		hl, sp

	push	ix
	ld		c, (hl)				;;	bc <= p_fcb
	inc		hl
	ld		b, (hl)
	inc		hl
	push	bc
	ld		e, (hl)				;;	de <= p_buffer
	inc		hl
	ld		d, (hl)
	inc		hl

	push	hl
	ld		c, #0x1A			;;	set DTA (DOS function)
	call	0x0005				;;	call system
	pop		hl

	pop		de					;;	de <= p_fcb
	ld		c, (hl)				;;	hl <= size
	inc		hl
	ld		h, (hl)
	ld		l, c

	ld		c, #0x27			;;	read data 2 (DOS function)
	call	0x0005				;;	call system
	pop		ix
	ret							;;	return read size

;;	unsigned char fcb_write( FCB *p_fcb, const void *p_buffer, int size );
_fcb_write::
	ld		hl, #2
	add		hl, sp

	push	ix
	ld		c, (hl)				;;	bc <= p_fcb
	inc		hl
	ld		b, (hl)
	inc		hl
	push	bc
	ld		e, (hl)				;;	de <= p_buffer
	inc		hl
	ld		d, (hl)
	inc		hl

	push	hl
	ld		c, #0x1A			;;	set DTA (DOS function)
	call	0x0005				;;	call system
	pop		hl

	pop		de					;;	de <= p_fcb
	ld		c, (hl)				;;	hl <= size
	inc		hl
	ld		h, (hl)
	ld		l, c

	ld		c, #0x26			;;	write data 2 (DOS function)
	call	0x0005				;;	call system
	pop		ix
	ld		l, a
	ret							;;	return wrote size

