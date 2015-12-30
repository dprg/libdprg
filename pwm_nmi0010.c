/**
 * \file pwm_nmi0010.c
 * \brief Signed-magnitude PWM for New Micros NMIH-0010 h-bridge
 * \author R. Steven Rainwater <steve@ncc.com>
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Provides signed-magnitdue PWM for the New Micros NMIH-0010 and
 * NMIH-0010-LED h-bridge. The NMIH-0010 is a quadruple half-h driver
 * based on the SN754410. It can two drive motors at 4.5v - 36v, 1A
 * continous, 2A peak per channel. A large portion of this code is
 * based on code originally developed by David P. Anderson.
 *
 * <b>History:</b>
 *
 * 12 Feb 2006 rsr - Created
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 */

/*
 Copyright (C) 2006-2007 R. Steven Rainwater <steve@ncc.com>

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


/* -------------------------------------------------------------------- */
/* pwm.c	Pulse width Modulation for two motor channels
		using the TPU Mask2 "Queued Output Match" function
		on TPU channels 0 (left motor) and 1 (right motor).

		Motor direction bits are set from Port F bits 
		0 (left motor) and 1 (right motor).

13 Oct 04 dpa Created.
14 Oct 04 dpa Added TPU channels 2&3 (or PORT F bits 0&1) as
              direction bits.  This allows software choice of
              Signed-Magnitude or Lock-Antiphase PWM control
              of motors.
19 Oct 04 dpa Swapped channels 0&1 and 2&3 to match cable orientation.
		0&1 are dir bits in SM and PWM bits in LA
		2&3 are PWM bits in SM and direction bits (always on) in LA
27 Oct 04 dpa pwm_init(flavor)	added flavor argument for init call.
28 Oct 04 dpa inverted PWM enable bits (tpu channel 2&3) in hardware,
              so HBRIDGE is DISABLED when system init pulls all TPU channels
              low. (i.e. starts with motors off).
29 Oct 04 dpa period is now int rather than #define, so pwm frequency can be
              changed on the fly		
30 Jan 05 dpa Inverted PWM cycles for new LM18200 H-Bridges
02 Feb 05 dpa Reversed left motor direction for Sign Magnitude PWM.
19 Feb 05 dpa 25 MHz
21 Feb 05 dpa Removed Locked Anti-Phase.  
              Moved Signed Mag to TPU chans 0&1, and PORTE bits 0&1
              Inverted duty cycle to match hardware encoders.
01 May 05 dpa Default PWM frequency 20khz, for new DAVENTECH H-Bridges.
              Swapped port E direction bits for new wiring harness.

/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Calculation of PWM period in TCR1 clock ticks.

Default MRM board clock frequency is set in "cpu_init.c" to:

	CPU clock = 16.788 mHz. 
	CPU clock = 25.166 mHz. 

Initialization of the TPU sets the TCR1 timer rate = clock/4, 

	TCR1 rate = 16.788/4 = 4.1945 mHz.
	TCR1 rate = 25.166/4 = 6.2915 mHz.

So the period for a given frequency expressed in TCR1 timer counts is:

	period = 4194500/frequency.
	period = 6291500/frequency.

For a PWM frequency of 1000 hz, period is:

	period@1 kHz = 4194500/1000 = 4194.5 
	period@1 kHz = 6291500/1000 = 6291.5

	
----------------------------------------------------------------------

The Queued Output Match function requires both the pulse high-time and
the pulse low-time to be written, rather than period and pulse-width.
So, for a given PW value from 0% to 100%,

	hi_time = (PW * period)/100;
	lo_time = period - hi_time;

For the QOM function, the hi and low times are left shifted 1 bit, and
the low-order bit is used to indicate whether the output pin should
go hi or lo.  So the complete value written to the TPU channel ram is:

	hi_time = (((PW * period)/100)*2) + 0;
	lo_time = ((period - hi_time)*2) + 1;

*/

#include "tpu.h"

#define PWM_FREQUENCY 20000 
#define PERIOD (TCR1_CLOCK/PWM_FREQUENCY) 

int period = PERIOD;		/* pwm period in TCR counts */

/* -------------------------------------------------------------------- */
/*	PORTE BITS 0&1 used for HBRIDGE Direction bits			*/
/* -------------------------------------------------------------------- */

/* init bits 0-3 */

int porte_init()
{
	*(unsigned char*)(0xfffa15) |= 0x0f;    /* PORTE bit0 & 1 are outputs */
	return (0);
}


