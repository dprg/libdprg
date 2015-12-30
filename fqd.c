/**
 * \file fqd.c
 * \brief Four Quadrant Decoder using TPU FQD on Mini-Robominds 68332
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 *
 * Uses the queue structure and functions from "queue.h"
 *
 * <b>History:</b>
 *
 * 04 Oct 2004 dpa - Created based on DLG's RTEMS bot code
 *
 * 05 Feb 2005 dpa - Inverted encoder direction to match hardware.
 *
 * 05 Feb 2005 dpa - Added left_pos, right pos
 *
 * 19 Dec 2006 rsr - Normalized comments to Doxygen style
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 * \todo Verify license status of DLG's code
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

#include "tpu.h"

/* int g->fqd_enable; */

/**
 * Configures a consecutive pair of TPU channels for quadrature decoding
 *
 * @param chan Base TPU channel to be used. Consecutive channel chan+1 will
 * also be configured.
 * @param prio Desired channel priority
 * @return -1 if not ready, 0 if successful
 */
int fqd_init(int chan, int prio)
{
	/* disable channel and channel+1 */
	tpu_set_cpr(chan, 0x0);
	tpu_set_cpr(chan + 1, 0x0);

	/* set the cfsr for channel and channel+1 to FQD (0x06) */
	tpu_set_cfsr(chan, 0x6);
	tpu_set_cfsr(chan+1, 0x6);

	/* primary channel (chan) */
	/* initialize Position Count */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x2)) ) = 0x0000; 

	/* address of pin state for corresponding channel */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0x8)) ) = (chan+1) * 0x10 + 6;

	/* Edge time LSB */
	( *(unsigned short *) (TPU_RAM + (chan * 16 + 0xA)) ) = chan * 0x10 + 1;

	/* secondary channel (chan+1) */
	/* address of pin state for corresponding channel */
	( *(unsigned short *) (TPU_RAM + (chan + 1) * 16 + 0x8) ) = chan * 0x10 + 6;

	/* Edge time LSB */
	( *(unsigned short *) (TPU_RAM + (chan + 1) * 16 + 0xA) ) = chan * 0x10 + 1;

	/* set channel as primary in Normal Mode */
	tpu_set_hsqr(chan, 0x0);

	/* set channel+1 as secondary in Normal Mode */
	tpu_set_hsqr(chan+1, 0x1);

	/* initialize both channels */
	tpu_set_hsrr(chan, 0x3);
	tpu_set_hsrr(chan + 1, 0x3);

	/* enable both channels and set priority */
	tpu_set_cpr(chan, prio);
	tpu_set_cpr(chan + 1, prio);

	return 0;
}


/**
 * Return the position counter value from the specified channel and
 * optionally zero the counter.
 *
 * @param chan TPU channel to use.
 * @param zero Should be set to a non-zero value in order to zero
 * the counter.
 * @return The current position counter value
 */
int fqd_position(int chan, int zero)
{
	int position;
	short *pointer;

	pointer = (short *)(TPU_RAM + (chan * 16 + 0x02));
	position = *pointer;
	if (zero) *pointer = 0;

	return (position);
}


/**
 * Set the position counter value of a specified channel
 *
 * @param chan TPU channel to set.
 * @param value The value to which the position counter will be set. 
 */
void fqd_write(int chan, unsigned value)
{
	*(unsigned short *) (TPU_RAM + (chan * 16 + 0x2)) = value;
}

#define VELOCITY 2000

/**
 * Global velocity values
 */
int left_velocity, right_velocity;

/**
 * Global position counters
 */
int left_position, right_position;

/**
 * Calculates the current velocity and position of the left and right
 * encoders. This should be run at 25hz.
 */
void speedometer(void)
{
	// temp - fix this!!
	int t1 = 2;
	int t2 = 4;

	left_velocity  = fqd_position(t1,1);
	right_velocity = fqd_position(t2,1);

	left_position += left_velocity;
	right_position += right_velocity;

	if ((left_velocity > VELOCITY) || (left_velocity < -VELOCITY)) {
		tpu_set_hsqr(t1, 0x2);	/* fast decode */
	} else {
		tpu_set_hsqr(t1, 0x0);	/* normal decode */
	}

	if ((right_velocity > VELOCITY) || (right_velocity < -VELOCITY)) {
		tpu_set_hsqr(t2, 0x2);	/* fast decode */
	} else {
		tpu_set_hsqr(t2, 0x0);	/* normal decode */
	}
}


#define SPD_WAIT 50

/**
 * Global speedometer handler wait counter
 */
int spd_wait;

/**
 * This handler should be run from the 1Khz system interrupt. It
 * will execute the speedometer function at 25hz (modulo 50).
 */
void speedometer_handler(void)
{
	if (spd_wait == 0) {
		speedometer();
		spd_wait = SPD_WAIT;
	}
	spd_wait--;
}
