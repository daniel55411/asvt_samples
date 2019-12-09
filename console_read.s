.globl getch
.globl getche
.globl kbhit
.globl set_icanon_mode
.globl set_icanon_echo_mode
.globl clear_icanon_mode
.globl clear_icanon_echo_mode

.bss
.lcomm struct, 60
.text

@ r0 - previous mode
set_icanon_mode:
    push {r1, r2, r3, r9, lr}
    mov	r0, #0
    ldr r1, =struct
    bl tcgetattr

    ldr r2, =struct
    ldr	r3, [r2, #12]
    mov r9, r3
    bic	r3, r3, #10
    ldr r2, =struct
    str	r3, [r2, #12]

    ldr	r2, =struct
    mov	r1, #0
    mov	r0, #0
    bl	tcsetattr
    mov r0, r9

    pop {r1, r2, r3, r9, pc}


@ r0 - previous mode
clear_icanon_mode:
    push {r1, r2, r3, r9, lr}
    mov	r0, #0
    ldr r1, =struct
    bl tcgetattr

    ldr r2, =struct
    ldr	r3, [r2, #12]
    mov r9, r3
    orr	r3, r3, #10
    ldr r2, =struct
    str	r3, [r2, #12]

    ldr	r2, =struct
    mov	r1, #0
    mov	r0, #0
    bl	tcsetattr
    mov r0, r9

    pop {r1, r2, r3, r9, pc}

@ r0 - previous mode
set_icanon_echo_mode:
    push {r1, r2, r3, lr}
    mov	r0, #0
    ldr r1, =struct
    bl tcgetattr

    ldr r2, =struct
    ldr	r3, [r2, #12]
    mov r9, r3
    bic	r3, r3, #2
    ldr r2, =struct
    str	r3, [r2, #12]

    ldr	r2, =struct
    mov	r1, #0
    mov	r0, #0
    bl	tcsetattr
    mov r0, r9
    pop {r1, r2, r3, pc}

clear_icanon_echo_mode:
    push {r1, r2, r3, lr}
    mov	r0, #0
    ldr r1, =struct
    bl tcgetattr

    ldr r2, =struct
    ldr	r3, [r2, #12]
    mov r9, r3
    orr	r3, r3, #2
    ldr r2, =struct
    str	r3, [r2, #12]

    ldr	r2, =struct
    mov	r1, #0
    mov	r0, #0
    bl	tcsetattr
    mov r0, r9
    pop {r1, r2, r3, pc}


getch:
    push	{r1-r12, lr}

    bl set_icanon_mode
    bl	getchar

    push {r0}
    bl clear_icanon_mode
    pop {r0}

    pop	{r1-r12, pc}


getche:
    push	{r1-r12, lr}

    bl set_icanon_echo_mode
    bl	getchar

    push {r0}
    bl clear_icanon_echo_mode
    pop {r0}

    pop	{r1-r12, pc}


@ Есть ли во входном потоке символ
@ r0 - адрес переменной, в которую пишется число символов во входном потоке
kbhit:
    push {r1, r2, lr}
    mov r2, r0
    mov r1, #21531
    mov r0, #0
    bl ioctl
    pop {r1, r2, pc}
