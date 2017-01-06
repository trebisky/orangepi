/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */
/* locore.S
 *
 * Kyu project  5-12-2015  Tom Trebisky
 *
 * temporary catch-all for assembly language
 *
 * Amazingly enough, ARM by default is little-endian.
 * It can switch, but watch out if you do!
 */

#define PSR_M_SUPER	0x13
#define PSR_M_IRQ	0x12
#define PSR_M_FIQ	0x11

#define PSR_IRQ_DIS	0x80
#define PSR_FIQ_DIS	0x40
#define PSR_INT_DIS	0xc0

#define PSR_DEFAULT	PSR_M_SUPER

/* For the time being, this is the first thing linked
 * and U-boot simply branches to a hard address 80300000.
 * So it had better find this code there !!
 * For some reason the compiler was reordering routines
 * compiled from main.c, this makes us immune to that.
 */
	.globl kyu_startup
kyu_startup:
	mrs	r0, cpsr		/* disable interrupts */
	orr	r0, r0, #PSR_INT_DIS
	msr	cpsr, r0

	movw	r0, #:lower16:vectors	/* set VBAR */
	movt	r0, #:upper16:vectors
	mcr 	p15, 0, r0, c12, c0, 0

	bl	clear_bss
/*	bl	kern_startup	*/
@	bl	kern_startup
	bl	main
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

xdelay:
	mov	r0, #0xFFFFFF
dloop:	sub	r0, #1
	cmp	r0, #0
	bgt	dloop
	bx	lr

/* spit out endless characters -- sanity check */
	.globl sanity
sanity:
	movw	r0, #'b'
/*	bl	serial_putc	*/
	bl	xdelay
	b	sanity

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
 * power of 2.  So on the ARM .align 3 is like .align 8 on the x86.
 * And so on the ARM .align 5 is aligning on 32 byte boundaries.
 *
 * I am probably being overly fussy about alignment,
 *  but maybe I will thank myself if I move to 64 bit ARM processors.
 */

	.align  5
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

#define S_R0		0
#define S_R1		4
#define S_R2		8
#define S_R3		12
#define S_R4		16
#define S_R5		20
#define S_R6		24
#define S_R7		28
#define S_R8		32
#define S_R9		36
#define S_R10		40
#define S_R11		44
#define S_R12		48
#define S_SP		52	/* r13 */
#define S_LR		56	/* r14 */
#define S_PC		60	/* r15 */

#define S_PSR		64

#define FRAME_SIZE	68

	.globl	cur_thread

