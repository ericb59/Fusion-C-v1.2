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
;
;           The PT3 replayer for SDCC Ported by Fernando Garcia
;

	.area	_XDATA

H_TIMI_SAVED_HOOK:	.ds	5

PT3_SETUP:			.ds 1;ds	1	;set bit0 to 1, if you want to play without looping
					;bit7 is set each time, when loop point is passed
PT3_MODADDR:		.ds	2
PT3_CrPsPtr:		.ds	2
PT3_SAMPTRS:		.ds	2
PT3_OrnPtrs:		.ds	2
PT3_PDSP:			.ds	2
PT3_CSP:			.ds	2
PT3_PSP:			.ds	2
PT3_PrNote:			.ds	1
PT3_PrSlide:		.ds	2
PT3_AdInPtA:		.ds	2
PT3_AdInPtB:		.ds	2
PT3_AdInPtC:		.ds	2
PT3_LPosPtr:		.ds	2
PT3_PatsPtr:		.ds	2
PT3_Delay:			.ds	1
PT3_AddToEn:		.ds	1
PT3_Env_Del:		.ds	1
PT3_ESldAdd:		.ds	2
PT3_NTL3:			.ds	2	; AND A / NOP (note table creator)

VARS:

ChanA:			.ds	29			;CHNPRM_Size
ChanB:			.ds	29			;CHNPRM_Size
ChanC:			.ds	29			;CHNPRM_Size

;GlobalVars
DelyCnt:		.ds	1
CurESld:		.ds	2
CurEDel:		.ds	1
Ns_Base_AddToNs:
Ns_Base:		.ds	1
AddToNs:		.ds	1

NT_:			.ds	192	; Puntero a/tabla de frecuencias

AYREGS:
VT_:			.ds	14
EnvBase:		.ds	2
VAR0END:

T1_:			
T_NEW_1:
T_OLD_1:		.ds	24
T_OLD_2:		.ds	24
T_NEW_3:
T_OLD_3:		.ds	2
T_OLD_0:	
T_NEW_0:		.ds	24
T_NEW_2:		.ds	166

	
	



		; --- CONSTANT VALUES DEFINITION ---
	.area _CODE
	
	

	
	
;ChannelsVars
;struc	CHNPRM
;reset group
CHNPRM_PsInOr	= 0	;RESB 1
CHNPRM_PsInSm	= 1	;RESB 1
CHNPRM_CrAmSl	= 2	;RESB 1
CHNPRM_CrNsSl	= 3	;RESB 1
CHNPRM_CrEnSl	= 4	;RESB 1
CHNPRM_TSlCnt	= 5	;RESB 1
CHNPRM_CrTnSl	= 6	;RESW 1
CHNPRM_TnAcc	= 8	;RESW 1
CHNPRM_COnOff	= 10	;RESB 1
;reset group

CHNPRM_OnOffD	= 11	;RESB 1

;IX for PTDECOD here [+12]
CHNPRM_OffOnD	= 12	;RESB 1
CHNPRM_OrnPtr	= 13	;RESW 1
CHNPRM_SamPtr	= 15	;RESW 1
CHNPRM_NNtSkp	= 17	;RESB 1
CHNPRM_Note	= 18	;RESB 1
CHNPRM_SlToNt	= 19	;RESB 1
CHNPRM_Env_En	= 20	;RESB 1
CHNPRM_Flags	= 21	;RESB 1
 ;Enabled - 0,SimpleGliss - 2
CHNPRM_TnSlDl	= 22	;RESB 1
CHNPRM_TSlStp	= 23	;RESW 1
CHNPRM_TnDelt	= 25	;RESW 1
CHNPRM_NtSkCn	= 27	;RESB 1
CHNPRM_Volume	= 28	;RESB 1
CHNPRM_Size	= 29	;RESB 1
;endstruc

;struc	AR
AR_TonA		= 0	;RESW 1
AR_TonB		= 2	;RESW 1
AR_TonC		= 4	;RESW 1
AR_Noise	= 6	;RESB 1
AR_Mixer	= 7	;RESB 1
AR_AmplA	= 8	;RESB 1
AR_AmplB	= 9	;RESB 1
AR_AmplC	= 10	;RESB 1
AR_Env		= 11	;RESW 1
AR_EnvTp	= 13	;RESB 1
;endstruc




IM1_ISR_HOOK	= #0x0038
H_TIMI			= IM1_ISR_HOOK;#0xFD9F


