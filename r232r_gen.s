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
	ldr	r0, .L11
	movw	r1, #4098
	push	{r4, r5, r6, r7, lr}
.LPIC0:
	add	r0, pc
	sub	sp, sp, #12
	ldr	r7, .L11+4
	bl	open(PLT)
.LPIC2:
	add	r7, pc
	adds	r2, r0, #1
	beq	.L8
	mov	r4, r0
	str	r0, [sp]
	mov	r2, #29491200
	movs	r3, #1
	str	r2, [sp, #4]
	mov	r1, #4096
	movs	r2, #3
	movs	r0, #0
	bl	mmap(PLT)
	ldr	r2, .L11+8
	movs	r3, #1
	mov	r1, #32768
	movt	r1, 450
	ldr	r5, .L11+12
	ldr	r6, [r7, r2]
	movs	r2, #3
	str	r4, [sp]
.LPIC3:
	add	r5, pc
	str	r1, [sp, #4]
	mov	r1, #4096
	ldr	r4, .L11+16
	str	r0, [r6]
	movs	r0, #0
	bl	mmap(PLT)
	ldr	r3, [r6]
	ldr	r4, [r7, r4]
	movs	r6, #78
	movs	r1, #0
	ldr	r2, [r3, #728]
	bic	r2, r2, #262144
	str	r2, [r3, #728]
	ldr	r2, [r3, #108]
	orr	r2, r2, #262144
	str	r2, [r3, #108]
	ldr	r2, [r3, #728]
	orr	r2, r2, #262144
	str	r2, [r3, #728]
	ldr	r3, [r0, #2060]
	str	r0, [r4]
	orr	r3, r3, #131
	str	r3, [r0, #2060]
	str	r6, [r0, #2048]
	str	r1, [r0, #2052]
	ldr	r3, [r0, #2060]
	bic	r3, r3, #128
	str	r3, [r0, #2060]
.L3:
	ldr	r3, [r0, #2068]
	lsls	r3, r3, #31
	bpl	.L3
	ldr	r2, [r0, #2048]
	mov	r1, r5
	movs	r0, #1
	bl	__printf_chk(PLT)
	ldr	r0, [r4]
	b	.L3
.L8:
	ldr	r0, .L11+20
.LPIC1:
	add	r0, pc
	bl	puts(PLT)
	movs	r0, #1
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L12:
	.align	2
.L11:
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.word	ccu_regs(GOT)
	.word	.LC2-(.LPIC3+4)
	.word	uart_regs(GOT)
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
	.ascii	"%c\000"
	.ident	"GCC: (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",%progbits
