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
;|   G9klib is Fusion-C Library for GFX9000 Graphic Card     |
;|                                                           |
;|    This G9KLIB is a port of the ASM G9KLIB by Team Bomba  |
;|            https://www.teambomba.net/gfx9klib.html        |
;|                      By Sylvain G.                        |
;\___________________________________________________________/
;		 
;
;   This G9KLIB is currently in Beta Version. No manual Available at this time. 
;
;----------------------------------------------------------------------------;
; Gfx9000 general functions                                                  ;
;----------------------------------------------------------------------------;

;----------------------------------------------------------------------------;
; V9990 register and port defines                                            ;
;----------------------------------------------------------------------------;

; Port defines
	G9K_VRAM			.equ 0x60	; R/W
	G9K_PALETTE			.equ 0x61	; R/W
	G9K_CMD_DATA		.equ 0x62	; R/W
	G9K_REG_DATA		.equ 0x63	; R/W
	G9K_REG_SELECT		.equ 0x64	; W
	G9K_STATUS			.equ 0x65	; R
	G9K_INT_FLAG		.equ 0x66	; R/W
	G9K_SYS_CTRL		.equ 0x67	; W
	G9K_OUTPUT_CTRL 	.equ 0x6F	; R/W

; Register defines
	G9K_WRITE_ADDR		.equ 0	; W
	G9K_READ_ADDR		.equ 3	; W
	G9K_SCREEN_MODE0	.equ 6	; R/W
	G9K_SCREEN_MODE1	.equ 7	; R/W
	G9K_CTRL			.equ 8	; R/W
	G9K_INT_ENABLE		.equ 9	; R/W
	G9K_INT_V_LINE_LO	.equ 10	; R/W	
	G9K_INT_V_LINE_HI	.equ 11	; R/W
	G9K_INT_H_LINE		.equ 12	; R/W	
	G9K_PALETTE_CTRL	.equ 13	; W
	G9K_PALETTE_PTR		.equ 14	; W
	G9K_BACK_DROP_COLOR	.equ 15	; R/W
	G9K_DISPLAY_ADJUST	.equ 16	; R/W
	G9K_SCROLL_LOW_Y	.equ 17	; R/W
	G9K_SCROLL_HIGH_Y	.equ 18	; R/W
	G9K_SCROLL_LOW_X	.equ 19	; R/W
	G9K_SCROLL_HIGH_X	.equ 20	; R/W
	G9K_SCROLL_LOW_Y_B	.equ 21	; R/W
	G9K_SCROLL_HIGH_Y_B	.equ 22	; R/W
	G9K_SCROLL_LOW_X_B	.equ 23	; R/W
	G9K_SCROLL_HIGH_X_B	.equ 24	; R/W
	G9K_PAT_GEN_TABLE  	.equ 25	; R/W
	G9K_LCD_CTRL       	.equ 26	; R/W
	G9K_PRIORITY_CTRL  	.equ 27	; R/W
	G9K_SPR_PAL_CTRL	.equ 28	; W
	G9K_SC_X			.equ 32	; W
	G9K_SC_Y			.equ 34	; W
	G9K_DS_X			.equ 36	; W
	G9K_DS_Y			.equ 38	; W
	G9K_NX				.equ 40	; W
	G9K_NY				.equ 42	; W
	G9K_ARG				.equ 44	; W
	G9K_LOP				.equ 45	; W Logical Operation WC (Write Color) is obtained through logical operation with SC (Source Color) and DC (Destination Color)
								; 	b0 L00	: WC value for the bit with SC=0 and DC=0
								;	b1 L01	: WC value for the bit with SC=0 and DC=1
								;	b2 L10	: WC value for the bit with SC=1 and DC=0
								;	b3 L11	: WC value for the bit with SC=1 and DC=1
								;	b4 TP	: When this bit is "1", the data with SC as all "0" (by pixel for the X/Y-coordinates and by byte for the linear address) is not transferred. 
	G9K_WRITE_MASK		.equ 46	; W
	G9K_FC				.equ 48	; W
	G9K_BC				.equ 50	; W
	G9K_OPCODE			.equ 52	; W

; Register Select options
	G9K_DIS_INC_READ	.equ 64
	G9K_DIS_INC_WRITE	.equ 128

