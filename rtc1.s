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
	.file	"rtc1.c"
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
	ldr	r0, .L4
	movw	r1, #4098
	push	{r4, r5, r6, lr}
.LPIC0:
	add	r0, pc
	sub	sp, sp, #8
	ldr	r5, .L4+4
	bl	open(PLT)
	mov	r2, #32505856
	movs	r3, #1
	str	r2, [sp, #4]
	mov	r1, #4096
	movs	r2, #3
.LPIC1:
	add	r5, pc
	ldr	r4, .L4+8
.LPIC3:
	add	r4, pc
	str	r0, [sp]
	movs	r0, #0
	bl	mmap(PLT)
	ldr	r3, .L4+12
	ldr	r1, .L4+16
	ldr	r5, [r5, r3]
.LPIC2:
	add	r1, pc
	mov	r6, r0
	movs	r0, #1
	ldr	r2, [r6, #16]
	str	r6, [r5]
	bl	__printf_chk(PLT)
	ldr	r3, [r5]
	mov	r1, r4
	movs	r0, #1
	ldr	r2, [r3, #20]
	bl	__printf_chk(PLT)
	ldr	r3, [r5]
	movs	r2, #249
	mov	r1, r4
	movs	r0, #1
	str	r2, [r3, #20]
	ldr	r2, [r3, #20]
	bl	__printf_chk(PLT)
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L5:
	.align	2
.L4:
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	.LC2-(.LPIC3+4)
	.word	databuf(GOT)
	.word	.LC1-(.LPIC2+4)
	.size	main, .-main
	.comm	databuf,4,4
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"/dev/mem\000"
	.space	3
.LC1:
	.ascii	"RTC Year-Month-Day Register      (0x10) : %08x\012\000"
.LC2:
	.ascii	"RTC Hour-Minute-Second Register  (0x14) : %08x\012\000"
	.ident	"GCC: (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",%progbits
