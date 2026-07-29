	.file	"GPIO.c"
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
	subi r30,lo8(-(GPIO_DDPx))
	sbci r31,hi8(-(GPIO_DDPx))
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
	subi r30,lo8(-(GPIO_DDPx))
	sbci r31,hi8(-(GPIO_DDPx))
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
	brsh .L15
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_DDPx))
	sbci r31,hi8(-(GPIO_DDPx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	tst r22
	breq .L13
	cpi r22,lo8(1)
	breq .L14
	st Z,r22
	rjmp .L17
.L13:
	st Z,__zero_reg__
.L17:
	ldi r25,0
	ldi r24,0
	ret
.L14:
	ldi r24,lo8(-1)
	st Z,r24
	rjmp .L17
.L15:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	GPIO_SetPortDirection, .-GPIO_SetPortDirection
.global	GPIO_GetPinValue
	.type	GPIO_GetPinValue, @function
GPIO_GetPinValue:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L22
	cpi r22,lo8(8)
	brsh .L22
	cp r20,__zero_reg__
	cpc r21,__zero_reg__
	breq .L22
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PINx))
	sbci r31,hi8(-(GPIO_PINx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	ld r24,Z
	ldi r25,0
	rjmp 2f
	1:
	asr r25
	ror r24
	2:
	dec r22
	brpl 1b
	andi r24,lo8(1)
	movw r30,r20
	st Z,r24
	ldi r25,0
	ldi r24,0
	ret
.L22:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	GPIO_GetPinValue, .-GPIO_GetPinValue
.global	GPIO_GetPortStatus
	.type	GPIO_GetPortStatus, @function
GPIO_GetPortStatus:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L26
	cp r22,__zero_reg__
	cpc r23,__zero_reg__
	breq .L26
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PINx))
	sbci r31,hi8(-(GPIO_PINx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	ld r24,Z
	movw r30,r22
	st Z,r24
	ldi r25,0
	ldi r24,0
	ret
.L26:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	GPIO_GetPortStatus, .-GPIO_GetPortStatus
.global	GPIO_PinToggle
	.type	GPIO_PinToggle, @function
GPIO_PinToggle:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L30
	cpi r22,lo8(8)
	brsh .L30
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PORTx))
	sbci r31,hi8(-(GPIO_PORTx))
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
	eor r24,r18
	st Z,r24
	ldi r25,0
	ldi r24,0
	ret
.L30:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	GPIO_PinToggle, .-GPIO_PinToggle
.global	GPIO_PinSetValue
	.type	GPIO_PinSetValue, @function
GPIO_PinSetValue:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L38
	cpi r22,lo8(8)
	brsh .L38
	tst r20
	breq .L33
	cpi r20,lo8(1)
	breq .L34
.L38:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
.L33:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PORTx))
	sbci r31,hi8(-(GPIO_PORTx))
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
.L39:
	st Z,r24
	ldi r25,0
	ldi r24,0
	ret
.L34:
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PORTx))
	sbci r31,hi8(-(GPIO_PORTx))
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
	rjmp .L39
	.size	GPIO_PinSetValue, .-GPIO_PinSetValue
.global	GPIO_PortSetValue
	.type	GPIO_PortSetValue, @function
GPIO_PortSetValue:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cpi r24,lo8(4)
	brsh .L42
	mov r30,r24
	ldi r31,0
	lsl r30
	rol r31
	subi r30,lo8(-(GPIO_PORTx))
	sbci r31,hi8(-(GPIO_PORTx))
	ld __tmp_reg__,Z+
	ld r31,Z
	mov r30,__tmp_reg__
	st Z,r22
	ldi r25,0
	ldi r24,0
	ret
.L42:
	ldi r24,lo8(1)
	ldi r25,0
/* epilogue start */
	ret
	.size	GPIO_PortSetValue, .-GPIO_PortSetValue
	.section	.rodata
	.type	GPIO_PORTx, @object
	.size	GPIO_PORTx, 8
GPIO_PORTx:
	.word	59
	.word	56
	.word	53
	.word	50
	.type	GPIO_PINx, @object
	.size	GPIO_PINx, 8
GPIO_PINx:
	.word	57
	.word	54
	.word	51
	.word	48
	.type	GPIO_DDPx, @object
	.size	GPIO_DDPx, 8
GPIO_DDPx:
	.word	58
	.word	55
	.word	52
	.word	49
	.ident	"GCC: (GNU) 7.3.0"
.global __do_copy_data
