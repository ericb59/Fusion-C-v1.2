
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	screen.s for MSX														;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	2006/11/26	t.hara														;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.area _CODE

;;	void Screen( unsigned char mode );
_Screen::
	xor		a
	ld		hl, #0xFAF5		;;	DPPAGE
	ld		(hl), a
	inc		hl				;;	ACPAGE
	ld		(hl), a

	ld		hl, #2
	add		hl, sp

	ld		a, (hl)			;;	mode
	ld 		hl,#0xFCAF		;; Save current mode to FCAF 
	ld 		(hl),a
	push	ix
	ld		ix, #0x005f		;;	chgmod on bios
	ld		iy, (0xfcc0)	;;	iyh <= (0xfcc1) : EXPTBL(MAIN-ROM SLOT)
	call	0x001c			;;	CALSLT
	pop		ix
	ret