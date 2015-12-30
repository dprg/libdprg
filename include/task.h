/* ------------------------------------------------------------------- */
/* task.h       struct definitions for multi-tasking 

05 Feb 98 dpa   Created.
08 Oct 04 dpa	MRM 68332.  changed unsigned chars to ints.
20 Feb 05 dpa	Added external reference for sysclock.

/* ------------------------------------------------------------------- */

typedef struct task TASK;

struct task
{
        TASK    *next;
        void    (*func)();
        int     arg;
        int	pid;
        int	state;
	int	*fp;		/* frame pointer, %a6 */
        int    stack[1];
};

/* For access from assembly:

	0	next
	4	(*func)()
	8	arg
	12	pid
	16	state
	20	*fp
	24	stack[1]
*/

/* ------------------------------------------------------------------- */

extern int sysclock;

/* ------------------------------------------------------------------- */
/* EOF task.h */

