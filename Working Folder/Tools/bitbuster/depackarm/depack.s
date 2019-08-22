
@ depack data crunched with bitbuster
@ In: r0 - address source data
@     r1 - destionation address

	.globl	Depack
	.type	 Depack,function
	.thumb_func
	.THUMB
Depack:
	push	{r2}
	
	adr	r2,1f
	bx	r2
		
	.ALIGN
	.ARM
1:
	stmfd	sp!,{r1-r8,r14}		@save registers on stack

	ldr	r2,[r0],#4		@get length of original data
	mov	r3,#0x080000000

Depack_loop:
	bl	Read_bit_from_bitstream
	bcs	Depack_compressed

	ldrb	r4,[r0],#1		@get literal byte
	strb	r4,[r1],#1		@store literal byte
	subs	r2,r2,#1		@one byte less to go

	bne	Depack_loop		@repeat while necessary
	
Depack_end:
	ldmfd	sp!,{r1-r8,r14}
	ldmfd	sp!,{r2}
	bx	lr
	

Depack_compressed:
	ldrb	r4,[r0],#1		@get lowest 8 bits of offset
	cmp	r4,#0			@if offset is zero
	beq	Depack_rle		@then handle run length compression
	
Depack_match:
	tst	r4,#128		@bit 7 set?
	beq	Depack_match1	@if so, no need to get 4 extra bits

	bl	Read_bit_from_bitstream
	addcs	r4,r4,#1024
	bl	Read_bit_from_bitstream
	addcs	r4,r4,#512
	bl	Read_bit_from_bitstream
	addcs	r4,r4,#256
	bl	Read_bit_from_bitstream
	subcc	r4,r4,#128
Depack_match1:
	add	r4,r4,#1

	bl	Get_gamma_value	@r6 = length
	sub	r5,r1,r4		@calculate source address
	mov	r7,r6
	
Depack_match_copy:
	ldrb	r4,[r5],#1
	strb	r4,[r1],#1
	subs	r7,r7,#1
	bne	Depack_match_copy
	
	subs	r2,r2,r6
	bne	Depack_loop
	b	Depack_end
	

Depack_rle:
	bl	Get_gamma_value	@r6 = length
	
	ldrb	r5,[r1,#-1]		@get value that has to be repeated

	mov	r7,r6
Depack_rle_fill:
	strb	r5,[r1],#1
	subs	r7,r7,#1
	bne	Depack_rle_fill


		
	subs	r2,r2,r6
	bne	Depack_loop
	b	Depack_end


Get_gamma_value:
	mov	r8,r14
	
	mov	r6,#1		@initial value
	mov	r7,#1		@bitcount
Get_gamma_value_size:
	bl	Read_bit_from_bitstream
	bcc	Get_gamma_value_size_end
	add	r7,r7,#1
	b	Get_gamma_value_size

Get_gamma_value_bits:
	bl	Read_bit_from_bitstream
	mov	r6,r6,lsl #1
	addcs	r6,r6,#1
Get_gamma_value_size_end:
	subs	r7,r7,#1
	bne	Get_gamma_value_bits

	add	r6,r6,#1
	
	mov	r14,r8
	bx	lr
	

Read_bit_from_bitstream:
	movs	r3,r3,lsl #1	@shift one bit out
	bxne	lr			@return if there's still data left in
	ldrb	r3,[r0],#1		@get 8 new bits from bitstream
	movs	r3,r3,lsl #25	@shift highest bit out
	add	r3,r3,#0x01000000	@set bitstream terminator
	bx	lr	

