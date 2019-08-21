; ___________________________________________________________
;/               __           _                              \
;|              / _|         (_)                             |
;|             | |_ _   _ ___ _  ___  _ __                   |
;|             |  _| | | / __| |/ _ \| '_ \                  |
;|             | | | |_| \__ \ | (_) | | | |                 |
;|             |_|  \__,_|___/_|\___/|_| |_| *               |
;|                                                           |
;|               The MSX C Library for SDCC                  |
;|                   V1.1 -   05 - 2019                      |
;|                                                           |
;|                Eric Boez &  Fernando Garcia               |
;|                                                           |
;|               A S M  S O U R C E   C O D E                |
;|                                                           |
;|                                                           |
;\___________________________________________________________/
;
	;--- crt0.s for MSX-DOS - by Konami Man & Avelino, 11/2004
	;    Simple version: allows "void main()" only.
	;    Overhead: 6 bytes.
	;
	;    Compile programs with --code-loc 0x106 --data-loc X
	;    X=0  -> global vars will be placed immediately after code
	;    X!=0 -> global vars will be placed at address X
	;            (make sure that X>0x100+code size)
	;
	;    Corrected and updated by Eugeny Brychkov 20190416

	.globl	_main
	.globl	l__INITIALIZER
	.globl	s__INITIALIZED
	.globl	s__INITIALIZER

	.area _HEADER (ABS)

	.org    0x0100  ;MSX-DOS .COM programs start address

	;--- Initialize globals and jump to "main"

init:   call gsinit
	jp   __pre_main

	;--- Program code and data (global vars) start here

	;* Place data after program code, and data init code after data

	.area	_CODE

__pre_main:
	push hl
	ld hl,#_HEAP_start
	ld (_heap_top),hl
	pop hl
	jp _main

	.area	_DATA
	.area	_INITIALIZER
	.area	_INITIALIZED

_heap_top::
	.dw 0

	.area   _GSINIT

gsinit:
	ld	bc, #l__INITIALIZER
	ld	a, b
	or	a, c
	jr	Z, gsinit_next
	ld	de, #s__INITIALIZED
	ld	hl, #s__INITIALIZER
	ldir
gsinit_next:

	.area   _GSFINAL
	ret

	;* These doesn't seem to be necessary... (?)

	;.area  _OVERLAY
	;.area	_HOME
	;.area  _BSS

	.area	_HEAP

_HEAP_start::
