#ifndef _BOARD_H
#define _BOARD_H

/*
 * Copyright (C) 2017  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * board.h for the Orange Pi PC
 *
 *  Kyu project  1-6-2017  Tom Trebisky
 *
 */

#define BOARD_ORANGE_PI
#define BOARD_ORANGE_PI_PC
#define ARCH_ARM

#define NUM_INTS	157
#define NUM_CORES	4

#define CORE_STACK_SIZE		16*1024

/* XXX - This is 1G of RAM as I have on my PC and PC plus boards.
 * There are H3 based boards with 1/2 G (the Lite and One)
 * as well as H3 based boards with 2 G (the PC 2),
 *  but I am not working with any of those.
 */
#define BOARD_RAM_SIZE	0x40000000	/* 1G */

#define BOARD_RAM_START	0x40000000
#define BOARD_RAM_ENDP	(BOARD_RAM_START + BOARD_RAM_SIZE)	/* 0x80000000 */
// #define BOARD_RAM_END	(BOARD_RAM_ENDP - 1)
#define BOARD_RAM_END	0x7FFFFFFF

/* Some things that we pulled addresses out of the air for */
// #define THR_STACK_BASE	0x48000000
// #define MALLOC_BASE	0x50000000

#define THR_STACK_LIMIT	4096 * 128
#define MALLOC_SIZE	4 * 1024 * 1024

#define CONSOLE_BAUD		115200

#define INITIAL_CONSOLE		SERIAL

/* We add the 2 bytes on the ARM to get 4 byte alignment
 * of the IP addresses in the IP header.
 */
// #define NETBUF_PREPAD	2
#define NETBUF_PREPAD	0

/* THE END */
#endif /* _BOARD_H */
