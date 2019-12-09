.globl _start

.text
_start:
  cmp r0, #27
  beq exit
  b _start

exit:
	mov 	r7, #1
	eor 	r0, r0
	svc 	#0
