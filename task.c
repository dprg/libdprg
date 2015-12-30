/**
 * \file task.c
 * \brief Cooperative multitasker with round-robin scheduler
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * 1 kHz system clock resources for MRM 68332. Modeled after
 * robo/srat04/libsrc/sysclock.c and Mark Castelluccio's MEMTest code.
 *
 * <b>History:</b>
 *
 * 08 Oct 2004 dpa - Ported from SR04 C code for the Motorola 68HC11
 *
 * 17 Feb 2005 dpa - Fixed passing of argument to task as startup
 *
 * 19 Feb 2005 dpa - Removed timer_done() calls in msleep(), and tsleep()
 *                   to speed up execution
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 * \todo Merge inline assembler into single function calls if possible
 * \todo Identify and comment the chunk of inline assembler as defer()
 * \todo Desc of tsleep() and better distinction from msleep()
 *
 */

/*
 Copyright (C) 2004-2007 David P. Anderson <dpa@io.isem.smu.edu>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include "task.h"

//extern long int sysclock;

/* globals, .bss (init to 0) segment */

/**
 * Pointer to currently running task
 */
TASK *current;

/**
 * Global process ID counter
 */
int gbl_pid;

/**
 * Idle task counter
 */
int proc_counter;

/**
 * System run level: 0 = single task, 1 = multi-task
 */
int run_level;

/**
 * Creates a task structure and links it into the current task list
 *
 * @param (*func)() Pointer to task function
 * @param arg Pointer to task function argument
 * @param stack_size Requested stack size
 * @return The process ID if successfull or a calloc error on failure
 */
int create_task(void (*func)(), int arg, int stack_size)
{
	TASK *p;
	int *s;

	/* allocate space for task struct and stack */
	p = (TASK *)calloc(1, sizeof (TASK) + (stack_size*sizeof(int)) );

	/* initialize task struct */

	p->func = func;
 	p->arg = arg;
  	p->state = 0;
  	p->pid = ++gbl_pid;

	/* setup a6 and stack for "unlk %a6" and "rts" */

	s = &p->stack[stack_size-1];		/* get stack address to s */
	*s-- = arg;				/* push argument to stack using s */
	*s-- = (int) &p->stack[stack_size-1];	/* push stack address to stack using s */
	p->fp = s;				/* use post decremented s as initial fp */
						/* (unlk %a6 is a6 -> sp, (*sp)++ -> a6) */

	/* add ourselves to linked list */

	if (current == 0) {
		current = p; 
		p->next = p;
	} else {
		p->next = current->next;
		current->next = p;
	}
                
	return p->pid;
}

/*----------------------------------------------------------------------- */
/* defer()	Do context switch, round-robin through linked task list */
/*

Comments for assembly code are here.  Actual code is below, uncommented.
(For some reason I can't get assembly comments to work.)

void defer(void)		
{                              ; defer() starts with "link %a6,#0" 
                               ; a6 -> -(*sp), sp -> a6, sp += #0 
	if (run_level) {
		asm("\n"
		"   move.l   current,%a0 ; get pointer to current task struct\n"
		"   move.l   %a6,20(%a0) ; store our frame pointer in task struct\n"
		"   move.l   0(%a0),%a0  ; fetch next frame pointer (linked list)\n"
		"   move.l   %a0,current ; and make it current");

		asm("\n"
		"   tst.l    16(%a0)     ; is this a new task?\n"
		"   beq      new_task    ; yes, go initialize its stack\n"
		"   move.l   20(%a0),%a6 ; else just load frame pointer from new task\n"
		"                        ; and use the "unlk %a6" and "rts" from defer()");
	}
}

new_task() assembly call: setup stack for new task and execute it 
called from defer() for a new task, and from scheduler() at startup.

	asm("\n"
	"new_task:                  ; entry for defer() and scheduler()\n"
	"   move.l   current,%a0    ; get pointer to current task struct\n"
	"   move.l   20(%a0),%a6    ; get frame pointer from struct\n"
	"   unlk     %a6            ; a6 -> sp, (*sp)++ -> a6\n"
	"   move.l   4(%a0),-(%sp)  ;*func() -> --(*sp)\n"
	"   move.l   8(%a0),%d0     ; arg -> d0 (pass to *func())\n"
	"   move.l   #1,16(%a0)     ; set task state = 1\n"
	"   rts                     ; rts to *func(), (already unlk)");

End of Comments
*/


/**
 * Do a round-robin context switch to the next linked task
 */
void defer(void)
{
	if (run_level) {
		asm("move.l	current,%a0");
		asm("move.l	%a6,20(%a0)");
		asm("move.l	0(%a0),%a0");
		asm("move.l	%a0,current");

		asm("tst.l	16(%a0)");
		asm("beq	new_task");
		asm("move.l	20(%a0),%a6");
	}
}


/*----------------------------------------------------------------------- */
/* new_task() assembly call: setup stack for new task and execute it */
/* called from defer() for a new task, and from scheduler() at startup */

	asm("new_task:");
	asm("move.l	current,%a0");
	asm("move.l	20(%a0),%a6");
	asm("unlk	%a6");
	asm("move.l	4(%a0),-(%sp)");
	asm("move.l	8(%a0),%d0");
	asm("move.l	#1,16(%a0)");
	asm("rts");


/**
 * Scheduler. Starts multi-tasking. This should be the last thing run
 * in main() as it does not exit.
 */
void scheduler()
{
	extern void null_task();

	proc_counter = 0;
	run_level = 1;
	create_task(null_task,0,64);
	asm("jmp new_task");
}


/**
 * Increments a counter that acts as a metric for determining the currently
 * available unused task capacity.
 *
 * @param x Unused
 */
void null_task(int x)
{
	while(1) {
		proc_counter++;
		defer();
	}
}


/**
 * Finds the task pointer of a requested process ID.
 *
 * @param pid The process ID of the target process
 * @return The task pointer of the process or -1 if it is not found
 */
TASK *findpid(int pid)
{
	TASK *p;

	p = current;
	while (p->pid != pid) {
		p = p->next;
		if (p == current) {
			p = (TASK*)-1;
			break;
		}
	}
	return p;
}


/**
 * Finds task pointer to previous task in linked list
 *
 * @param t Pointer to current task
 * @return Pointer to previous task
 */
TASK *findprev(TASK *t)
{
	TASK *p;
	p = t;
	while (p->next != t)
		p = p->next;
	return p;
}



/**
 * Kill a task by process ID
 *
 * @param pid Process ID of task to be killed
 */
void kill_process(int pid) 
{
 	TASK *t,*p;

	if (pid) {
 		if((t = findpid(pid)) != (TASK*)-1) {

			p = findprev(t);
			p->next = t->next;
			free(t);

			if (t == current) {
				current = p->next;
				asm("move.l current,%a0");
				asm("move.l 20(%a0),%a6");
			}
		}
	}
}


/**
 * Kills the calling process
 */
void terminate()
{
	kill_process(current->pid);
}


/**
 * Suspend execution of a task in the round-robin queue
 *
 * @param delay Number of milliseconds to suspend the task
 */
void msleep(int delay)
{
	long t;

	t = sysclock+delay;
	while (sysclock < t) defer();
}


/**
 * Variant of sleep - need description
 */
long tsleep(long t, int delay)
{
	t += delay;
	while (sysclock < t) defer();
	return(t);
}
