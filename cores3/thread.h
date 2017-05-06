/*
 * Copyright (C) 2016  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */
/* thread.h
 * T. Trebisky  8/25/2002
 */

#ifndef NULL
#define NULL	(0)
#endif

enum console_mode { SERIAL, VGA, SIO_0, SIO_1 };

typedef void (*tfptr) ( int );
typedef void (*vfptr) ( void );

/* priorities >= MAGIC are for special use.
 * (and I will probably reserve single digit
 *  priorities similarly.)
 * No priority *ever* gets set as high as MAX_PRI
#define		PRI_USER	950
 */
#define		PRI_SYS		5
#define		PRI_USER	10
#define		PRI_MAGIC	900
#define		MAX_PRI		1001
#define		PRI_IDLE	1234

enum thread_state {
	READY,		/* 0 - ready to go */
	WAIT,		/* 1 - blocked */
	SWAIT,		/* 2 - blocked on semaphore */
	DELAY,		/* 3 - blocked on timer event */
	IDLE,		/* 4 - running idle loop */
	JOIN,		/* 5 - waiting to join somebody */
	ZOMBIE,		/* 6 - waiting to be joined */
	FAULT,		/* 7 - did something bad */
	REPEAT,		/* 8 - blocked on repeat event */
	DEAD		/* 9 - on free list */
};

struct thread *thr_new ( char *, tfptr, void *, int, int );
struct thread *thr_new_repeat ( char *, tfptr, void *, int, int, int );
struct thread *thr_self ( void );
void thr_kill ( struct thread * );
void thr_exit ( void );

void thr_block ( enum thread_state );
void thr_unblock ( struct thread * );

struct thread * safe_thr_new ( char *, tfptr, void *, int, int );

/* flags for thr_new:
 */

#define	TF_BLOCK	0x0001
#define	TF_FPU		0x0002
#define	TF_JOIN		0x0004
#define	TF_REPEAT	0x0008

/* flags for sem_new:
 */
#define	SEM_FIFO	0x0000
#define	SEM_PRIO	0x0001
#define	SEM_TIMEOUT	0x0002

enum sem_state { CLEAR, SET };

// struct sem *sem_new ( enum sem_state, int );
struct sem *sem_mutex_new ( int );
struct sem *sem_signal_new ( int );

void sem_block ( struct sem * );
void sem_unblock ( struct sem * );

struct cv *cv_new ( struct sem * );
void cv_wait ( struct cv * );
void cv_signal ( struct cv * );

#ifdef notyet
struct sem * cpu_new ( void );
void cpu_wait ( struct sem * );
void cpu_signal ( struct sem * );
#endif

/* This was 6 in Skidoo */
#define MAX_TNAME	10

#ifdef ARCH_ARM
/* XXX - move this */
/* We only need to save registers that the
 * compiler EABI says we should, since this
 * is always done from synchronous calls
 */
struct jmp_regs {
	int regs[17];
};

/* We have to save all registers here, since
 * an interrupt is entirely unpredictable.
 */
struct int_regs {
	int regs[17];
};

/* These aren't really registers.
 * We bit the bullet on the ARM, and put the
 * info for cont (and quick) in their own place
 * I think there were bugs waiting in the x86 code by
 * trying to keep this in the jmp_regs, so this ought
 * to be retroed to the x86 someday.
 */
struct cont_regs {
	int regs[4];
};
#endif

#ifdef ARCH_X86
/* XXX - move this stuff */
/* This is what is saved by save_t/restore_t
 * if the size of this changes, you must fiddle
 * some constants in locore.S so that the next
 * batch of registers get found properly.
 */
struct jmp_regs {
	int ebx;
	int esp;
	int ebp;
	int esi;
	int edi;
	int eip;	/* We need this */
};

/* We don't need eip here, since ...
 * well things are just different here.
 */
struct int_regs {
	int eax;
	int ebx;
	int ecx;
	int edx;
	int esi;
	int edi;
	int ebp;
	int esp;
};
#endif


enum thread_mode { JMP, INT, CONT };

/* The iregs structure is referenced from the assembly language
 * interrupt handling code which expects to find a place to store
 * the interrupt context at the start of this structure.
 * It is not clear that the position of "regs" is constrained
 * in any way, but comments from x86 skidoo code suggest so.
 */

struct thread {
	struct int_regs iregs;		/* must be first */
	struct jmp_regs regs;
	struct cont_regs cregs;		/* new for ARM */
	int prof;
	struct thread *next;		/* all threads */
	struct thread *wnext;		/* waiting list */
	enum thread_state state;
	enum thread_mode mode;		/* how to resume */
	enum console_mode con_mode;	/* type of console */
	char *stack;
	int stack_size;
	int pri;
	int delay;
	int rep_reload;
	int rep_count;
	struct thread *rep_next;	/* list of repeating threads */
	int overruns;
	int fault;		/* why we are suspended */
	char name[MAX_TNAME];
#ifdef notdef
	tfptr c_func;		/* continuation function */
	int c_arg;		/* continuation argument */
#endif
	int flags;
	struct thread *join;	/* who wants to join us */
	struct thread *yield;	/* who yielded to us */
};

/* Here are fault codes (kind of like errno)
 * XXX - maybe this should be an enum.
 */

#define FA_NIL		0
#define FA_ZDIV		1

/* ---------------------------------------------------------
 */

struct sem {
	struct sem *next;		/* links together avail and on timer */
	struct thread *list;		/* list of threads blocked on this */
	int state;			/* SET or CLEAR */
	int flags;
	int delay;			/* for sem with timeout */
};

struct cv {
	struct cv *next;
	struct sem *signal;
	struct sem *mutex;
};

/* THE END */
