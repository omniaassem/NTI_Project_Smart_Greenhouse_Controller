	.file	"seven_segment.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	SevenSeg_DisplayDigit
	.type	SevenSeg_DisplayDigit, @function
SevenSeg_DisplayDigit:
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 6 */
.L__stack_usage = 6
	movw r16,r24
	sbiw r24,0
	brne .+2
	rjmp .L10
	cpi r22,lo8(10)
	brlo .+2
	rjmp .L10
	movw r30,r24
	ldd r24,Z+4
	cpi r24,lo8(4)
	brlo .+2
	rjmp .L10
	ldd r24,Z+2
	ldd r25,Z+3
	sbiw r24,0
	brne .L3
	mov r28,r22
	ldi r29,0
	subi r28,lo8(-(SevenSeg_DigitTable))
	sbci r29,hi8(-(SevenSeg_DigitTable))
	ld r22,Y
.L3:
	movw r30,r16
	ld r18,Z
	ldd r19,Z+1
	cpi r18,1
	cpc r19,__zero_reg__
	brne .L4
	com r22
.L4:
	mov r28,r22
	ldi r29,0
	mov r15,__zero_reg__
	mov r14,__zero_reg__
	or r24,r25
	brne .L5
.L6:
	movw r20,r28
	mov r0,r14
	rjmp 2f
	1:
	asr r21
	ror r20
	2:
	dec r0
	brpl 1b
	andi r20,lo8(1)
	movw r30,r16
	ldd r22,Z+5
	add r22,r14
	ldd r24,Z+4
	call GPIO_PinSetValue
	ldi r31,-1
	sub r14,r31
	sbc r15,r31
	ldi r24,7
	cp r14,r24
	cpc r15,__zero_reg__
	brne .L6
.L7:
	ldi r25,0
	ldi r24,0
.L1:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	ret
.L5:
	movw r20,r28
	mov r0,r14
	rjmp 2f
	1:
	asr r21
	ror r20
	2:
	dec r0
	brpl 1b
	andi r20,lo8(1)
	movw r30,r16
	ldd r22,Z+5
	add r22,r14
	ldd r24,Z+4
	call GPIO_PinSetValue
	ldi r31,-1
	sub r14,r31
	sbc r15,r31
	ldi r24,4
	cp r14,r24
	cpc r15,__zero_reg__
	brne .L5
	rjmp .L7
.L10:
	ldi r24,lo8(1)
	ldi r25,0
	rjmp .L1
	.size	SevenSeg_DisplayDigit, .-SevenSeg_DisplayDigit
.global	SevenSeg_Clear
	.type	SevenSeg_Clear, @function
SevenSeg_Clear:
	push r15
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 5 */
.L__stack_usage = 5
	movw r28,r24
	or r24,r25
	breq .L20
	ldd r24,Y+4
	cpi r24,lo8(4)
	brsh .L20
	clr r15
	inc r15
	ld r24,Y
	ldd r25,Y+1
	or r24,r25
	brne .L16
	mov r15,__zero_reg__
.L16:
	ldd r24,Y+2
	ldd r25,Y+3
	ldi r16,lo8(4)
	or r24,r25
	brne .L17
	ldi r16,lo8(7)
.L17:
	ldi r17,0
.L18:
	ldd r22,Y+5
	add r22,r17
	mov r20,r15
	ldd r24,Y+4
	call GPIO_PinSetValue
	subi r17,lo8(-(1))
	cpse r17,r16
	rjmp .L18
	ldi r25,0
	ldi r24,0
.L14:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	ret
.L20:
	ldi r24,lo8(1)
	ldi r25,0
	rjmp .L14
	.size	SevenSeg_Clear, .-SevenSeg_Clear
.global	SevenSeg_Init
	.type	SevenSeg_Init, @function
SevenSeg_Init:
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 3 */
.L__stack_usage = 3
	sbiw r24,0
	breq .L29
	movw r30,r24
	ldd r18,Z+4
	cpi r18,lo8(4)
	brsh .L29
	movw r28,r24
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r17,0
	or r24,r25
	brne .L25
.L26:
	ldd r22,Y+5
	add r22,r17
	ldi r20,lo8(1)
	ldd r24,Y+4
	call GPIO_SetPinDirection
	subi r17,lo8(-(1))
	cpi r17,lo8(7)
	brne .L26
.L27:
	movw r24,r28
	call SevenSeg_Clear
	ldi r25,0
	ldi r24,0
.L23:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	ret
.L25:
	ldd r22,Y+5
	add r22,r17
	ldi r20,lo8(1)
	ldd r24,Y+4
	call GPIO_SetPinDirection
	subi r17,lo8(-(1))
	cpi r17,lo8(4)
	brne .L25
	rjmp .L27
.L29:
	ldi r24,lo8(1)
	ldi r25,0
	rjmp .L23
	.size	SevenSeg_Init, .-SevenSeg_Init
.global	SevenSeg_EnableDigit
	.type	SevenSeg_EnableDigit, @function
SevenSeg_EnableDigit:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L37
	cpi r22,lo8(8)
	brsh .L37
	ldi r25,lo8(1)
	cpi r20,1
	cpc r21,__zero_reg__
	breq .L35
	ldi r25,0
.L35:
	mov r20,r25
	call GPIO_PinSetValue
	ldi r25,0
	ldi r24,0
	ret
.L37:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	SevenSeg_EnableDigit, .-SevenSeg_EnableDigit
.global	SevenSeg_DisableDigit
	.type	SevenSeg_DisableDigit, @function
SevenSeg_DisableDigit:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L45
	cpi r22,lo8(8)
	brsh .L45
	ldi r25,lo8(1)
	cpi r20,1
	cpc r21,__zero_reg__
	brne .L43
	ldi r25,0
.L43:
	mov r20,r25
	call GPIO_PinSetValue
	ldi r25,0
	ldi r24,0
	ret
.L45:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	SevenSeg_DisableDigit, .-SevenSeg_DisableDigit
	.section	.rodata
	.type	SevenSeg_DigitTable, @object
	.size	SevenSeg_DigitTable, 10
SevenSeg_DigitTable:
	.byte	63
	.byte	6
	.byte	91
	.byte	79
	.byte	102
	.byte	109
	.byte	125
	.byte	7
	.byte	127
	.byte	111
	.ident	"GCC: (GNU) 7.3.0"
.global __do_copy_data
