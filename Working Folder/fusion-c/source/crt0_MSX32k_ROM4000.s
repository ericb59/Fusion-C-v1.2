; crt0 for MSX ROM of 32KB, starting at 0x4000
; suggested options: --code-loc 0x4050 --data-loc 0xC000

; Example for compile and create a 32K ROM binary:

; sdcc -mz80 --code-loc 0x4050 --data-loc 0xC000 --use-stdout --no-std-crt0 crt0_MSX32kROM4000.rel CFILENAME.c
; hex2bin -e ROM -s 4000 -l 8000 CFILENAME.ihx <--- generate a binary file and fill to 32768 Bytes size (8000h)

; Version: 1.1

; Date: 12/04/2018

; Author: mvac7/303bcn

.globl	_main

.area _HEADER (ABS)
; Reset vector
	.org 0x4000
  
	.db  0x41
	.db  0x42
	.dw  init
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000
	.dw  0x0000

	
init:
  di
	ld   SP,(#0xFC4A) ;Stack at the top of memory.
	ei
	call find_rom_page_2
	call _main
	RST  0 ;CHKRAM
  

;------------------------------------------------
; find_rom_page_2
; original name     : LOCALIZAR_SEGUNDA_PAGINA
; Original author   : Eduardo Robsy Petrus
; Snippet taken from: http://karoshi.auic.es/index.php?topic=117.msg1465
;
; Rutina que localiza la segunda pagina de 16 KB
; de una ROM de 32 KB ubicada en 4000h
; -Basada en la rutina de Konami-
; Compatible con carga en RAM
; Compatible con expansores de slots
;------------------------------------------------
; Comprobacion de RAM/ROM

find_rom_page_2::
	ld hl, #0x4000
	ld b, (hl)
	xor a
	ld (hl), a
	ld a, (hl)
	or a
	jr nz,5$ ; jr nz,@@ROM
	; El programa esta en RAM - no buscar
	ld (hl),b
	ret
5$: ; ----------- @@ROM:
	di
	; Slot primario
	call #0x0138 ; call RSLREG
	rrca
	rrca
	and #0x03
	; Slot secundario
	ld c, a
	ld hl, #0xfcc1
	add a, l
	ld l, a
	ld a, (hl)
	and #0x80
	or c
	ld c, a
	inc l
	inc l
	inc l
	inc l
	ld a, (hl)
	; Definir el identificador de slot
	and #0x0c
	or c
	ld h, #0x80
	; Habilitar permanentemente
	call #0x0024 ; call ENASLT
	ei
	ret
;------------------------------------------------


.area	_CODE

.area   _DATA

.area   _GSINIT
     
.area   _GSFINAL