/* When an interrupt happens we have our own private r13 and r14 (sp and lr)
 * r14 has the address to return to (plus some adjustment),
 *  but sp is entirely free to be used.
 *
 * So, we start the prolog with an adjusted PC in the lr register.
 * we simply use sp as a scratch register (and lr as well later on).
 *
 * Using the "^" to get the "non-interrupt" registers would be a mistake.
 *  this gets the user mode registers, which we have no interest in.
 */
	.macro kyu_prolog
	movw	sp, #:lower16:cur_thread
	movt	sp, #:upper16:cur_thread
	ldr	sp, [sp, #0]

	stmia	sp, {r0-r12}

	mrs	ip, spsr	/* save before forcing to super */
	str	ip, [sp,#S_PSR]	/* saved SR */

	add	r0, sp, #0	/* our cur_thread */
	add	r1, lr, #0	/* shuffle this (becomes PC on resume) */

	mrs     ip, cpsr	/* get current mode */
        bic     ip, ip, #0x1f
        orr     ip, ip, #PSR_M_SUPER
        msr     cpsr, ip	/* force it to super */

	str	sp, [r0,#S_SP]	/* save this */
	str	lr, [r0,#S_LR]	/* this too */

	str	r1, [r0,#S_PC]	/* PC when we resume */

	.endm

	/* The way things are currently coded, we never use the epilog.
	 * We always return from interrupts using resume_i()
	 *  or some other specific resume method.
	 */
	.macro kyu_epilog
	b	spin
	.endm

	/* We get a pointer to a thread in r0
	 * XXX XXX - Note that turning on both IRQ and FIQ is wrong
	 *  and will yields bugs when we distinguish FIQ.
	 */
	.globl resume_i
resume_i:
	/* keep r0 till bitter end */
	ldr	ip, [r0, #S_PSR]
	msr	spsr, ip

	ldmia	r0, {r0-pc}^

OLDresume_i:
	/* keep r0 till bitter end */
	ldr	r1, [r0, #S_PSR]
	msr	spsr, r1
	ldr	r1, [r0, #S_SP]
	ldr	r2, [r0, #S_LR]

	mrs     ip, cpsr	/* save mode */
        bic     r3, ip, #0x1f
        orr     r3, r3, #PSR_M_SUPER	/* enable interrupts */
        msr     cpsr, r3	/* switch back to super */

	add	sp, r1, #0	/* setup SP and LR */
	add	lr, r2, #0
	msr	cpsr, ip

	ldr	lr, [r0, #S_PC]
	ldmia	r0, {r0-r12}
	movs	pc, lr

/* This routine launches a thread from scratch.
 *  on the ARM it gets a pointer to an array of 4 items:
 *	an argument to pass
 *	a place to start up at.
 *	an initial stack pointer.
 *	a place to return to (usually thr_exit)
 * XXX danger if this is used from interrupt as we
 *  are not doing sp and lr properly
 */
	.globl resume_c
resume_c:
	ldmia	r0,{r0,ip,sp,lr}
	mrs     r1, cpsr
	bic	r1, r1, #0xff
	orr	r1, r1, #PSR_M_SUPER	/* enable both IRQ and FIQ */
	msr	cpsr, r1
	mov	fp, #0		/* added 8-13-2016 to end backtraces */
	mov	pc, ip

/* resume_j and save_j don't need to save all registers
 * since these are called "synchronously", the compiler
 * conventions should apply, and r0-r3 should not need
 * to be saved.  Also ip and maybe others.
 * But it is simple to save them all .....
 * 
 * *** resume_j must be capable of being called from
 *      interrupt level.
 */

VERYOLDresume_j:
	ldmia	r0, {r0-pc}

	/* This is always called from supervisor level,
	 * so we don't need to fuss with sp and lr.
	 */
	.globl save_j
save_j:
	stmia	r0, {r0-lr}
	mrs     ip, cpsr
	str	ip, [r0, #S_PSR]
	mov	ip, #1		/* resume will return 1 */
	str	ip, [r0]	/* coincidence this points to r0 */
	str	lr, [r0,#S_PC]	/* resume at this pc */
	mov	r0, #0		/* we return 0 */
	bx	lr

	.globl resume_j
resume_j:
	/* keep r0 till bitter end */
	ldr	ip, [r0, #S_PSR]
	msr	spsr, ip

	ldmia	r0, {r0-pc}^

OLDresume_j:
	/* keep r0 till the bitter end */
	mrs     r1, cpsr
	bic	r1, r1, #0xff
	orr	r1, r1, #PSR_M_SUPER	/* enable interrupts */
	msr	spsr, r1

	ldr	r1, [r0, #S_SP]
	ldr	r2, [r0, #S_LR]

	mrs     ip, cpsr	/* save mode */
        bic     r3, ip, #0x1f
        orr     r3, r3, #PSR_M_SUPER	
        msr     cpsr, r3	/* switch back to super */

	add	sp, r1, #0	/* setup SP and LR */
	add	lr, r2, #0
	msr	cpsr, ip

	ldr	lr, [r0, #S_PC]
	ldmia	r0, {r0-r12}
	movs	pc, lr

/*
 * exception handlers
 */
	.align  5
undefined_instruction:
	kyu_prolog
	bl	do_undefined_instruction
	kyu_epilog

	.align	5
software_interrupt:
	kyu_prolog
	bl	do_software_interrupt
	kyu_epilog

	.align	5
prefetch_abort:
	sub	lr, lr, #4
	kyu_prolog
	bl	do_prefetch_abort
	kyu_epilog

	.align	5
data_abort:
	sub	lr, lr, #8
	kyu_prolog
	bl	do_data_abort
	kyu_epilog

	.align	5
not_used:
	kyu_prolog
	bl	do_not_used
	kyu_epilog

	.align	5
irq:
	sub	lr, lr, #4
	kyu_prolog
	bl	do_irq
	kyu_epilog

	.align	5
fiq:
	sub	lr, lr, #4
	kyu_prolog
	bl	do_fiq
	kyu_epilog

/* ----------------------------------------- */
/* ----------------------------------------- */

        .globl get_sp
get_sp:
        add     r0, sp, #0
	bx	lr

        .globl get_fp
get_fp:
        add     r0, fp, #0
	bx	lr

/* Use only in dire situations */
        .globl set_sp
set_sp:
        add     sp, r0, #0
	bx	lr

        .globl get_regs
get_regs:
	stmia	r0, {r0-pc}
	mrs     ip, cpsr
	str	ip, [r0,#S_PSR]
	bx	lr

/* For an experiment from interrupt code */
        .globl get_ssp
get_ssp:
	mrs	ip, cpsr
	bic	r0, ip, #0x1f
	orr	r0, r0, #PSR_M_SUPER
	msr	cpsr, r0

	add	r0, sp, #0

	msr	cpsr, ip
	bx	lr

	.globl spin
spin:
	b	spin

        .globl get_pc
get_pc:
        add     r0, pc, #0
	bx	lr

        .globl get_cpsr
get_cpsr:
	mrs     r0, cpsr
	bx	lr

/* Read VBAR Register */

	.globl get_vbar
get_vbar:
	mrc 	p15, 0, r0, c12, c0, 0
	bx	lr

	.globl set_vbar
set_vbar:
	mcr 	p15, 0, r0, c12, c0, 0
	bx	lr

/* SCTRL - System Control*/
	.globl get_sctrl
get_sctrl:
	mrc     p15, 0, r0, c1, c0, 0
	bx	lr

	.globl set_sctrl
set_sctrl:
	mcr 	p15, 0, r0, c1, c0, 0
	bx	lr

/* ASCTRL - Auxiliary System Control*/
	.globl get_asctrl
get_asctrl:
	mrc     p15, 0, r0, c1, c0, 1
	bx	lr

	.globl set_asctrl
set_asctrl:
	mcr 	p15, 0, r0, c1, c0, 1
	bx	lr

/* MMU table base */
	.globl get_mmu
get_mmu:
	mrc     p15, 0, r0, c2, c0, 0
	bx	lr

	.globl set_mmu
set_mmu:
	mcr 	p15, 0, r0, c2, c0, 0
	bx	lr

/* Protection Unit base */
	.globl get_prot
get_prot:
	mrc     p15, 0, r0, c6, c0, 0
	bx	lr

	.globl set_prot
set_prot:
	mcr 	p15, 0, r0, c6, c0, 0
	bx	lr


/* The cycle count register is a 32 bit register
 * This is for a Cortex A8 arm chip.
 */
	.globl get_ccnt
get_ccnt:
	mrc     p15, 0, r0, c9, c13, 0
	bx	lr

	.globl set_ccnt
set_ccnt:
	mcr     p15, 0, r0, c9, c13, 0
	bx	lr

/* The performance monitor control register controls
 * the behavior of the CCNT register.
 */
	.globl get_pmcr
get_pmcr:
	mrc     p15, 0, r0, c9, c12, 0
	bx	lr

	.globl set_pmcr
set_pmcr:
	mcr     p15, 0, r0, c9, c12, 0
	bx	lr

/* Counter enable register.
 */
	.globl get_cena
get_cena:
	mrc     p15, 0, r0, c9, c12, 1
	bx	lr

	.globl set_cena
set_cena:
	mcr     p15, 0, r0, c9, c12, 1
	bx	lr

/* Counter disable register.
 */
	.globl get_cdis
get_cdis:
	mrc     p15, 0, r0, c9, c12, 2
	bx	lr

	.globl set_cdis
set_cdis:
	mcr     p15, 0, r0, c9, c12, 2
	bx	lr

/* Counter overflow register.
 */
	.globl get_covr
get_covr:
	mrc     p15, 0, r0, c9, c12, 3
	bx	lr

	.globl set_covr
set_covr:
	mcr     p15, 0, r0, c9, c12, 3
	bx	lr

/* enable_irq (void); */
	.globl enable_irq
enable_irq:
	mrs	r0, cpsr
	bic	r0, r0, #0x80
	msr	cpsr, r0
	bx	lr

/* enable_fiq (void); */
	.globl enable_fiq
enable_fiq:
	mrs	r0, cpsr
	bic	r0, r0, #0x40
	msr	cpsr, r0
	bx	lr

/* enter critical region
 * i.e. disable both irq and fiq
 */
	.globl cpu_enter
cpu_enter:
	mrs	r0, cpsr
	orr	r0, r0, #0xc0
	msr	cpsr, r0
	bx	lr

/* leave critical region
 * i.e. enable both irq and fiq
 * XXX - do we really want to enable both?
 */
	.globl cpu_leave
cpu_leave:
	mrs	r0, cpsr
	bic	r0, r0, #0xc0
	msr	cpsr, r0
	bx	lr

/* THE END */
