/*
 * Copyright (C) 2016  Nexell Co., Ltd.
 * Author: Sangjong, Han <hans@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* From Fire3/Fire3-bl1/src/aarch64_libs.S
 *  extracted just the SwitchToEL2 routines.
 *  added my own "main"
 * Tom Trebisky  12-7-2018
 *
	.align 3
 */

#define UBOOT_START	0x4a000000

.global Startup
Startup:
	bl		SwitchToEL2
	mov		x0, UBOOT_START
	br		x0


/* Since X30 is the lr register, this sets things up so the eret
 * will resume execution at that address, effectively returning from
 * this call, but in the way necessary to switch the EL.
 */

	.global SwitchToEL2
//; void SwitchToEL2(void)
SwitchToEL2:
	mov		x0, #(3<<4)			//; #0x5B5	RES1
//	orr		x0, x0, #(1<<13)		//; 0:not traped WFE, 1:traped WFE
//	orr		x0, x0, #(1<<12)		//; 0:not traped WFI, 1:traped WFI
//	orr		x0, x0, #(1<<11)		//; 0: CNTPS_xxx are only accessible in EL3, 1: if NS==0, EL3 and EL1 can access CNTPS_xxx
	orr		x0, x0, #(1<<10)		//; 0: Lower levels are all AArch32, 1: The next lower level is AArch64
//	orr		x0, x0, #(1<<9)			//; 0: Secure state inst fetches from Non-secure mem are permitted, 1: not permitted
//	orr		x0, x0, #(1<<8)			//; 0: Hypervisor call is undefined, 1: call ferforms a Hypervisor
//	orr		x0, x0, #(1<<7)			//; 0: SMC is enabled, 1: disabled
//	orr		x0, x0, #(1<<3)			//; 0: External Abort and SError interrupt are not taken to EL3, taken to EL3
	orr		x0, x0, #(1<<2)			//;	0: Physical FIQ are not taken to EL3, 1: taken to EL3
//	orr		x0, x0, #(1<<1)			//; 0: Physical IRQ are not taken to EL3, 1: taken to EL3
	orr		x0, x0, #(1<<0)			//; 0: EL0 and EL1 are in Secure state, 1: Non-Secure state ----- EL2 is always NS
	msr		scr_el3, x0

	msr		cptr_el3, xzr			//; [31] 0: Does not cause access to the CPACR_EL1 or CPTR_EL2 to be trapped. 1: trapped
									//; [20] 0: Does not cause System register access to the Trace Functionality to be trapped.
									//; [10] 0: Does not cause any instruction to be trapped.

	mov		x0, #0x33FF			//; RES1
	msr		cptr_el2, x0

	mov		x0, #0x0830			//; RES1
	movk		x0, #0x30C5, lsl #16		//; RES1
	msr		sctlr_el2, x0			//; MMU off, I and C bit off, Align bit off, little endian, execute never

	mov		x0, sp
	msr		sp_el2, x0

	mrs		x0, vbar_el3
	msr		vbar_el2, x0

	mov		x0, #0x3c9			//; D, A, I, F, EL2_SP2
	msr		spsr_el3, x0

	msr		elr_el3, x30			//; lr
	eret

	.global SwitchToEL1
//; void SwitchToEL1(void)
SwitchToEL1:
	mrs		x0, cnthctl_el2			//; not work at 5430 but for future
	orr		x0, x0, #0x3			//; disables the event stream, assessible form non-secure EL1, 0 mode to CNTP_xxx, accessible form Non-secure EL1, 0 to CNTPCT_EL0
	msr		cnthctl_el2, x0

	msr		cntvoff_el2, xzr		//; firtual offset

	mrs		x0, cntkctl_el1
	orr		x0, x0, #03				//; see the top cnthctl_el2 setting
	msr		cntkctl_el1, x0

	mrs		x0, midr_el1
	msr		vpidr_el2, x0

	mrs		x1, mpidr_el1
	msr		vmpidr_el2, x1

	mov		x0, #0x33ff			//; RES1
	msr		cptr_el2, x0		//; disable coprocessor traps to EL2

	msr		hstr_el2, xzr		//; disable coprocessor traps to EL2	Hypervisor System Trap Register 0: has no effect on Non-secure accesses to CP15

	mov		x0, #3<<20
	msr		cpacr_el1, x0		//; enable FP/SIMD at EL1.	Does not cause any instruction to be trapped.

	mov		x0, #(1<<31)		//; rw 0:lower levels are all aarch32, 1: EL1 is aarch64
	orr		x0, x0, #(1<<29)	//; disable HVC
//;	orr		x0, x0, #(1<<0)		//; enable 2nd stage trastion for excution in non-secure EL1 and EL0.
	msr		hcr_el2, x0

	mov		x0, #0x0800
	movk	x0, #0x30d0, lsl #16	//; MMU off, ICA bit off, all disable
	msr		sctlr_el1, x0

	mov		x0, sp
	msr		sp_el1, x0

	mrs		x0, vbar_el2
	msr		vbar_el1, x0

	mov		x0, #0x3c5				//; D, A, I, F, EL1_SP1
	msr		spsr_el2, x0

	msr		elr_el2, x30		//; lr
	eret

// THE END
