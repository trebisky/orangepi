/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */

#define PSR_INT_DIS     0xc0

/* On the Orange Pi this loads to 0x40000000 (these days anyway)
 */
	.globl _startup
_startup:
	mrs     r0, cpsr                /* disable interrupts */
        orr     r0, r0, #PSR_INT_DIS
        msr     cpsr, r0

	@movw	sp, #0
	@movt	sp, #0x4fff
	ldr     sp, =0x50000000
	bl	main

spin:
	b	spin


clear_bss:
        movw    r3, #:lower16:__bss_start__
        movt    r3, #:upper16:__bss_start__
        movw    r2, #:lower16:__bss_end__
        movt    r2, #:upper16:__bss_end__
        mov     r1, #0
loop:   cmp     r3, r2
        strcc   r1, [r3]
        addcc   r3, r3, #4
        bcc     loop
        bx      lr

#define SENTINEL        0x4

        .globl newcore
newcore:
        /* Read processor affinity register */
        mrc     p15, 0, r0, c0, c0, 5
        and     r0, #3
        lsl     r0, #14
        ldr     r2, =core_stacks
        ldr     r2, [r2]
        add     sp, r2, r0

        /* pass core number in r0 as argument */
        mrc     p15, 0, r0, c0, c0, 5
        and     r0, #3

        bl      kyu_newcore

@ If we return, we will spin here rapidly
@ incrementing the sentinel location.
@ note that there is special static ram at addr 0

        mov     r0, #0
        ldr     r1, =SENTINEL
loopcore:
        add     r0, #1
        str     r0, [r1]
        b      loopcore

	/* Avoid stupid linker warning */
        .section        .note.GNU-stack,"",%progbits

/* THE END */
