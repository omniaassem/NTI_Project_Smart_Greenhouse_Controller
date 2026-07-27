	.file	"gpio.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	GPIO_SetPinDirection
	.type	GPIO_SetPinDirection, @function
GPIO_SetPinDirection:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L8
	cpi r22,lo8(8)
	brsh .L8
	tst r20
	breq .L3
	cpi r20,lo8(1)
	breq .L4
.L8:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
.L3:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_DDRx))
	sbci r31,hi8(-(GPIO_DDRx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	ld r18,Z
	ldi r24,lo8(1)
	ldi r25,0
	rjmp 2f
	1:
	lsl r24
	2:
	dec r22
	brpl 1b
	com r24
	and r24,r18
.L9:
	st Z,r24
	ldi r25,0
	ldi r24,0
	ret
.L4:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_DDRx))
	sbci r31,hi8(-(GPIO_DDRx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	ld r24,Z
	ldi r18,lo8(1)
	ldi r19,0
	rjmp 2f
	1:
	lsl r18
	2:
	dec r22
	brpl 1b
	or r24,r18
	rjmp .L9
	.size	GPIO_SetPinDirection, .-GPIO_SetPinDirection
.global	GPIO_SetPortDirection
	.type	GPIO_SetPortDirection, @function
GPIO_SetPortDirection:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L16
	tst r22
	breq .L12
	cpi r22,lo8(1)
	breq .L13
.L16:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
.L12:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_DDRx))
	sbci r31,hi8(-(GPIO_DDRx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	st Z,__zero_reg__
.L17:
	ldi r25,0
	ldi r24,0
	ret
.L13:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_DDRx))
	sbci r31,hi8(-(GPIO_DDRx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	ldi r24,lo8(-1)
	st Z,r24
	rjmp .L17
	.size	GPIO_SetPortDirection, .-GPIO_SetPortDirection
	.section	.rodata
	.type	GPIO_DDRx, @object
	.size	GPIO_DDRx, 8
GPIO_DDRx:
	.word	58
	.word	55
	.word	52
	.word	49
	.ident	"GCC: (GNU) 7.3.0"
.global __do_copy_data
