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
	.file	"r232w.c"
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
	ldr	r0, .L12
	movw	r1, #4098
	push	{r4, r5, r6, r7, lr}
.LPIC0:
	add	r0, pc
	sub	sp, sp, #12
	ldr	r4, .L12+4
	bl	open(PLT)
.LPIC2:
	add	r4, pc
	adds	r2, r0, #1
	beq	.L11
	mov	r5, r0
	str	r0, [sp]
	mov	r2, #29491200
	movs	r3, #1
	str	r2, [sp, #4]
	mov	r1, #4096
	movs	r2, #3
	movs	r0, #0
	bl	mmap(PLT)
	ldr	r1, .L12+8
	movs	r3, #1
	movs	r2, #3
	mov	r7, #32768
	movt	r7, 450
	ldr	r6, [r4, r1]
	mov	r1, #4096
	str	r5, [sp]
	str	r7, [sp, #4]
	ldr	r5, .L12+12
	str	r0, [r6]
	movs	r0, #0
	bl	mmap(PLT)
	ldr	r3, [r6]
	ldr	r5, [r4, r5]
	movs	r1, #0
	movs	r6, #12
	ldr	r2, [r3, #728]
	bic	r2, r2, #131072
	str	r2, [r3, #728]
	ldr	r2, [r3, #108]
	orr	r2, r2, #131072
	str	r2, [r3, #108]
	ldr	r2, [r3, #728]
	orr	r2, r2, #131072
	str	r2, [r3, #728]
	ldr	r3, .L12+16
	ldr	r2, [r0, #1036]
	str	r0, [r5]
	orr	r2, r2, #131
	str	r2, [r0, #1036]
	str	r6, [r0, #1024]
	str	r1, [r0, #1028]
	ldr	r2, [r0, #1036]
	bic	r2, r2, #128
	str	r2, [r0, #1036]
	ldr	r4, [r4, r3]
	ldr	r0, [r4]
	bl	_IO_getc(PLT)
	uxtb	r0, r0
	cbz	r0, .L1
.L6:
	ldr	r2, [r5]
.L5:
	ldr	r3, [r2, #1044]
	lsls	r3, r3, #26
	bpl	.L5
	str	r0, [r2, #1024]
	ldr	r0, [r4]
	bl	_IO_getc(PLT)
	uxtb	r0, r0
	cmp	r0, #0
	bne	.L6
.L1:
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L11:
	ldr	r0, .L12+20
.LPIC1:
	add	r0, pc
	bl	puts(PLT)
	movs	r0, #1
	b	.L1
.L13:
	.align	2
.L12:
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.word	ccu_regs(GOT)
	.word	uart_regs(GOT)
	.word	stdin(GOT)
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
	.ident	"GCC: (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",%progbits
