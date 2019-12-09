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
output: .asciz  "%c"
debug:	.asciz 	"Dubg there"
clear: 	.asciz 	"\x1b[H\x1b[2J"
move: .asciz "\x1b[%c"
set_text_color: .asciz "\x1b[3%cm"
set_foreground: .asciz "\x1b[4%cm"
def_text_color: .asciz "\x1b[39m"
def_foreground: .asciz "\x1b[49m"

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
  bic     r2, r2, #262144
  str     r2, [r0, #728]
  ldr     r2, [r0, #108]
  orr     r2, r2, #262144
  str     r2, [r0, #108]
  ldr     r2, [r0, #728]
  orr     r2, r2, #262144
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
  ldr     r2, [r0, #2060]
  orr     r2, r2, #131
  str     r2, [r0, #2060]
  str     r6, [r0, #2048]
  str     r1, [r0, #2052]
  ldr     r2, [r0, #2060]
  bic     r2, r2, #128
  str     r2, [r0, #2060]

	push 	{r0-r7}
	ldr 	r0, =clear
	bl 		printf
	pop		{r0-r7}

	mov 	r5, r0

	.L15:
	        mov     r2, r5
	.L4:
	        ldr     r3, [r2, #2068]
	        lsls    r1, r3, #31
	        bpl     .L4
	        ldr     r0, [r2, #2048]
	        cmp     r4, #0
	        it      eq
	        cmpeq   r0, #27
	        beq     .L16
	        cmp     r4, #0
	        beq     .L25
	        sub     r3, r4, #1
	        cmp     r0, #91
	        it      eq
	        cmpeq   r4, #1
	        clz     r3, r3
	        lsr     r3, r3, #5
	        beq     .L17
	        cmp     r0, #79
	        ite     ne
	        movne   r3, #0
	        andeq   r3, r3, #1
	        cmp     r3, #0
	        bne     .L18
	        cmp     r4, #1
	        beq     .L28
	        sub     r2, r4, #2
	        clz     r2, r2
	        lsrs    r2, r2, #5
	        cmp     r0, #64
	        ite     ls
	        movls   r1, #0
	        andhi   r1, r2, #1
	        cmp     r1, #0
	        beq     .L8
	        cmp     r0, #68
					bls     .L29
	.L9:
	        sub     r3, r4, #3
	        sub     r2, r0, #80
	        clz     r3, r3
	        lsrs    r3, r3, #5
	        cmp     r2, #2
	        ite     hi
	        movhi   r2, #0
	        andls   r2, r3, #1
	        cmp     r2, #0
	        bne     .L30
	        cmp     r0, #83
	        ite     ne
	        movne   r3, #0
	        andeq   r3, r3, #1
	        cmp     r3, #0
	        bne     .L31
	        sub     r3, r0, #53
	        sub     r2, r4, #4
	        clz     r2, r2
	        cmp     r3, #2
	        it      ls
	        cmpls   r4, #4
	        lsr     r2, r2, #5
	        beq     .L32
	        cmp     r0, #56
	        ite     ne
	        movne   r3, #0
	        andeq   r3, r2, #1
	        cmp     r3, #0
	        bne     .L33
	        cmp     r0, #57
	        ite     ne
	        movne   r2, #0
	        andeq   r2, r2, #1
	        cmp     r2, #0
	        bne     .L34
	        cmp     r4, #5
	        it      eq
	        moveq   r4, #0
	.L5:
	        movs    r0, #0
	        bl      fflush
	        b       .L15
	.L16:
	        movs    r4, #1
	        b       .L5
	.L28:
	        mov     r4, r3
	.L25:
    bl      putchar
    b       .L5
	.L17:
	        movs    r4, #2
	        b       .L5
	.L18:
	        movs    r4, #3
	        b       .L5
	.L8:
	        cmp     r0, #49
	        ite     ne
	        movne   r3, #0
	        andeq   r3, r2, #1
	        movs    r4, #4
	        b       .L5
	.L29:
	        mov     r1, r0
	        ldr    	r0, =move
	        mov     r4, r3
	        bl      printf
	        b       .L5
	.L31:
	        ldr	    r0, =def_text_color
	        mov     r4, r2
	        bl      printf
	        b       .L5
	.L30:
	        sub     r2, r0, #32
	        mov    r1, r2
	        ldr    	r0, =set_text_color
	        movs    r4, #0
	        bl      printf
	        b       .L5
	.L33:
	        ldr	    r0, =def_foreground
	        bl      printf
	        movs    r4, #5
	        b       .L5
	.L34:
	        ldr	    r0, =clear
	        bl      printf
	        movs    r4, #5
	        b       .L5
	.L32:
	        mov     r2, r0
	        mov     r1, r0
	        ldr	    r0, =set_foreground
	        movs    r4, #5
	        bl      printf
	        b       .L5



exit:
	mov	r0, #0			@ код возврата

	add	sp, sp, #8
	pop	{r0-r7, pc}
