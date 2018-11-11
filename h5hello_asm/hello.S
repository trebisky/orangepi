/* Startup file take from Fire3 project */

	.global start
start:
	msr		DAIFSet, #7		// disable interrupts

	ldr		x4, =str_hello
	bl		puts
	bl		spin

// -----------------------------------

str_hello:
	.asciz		"Hello World\n\r"
str_rat:
	.asciz		" Rat\n\r"

	.align		4

// -----------------------------------

#define	UART0_BASE	0x01c28000
#define	UART0_DATA	0x01c28000
#define	UART0_LSR	0x01c28014

wr_uart:
	ldr		x0, =UART0_LSR
	ldr		w1, [x0]
	and		w1, w1, 0x40
	cmp		w1, #0
	beq		wr_uart

	ldr		x0, =UART0_DATA
	str		x2, [x0]
	ret

puts:
	mov		x19, x30
1:
	ldrb		w2,[x4]
	and		x2, x2, #0xff
	cmp		w2, #0
	beq		2f
	bl		wr_uart
	add		x4, x4, #1
	b		1b
2:
	mov		x30, x19
	ret

rat:
	mov		x20, x30
	bl		wr_uart
	ldr		x4, =str_rat
	bl		puts
	mov		x30, x20
	ret

spin:
	b		spin

flood_uart:
	mov		x2, #0x37		
	bl		wr_uart
	mov		x2, #0x38		
	bl		wr_uart
	b		flood_uart

// -----------------------------------
// I never did get the PWR led to come on.

#define	GPIO_L_BASE	0x01f02c00
#define	GPIO_L_CONFIG0	0x01f02c00
#define	GPIO_L_CONFIG1	0x01f02c04
#define	GPIO_L_DATA	0x01f02c10

#define	CCU_GATE_X	0x01c20068

pwr_led_on:
	// power on the PIO's
	ldr		x0, =CCU_GATE_X
	ldr		w1, [x0]
	orr		w1, w1, #0x20
	str		w1, [x0]

	// make then drive outputs
	ldr		w1, =0x11111111
	ldr		x0, =GPIO_L_CONFIG0
	ldr		w1, [x0]
	ldr		x0, =GPIO_L_CONFIG1
	ldr		w1, [x0]

	// drive high
	ldr		w1, =0xffffffff
	ldr		x0, =GPIO_L_DATA
	ldr		w1, [x0]

	ret

other_pwr_led_on:
	ldr		x0, =GPIO_L_CONFIG1
	ldr		w1, [x0]
	bic		w1, w1, #0xf00
	orr		w1, w1, #0x100
	str		w1, [x0]

	ldr		x0, =GPIO_L_DATA
	ldr		w1, [x0]
	orr		w1, w1, #0x400
	str		w1, [x0]

	ret

// THE END
