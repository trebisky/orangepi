/*
 * Copyright (C) 2023  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * core_start.S
 *
 * Assembly code related to starting a second core
 */

@ Apparently the at-sign is the ARM comment character.
@ on the x86 you use the pound sign.

#ifdef notdef
#define PSR_M_SUPER     0x13
#define PSR_M_IRQ       0x12
#define PSR_M_FIQ       0x11

#define PSR_IRQ_DIS     0x80
#define PSR_FIQ_DIS     0x40

#define PSR_DEFAULT     PSR_M_SUPER
#endif

#define PSR_INT_DIS     0xc0

	.globl core_startup
core_startup:

	mrs	r0, cpsr		/* disable interrupts */
	orr	r0, r0, #PSR_INT_DIS
	msr	cpsr, r0

	/* VBAR */
	/* Same vectors as core 0 */
	ldr r0, =vectors
	mcr 	p15, 0, r0, c12, c0, 0

	// Set up our stack
	ldr sp, =0x68000000

	bl	clear_bss
	bl	core_main

xspin:	b	xspin
	bx	lr

clear_bss:
	movw	r3, #:lower16:__bss_start__
	movt	r3, #:upper16:__bss_start__
	movw	r2, #:lower16:__bss_end__
	movt	r2, #:upper16:__bss_end__
	mov	r1, #0
loop:	cmp	r3, r2
	strcc	r1, [r3]
	addcc	r3, r3, #4
	bcc	loop
	bx	lr


#ifdef notdef
@	.align  5
	.align  7
.globl vectors
vectors:
	b	reset
	ldr	pc, _undef
	ldr	pc, _swi
	ldr	pc, _iabt
	ldr	pc, _dabt
	ldr	pc, _unused
	ldr	pc, _irq
	ldr	pc, _fiq


	/* This branch table lets the handlers be anywhere */
	.globl	_undef
	.globl	_swi
	.globl	_iabt
	.globl	_dabt
	.globl	_unused
	.globl	_irq
	.globl	_fiq

_undef:		.word undefined_instruction
_swi:		.word software_interrupt
_iabt:		.word prefetch_abort
_dabt:		.word data_abort
_unused:	.word not_used
_irq:		.word irq
_fiq:		.word fiq

/* XXX - spin and hang */
reset:
        b      reset

	.align  5
undefined_instruction:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_ui
	b  vspin
software_interrupt:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_si
	b  vspin
prefetch_abort:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_pa
	b  vspin
data_abort:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_da
	b  vspin
not_used:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_nu
	b  vspin
fiq:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_fiq
	b  vspin

#ifdef notdef
irq:
	ldr	r2, =0x52000000
	add	sp, r2, #0
	bl trap_irq
	b  vspin
#endif

irq:
        sub     lr, lr, #4
        push    {r0-r12, lr}
        bl      gic_handler
        ldm     sp!,{r0-r12,pc}^

vspin:	b	vspin

@ Enable the VFP vector floating point unit

        .globl fp_enable
fp_enable:
        mrc     p15, 0, r0, c1, c0, 2
        orr     r0, r0, #0x300000 @ single precision
        orr     r0, r0, #0xC00000 @ double precision
        mcr     p15, 0, r0, c1, c0, 2
        isb
        mov     r0, #0x40000000
        fmxr    fpexc,r0
        mov     pc, lr
#endif

	@ avoid stupid linker warning
        .section        .note.GNU-stack,"",%progbits

/* THE END */