; Bit defines G9K_SCREEN_MODE0 (register 6)
	G9K_SCR0_STANDBY	.equ 192	; Stand by mode
	G9K_SCR0_BITMAP		.equ 128	; Select Bit map mode
	G9K_SCR0_P2			.equ 64	; Select P2 mode
	G9K_SCR0_P1			.equ 0	; Select P1 mode
	G9K_SCR0_DTCLK		.equ 32	; Master Dot clock not divided
	G9K_SCR0_DTCLK2		.equ 16	; Master Dot clock divided by 2
	G9K_SCR0_DTCLK4		.equ 0	; Master Dot clock divided by 4
	G9K_SCR0_XIM2048	.equ 12	; Image size = 2048
	G9K_SCR0_XIM1024	.equ 8	; Image size = 1024
	G9K_SCR0_XIM512		.equ 4	; Image size = 512
	G9K_SCR0_XIM256		.equ 0	; Image size = 256
	G9K_SCR0_16BIT		.equ 3	; 16 bits/dot
	G9K_SCR0_8BIT		.equ 2	; 8 bits/dot
	G9K_SCR0_4BIT		.equ 1	; 4 bits/dot
	G9K_SCR0_2BIT		.equ 0	; 2 bits/dot

; Bit defines G9K_SCREEN_MODE1 (register 7)
	G9K_SCR1_C25M		.equ 64	; Select 640*480 mode
	G9K_SCR1_SM1		.equ 32	; Selection of 263 lines during non interlace , else 262
	G9K_SCR1_SM			.equ 16	; Selection of horizontal frequency 1H=fsc/227.5
	G9K_SCR1_PAL		.equ 8	; Select PAL, else NTSC
	G9K_SCR1_EO			.equ 4	; Select of vertical resoltion of twice the non-interlace resolution
	G9K_SCR1_IL			.equ 2	; Select Interlace
	G9K_SCR1_HSCN		.equ 1	; Select High scan mode

; Bit defines G9K_CTRL    (Register 8)
	G9K_CTRL_DISP		.equ 128	; Display VRAM
	G9K_CTRL_DIS_SPD	.equ 64		; Disable display sprite (cursor)
	G9K_CTRL_YSE		.equ 32		; /YS Enable
	G9K_CTRL_VWTE		.equ 16		; VRAM Serial data bus control during digitization
	G9K_CTRL_VWM		.equ 8		; VRAM write control during digitization
	G9K_CTRL_DMAE		.equ 4		; Enable DMAQ output
	G9K_CTRL_VRAM512	.equ 2		; VRAM=512KB
	G9K_CTRL_VRAM256	.equ 1		; VRAM=256KB
	G9K_CTRL_VRAM128	.equ 0		; VRAM=128KB

; Bit Defines G9K_PALETTE_CTRL  (Register 13)
	G9K_PAL_CTRL_YUV	.equ 192	; YUV mode
	G9K_PAL_CTRL_YJK	.equ 128	; YJK mode
	G9K_PAL_CTRL_256	.equ 64		; 256 color mode
	G9K_PAL_CTRL_PAL	.equ 0		; Pallete mode
	G9K_PAL_CTRL_YAE	.equ 32		; Enable YUV/YJK RGB mixing mode

; Bit defines G9K_SYS_CTRL
	G9K_SYS_CTRL_SRS	.equ 2	; Power on reset state
	G9K_SYS_CTRL_MCKIN	.equ 1	; Select MCKIN terminal
	G9K_SYS_CTRL_XTAL	.equ 0	; Select XTAL

; Blitter Commands G9K_OPCODE    (Register 52)
	G9K_OPCODE_STOP		.equ 0x00	; Command being excuted is stopped 
	G9K_OPCODE_LMMC		.equ 0x10	; Data is transferred from CPU to VRAM rectangle area
	G9K_OPCODE_LMMV		.equ 0x20    ; VRAM rectangle area is painted out
	G9K_OPCODE_LMCM		.equ 0x30    ; VRAM rectangle area is transferred to CPU
	G9K_OPCODE_LMMM		.equ 0x40    ; Rectangle area os transferred from VRAM to VRAM
	G9K_OPCODE_CMMC		.equ 0x50    ; CPU character data is color-developed and transferred to VRAM rectangle area
	G9K_OPCODE_CMMK		.equ 0x60    ; Kanji ROM data is is color-developed and transferred to VRAM rectangle area
	G9K_OPCODE_CMMM		.equ 0x70    ; VRAM character data is color-developed and transferred to VRAM rectangle area 
	G9K_OPCODE_BMXL		.equ 0x80    ; Data on VRAM linear address is transferred to VRAM rectangle area
	G9K_OPCODE_BMLX		.equ 0x90    ; VRAM rectangle area is transferred to VRAM linear address 
	G9K_OPCODE_BMLL		.equ 0xA0    ; Data on VRAM linear address is transferred to VRAM linear address 
	G9K_OPCODE_LINE		.equ 0xB0    ; Straight line is drawer on X-Y co-ordinates
	G9K_OPCODE_SRCH		.equ 0xC0    ; Border color co-ordinates on X-Y are detected
	G9K_OPCODE_POINT	.equ 0xD0    ; Color code on specified point on X-Y is read out
	G9K_OPCODE_PSET		.equ 0xE0    ; Drawing is executed at drawing point on X-Y co-ordinates
	G9K_OPCODE_ADVN		.equ 0xF0    ; Drawing point on X-Y co-ordinates is shifted

