.globl _start
.data
msg:	.ascii "Ошибка!\n"
lmsg = . - msg
maxfilelen = 2000000

.bss
.lcomm	filelen, 4
.lcomm	filebuffer, maxfilelen
.lcomm	out_buffer, maxfilelen

.text
error:
	mov 	r7, #4
	mov 	r0, #1
	ldr 	r1, =msg
	mov 	r2, #lmsg
	svc 	#0
	b exit

_start:
	mov	r7, #3
	mov	r0, #5
	ldr 	r1, =filebuffer
	ldr 	r2, =maxfilelen
	svc 	#0
	cmp 	r0, #0
	bgt 	1f
	bl 	error
	b 	exit
1:
	ldr 	r1, =filelen
	str 	r0, [r1]
main_loop:
	eor r0, r0
	bl getch
	mov r6, r0

	cmp r6, #0x1b
	beq exit

	# 0 - plain text (ok)
	# 1 - default mode (ok)
	# 2 - colorful text (ok)
	# 3 - first line with default mode (ok)
	# 4 - first line by plain text (ok)
	# 5 - first word (ok)
	# 6 - reversed first line by plain text (ok)

	cmp r6, #0x30
	blt error
	cmp r6, #0x37
	bge error

	ldr 	r0, =filebuffer
	ldr 	r1, =filelen
	ldr 	r1, [r1]
	ldr 	r4, =out_buffer

	eor 	r7, r7

	eor 	r2, r2
	eor 	r3, r3
	eor 	r5, r5

line_processing:
	cmp r6, #0x32
	bleq start_formatting

1:
  ldrb 	r3, [r0, r2]
  cmp r3, #0x21
  bge 2f
  strb  r3, [r4, r5]
  add   r2, #1
  add   r5, #1

  subs 	r1, #1
	beq 	3f

  b 1b
2:
  b word_processing
3:
  b write

word_processing:
	cmp r6, #0x30
	beq 1f
	cmp r6, #0x32
	beq 1f
	cmp r6, #0x34
	beq 1f
	cmp r6, #0x36
	beq 1f

  bl start_formatting
1:
  ldrb 	r3, [r0, r2]
  cmp r3, #0x21
  blt 2f
  bl to_upper
  strb  r3, [r4, r5]
  add   r2, #1
  add   r5, #1

  subs 	r1, #1
	beq 	3f

  b 1b
2:
	cmp r6, #0x30
	beq end_line_processing
	cmp r6, #0x32
	beq end_line_processing
	cmp r6, #0x34
	beq end_line_processing
	cmp r6, #0x36
	beq end_line_processing

  bl end_formatting
  b end_line_processing
3:
  b write

end_line_processing:

1:
  ldrb 	r3, [r0, r2]
  cmp r3, #0x0a
  beq 2f
  strb  r3, [r4, r5]
  add   r2, #1
  add   r5, #1

  subs 	r1, #1
	beq 	3f

	cmp r6, #0x35
	beq 3f

  b 1b
2:
	cmp r6, #0x32
	bleq end_formatting

	cmp r6, #0x33
	beq 	3f
	cmp r6, #0x34
	beq 	3f
	cmp r6, #0x36
	beq 	3f

  b line_processing
3:
	cmp r6, #0x32
	bleq end_formatting

  b write

start_formatting:
	push 	{lr}
	bl 	magenta
	bl 	bold

	pop 	{r15}

end_formatting:
	eor 	r9, r9

	mov 	r9, #0x1b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x5b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x30
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x6d
	str 	r9, [r4, r5]
	add 	r5, #1

	bx 	lr

bold:
	eor 	r9, r9

	mov 	r9, #0x1b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x5b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x31
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x6d
	str 	r9, [r4, r5]
	add 	r5, #1
	bx 	lr
magenta:
	eor 	r9, r9

	mov 	r9, #0x1b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x5b
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x33
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x35
	str 	r9, [r4, r5]
	add 	r5, #1

	mov 	r9, #0x6d
	str 	r9, [r4, r5]
	add 	r5, #1
	bx 	lr
to_upper:
	cmp 	r3, #0x61
	bmi	1f
	cmp	r3, #0x7b
	submi	r3, #0x20
1:
	bx 	lr

write:
	cmp r6, #0x36
	bleq reverse_buffer

	mov 	r9, #0x0a
	str 	r9, [r4, r5]
	add		r5, #1

	mov 	r7, #4
	mov 	r0, #1
	ldr 	r1, =out_buffer
	mov 	r2, r5

	svc 	#0
	b 	main_loop

reverse_buffer:
# r2 - index
# r3 - second index
# r9 - first char
# r10 - second char
	eor r2, r2
	subs r3, r5, #1
1:
	ldrb 	r9, [r4, r2]
	ldrb	r10, [r4, r3]

	strb 	r9, [r4, r3]
	strb  r10, [r4, r2]

	add		r2, #1
	sub   r3, r3, #1

	sub r6, r2, r3
	cmp r6, #0
	beq 2f
	cmp r6, #1
	beq 2f

	b 1b
2:
	bx lr

exit:
	mov 	r7, #1
	eor 	r0, r0
	svc 	#0
