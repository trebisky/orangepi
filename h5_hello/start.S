/* Extremely simple (and virtually unnecessary)
 * assembly language startup file.
 */

	.global start
start:
	msr		DAIFSet, #7		// disable interrupts

	bl		main
	bl		spin

spin:	b		spin


// THE END
