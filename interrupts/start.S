/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */
	.globl _startup
_startup:
	movw	sp, #0
	movt	sp, #0x4fff
	bl	main

	.globl new_core2
new_core2:
	movw	sp, #0
	movt	sp, #0x40ff
/*	bl	bounce_core	*/

	.globl new_core
new_core:

	.globl blink_asm
blink_asm:
	movw	sp, #0
	movt	sp, #0x4fff
bloop:	bl	red_on
	bl	delay_asm
	bl	red_off
	bl	delay_asm
	b	bloop

	.globl red_on
red_on:
	movw	r0, #0x0810
	movt	r0, #0x01c2
	ldr	r3,[r0]
	orr	r3, r3, #0x00008000
	str	r3,[r0]
	bx	lr

	.globl red_off
red_off:
	movw	r0, #0x0810
	movt	r0, #0x01c2
	ldr	r3,[r0]
	bic	r3, r3, #0x00008000
	str	r3,[r0]
	bx	lr

	.globl	delay_asm
delay_asm:
	movw	r3, #0x0000
	movt	r3, #0x0400
loop:	sub	r3, r3, #1
	nop
	nop
	cmp	r3, #0
	bne	loop
	bx	lr
    
/* THE END */
