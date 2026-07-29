	.file	"keypad.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	Keypad_Init
	.type	Keypad_Init, @function
Keypad_Init:
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 3 */
.L__stack_usage = 3
	movw r28,r24
	or r24,r25
	breq .L7
	ld r24,Y
	cpi r24,lo8(4)
	brsh .L7
	ldd r24,Y+2
	cpi r24,lo8(4)
	brsh .L7
	ldi r17,0
.L3:
	ldd r22,Y+1
	add r22,r17
	ldi r20,lo8(1)
	ld r24,Y
	call GPIO_SetPinDirection
	ldd r22,Y+1
	add r22,r17
	ldi r20,lo8(1)
	ld r24,Y
	call GPIO_PinSetValue
	subi r17,lo8(-(1))
	cpi r17,lo8(4)
	brne .L3
	ldi r17,0
.L4:
	ldd r22,Y+3
	add r22,r17
	ldi r20,0
	ldd r24,Y+2
	call GPIO_SetPinDirection
	ldd r22,Y+3
	add r22,r17
	ldi r20,lo8(1)
	ldd r24,Y+2
	call GPIO_PinSetValue
	subi r17,lo8(-(1))
	cpi r17,lo8(4)
	brne .L4
	ldi r25,0
	ldi r24,0
.L1:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	ret
.L7:
	ldi r24,lo8(1)
	ldi r25,0
	rjmp .L1
	.size	Keypad_Init, .-Keypad_Init
.global	Keypad_GetKey
	.type	Keypad_GetKey, @function
Keypad_GetKey:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	rcall .
	rcall .
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 4 */
/* stack size = 14 */
.L__stack_usage = 14
	sbiw r24,0
	brne .+2
	rjmp .L29
	cp r22,__zero_reg__
	cpc r23,__zero_reg__
	brne .+2
	rjmp .L29
	movw r30,r24
	ld r18,Z
	cpi r18,lo8(4)
	brlo .+2
	rjmp .L29
	ldd r18,Z+2
	cpi r18,lo8(4)
	brlo .+2
	rjmp .L29
	movw r14,r22
	movw r16,r24
	ldi r24,lo8(-1)
	movw r30,r22
	st Z,r24
	mov r13,__zero_reg__
.L12:
	mov r12,__zero_reg__
.L13:
	movw r30,r16
	ldd r22,Z+1
	add r22,r12
	ldi r20,lo8(1)
	ld r24,Z
	call GPIO_PinSetValue
	inc r12
	ldi r31,lo8(4)
	cpse r12,r31
	rjmp .L13
	movw r30,r16
	ldd r22,Z+1
	add r22,r13
	ldi r20,0
	ld r24,Z
	call GPIO_PinSetValue
	mov r12,__zero_reg__
.L24:
	std Y+4,__zero_reg__
	movw r30,r16
	ldd r22,Z+3
	add r22,r12
	movw r20,r28
	subi r20,-4
	sbci r21,-1
	ldd r24,Z+2
	call GPIO_GetPinValue
	movw r10,r24
	or r24,r25
	brne .L29
	ldd r24,Y+4
	cpse r24,__zero_reg__
	rjmp .L16
	ldi r24,lo8(21)
	ldi r25,0
	rjmp .L15
.L19:
	std Y+2,__zero_reg__
	std Y+1,__zero_reg__
.L17:
	ldd r18,Y+1
	ldd r19,Y+2
	cpi r18,-72
	sbci r19,11
	brlo .L18
.L15:
	sbiw r24,1
	brne .L19
	movw r30,r16
	ldd r22,Z+3
	add r22,r12
	movw r20,r28
	subi r20,-4
	sbci r21,-1
	ldd r24,Z+2
	call GPIO_GetPinValue
	or r24,r25
	brne .L29
	ldd r24,Y+4
	cpse r24,__zero_reg__
	rjmp .L16
	ldi r20,lo8(4)
	mul r13,r20
	movw r30,r0
	clr __zero_reg__
	add r30,r16
	adc r31,r17
	add r30,r12
	adc r31,__zero_reg__
	ldd r24,Z+4
	movw r30,r14
	st Z,r24
.L23:
	std Y+3,__zero_reg__
	movw r30,r16
	ldd r22,Z+3
	add r22,r12
	movw r20,r28
	subi r20,-3
	sbci r21,-1
	ldd r24,Z+2
	call GPIO_GetPinValue
	movw r10,r24
	or r24,r25
	breq .L20
.L29:
	clr r10
	inc r10
	mov r11,__zero_reg__
	rjmp .L10
.L18:
	ldd r18,Y+1
	ldd r19,Y+2
	subi r18,-1
	sbci r19,-1
	std Y+2,r19
	std Y+1,r18
	rjmp .L17
.L20:
	ldd r24,Y+3
	tst r24
	breq .L23
.L10:
	movw r24,r10
/* epilogue start */
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret
.L16:
	inc r12
	ldi r31,lo8(4)
	cpse r12,r31
	rjmp .L24
	inc r13
	ldi r20,lo8(4)
	cpse r13,r20
	rjmp .L12
	rjmp .L10
	.size	Keypad_GetKey, .-Keypad_GetKey
.global	Keypad_WaitForKey
	.type	Keypad_WaitForKey, @function
Keypad_WaitForKey:
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 4 */
.L__stack_usage = 4
	sbiw r24,0
	breq .L38
	movw r28,r22
	movw r16,r24
	sbiw r28,0
	breq .L38
.L39:
	movw r22,r28
	movw r24,r16
	call Keypad_GetKey
	sbiw r24,1
	breq .L38
	ld r24,Y
	cpi r24,lo8(-1)
	breq .L39
	ldi r25,0
	ldi r24,0
.L37:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	pop r16
	ret
.L38:
	ldi r24,lo8(1)
	ldi r25,0
	rjmp .L37
	.size	Keypad_WaitForKey, .-Keypad_WaitForKey
	.ident	"GCC: (GNU) 7.3.0"