; Bit defines G9K_ARG
	G9K_ARG_MAJ		.equ	1
	G9K_ARG_NEG		.equ	2
	G9K_ARG_DIX		.equ	4
	G9K_ARG_DIY		.equ	8

; Bit defines G9K_STATUS
	G9K_STATUS_TR	.equ	128
	G9K_STATUS_VR	.equ	64
	G9K_STATUS_HR	.equ	32
	G9K_STATUS_BD	.equ	16
	G9K_STATUS_MSC	.equ	4
	G9K_STATUS_EO	.equ	2
	G9K_STATUS_CE	.equ	1

VFF_OBJECT:
	VFF_OBJECT.fileHandle 	.equ 0
	VFF_OBJECT.name 		.equ 2
	VFF_OBJECT.width 		.equ 18
	VFF_OBJECT.height 		.equ 19
	VFF_OBJECT.pitch 		.equ 20
	VFF_OBJECT.dataSize 	.equ 21
	VFF_OBJECT.ptrOffsetTable .equ 23
	VFF_OBJECT.vramOffset 	.equ 25
	VFF_OBJECT.ramFont 		.equ 27

G9B_OBJECT:
	G9B_OBJECT.fileHandle	.equ 0	; 2	; Dos2 file handle of the openend G9B file
	G9B_OBJECT.bitDepth		.equ 2	; 1	; 2,4,8 or 16 bit
	G9B_OBJECT.colorType	.equ 3	; 1	; 0=64 color palette mode,64=256 color fixed ,128=YJK and 192=YUV mode
	G9B_OBJECT.nrColors		.equ 4	; 1	; Number of colors in palette mode
	G9B_OBJECT.width		.equ 5	; 2	; Width
	G9B_OBJECT.height	    .equ 7	; 2	; Height
	G9B_OBJECT.compression	.equ 9	; 1	; 0 = no compression, other value = compression used
	G9B_OBJECT.dataSize		.equ 10	; 3 ; 24 bit data size	

G9B_COMPRESSION_BITBUST .equ	1

;----------------------------------------------------------------------------;
; ;G9k Error codes                                                           ;
;----------------------------------------------------------------------------;
			
; Error codes
_NOVFF      .equ	1   ;  Input file is not a VFF file
_NOG9B		.equ	2 	;  Input file is not a G9B file

; RAM copy and print mode
TO_VRAM      .equ	0 ;
TO_RAM       .equ	1 ;
TO_XY        .equ	1 ;

	.macro	G9kCmdWait ?localLabel
; Wait for command execution completion
; Modifies A
localLabel:
	IN	A,(G9K_STATUS)
	RRA
	JR	C,localLabel
	.endm

	.macro G9kWriteReg register,data
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,#data
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kWriteRegB register
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,B
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kWriteRegC register
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,C
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kWriteRegD register
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,D
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kWriteRegE register
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,E
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kWriteRegL register
; Write gfx9000 register
; modifies: A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	LD	A,L
	OUT	(G9K_REG_DATA),A
	.endm

	.macro G9kReadReg register
; Read Gfx9000 register
; Result in A
	LD	A,#register
	OUT	(G9K_REG_SELECT),A
	IN	A,(G9K_REG_DATA)
	.endm

	.macro G9kWaitComReady	?lwait
; Wait for Command data ready	
lwait:
	IN      A,(#G9K_STATUS)
	RLA
	JR	NC,lwait ;$-3
	.endm
