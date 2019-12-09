	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"r232r.c"
	.text
	.section	.text.startup,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r0, .L36
	movw	r1, #4098
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
.LPIC0:
	add	r0, pc
	sub	sp, sp, #8
	ldr	r5, .L36+4
	bl	open(PLT)
.LPIC2:
	add	r5, pc
	adds	r4, r0, #1
	beq	.L27
	mov	r4, r0
	str	r0, [sp]
	mov	r2, #29491200
	movs	r3, #1
	str	r2, [sp, #4]
	mov	r1, #4096
	movs	r2, #3
	movs	r0, #0
	mov	r9, r0
	bl	mmap(PLT)
	ldr	r2, .L36+8
	movs	r3, #1
	mov	r1, #32768
	movt	r1, 450
	ldr	r8, .L36+12
	ldr	r10, [r5, r2]
	movs	r2, #3
	str	r4, [sp]
.LPIC9:
	add	r8, pc
	str	r1, [sp, #4]
	mov	r1, #4096
	ldr	r7, .L36+16
	mov	r4, r9
	ldr	r6, .L36+20
.LPIC8:
	add	r7, pc
.LPIC7:
	add	r6, pc
	str	r0, [r10]
	mov	r0, r9
	bl	mmap(PLT)
	ldr	r2, [r10]
	ldr	r1, [r2, #728]
	bic	r1, r1, #262144
	mov	r3, r0
	ldr	r0, .L36+24
	ldr	r5, [r5, r0]
	movs	r0, #1
	str	r1, [r2, #728]
	ldr	r1, [r2, #108]
	str	r3, [r5]
	orr	r1, r1, #262144
	str	r1, [r2, #108]
	ldr	r1, [r2, #728]
	orr	r1, r1, #262144
	str	r1, [r2, #728]
	ldr	r2, [r3, #2060]
	movs	r1, #78
	orr	r2, r2, #131
	str	r2, [r3, #2060]
	str	r1, [r3, #2048]
	str	r9, [r3, #2052]
	ldr	r1, .L36+28
	ldr	r2, [r3, #2060]
.LPIC3:
	add	r1, pc
	bic	r2, r2, #128
	str	r2, [r3, #2060]
	bl	__printf_chk(PLT)
.L15:
	ldr	r2, [r5]
.L4:
	ldr	r3, [r2, #2068]
	lsls	r1, r3, #31
	bpl	.L4
	ldr	r0, [r2, #2048]
	uxtb	r0, r0
	cmp	r4, #0
	it	eq
	cmpeq	r0, #27
	beq	.L16
	cmp	r4, #0
	beq	.L25
	sub	r3, r4, #1
	cmp	r0, #91
	it	eq
	cmpeq	r4, #1
	clz	r3, r3
	lsr	r3, r3, #5
	beq	.L17
	cmp	r0, #79
	ite	ne
	movne	r3, #0
	andeq	r3, r3, #1
	cmp	r3, #0
	bne	.L18
	cmp	r4, #1
	beq	.L28
	sub	r2, r4, #2
	clz	r2, r2
	lsrs	r2, r2, #5
	cmp	r0, #64
	ite	ls
	movls	r1, #0
	andhi	r1, r2, #1
	cmp	r1, #0
	beq	.L8
	cmp	r0, #68
	bls	.L29
.L9:
	sub	r3, r4, #3
	sub	r2, r0, #80
	clz	r3, r3
	lsrs	r3, r3, #5
	cmp	r2, #2
	ite	hi
	movhi	r2, #0
	andls	r2, r3, #1
	cmp	r2, #0
	bne	.L30
	cmp	r0, #83
	ite	ne
	movne	r3, #0
	andeq	r3, r3, #1
	cmp	r3, #0
	bne	.L31
	sub	r3, r0, #53
	sub	r2, r4, #4
	clz	r2, r2
	cmp	r3, #2
	it	ls
	cmpls	r4, #4
	lsr	r2, r2, #5
	beq	.L32
	cmp	r0, #56
	ite	ne
	movne	r3, #0
	andeq	r3, r2, #1
	cmp	r3, #0
	bne	.L33
	cmp	r0, #57
	ite	ne
	movne	r2, #0
	andeq	r2, r2, #1
	cmp	r2, #0
	bne	.L34
	cmp	r4, #5
	it	eq
	moveq	r4, #0
.L5:
	movs	r0, #0
	bl	fflush(PLT)
	b	.L15
.L16:
	movs	r4, #1
	b	.L5
.L28:
	mov	r4, r3
.L25:
	bl	putchar(PLT)
	b	.L5
.L17:
	movs	r4, #2
	b	.L5
.L18:
	movs	r4, #3
	b	.L5
.L8:
	cmp	r0, #49
	ite	ne
	movne	r3, #0
	andeq	r3, r2, #1
	cbz	r3, .L35
	movs	r4, #4
	b	.L5
.L29:
	ldr	r1, .L36+32
	mov	r2, r0
	movs	r0, #1
	mov	r4, r3
.LPIC4:
	add	r1, pc
	bl	__printf_chk(PLT)
	b	.L5
.L31:
	ldr	r1, .L36+36
	movs	r0, #1
	mov	r4, r2
.LPIC6:
	add	r1, pc
	bl	__printf_chk(PLT)
	b	.L5
.L35:
	cmp	r0, #50
	ite	ne
	movne	r2, #0
	andeq	r2, r2, #1
	cmp	r2, #0
	beq	.L9
	mov	r0, r3
.L1:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L30:
	sub	r2, r0, #32
	ldr	r1, .L36+40
	movs	r0, #1
	movs	r4, #0
.LPIC5:
	add	r1, pc
	uxtb	r2, r2
	bl	__printf_chk(PLT)
	b	.L5
.L33:
	mov	r1, r7
	movs	r0, #1
	bl	__printf_chk(PLT)
	movs	r4, #5
	b	.L5
.L34:
	mov	r1, r8
	movs	r0, #1
	bl	__printf_chk(PLT)
	movs	r4, #5
	b	.L5
.L32:
	mov	r2, r0
	mov	r1, r6
	movs	r0, #1
	movs	r4, #5
	bl	__printf_chk(PLT)
	b	.L5
.L27:
	ldr	r0, .L36+44
.LPIC1:
	add	r0, pc
	bl	puts(PLT)
	movs	r0, #1
	b	.L1
.L37:
	.align	2
.L36:
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.word	ccu_regs(GOT)
	.word	.LC2-(.LPIC9+4)
	.word	.LC7-(.LPIC8+4)
	.word	.LC6-(.LPIC7+4)
	.word	uart_regs(GOT)
	.word	.LC2-(.LPIC3+4)
	.word	.LC3-(.LPIC4+4)
	.word	.LC5-(.LPIC6+4)
	.word	.LC4-(.LPIC5+4)
	.word	.LC1-(.LPIC1+4)
	.size	main, .-main
	.comm	uart_regs,4,4
	.comm	ccu_regs,4,4
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"/dev/mem\000"
	.space	3
.LC1:
	.ascii	"\320\236\321\210\320\270\320\261\320\272\320\260 \320"
	.ascii	"\276\321\202\320\272\321\200\321\213\321\202\320\270"
	.ascii	"\321\217 \321\204\320\260\320\271\320\273\320\260 /"
	.ascii	"dev/mem\000"
	.space	2
.LC2:
	.ascii	"\033[H\033[2J\000"
.LC3:
	.ascii	"\033[%c\000"
	.space	3
.LC4:
	.ascii	"\033[3%cm\000"
	.space	1
.LC5:
	.ascii	"\033[39m\000"
	.space	2
.LC6:
	.ascii	"\033[4%cm\000"
	.space	1
.LC7:
	.ascii	"\033[49m\000"
	.ident	"GCC: (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",%progbits
