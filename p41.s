@
@ чтение регистра RTC Orange Pi Zero
@
@ сборка
@		gcc -o p41 p41.s
@ запуск
@		sudo ./p41
@

.data
ferror:	.asciz	"Input file open error\n"
merror:	.asciz	"Input file mmap error\n"
fname:	.asciz	"/dev/mem"
rtc:		.asciz	"RTC Hour-Minute-Second Register  (0x14) : %02d-%02d-%02d\n"
rtc_debug:		.asciz	"RTC Hour-Minute-Second Register  (0x14) : %08x\n"
clear: 	.asciz 	"\x1b[H\x1b[2J"
debug:	.asciz 	"Saved=%d\n"

.bss
.lcomm hitted, 4
.lcomm power,	 1
.lcomm saved, 1
.lcomm rtc_address, 4
.lcomm rtc_backup, 4

.text
	.syntax unified
	.cpu arm10tdmi
file_access         = 2			@ доступ к памяти O_RDWR
timer_register_base = 0x01f00000	@ физический адрес места расположения регистров, отображаются только страницы с их реального начала, д.б. кратно 0х1000
base_address        = 0			@ базовый адрес от места расположения
page_size           = 4096		@ размер области отображения
prot_access         = 3			@ доступ PROT_READ | PROT_WRITE
map_access          = 1			@ доступ MAP_SHARED

.global	main
.align	4
main:
	push	{r0-r7, lr}

	ldr 	r0, =power
	eor		r1, r1
	strb	r1, [r0]
	ldr		r0, =saved
	strb 	r1, [r0]

	ldr		r0, =saved
	ldrb	r0, [r0]
	mov	r1, r0
	ldr r0, =debug
	bl 	printf

	sub	sp, sp, #8
@ открытие входного файла /dev/mem
	ldr	r0, =fname
	mov	r1, #file_access
	bl	open			@ r0 -> номер дескриптора или код ошибки
	cmn	r0, #1
	bne	1f
@ ошибка открытия
	ldr	r0, =ferror
	bl	printf
	mov	r0, #1			@ код возврата
	b	exit
1: @ mmap - отображаем физическую память в виртуальную
	ldr	r3, =timer_register_base
	stmia	sp, {r0, r3}		@ дескриптор файла и физический адрес страницы - в стек
	mov	r3, #map_access
	mov	r2, #prot_access
	mov	r1, #page_size
	mov	r0, #base_address
	bl	mmap			@ r0 -> виртуальный адрес нужной страницы физической памяти или код ошибки

	@ Сохраняем адрес
	ldr r1, =rtc_address
	str r0, [r1]

	cmn	r0, #1
	bne	main_loop
@ ошибка mmap
	ldr	r0, =merror
	bl	printf
	mov	r0, #2			@ код возврата
	b	exit

main_loop:
	bl 		set_icanon_mode
	ldr 	r0, =hitted
	bl 		kbhit
	bl 		clear_icanon_mode

	ldr 	r0, =hitted
	ldrb 	r0, [r0]
	cmp		r0, #0
	beq 	1f

	eor 	r0, r0
	bl 		getch
	cmp		r0, #0x1b
	beq 	exit
	cmp		r0, #0x20
	bleq 	toggle_power
	cmp		r0, #0x31
	bleq	inc_hour
	cmp		r0, #0x32
	bleq	dec_hour
	cmp		r0, #0x33
	bleq	inc_minute
	cmp		r0, #0x34
	bleq	dec_minute
	cmp		r0, #0x35
	bleq	inc_second
	cmp		r0, #0x36
	bleq	dec_second

@ печатаем RTC Hour-Minute-Second Register (0x14)
1:
	bl 		check_power_on
	beq		2f

	bl		check_rtc_saved
	beq 	main_loop

	bl 		print_rtc
	bl 		save_rtc

	b 		main_loop
2:
	bl		check_rtc_saved
	bleq 	restore_rtc

	bl 	print_rtc
	bl	clear_console

	b main_loop

