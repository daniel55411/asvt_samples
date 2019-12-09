.globl _s

.data
    of: .asciz "OUT_FILE_NAME"
    e_2: .asciz "Не указан выходной файл!"
    e_3: .asciz "Не указаны входные файлы!"
    e_4_1: .ascii "Файл "
    e_4_2: .asciz " не найден!"
    e_5: .ascii "Ошибка при работе с файлом "

.bss
    .lcomm ofd, 4
    .lcomm ofn, 4
    .lcomm bc, 4
    .lcomm b, 33554432

.text
_s:
    mov     r5, sp
    ldrb    r5, [r5]
    add     r6, r5, #2
1:
    ldr    r0, [sp, r6, LSL #2]
    cmp r0, #0
    beq 1f
    bl env_cmp
    cmp r0, #0
    addne r6, r6, #1
    bne 1b
    moveq r6, r1
    b 2f
1:
    mov     r0, #1 
    ldr     r1, =e_2
    mov     r2, #44
    mov     r3, #2
    b e_ex
 2:  
    ldrb r0, [r6]
    cmp r0, #0
    beq 1b

    push {r1}
    mov     r0, #1 
    ldr     r1, =e_3
    mov     r2, #46
    mov     r3, #3
    cmp     r5, #1
    beq e_ex
    pop {r1}

    mov     r6, r1
    mov     r0, r1
    movw    r1, #577
    movt    r1, #8
    mov     r2, #0666
    mov     r7, #5
    svc #0

    ldr r1, =ofn
    str r6, [r1]

    cmp r0, #0
    movlt r0, r6
    movlt r1, #6
    blt e6_ex

    ldr r1, =ofd
    str r0, [r1]

    mov r0, #0
    ldr r6, =bc
    str r0, [r6]

    mov r6, #2
3:
    cmp r6, r5
    bgt 3f

    ldr    r0, [sp, r6, LSL #2]
    mov     r4, r0
    mov     r1, #524288
    mov     r7, #5
    svc #0

    cmp r0, #-2
    moveq r0, r4
    beq e4_ex

    bl rw

    mov r7, #6
    svc #0

    add r6, #1
    b 3b
3:
    ldr r0, =ofd
    ldr r0, [r0]
    ldr r1, =b
    ldr r2, =bc
    ldr r2, [r2]
    mov r7, #4
    svc #0

    cmp r0, #0
    ldrlt r0, =ofn
    movlt r1, #6
    blt e6_ex

    ldr r0, =ofd
    ldr r0, [r0]
    mov r7, #6
    svc #0

    b ok_ex    

env_cmp:
    push {r2-r7, lr}
    ldr r1, =of
    mov r2, #0
    mov r3, #0
1:
    ldrb r4, [r0, r2]
    ldrb r5, [r1, r3]
    cmp r4, #61
    beq 2f
    cmp r4, r5
    movne r0, #1  
    bne 3f
    add r3, #1
    add r2, #1
    b 1b
2:
    cmp r5, #0
    addeq r1, r0, r3
    addeq r1, r1, #1
    moveq r0, #0
    beq 3f
    mov r0, #1
3:
    pop {r2-r7, pc}

e6_ex:
    push {r0, r1}
    mov     r0, #1
    ldr     r1, =e_5
    mov     r2, #49
    mov     r7, #4
    svc #0

    pop {r0, r3}
    mov r6, r0
    bl len
    mov     r2, r0
    mov     r0, #1
    mov     r1, r6
    b e_ex

e4_ex:
    push    {r0}
    mov     r0, #1
    ldr     r1, =e_4_1
    mov     r2, #9
    mov     r7, #4
    svc #0

    pop {r0}
    mov r6, r0
    bl len
    mov     r2, r0
    mov     r0, #1
    mov     r1, r6
    svc #0

    mov     r0, #1
    ldr     r1, =e_4_2
    mov     r2, #19
    mov     r3, #4
    b e_ex

e_ex:
    mov     r7, #4
    svc #0
    mov     r0, r3
    mov     r7, #1
    svc #0

ok_ex:
    mov     r7, #1
    eor     r0, r0
    svc #0
    
len:
    push {r1-r7, lr}
    mov     r1, #0
1:
    ldrb r2, [r0, r1]
    cmp r2, #0
    moveq r0, r1
    beq 1f
    add r1, #1
    bne 1b
1:
    pop {r1-r7, pc}

rw:
    push {r0, r4, r5, lr}
    mov r5, r0
    ldr r3, =bc
    ldr r3, [r3]
1:
    mov r0, r5
    ldr r1, =b
    add r1, r3
    mov r2, #1024
    mov r7, #3
    svc #0
    add r3, r0
    cmp r0, #1024
    beq 1b
    cmp r0, #0
    blt 1f
    ldr r4, =bc
    str r3, [r4]
    pop {r0, r4, r5, pc}
1:
    ldr r0, =ofd
    ldr r0, [r0]
    mov r7, #6
    svc #0

    pop {r0, r4, r7}
    movlt r0, r4
    movlt r1, #5
    blt e6_ex