_InstallReplayer::
		di
		
		ld		hl,#H_TIMI
		ld		de,#H_TIMI_SAVED_HOOK
		ld		bc,#5
		ldir
		
		ld		a,#0xC3
		ld		hl,#__pt3_isr
		ld		(H_TIMI),a
		ld		(H_TIMI+1),hl
		
		ei

		ret


_UninstallReplayer::
		di
		
		ld		hl,#H_TIMI_SAVED_HOOK
		ld		de,#H_TIMI
		ld		bc,#5
		ldir		
		
		ei
		ret





__pt3_isr:
		push	hl
		push	de
		push	bc
		push	af
		exx
		ex		af,af'
		push	hl
		push	de
		push	bc
		push	af
		push	iy
		push	ix
		

		;in a,(#0x99)


		call	_PT3Rout
		call	_PT3Play



		pop ix
		pop iy
		pop af
		pop bc
		pop de
		pop	hl
		ex	af,af'
		exx
		pop af
		pop bc
		pop de
		pop	hl
		
		;ld		hl,#__pt3_isr_return		;call original hook
		;push	hl
		jp		H_TIMI_SAVED_HOOK
;__pt3_isr_return::		

		;ret
		
		

_CHECKLP:	
		LD	HL,#PT3_SETUP
		SET	7,(HL)
		BIT	0,(HL)
		RET	Z
		POP	HL
		LD	HL,#DelyCnt
		INC	(HL)
		LD	HL,#ChanA+#CHNPRM_NtSkCn
		INC	(HL)

_PT3Mute::
		XOR	A
		LD	H,A
		LD	L,A
		LD	(AYREGS+AR_AmplA),A
		LD	(AYREGS+AR_AmplB),HL
		JP	ROUT_A0

_PT3Init::	;HL - AddressOfModule - 100

		push ix
		ld ix,#0
		add ix,sp
		ld l,4(ix)
		ld h,5(ix)
		ld a,6(ix)
		pop ix
		
		;ld	a,#1
		;xor		a
		ld	(PT3_SETUP),a	;loop
		
		
		ld	de,#99
		or	a
		sbc	hl,de		;-99


		LD (PT3_MODADDR),HL
		PUSH HL
		LD DE,#100
		ADD HL,DE
		LD A,(HL)
		LD (PT3_Delay),A
		PUSH HL
		POP IX
		ADD HL,DE
		LD (PT3_CrPsPtr),HL
		LD E,2(IX)
		ADD HL,DE
		INC HL
		LD (PT3_LPosPtr),HL
		POP DE
		LD L,3(IX)
		LD H,4(IX)
		ADD HL,DE
		LD (PT3_PatsPtr),HL
		LD HL,#169
		ADD HL,DE
		LD (PT3_OrnPtrs),HL
		LD HL,#105
		ADD HL,DE
		LD (PT3_SAMPTRS),HL
		LD HL,#PT3_SETUP
		RES 7,(HL)

		; --- note table data depacker ---
		; Depacks first 12 tones of each tone table

		LD DE,#T_PACK
		LD BC,#T1_+(2*49)-1
.TP_0:		
		LD A,(DE)
		INC DE
		CP #15*2
		JR NC,.TP_1
		LD H,A
		LD A,(DE)
		LD L,A
		INC DE
		JR .TP_2
.TP_1:		
		PUSH DE
		LD D,#0
		LD E,A
		ADD HL,DE
		ADD HL,DE
		POP DE
.TP_2:	
		LD A,H
		LD (BC),A
		DEC BC
		LD A,L
		LD (BC),A
		DEC BC
		SUB #0xF0
		JR NZ,.TP_0

		; --- INITIALIZE PT3 VARIABLES ---
		LD HL,#VARS
		LD (HL),A
		LD DE,#VARS+1
		LD BC,#VAR0END-VARS-1
		LDIR

		INC A
		LD (DelyCnt),A
		LD HL,#0xF001 ;H - CHNPRM_Volume, L - CHNPRM_NtSkCn
		LD (ChanA+CHNPRM_NtSkCn),HL
		LD (ChanB+CHNPRM_NtSkCn),HL
		LD (ChanC+CHNPRM_NtSkCn),HL

		LD HL,#EMPTYSAMORN
		LD (PT3_AdInPtA),HL ;ptr to zero
		LD (ChanA+CHNPRM_OrnPtr),HL ;ornament 0 is "0,1,0"
		LD (ChanB+CHNPRM_OrnPtr),HL ;in all versions from
		LD (ChanC+CHNPRM_OrnPtr),HL ;3.xx to 3.6x and VTII

		LD (ChanA+CHNPRM_SamPtr),HL ;S1 There is no default
		LD (ChanB+CHNPRM_SamPtr),HL ;S2 sample in PT3, so, you
		LD (ChanC+CHNPRM_SamPtr),HL ;S3 can comment S1,2,3; see
					    ;also EMPTYSAMORN comment

		; --- NOTE TABLE CREATOR (c) Ivan Roshin, adapted by SapphiRe ---
		LD A,-1(IX) ;TONE TABLE NUMBER
		RLA
		AND #7
		LD HL,#NT_DATA
		PUSH DE
		LD D,B		; ld d,0 (bc is 0000 after LDIR)
		ADD A,A
		LD E,A
		ADD HL,DE	; hl -> init of correct note table data
		LD E,(HL)
		INC HL
		SRL E
		SBC A,A
		AND #0xA7			;$00 (NOP) or $A7 (AND A)
		LD (PT3_NTL3),A		; CORRECT INSTRUCTION, DEPENDS OF SELECTED TABLE
		LD A,#0xC9		; RET CODE
		LD (PT3_NTL3+1),A	; RET PLACED
		EX DE,HL
		POP BC ;BC=T1_
		ADD HL,BC

		LD A,(DE)                           

		LD BC,#T_
		ADD A,C
		LD C,A
		ADC A,B

		SUB C
		LD B,A
		PUSH BC
		LD DE,#NT_
		PUSH DE

		LD B,#12
.L1:		
		PUSH BC
		LD C,(HL)
		INC HL
		PUSH HL
		LD B,(HL)

		PUSH DE
		EX DE,HL
		LD DE,#23
		.db #0xDD,#0x26,#0x08	;LD XH,8

.L2:		
		SRL B
		RR C
.L3:		
		CALL PT3_NTL3	;AND A or NOP
		LD A,C
		ADC A,D	;=ADC 0
		LD (HL),A
		INC HL
		LD A,B
		ADC A,D
		LD (HL),A
		ADD HL,DE
		.db #0xDD,#0x25	;DEC XH
		JR NZ,.L2

		POP DE
		INC DE
		INC DE
		POP HL
		INC HL
		POP BC
		DJNZ .L1

		POP HL
		POP DE

		LD A,E
		PUSH DE
		LD DE,#TCOLD_1
		CP E
		POP DE
		JR NZ,.CORR_1
		LD A,#0xFD
		LD (NT_+#0x2E),A

.CORR_1:	
		LD A,(DE)
		AND A
		JR Z,.TC_EXIT
		RRA
		PUSH AF
		ADD A,A
		LD C,A
		ADD HL,BC
		POP AF
		JR NC,.CORR_2
		DEC (HL)
		DEC (HL)
.CORR_2:	
		INC (HL)
		AND A
		SBC HL,BC
		INC DE
		JR .CORR_1

.TC_EXIT:	;POP AF

		; --- CREATE PT3 VOLUME TABLE (c) Ivan Roshin, adapted by SapphiRe ---
		ld	hl,#0x11
		ld	d,h
		ld	e,h
		ld	IX,#VT_+16
		ld	b,#15
.INITV1:	
		push	hl
		add	hl,de
		ex	de,hl
		sbc	hl,hl
		ld	c,b
		ld	b,#16
.INITV2:	
		ld	a,l
		rla
		ld	a,h
		adc	a,#0
		ld	(ix),a
		inc	ix
		add	hl,de
		djnz	.INITV2
		pop	hl
		ld	a,e
		cp	#0x77
		jr	nz,.INITV3
		inc	e
.INITV3:	
		ld	b,c
		djnz	.INITV1

		RET

		;pattern decoder
PD_OrSm:	
		LD 8(IX),#0
		CALL SETORN
		LD A,(BC)
		INC BC
		RRCA

PD_SAM:		
		ADD A,A
PD_SAM_:	
		LD E,A
		LD D,#0
		LD HL,(PT3_SAMPTRS)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		LD HL,(PT3_MODADDR)
		ADD HL,DE
		LD 3(IX),L
		LD 4(IX),H
		JR PD_LOOP

PD_VOL:		
		RLCA
		RLCA
		RLCA
		RLCA
		LD 16(IX),A
		JR PD_LP2
	
PD_EOff:	
		LD 8(IX),A
		LD CHNPRM_PsInOr-12(IX),A
		JR PD_LP2

PD_SorE:	
		DEC A
		JR NZ,PD_ENV
		LD A,(BC)
		INC BC
		LD CHNPRM_NNtSkp-12(IX),A
		JR PD_LP2

PD_ENV:		
		CALL SETENV
		JR PD_LP2

PD_ORN:	
		CALL SETORN
		JR PD_LOOP
       
PD_ESAM:	
		LD CHNPRM_Env_En-12(IX),A
		LD CHNPRM_PsInOr-12(IX),A
		CALL NZ,SETENV
		LD A,(BC)
		INC BC
		JR PD_SAM_

PTDECOD:	
		LD A,CHNPRM_Note-12(IX)
		LD (PT3_PrNote),A
		LD L,CHNPRM_CrTnSl-12(IX)
		LD H,CHNPRM_CrTnSl+1-12(IX)
		LD (PT3_PrSlide),HL

PD_LOOP:	
		LD DE,#0x2010
PD_LP2:	
		LD A,(BC)
		INC BC
		ADD A,E
		JR C,PD_OrSm
		ADD A,D
		JR Z,PD_FIN
		JR C,PD_SAM
		ADD A,E
		JR Z,PD_REL
		JR C,PD_VOL
		ADD A,E
		JR Z,PD_EOff
		JR C,PD_SorE
		ADD A,#96
		JR C,PD_NOTE
		ADD A,E
		JR C,PD_ORN
		ADD A,D
		JR C,PD_NOIS
		ADD A,E
		JR C,PD_ESAM
		ADD A,A
		LD E,A
		LD HL,#SPCCOMS+0xDF20;#SPCCOMS+0xDF20 % 65536)	; Adapted from original Speccy version (saves 6 bytes)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		PUSH DE
		JR PD_LOOP

PD_NOIS:	
		LD (Ns_Base),A
		JR PD_LP2

PD_REL:		
		RES 0,CHNPRM_Flags-12(IX)
		JR PD_RES
	
PD_NOTE:	
		LD CHNPRM_Note-12(IX),A
		SET 0,CHNPRM_Flags-12(IX)
		XOR A

PD_RES:		
		LD (PT3_PDSP),SP
		LD SP,IX
		LD H,A
		LD L,A
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		PUSH HL
		LD SP,(PT3_PDSP)

PD_FIN:		
		LD A,CHNPRM_NNtSkp-12(IX)
		LD CHNPRM_NtSkCn-12(IX),A
		RET

C_PORTM:	
		RES 2,CHNPRM_Flags-12(IX)
		LD A,(BC)
		INC BC
		;SKIP PRECALCULATED TONE DELTA [BECAUSE
		;CANNOT BE RIGHT AFTER PT3 COMPILATION)
		INC BC
		INC BC
		LD CHNPRM_TnSlDl-12(IX),A
		LD CHNPRM_TSlCnt-12(IX),A
		LD DE,#NT_
		LD A,CHNPRM_Note-12(IX)
		LD CHNPRM_SlToNt-12(IX),A
		ADD A,A
		LD L,A
		LD H,#0
		ADD HL,DE
		LD A,(HL)
		INC HL
		LD H,(HL)
		LD L,A
		PUSH HL
		LD A,(PT3_PrNote)
		LD CHNPRM_Note-12(IX),A
		ADD A,A
		LD L,A
		LD H,#0
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		POP HL
		SBC HL,DE
		LD CHNPRM_TnDelt-12(IX),L
		LD CHNPRM_TnDelt+1-12(IX),H
		LD DE,(PT3_PrSlide)
		LD CHNPRM_CrTnSl-12(IX),E
		LD CHNPRM_CrTnSl+1-12(IX),D
		LD A,(BC) ;SIGNED TONE STEP
		INC BC
		EX AF,AF'
		LD A,(BC)
		INC BC
		AND A
		JR Z,.NOSIG
		EX DE,HL
.NOSIG:	
		SBC HL,DE
		JP P,SET_STP
		CPL
		EX AF,AF'
		NEG
		EX AF,AF'
SET_STP:	
		LD CHNPRM_TSlStp+1-12(IX),A
		EX AF,AF'
		LD CHNPRM_TSlStp-12(IX),A
		LD CHNPRM_COnOff-12(IX),#0
		RET

C_GLISS:	
		SET 2,CHNPRM_Flags-12(IX)
		LD A,(BC)
		INC BC
		LD CHNPRM_TnSlDl-12(IX),A
		LD CHNPRM_TSlCnt-12(IX),A
		LD A,(BC)
		INC BC
		EX AF,AF'
		LD A,(BC)
		INC BC
		JR SET_STP

C_SMPOS:		
		LD A,(BC)
		INC BC
		LD CHNPRM_PsInSm-12(IX),A
		RET

C_ORPOS:	
		LD A,(BC)
		INC BC
		LD CHNPRM_PsInOr-12(IX),A
		RET

C_VIBRT:	
		LD A,(BC)
		INC BC
		LD CHNPRM_OnOffD-12(IX),A
		LD CHNPRM_COnOff-12(IX),A
		LD A,(BC)
		INC BC
		LD CHNPRM_OffOnD-12(IX),A
		XOR A
		LD CHNPRM_TSlCnt-12(IX),A
		LD CHNPRM_CrTnSl-12(IX),A
		LD CHNPRM_CrTnSl+1-12(IX),A
		RET

C_ENGLS:	
		LD A,(BC)
		INC BC
		LD (PT3_Env_Del),A
		LD (CurEDel),A
		LD A,(BC)
		INC BC
		LD L,A
		LD A,(BC)
		INC BC
		LD H,A
		LD (PT3_ESldAdd),HL
		RET

C_DELAY:	
		LD A,(BC)
		INC BC
		LD (PT3_Delay),A
		RET
	
SETENV:		
		LD CHNPRM_Env_En-12(IX),E
		LD (AYREGS+AR_EnvTp),A
		LD A,(BC)
		INC BC
		LD H,A
		LD A,(BC)
		INC BC
		LD L,A
		LD (EnvBase),HL
		XOR A
		LD CHNPRM_PsInOr-12(IX),A
		LD (CurEDel),A
		LD H,A
		LD L,A
		LD (CurESld),HL
C_NOP:		
		RET

SETORN:		
		ADD A,A
		LD E,A
		LD D,#0
		LD CHNPRM_PsInOr-12(IX),D
		LD HL,(PT3_OrnPtrs)
		ADD HL,DE
		LD E,(HL)
		INC HL
		LD D,(HL)
		LD HL,(PT3_MODADDR)
		ADD HL,DE
		LD CHNPRM_OrnPtr-12(IX),L
		LD CHNPRM_OrnPtr+1-12(IX),H
		RET

		;ALL 16 ADDRESSES TO PROTECT FROM BROKEN PT3 MODULES
SPCCOMS:	
		.dw C_NOP
		.dw C_GLISS
		.dw C_PORTM
		.dw C_SMPOS
		.dw C_ORPOS
		.dw C_VIBRT
		.dw C_NOP
		.dw C_NOP
		.dw C_ENGLS
		.dw C_DELAY
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP
		.dw C_NOP

CHREGS:		
		XOR A
		LD (AYREGS+AR_AmplC),A
		BIT 0,CHNPRM_Flags(IX)
		PUSH HL
		JP Z,.CH_EXIT
		LD (PT3_CSP),SP
		LD L,CHNPRM_OrnPtr(IX)
		LD H,CHNPRM_OrnPtr+1(IX)
		LD SP,HL
		POP DE
		LD H,A
		LD A,CHNPRM_PsInOr(IX)
		LD L,A
		ADD HL,SP
		INC A
		CP D
		JR C,.CH_ORPS
		LD A,E
.CH_ORPS:	
		LD CHNPRM_PsInOr(IX),A
		LD A,CHNPRM_Note(IX)
		ADD A,(HL)
		JP P,.CH_NTP
		XOR A
.CH_NTP:	
		CP #96
		JR C,.CH_NOK
		LD A,#95
.CH_NOK:	
		ADD A,A
		EX AF,AF'
		LD L,CHNPRM_SamPtr(IX)
		LD H,CHNPRM_SamPtr+1(IX)
		LD SP,HL
		POP DE
		LD H,#0
		LD A,CHNPRM_PsInSm(IX)
		LD B,A
		ADD A,A
		ADD A,A
		LD L,A
		ADD HL,SP
		LD SP,HL
		LD A,B
		INC A
		CP D
		JR C,.CH_SMPS
		LD A,E
.CH_SMPS:	
		LD CHNPRM_PsInSm(IX),A
		POP BC
		POP HL
		LD E,CHNPRM_TnAcc(IX)
		LD D,CHNPRM_TnAcc+1(IX)
		ADD HL,DE
		BIT 6,B
		JR Z,.CH_NOAC
		LD CHNPRM_TnAcc(IX),L
		LD CHNPRM_TnAcc+1(IX),H
.CH_NOAC:	
		EX DE,HL
		EX AF,AF'
		LD L,A
		LD H,#0
		LD SP,#NT_
		ADD HL,SP
		LD SP,HL
		POP HL
		ADD HL,DE
		LD E,CHNPRM_CrTnSl(IX)
		LD D,CHNPRM_CrTnSl+1(IX)
		ADD HL,DE
		LD SP,(PT3_CSP)
		EX (SP),HL
		XOR A
		OR CHNPRM_TSlCnt(IX)
		JR Z,.CH_AMP
		DEC CHNPRM_TSlCnt(IX)
		JR NZ,.CH_AMP
		LD A,CHNPRM_TnSlDl(IX)
		LD CHNPRM_TSlCnt(IX),A
		LD L,CHNPRM_TSlStp(IX)
		LD H,CHNPRM_TSlStp+1(IX)
		LD A,H
		ADD HL,DE
		LD CHNPRM_CrTnSl(IX),L
		LD CHNPRM_CrTnSl+1(IX),H
		BIT 2,CHNPRM_Flags(IX)
		JR NZ,.CH_AMP
		LD E,CHNPRM_TnDelt(IX)
		LD D,CHNPRM_TnDelt+1(IX)
		AND A
		JR Z,.CH_STPP
		EX DE,HL
.CH_STPP:	
		SBC HL,DE
		JP M,.CH_AMP
		LD A,CHNPRM_SlToNt(IX)
		LD CHNPRM_Note(IX),A
		XOR A
		LD CHNPRM_TSlCnt(IX),A
		LD CHNPRM_CrTnSl(IX),A
		LD CHNPRM_CrTnSl+1(IX),A
.CH_AMP:	
		LD A,CHNPRM_CrAmSl(IX)
		BIT 7,C
		JR Z,.CH_NOAM
		BIT 6,C
		JR Z,.CH_AMIN
		CP #15
		JR Z,.CH_NOAM
		INC A
		JR .CH_SVAM
.CH_AMIN:	
		CP #-15
		JR Z,.CH_NOAM
		DEC A
.CH_SVAM:	
		LD CHNPRM_CrAmSl(IX),A
.CH_NOAM:	
		LD L,A
		LD A,B
		AND #15
		ADD A,L
		JP P,.CH_APOS
		XOR A
.CH_APOS:	
		CP #16
		JR C,.CH_VOL
		LD A,#15
.CH_VOL:	
		OR CHNPRM_Volume(IX)
		LD L,A
		LD H,#0
		LD DE,#VT_
		ADD HL,DE
		LD A,(HL)
.CH_ENV:	
		BIT 0,C
		JR NZ,.CH_NOEN
		OR CHNPRM_Env_En(IX)
.CH_NOEN:	
		LD (AYREGS+AR_AmplC),A
		BIT 7,B
		LD A,C
		JR Z,.NO_ENSL
		RLA
		RLA
		SRA A
		SRA A
		SRA A
		ADD A,CHNPRM_CrEnSl(IX) ;SEE COMMENT BELOW
		BIT 5,B
		JR Z,.NO_ENAC
		LD CHNPRM_CrEnSl(IX),A
.NO_ENAC:	
		LD HL,#PT3_AddToEn
		ADD A,(HL) ;BUG IN PT3 - NEED WORD HERE.
			   ;FIX IT IN NEXT VERSION?
		LD (HL),A
		JR .CH_MIX
.NO_ENSL:	
		RRA
		ADD A,CHNPRM_CrNsSl(IX)
		LD (AddToNs),A
		BIT 5,B
		JR Z,.CH_MIX
		LD CHNPRM_CrNsSl(IX),A
.CH_MIX:	
		LD A,B
		RRA
		AND #0x48
.CH_EXIT:	
		LD HL,#AYREGS+AR_Mixer
		OR (HL)
		RRCA
		LD (HL),A
		POP HL
		XOR A
		OR CHNPRM_COnOff(IX)
		RET Z
		DEC CHNPRM_COnOff(IX)
		RET NZ
		XOR CHNPRM_Flags(IX)
		LD CHNPRM_Flags(IX),A
		RRA
		LD A,CHNPRM_OnOffD(IX)
		JR C,.CH_ONDL
		LD A,CHNPRM_OffOnD(IX)
.CH_ONDL:	
		LD CHNPRM_COnOff(IX),A
		RET

_PT3Play::
		XOR A
		LD (PT3_AddToEn),A
		LD (AYREGS+AR_Mixer),A
		DEC A
		LD (AYREGS+AR_EnvTp),A
		LD HL,#DelyCnt
		DEC (HL)
		JP NZ,.PL2
		LD HL,#ChanA+CHNPRM_NtSkCn
		DEC (HL)
		JR NZ,.PL1B
		LD BC,(PT3_AdInPtA)
		LD A,(BC)
		AND A
		JR NZ,.PL1A
		LD D,A
		LD (Ns_Base),A
		LD HL,(PT3_CrPsPtr)
		INC HL
		LD A,(HL)
		INC A
		JR NZ,.PLNLP
		CALL _CHECKLP
		LD HL,(PT3_LPosPtr)
		LD A,(HL)
		INC A
.PLNLP:	
		LD (PT3_CrPsPtr),HL
		DEC A
		ADD A,A
		LD E,A
		RL D
		LD HL,(PT3_PatsPtr)
		ADD HL,DE
		LD DE,(PT3_MODADDR)
		LD (PT3_PSP),SP
		LD SP,HL
		POP HL
		ADD HL,DE
		LD B,H
		LD C,L
		POP HL
		ADD HL,DE
		LD (PT3_AdInPtB),HL
		POP HL
		ADD HL,DE
		LD (PT3_AdInPtC),HL
		LD SP,(PT3_PSP)

.PL1A:		
		LD IX,#ChanA+12
		CALL PTDECOD
		LD (PT3_AdInPtA),BC

.PL1B:		
		LD HL,#ChanB+CHNPRM_NtSkCn
		DEC (HL)
		JR NZ,.PL1C
		LD IX,#ChanB+12
		LD BC,(PT3_AdInPtB)
		CALL PTDECOD
		LD (PT3_AdInPtB),BC

.PL1C:		
		LD HL,#ChanC+CHNPRM_NtSkCn
		DEC (HL)
		JR NZ,.PL1D
		LD IX,#ChanC+12
		LD BC,(PT3_AdInPtC)
		CALL PTDECOD
		LD (PT3_AdInPtC),BC

.PL1D:		
		LD A,(PT3_Delay)
		LD (DelyCnt),A

.PL2:		
		LD IX,#ChanA
		LD HL,(AYREGS+AR_TonA)
		CALL CHREGS
		LD (AYREGS+AR_TonA),HL
		LD A,(AYREGS+AR_AmplC)
		LD (AYREGS+AR_AmplA),A
		LD IX,#ChanB
		LD HL,(AYREGS+AR_TonB)
		CALL CHREGS
		LD (AYREGS+AR_TonB),HL
		LD A,(AYREGS+AR_AmplC)
		LD (AYREGS+AR_AmplB),A
		LD IX,#ChanC
		LD HL,(AYREGS+AR_TonC)
		CALL CHREGS
		LD (AYREGS+AR_TonC),HL

		LD HL,(Ns_Base_AddToNs)
		LD A,H
		ADD A,L
		LD (AYREGS+AR_Noise),A

		LD A,(PT3_AddToEn)
		LD E,A
		ADD A,A
		SBC A,A
		LD D,A
		LD HL,(EnvBase)
		ADD HL,DE
		LD DE,(CurESld)
		ADD HL,DE
		LD (AYREGS+AR_Env),HL

		XOR A
		LD HL,#CurEDel
		OR (HL)
		RET Z
		DEC (HL)
		RET NZ
		LD A,(PT3_Env_Del)
		LD (HL),A
		LD HL,(PT3_ESldAdd)
		ADD HL,DE
		LD (CurESld),HL
		RET

_PT3Rout::	
		XOR A
	
ROUT_A0:	; --- FIXES BITS 6 AND 7 OF MIXER ---
		LD	HL,#AYREGS+AR_Mixer
		set	7,(hl)
		res	6,(hl)

		LD C,#0xA0
		LD HL,#AYREGS
.LOUT:		
		OUT (C),A
		INC C
		OUTI 
		DEC C
		INC A
		CP #13
		JR NZ,.LOUT
		OUT (C),A
		LD A,(HL)
		AND A
		RET M
		INC C
		OUT (C),A
		RET

NT_DATA:	
		.db (T_NEW_0-T1_)*2
		.db TCNEW_0-T_
		.db (T_OLD_0-T1_)*2+1
		.db TCOLD_0-T_
		.db (T_NEW_1-T1_)*2+1
		.db TCNEW_1-T_
		.db (T_OLD_1-T1_)*2+1
		.db TCOLD_1-T_
		.db (T_NEW_2-T1_)*2
		.db TCNEW_2-T_
		.db (T_OLD_2-T1_)*2
		.db TCOLD_2-T_
		.db (T_NEW_3-T1_)*2
		.db TCNEW_3-T_
		.db (T_OLD_3-T1_)*2
		.db TCOLD_3-T_

T_:

TCOLD_0:	.db #0x00+1,#0x04+1,#0x08+1,#0x0A+1,#0x0C+1,#0x0E+1,#0x12+1,#0x14+1
		.db #0x18+1,#0x24+1,#0x3C+1,0
TCNEW_1:	
TCOLD_1:	.db #0x5C+1,0
TCOLD_2:	.db #0x30+1,#0x36+1,#0x4C+1,#0x52+1,#0x5E+1,#0x70+1,#0x82,#0x8C,#0x9C
		.db #0x9E,#0xA0,#0xA6,#0xA8,#0xAA,#0xAC,#0xAE,#0xAE,0
TCNEW_3:	.db #0x56+1
TCOLD_3:	.db #0x1E+1,#0x22+1,#0x24+1,#0x28+1,#0x2C+1,#0x2E+1,#0x32+1,#0xBE+1,0
TCNEW_0:	.db #0x1C+1,#0x20+1,#0x22+1,#0x26+1,#0x2A+1,#0x2C+1,#0x30+1,#0x54+1
		.db #0xBC+1,#0xBE+1,0
TCNEW_2:	.db #0x1A+1,#0x20+1,#0x24+1,#0x28+1,#0x2A+1,#0x3A+1,#0x4C+1,#0x5E+1
		.db #0xBA+1,#0xBC+1,#0xBE+1

EMPTYSAMORN: 	.db 0,1,0,#0x90 ;delete $90 if you don't need default sample

T_PACK:		; First 12 values of tone tables (packed)
		.db (#0x06EC*2)/256
		.db #0xd8
		.db #0x0755-#0x06EC
		.db #0x07C5-#0x0755
		.db #0x083B-#0x07C5
		.db #0x08B8-#0x083B
		.db #0x093D-#0x08B8
		.db #0x09CA-#0x093D
		.db #0x0A5F-#0x09CA
		.db #0x0AFC-#0x0A5F
		.db #0x0BA4-#0x0AFC
		.db #0x0C55-#0x0BA4
		.db #0x0D10-#0x0C55
		.db (#0x066D*2)/256
		.db #0xda
		.db #0x06CF-#0x066D
		.db #0x0737-#0x06CF
		.db #0x07A4-#0x0737
		.db #0x0819-#0x07A4
		.db #0x0894-#0x0819
		.db #0x0917-#0x0894
		.db #0x09A1-#0x0917
		.db #0x0A33-#0x09A1
		.db #0x0ACF-#0x0A33
		.db #0x0B73-#0x0ACF
		.db #0x0C22-#0x0B73
		.db #0x0CDA-#0x0C22
		.db (#0x0704*2)/256
		.db #0x08
		.db #0x076E-#0x0704
		.db #0x07E0-#0x076E
		.db #0x0858-#0x07E0
		.db #0x08D6-#0x0858
		.db #0x095C-#0x08D6
		.db #0x09EC-#0x095C
		.db #0x0A82-#0x09EC
		.db #0x0B22-#0x0A82
		.db #0x0BCC-#0x0B22
		.db #0x0C80-#0x0BCC
		.db #0x0D3E-#0x0C80
		.db (#0x07E0*2)/256
		.db #0xc0
		.db #0x0858-#0x07E0
		.db #0x08E0-#0x0858
		.db #0x0960-#0x08E0
		.db #0x09F0-#0x0960
		.db #0x0A88-#0x09F0
		.db #0x0B28-#0x0A88
		.db #0x0BD8-#0x0B28
		.db #0x0C80-#0x0BD8
		.db #0x0D60-#0x0C80
		.db #0x0E10-#0x0D60
		.db #0x0EF8-#0x0E10