check_rtc_saved:
	push 	{r0, r1, lr}

	ldr		r0, =saved
	ldrb	r0, [r0]
	cmp		r0, #1

	pop 	{r0, r1, pc}

check_power_on:
	push 	{r0, lr}

	ldr 	r0, =power
	ldrb 	r0, [r0]
	cmp 	r0, #0

	pop 	{r0, pc}

toggle_power:
	push 	{r0, r1, lr}

	ldr 	r0, =power
	ldrb 	r1, [r0]
	eor 	r1, r1, #1
	strb	r1, [r0]

	pop 	{r0, r1, pc}

save_rtc:
	push  {r0, r1, lr}

	ldr 	r0, =rtc_address
	ldr 	r0, [r0]
	ldr 	r0, [r0, 0x14]
	ldr		r1, =rtc_backup
	str		r0, [r1]
	mov		r1, #1
	ldr		r0, =saved
	strb	r1, [r0]

	pop		{r0, r1, pc}

restore_rtc:
	push  {r0, r1, lr}

	ldr 	r0, =rtc_backup
	ldr 	r0, [r0]
	ldr		r1, =rtc_address
	ldr		r1, [r1]
	str		r0, [r1, #0x14]
	mov		r1, #0
	ldr		r0, =saved
	strb	r1, [r0]

	pop		{r0, r1, pc}

print_rtc:
	push 	{r0-r3, lr}

	ldr 	r0, =rtc_address
	ldr 	r0, [r0]
	ldrb	r3, [r0, #0x14]
	ldrb	r2, [r0, #0x15]
	ldrb	r1, [r0, #0x16]
	ldr	r0, =rtc
	bl	printf

	pop 	{r0-r3, pc}

print_rtc_backup:
	push 	{r0-r3, lr}

	ldr 	r0, =rtc_backup
	ldrb	r3, [r0]
	ldrb	r2, [r0, #0x1]
	ldrb	r1, [r0, #0x2]
	ldr	r0, =rtc
	bl	printf

	pop 	{r0-r3, pc}

clear_console:
	push 	{r0, lr}

	ldr 	r0, =clear
	bl 		printf

	pop 	{r0, pc}

exit:
	mov	r0, #0			@ код возврата

	add	sp, sp, #8
	pop	{r0-r7, pc}

inc_hour:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0, 0x2]
	add		r1, r1, #1
	cmp		r1, 24
	blt		2f

	mov		r1, #0
2:
	strb 	r1, [r0, 0x2]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}

dec_hour:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0, 0x2]
	sub		r1, r1, #1
	cmp		r1, #0
	bge		2f

	mov		r1, #23
2:
	strb 	r1, [r0, 0x2]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}

inc_minute:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0, 0x1]
	add		r1, r1, #1
	cmp		r1, 60
	blt		2f

	mov		r1, #0
2:
	strb 	r1, [r0, 0x1]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}

dec_minute:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0, 0x1]
	sub		r1, r1, #1
	cmp		r1, #0
	bge		2f

	mov		r1, #59
2:
	strb 	r1, [r0, 0x1]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}

inc_second:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0]
	add		r1, r1, #1
	cmp		r1, 60
	blt		2f

	mov		r1, #0
2:
	strb 	r1, [r0]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}

dec_second:
	push 	{r0-r7, lr}

	@ check power off
	mov		r2, #0

	bl		check_power_on
	bleq	toggle_power

	bl		check_rtc_saved
	beq		1f

	mov		r2, #1
	bl		clear_console
	bl		print_rtc
	bl		save_rtc
1:
	ldr		r0,	=rtc_backup
	ldrb	r1, [r0]
	sub		r1, r1, #1
	cmp		r1, #0
	bge		2f

	mov		r1, #59
2:
	strb 	r1, [r0]

	cmp		r2, #0
	beq		3f

	bl		restore_rtc
	bl		toggle_power
	b			4f
3:
	bl 		clear_console
	bl		print_rtc_backup

4:
	pop 	{r0-r7, pc}
