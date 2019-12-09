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
debug:	.asciz 	"Dubg there"

.bss
.lcomm ccu_regs, 4
.lcomm uart_regs, 4

.text
	.syntax unified
	.cpu arm10tdmi
file_access         = 2			@ доступ к памяти O_RDWR
uart1_base_addr     = 0x01C28000
uart1_bit           = 1
ccu_base_addr       = 0x01C20000
base_address        = 0			@ базовый адрес от места расположения
page_size           = 4096		@ размер области отображения
prot_access         = 3			@ доступ PROT_READ | PROT_WRITE
map_access          = 1			@ доступ MAP_SHARED

.global	main
.align	4
main:
	push	{r0-r7, lr}

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
  mov r7, r0
	ldr	r3, =ccu_base_addr
	stmia	sp, {r0, r3}		@ дескриптор файла и физический адрес страницы - в стек
	mov	r3, #map_access
	mov	r2, #prot_access
	mov	r1, #page_size
	mov	r0, #base_address
	bl	mmap			@ r0 -> виртуальный адрес нужной страницы физической памяти или код ошибки

	@ Сохраняем адрес
	@ldr r1, =ccu_regs
  @str r0, [r1]
  @mov r3, r0

  @ ccu regs
  ldr     r2, [r0, #728]
  bic     r2, r2, #131072
  str     r2, [r0, #728]
  ldr     r2, [r0, #108]
  orr     r2, r2, #131072
  str     r2, [r0, #108]
  ldr     r2, [r0, #728]
  orr     r2, r2, #131072
  str     r2, [r0, #728]

2:
  ldr	r3, =uart1_base_addr
  mov r0, r7
  stmia	sp, {r0, r3}		@ дескриптор файла и физический адрес страницы - в стек
  mov	r3, #map_access
  mov	r2, #prot_access
  mov	r1, #page_size
  mov	r0, #base_address
  bl	mmap			@ r0 -> виртуальный адрес нужной страницы физической памяти или код ошибки

  @ Сохраняем адрес
  ldr r1, =uart_regs
  str r0, [r1]

  @ uart regs
  movs    r1, #0
  movs    r6, #78
  ldr     r2, [r0, #1036]
  orr     r2, r2, #131
  str     r2, [r0, #1036]
  str     r6, [r0, #1024]
  str     r1, [r0, #1028]
  ldr     r2, [r0, #1036]
  bic     r2, r2, #128
  str     r2, [r0, #1036]
  mov     r4, r0

  @ read cycle
  bl      getch
.L6:
  ldr     r2, =uart_regs
  ldr     r2, [r2]
.L5:
  ldr     r3, [r2, #1044]
  lsls    r3, r3, #26
  bpl     .L5
  str     r0, [r2, #1024]
  bl      getch
  cmp     r0, #0
  bne     .L6

  bl exit

exit:
	mov	r0, #0			@ код возврата

	add	sp, sp, #8
	pop	{r0-r7, pc}
