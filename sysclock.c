/**
 * \file sysclock.c
 * \brief 1 kHz system clock resources for MRM 68332
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * 1 kHz system clock resources for MRM 68332. Modeled after
 * robo/srat04/libsrc/sysclock.c and Mark Castelluccio's MEMTest code.
 *
 * <b>History:</b>
 *
 * 21 Aug 2001 dpa - Created
 *
 * 06 Oct 2004 dpa - added __attribute__((__interrupt__)) for 1kHz interrupt
 *
 * 10 Oct 2004 dpa - (*user_interrupt)()
 *
 * 26 Oct 2004 dpa - Removed __attritribute__((__interrupt__)) from hz1000_int
 *                   and hand coded interrupt entry to protest all registers
 *                   (because interrupt calls system and user vectors)
 *
 * 29 Nov 2004 dpa - Changed interrupt vector number from 64 to 80, so
 *                   TPU can use 16 interrupt vectors from 64-80.
 *
 * 19 Feb 2005 dpa - Sysclock no longer static (not global)
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 * \todo More specific on derivation - license issues?
 * \todo protest all registers? protect perhaps?
 * \todo check if possible to do multiple lines of inline assembler
 * \todo replace single instruction functions with macros
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


/**
 * System global 32 bit millisecond clock
 */
long int sysclock;	

/**
 * System hook for 1khz interrupt for lcd_service() and analog_service()
 */
void (*system_interrupt)();

/**
 * User hook for 1khz interrupt (speedometer and other regular sampling)
 */
void (*user_interrupt)();


/* ---------------------------------------------------------------------- */
/* assembly entry to hz1000 interrupt, saves/restores all registers */
/* so that interrupt can vector execution of system_interrupt()	and */
/* user_interrupt() */

asm(".global hz1000_int");
asm("hz1000_int:link %a6,#0");
asm("moveml %a6-%d0,-(%a7)");
asm("jsr hz1000_handler");
asm("moveml (%a7)+,%d0-%a6");
asm("unlk %a6");
asm("rte");


/**
 * 1kHz interrupt service. Increments sysclock and calls any system or
 * user functions that have been registered
 */
void hz1000_handler(void)
{
	sysclock++;
	if (system_interrupt) (*system_interrupt)();
	if (user_interrupt) (*user_interrupt)();
}


/**
 * Initialize 1khz interrupt using the SIM module (not a TPU timer).
 * The periodic interrupt is based on modulo counter in the low 7 bits of
 * PITR @ 0xffa24. For the 3.768 crystal, count = 8192/period. For 1Khz,
 * count = 8. This does not change when the internal clock is 16MHz or 
 * 25MHz. 
 */
int sysclock_init(void) 
{
	int extern vbraddr;
	int extern hz1000_int();

	*(long*)(vbraddr+80*4) = (long)hz1000_int;
	*(short int*)0xfffa24 = 8;		/* PITR, 8 = 1000 hz, 82 = 100hz */
	*(short int*)0xfffa22 = 0x600+80;	/* PICR control register */
					/* 0x600 is priority 6, 80 is vector number */
	return 0;
}


/**
 * Reset the system millisecond counter to zero
 */
void reset_sysclock(void) 
{
	sysclock = 0L;
}


/**
 * Return the current value of the system millisecond counter
 */
long mseconds()
{
	return sysclock;
}


/**
 * Test a provider timer value against the system clock.
 *
 * @param timer User provide timer value
 * @return 0 if sysclock has not reached timer value or 1 if it has
 */
int timer_done(long timer)
{
	return (timer < sysclock);
}


/**
 * Add a user supplied delay to the current system clock value
 *
 * @param delay User supplied delay in milliseconds
 * @return delay + sysclock
 */
long plus_sysclock(long delay)
{
	return sysclock+delay;
}


/**
 * Add a user supplied delay to a user supplied timer value
 *
 * @param timer User supplied timer in milliseconds
 * @param delay User supplied delay in milliseconds
 * @return timer+delay
 */
long plus_timer(long timer, long delay)
{
	return timer+delay;
}

/*
#define PLUS_TIMER(timer, delay) (timer+delay)
*/
