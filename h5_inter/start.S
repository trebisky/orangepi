/* Extremely simple (and virtually unnecessary)
 * assembly language startup file.
 */

	.global start
start:
	msr		DAIFSet, #7		// disable interrupts

	// if we don't remap vectors, U-Boot gets our exceptions.
	bl		remap_vectors
	bl		main
	bl		spin

spin:	b		spin

remap_vectors:
        adr     x3, vectors_el2
        msr     VBAR_EL2, x3
        ret

// ----------------------------------------------------
// Stuff after this added to support interrupt experiment

// Comes here with vector offset in x1 that needs
// to end up in x0 as argument to fault_handler
new_fault:

        // stp  x0, x1, [sp,-16]!       // push for now
        ldr     x0, =cur_thread
        ldr     x0, [x0]

        stp     x2, x3, [x0, 16]
        stp     x4, x5, [x0, 32]
        stp     x6, x7, [x0, 48]
        stp     x8, x9, [x0, 64]
        stp     x10, x11, [x0, 80]
        stp     x12, x13, [x0, 96]
        stp     x14, x15, [x0, 112]
        stp     x16, x17, [x0, 128]
        stp     x18, x19, [x0, 144]
        stp     x20, x21, [x0, 160]
        stp     x22, x23, [x0, 176]
        stp     x24, x25, [x0, 192]
        stp     x26, x27, [x0, 208]
        stp     x28, x29, [x0, 224]

        mov     x2, sp
        stp     x30, x2, [x0, 240]

        mrs     x2, elr_el2
        mrs     x3, spsr_el2
        stp     x2, x3, [x0, 256]

        ldp     x2, x3, [sp], 16        // pop x0, x1
        stp     x2, x3, [x0]            // save x0, x1

        mov     x0, x1
        b       fault_handler

// identical to the above, but calls the IRQ handler
// This is what we actually use.
new_irq:

        stp     x0, x1, [sp,-16]!       // push for now
        ldr     x0, =cur_thread
        ldr     x0, [x0]
        stp     x2, x3, [x0, 16]
        stp     x4, x5, [x0, 32]
        stp     x6, x7, [x0, 48]
        stp     x8, x9, [x0, 64]
        stp     x10, x11, [x0, 80]
        stp     x12, x13, [x0, 96]
        stp     x14, x15, [x0, 112]
        stp     x16, x17, [x0, 128]
        stp     x18, x19, [x0, 144]
        stp     x20, x21, [x0, 160]
        stp     x22, x23, [x0, 176]
        stp     x24, x25, [x0, 192]
        stp     x26, x27, [x0, 208]
        stp     x28, x29, [x0, 224]

        // mov  x1, sp
        add     x1, sp, #16
        stp     x30, x1, [x0, 240]

        mrs     x1, elr_el2
        mrs     x2, spsr_el2
        stp     x1, x2, [x0, 256]

        ldp     x1, x2, [sp], 16        // pop x0, x1
        stp     x1, x2, [x0]            // save x0, x1

        // b       do_irq
        b       irq_handler

// ----------------------------------------------------

.macro panic
        .align  7
1:      stp     x0, x1, [sp,-16]!       // push for now
        mov     x1, 1b - vectors_el2
        b       new_fault
        .word   0xdeadbeef
.endm


/* Vector table entries are 128 bytes in size, i.e. 32 instructions.
 *  (armv7 had 4 bytes per entry (1 instruction)).
 * There are 16 entries in the table.
 * This is 4 types of exceptions from 4 sources.
 * The 4 sources are:
 *  Same exception level when source SP = SP_EL0
 *  Same exception level when source SP = SP_ELx
 *  Lower exception level running on AArch64
 *  Lower exception level running on AArch32
 */

.globl vectors

        .text
        .align 11
vectors:
vectors_el2:
        panic           /* 0x000 Current EL w/ SP0 - Synchronous Thread */
        panic           /* 0x080 Current EL w/ SP0 - IRQ Thread */
        panic           /* 0x100 Current EL w/ SP0 - FIQ Thread */
        panic           /* 0x180 Current EL w/ SP0 - Error Thread */

        panic           /* 0x200 Current EL w/ SPx - Synchronous Handler */
        .align  7       /* 0x280 Current EL w/ SPx - IRQ Handler */
        b       new_irq
        // b    .irq
        panic           /* 0x300 Current EL w/ SPx - FIQ Handler */
        panic           /* 0x380 Current EL w/ SPx - Error Handler */

        panic           /* 0x400 Lower EL 64 bit - Synch */
        panic           /* 0x480 Lower EL 64 bit - IRQ */
        panic           /* 0x500 Lower EL 64 bit - FIQ */
        panic           /* 0x580 Lower EL 64 bit - Error */

        panic           /* 0x600 Lower EL 32 bit - Synch */
        panic           /* 0x680 Lower EL 32 bit - IRQ */
        panic           /* 0x700 Lower EL 32 bit - FIQ */
        panic           /* 0x780 Lower EL 32 bit - Error */

// THE END
