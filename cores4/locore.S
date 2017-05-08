/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * locore.S
 *
 * Kyu project  5-12-2015  Tom Trebisky
 *
 * temporary catch-all for assembly language
 *
 * Amazingly enough, ARM by default is little-endian.
 * It can switch, but watch out if you do!
 */

@#include "board.h"
#define BOARD_ORANGE_PI
#define BOARD_ORANGE_PI_PC
#define ARCH_ARM

@ Apparently the at-sign is the ARM comment character.
@ on the x86 you use the pound sign.

#define PSR_M_SUPER	0x13
#define PSR_M_IRQ	0x12
#define PSR_M_FIQ	0x11

#define PSR_IRQ_DIS	0x80
#define PSR_FIQ_DIS	0x40
#define PSR_INT_DIS	0xc0

#define PSR_DEFAULT	PSR_M_SUPER

/* For the time being, this is the first thing linked
 * and U-boot simply branches to a hard address.
 *  0x80000000 on the BBB
 *  0x40000000 on the Orange Pi
 *
 * As long as locore.o is the first thing linked,
 * we should be OK.
 *
 * For some reason the compiler was reordering routines
 * compiled from main.c, this makes us immune to that.
 */
	.globl asm_startup
asm_startup:

	mrs	r0, cpsr		/* disable interrupts */
	orr	r0, r0, #PSR_INT_DIS
	msr	cpsr, r0

	movw	r0, #:lower16:vectors	/* set VBAR */
	movt	r0, #:upper16:vectors
	mcr 	p15, 0, r0, c12, c0, 0

	bl	clear_bss
@	bl	kern_startup
	bl	test_core
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

/* on the BBB, the reset vector will never come here, it will start the
 * firmware ROM code running, which will transfer to the SPL, which will
 * transfer to U-boot, which will finally transfer to us.
 *
 * Someday we could eliminate U-boot, although I don't know why,
 * and just let the SPL load us directly.
 * At any rate, the reset vector here won't get any use on the BBB.
 */

/* Note on ".align".  On some processors (like the x86, not the arm)
 * this is the multiple of bytes to align on.  On the ARM it is the
 * power of 2.  So on the ARM:
 *  .align 3 is like .align 8 on the x86.
 *  .align 5 is aligning on 32 byte boundaries.
 *  .align 7 is aligning on 128 byte boundaries.
 *
 * The manuals say the vector table needs to be 128 aligned.
 */

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
	bl trap_ui
	b  vspin
software_interrupt:
	bl trap_si
	b  vspin
prefetch_abort:
	bl trap_pa
	b  vspin
data_abort:
	bl trap_da
	b  vspin
not_used:
	bl trap_nu
	b  vspin
irq:
	bl trap_irq
	b  vspin
fiq:
	bl trap_fiq
	b  vspin

vspin:	b	vspin

/* This padding here makes the cores start */
/* This allowed us to rip out a big block of Kyu
 * specific interrupt code, along with a bunch of
 * stub calls.
 * This is total black magic, but the overall goal is
 * to have the address of "newcore" at 0x400004e4
 */
	.skip 1056

	.align 2


/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */

/* Experimental section for Orange Pi multiple cores */

#ifdef BOARD_ORANGE_PI

#define SENTINEL	0x4

/* We realize that each core needs its own stack ...
 *  1-28-2017
 * Start each cpu with a 16K stack (very generous).
 */
	.globl newcore
newcore:

	mrs	r0, cpsr		/* disable interrupts */
	orr	r0, r0, #PSR_INT_DIS
	msr	cpsr, r0

	movw	r0, #:lower16:vectors	/* set VBAR */
	movt	r0, #:upper16:vectors
	mcr 	p15, 0, r0, c12, c0, 0

#ifdef notdef
	/*
         * Invalidate the L1 I and D cache
         */
        mov     r0, #0                  @ set up for MCR
        mcr     p15, 0, r0, c8, c7, 0   @ invalidate TLBs
        mcr     p15, 0, r0, c7, c5, 0   @ invalidate icache
        mcr     p15, 0, r0, c7, c5, 6   @ invalidate BP array
        mcr     p15, 0, r0, c7, c10, 4  @ DSB
        mcr     p15, 0, r0, c7, c5, 4   @ ISB

        /*
         * disable MMU stuff and caches
         */
        mrc     p15, 0, r0, c1, c0, 0
        bic     r0, r0, #0x00002000     @ clear bits 13 (--V-)
        bic     r0, r0, #0x00000007     @ clear bits 2:0 (-CAM)
        orr     r0, r0, #0x00000002     @ set bit 1 (--A-) Align
        orr     r0, r0, #0x00000800     @ set bit 11 (Z---) BTB

	@ enable the I cache
        @bic     r0, r0, #0x00001000     @ clear bit 12 (I) I-cache
        orr     r0, r0, #0x00001000     @ set bit 12 (I) I-cache

        mcr     p15, 0, r0, c1, c0, 0
#endif


	/* Read processor affinity register */
        mrc     p15, 0, r0, c0, c0, 5
	and	r0, #3
	lsl	r0, #14
	@ldr	r2, =core_stacks
	@ldr	r2, [r2]
	ldr	r2, =0x50000000
	add	sp, r2, r0

	/* pass core number in r0 as argument */
        mrc     p15, 0, r0, c0, c0, 5
	and	r0, #3

	bl	kyu_newcore

@ If we return, we will spin here rapidly
@ incrementing the sentinel location.
@ note that there is special static ram at addr 0

	mov	r0, #0
	ldr	r1, =SENTINEL
loopcore:
	add	r0, #1
        str     r0, [r1]
        b      loopcore

#endif /* BOARD_ORANGE_PI */

/* THE END */
