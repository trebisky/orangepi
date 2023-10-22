/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */

#define PSR_IRQ_DIS     0x80
#define PSR_FIQ_DIS     0x40
#define PSR_INT_DIS     0xc0

	.globl _startup
_startup:
        /* disable interrupts */
        mrs     r0, cpsr
        orr     r0, r0, #PSR_INT_DIS
        msr     cpsr, r0

	/* Set up the interrupt stack */
	;@//mov r0, #(PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
	mov r0,#0xD2
	msr cpsr_c, r0
	// ldr sp, =0x4fff0000
	ldr sp, =0x58000000

	/* Set up the regular stack */
	;@//mov r0, #(PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
	mov r0,#0xD3
	msr cpsr_c, r0
	// ldr sp, =0x48ff0000
	ldr sp, =0x50000000

	/* set up a stack pointer */
	//movw	sp, #0
	//movt	sp, #0x5000

	/* set the vector base */
        // movw    r0, #:lower16:vectors
        // movt    r0, #:upper16:vectors
	ldr r0, =vectors
        mcr     p15, 0, r0, c12, c0, 0

	bl	main

        .align  5
.globl vectors
vectors:
        b       reset
        ldr     pc, _undef
        ldr     pc, _swi
        ldr     pc, _iabt
        ldr     pc, _dabt
        ldr     pc, _unused
        ldr     pc, _irq
        ldr     pc, _fiq


        /* This branch table lets the handlers be anywhere */
        .globl  _undef
        .globl  _swi
        .globl  _iabt
        .globl  _dabt
        .globl  _unused
        .globl  _irq
        .globl  _fiq

_undef:         .word bogus
_swi:           .word bogus
_iabt:          .word bogus
_dabt:          .word bogus
_unused:        .word bogus
_irq:           .word irq
_fiq:           .word bogus

bogus:
reset:
	bl	blink_asm
        b      reset

/* This won't work because we never clear
 * the interrupt, we get into a hard IRQ loop.
 */
irqX:
	subs	pc,lr,#4

#ifdef notdef
/* This "works" - we get one interrupt, then
 * disable interrupts altogether.
 */
irqONE:
	bl	red_on
        mrs     r0, spsr
        orr     r0, r0, #0x80
        msr     spsr, r0
	subs	pc,lr,#4

/* Try something in 2023
 * like "irqX" above we lock up in a hard
 * interrupt loop since we don't clear the
 * interrupt.
 */
irqY:
	push	{r0, r1}
	/* -- */
	pop	{r0, r1}
	subs	pc,lr,#4

#endif

/* This works, for a while ... */
// irq_2017:
irqZ:
	ldr sp, =0x58000000
	stmfd	sp!,{r0-r12,lr}
	dmb
	bl	gic_handler
	dmb
	ldmfd	sp!,{r0-r12,lr}
	// The following instruction is magic
	// it also switches back from I mode to
	// whatever mode preceded the interrupt.
	subs	pc,lr,#4

irq:
	sub	lr, lr, #4
	push	{r0-r12, lr}
	bl	gic_handler
	ldm	sp!,{r0-r12,pc}^

        .globl enable_irq
enable_irq:
        mrs     r0, cpsr
        bic     r0, r0, #0x80
        msr     cpsr, r0
        bx      lr

        .globl disable_irq
disable_irq:
        mrs     r0, cpsr
        orr     r0, r0, #0x80
        msr     cpsr, r0
        bx      lr

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

	/* Avoid stupid linker warning */
        .section        .note.GNU-stack,"",%progbits

/* THE END */