/* -------------------------------------------------------------------- */
/* turn discrete portE  I/O pins for h-bridge direction bits on and off */

void onE0()
{
	*(unsigned char*)(0xfffa13) |= 0x01;    /* PORTE bit0 = 1 */
}


void offE0 ()
{
	*(unsigned char*)(0xfffa13) &= ~0x01;   /* PORTE bit0  = 0 */
}


void onE1()
{
	*(unsigned char*)(0xfffa13) |= 0x02;    /* PORTE bit1 = 1 */
}

void offE1 ()
{
        *(unsigned char*)(0xfffa13) &= ~0x02;   /* PORTE bit1  = 0 */
}

void onE2()
{
	*(unsigned char*)(0xfffa13) |= 0x04;    /* PORTE bit2 = 1 */
}

void offE2 ()
{
	*(unsigned char*)(0xfffa13) &= ~0x04;   /* PORTE bit2  = 0 */
}

void onE3()
{
	*(unsigned char*)(0xfffa13) |= 0x08;    /* PORTE bit3 = 1 */
}

void offE3 ()
{
	*(unsigned char*)(0xfffa13) &= ~0x08;   /* PORTE bit3  = 0 */
}


/* -------------------------------------------------------------------- */
/* PWM Initialization.  */

int pwm_init(int frequency)
{
	period = TCR1_CLOCK/frequency;	/* set period */

	pwm_sm_init(0);		/* init TPU channel 0 */	
	pwm_sm_init(1);		/* init TPU channel 1 */

	porte_init();		/* init PORTE bits 0&1 */
				
	pwmL(0);		/* turn motors off */
	pwmR(0);

	return (0);
}


/* -------------------------------------------------------------------- */
/* Signed-Magnitude PWM - initialize chan for QOM PWM and PORTE bit chan
 for direction bit */
int pwm_sm_init(int chan)
{
	/* disable channel */
	tpu_set_cpr(chan, 0x0);

	/* set the function for chan to QOM (0x0e) */
	tpu_set_cfsr(chan, 0xe);

	/* Setup PRAM for  QOM function */

	/* last offset is 0x06, bit0 = 0 = use TCR1 */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x0)) ) = (chan*16) + 0x06;  

	/* init loop count, init match = 0 */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x2)) ) = 0;

	/* initial hi time, init bit = 0 */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x4)) ) = 0;

	/* last offset, initial lo time, bit = 0 */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x6)) ) = 0;

	/* init host sequence reqisters, host service requests, and priority bits */

	tpu_set_hsqr(chan,2);		/* HSQR = %10, continuous mode */
	tpu_set_hsrr(chan,2);		/* HSRR = %10, init with pin low */
	tpu_set_cpr(chan,2);		/* Enable function, CPR = %10, priority 1 */

	return(0);
}


/* -------------------------------------------------------------------- */
/* pwmR(), pwmL()	set duty cycle  for right and left motors.	*/

int pwmR(int duty_cycle)	/* set duty cycle for Right motor */
{
	int hi, lo;
	long u;

	duty_cycle = -duty_cycle;	/* inverted encoders */

	if (duty_cycle < 0) {
		duty_cycle = -duty_cycle;
		offE0();
		onE1();
	} else {
		offE1();
		onE0();
	}

	if (duty_cycle > 100) duty_cycle = 100;
	/* duty_cycle = 100 - duty_cycle;	/* handle inverted I/O pins */

	hi = (period*duty_cycle)/100;
	lo = ((period-hi)*2)+1;
	hi = hi*2;

	u = (hi<<16)+lo; 
	*(long int*)(0xffff04) = u;	/* atomic */

	return (0);
}


int pwmL(int duty_cycle)	/* set duty cycle for Left motor */
{
	int hi, lo;
	long u;

	duty_cycle = -duty_cycle;	/* inverted encoders */

	if (duty_cycle < 0) {
		duty_cycle = -duty_cycle;
		offE2();
		onE3();
	} else {
		offE3();
		onE2();
	}
	if (duty_cycle > 100) duty_cycle = 100;
	/* duty_cycle = 100 - duty_cycle;	/* handle inverted I/O pins */

	hi = (period*duty_cycle)/100;
	lo = ((period-hi)*2)+1;
	hi = hi*2;
	
	u = (hi<<16)+lo;
	*(long int*)(0xffff14) = u;	/* atomic */

	return (0);
}
