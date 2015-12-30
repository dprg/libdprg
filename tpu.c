/**
 * \file tpu.c
 * \brief Functions to configure and initialize 68332 TPU 
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * A set of functions for configuring TPU registers on the 68332. Derived
 * in part from Duane Gustavus' RTEMS bot code.
 *
 * <b>History:</b>
 *
 * 20 Oct 2001 dpa - created
 *
 * \todo Check original source license for issues
 * \todo Most TPU functions need better comments
 * \todo Many functions are declared int but always return 0 - use void?
 * \todo Find out what iarb is in tpu_init()
 * \todo Several functions might be better as macros
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

#include <tpu.h>

/**
 * Initializes the TPU. Note that this clears and disables all interrupts,
 * so any functions that need to set interrupts must do their init() after
 * this one.
 *
 * @param iarb ???
 * @return Always returns 0
 */
int tpu_init(int iarb)		/* initialize TPU */
{
	TPU_TMCR = (0x0040+iarb);	/* PSCK = 1, TCR1 = clock/4 (PSCK=0 = clock/32 */
	TPU_TICR = 0x0640;	/* tpu interrupt, level 6, base of vectors at 64 */
	TPU_CIER = 0x0000;	/* clear tpu interrupt enable register, all channels */
	return (0);
}


/**
 * Set a channel 
 *
 * @param channel The TPU channel to set
 * @param function ???
 * @return Always returns 0
 */
int tpu_set_cfsr(short channel, short function)
{
	unsigned short *pcfsr, word_offset, nibble_offset, mask;

	/* set which cfsr register and which nibble offset */
	word_offset = (channel / 4) * 2;
	nibble_offset = (channel % 4) * 4;;

	/* construct mask to clear channel nibble */
	mask = 0x000F <<nibble_offset;

	/* move the function nibble to the correct position */
	function = (function <<nibble_offset) & mask;

	/* set TPU address for this channel. */
	pcfsr = (short *)(TPU_BASE + 0x12 - word_offset);

	/* clear the current function */
	*pcfsr &= ~mask;

	/* or in the new function */
	*pcfsr |= function;

	return (0);
}


/**
 * Sets a value in a host sequence register
 *
 * @param channel The desired TPU channel
 * @param function ????
 * @return Always returns 0
 */
int tpu_set_hsqr(short channel, short function)
{
	short *hsqr, mask;

	/* position function bits and mask */
	function = function <<((channel % 8) * 2);
	mask = 0x03 <<((channel % 8) * 2);

	/* pointer to correct hsqr register */
	hsqr = (short *)((TPU_BASE + 0x16) - ((channel / 8) * 2));

	/* clear and set function */
	*hsqr &= ~mask;
	*hsqr |= function;

  	return (0);
}


/**
 * Sets a value in host service request register
 *
 * @param channel The desired TPU channel
 * @param function ????
 * @return Always returns 0
 */
int tpu_set_hsrr(short channel, short function)
{
	short *hsrr, mask;

	/* position function bits and mask */
	function = function <<((channel % 8) * 2);
	mask = 0x03 <<((channel % 8) * 2);

	/* pointer to correct hsrr register */
	hsrr = (short *)((TPU_BASE + 0x1A) - ((channel / 8) * 2));

	/* clear and set function */
	*hsrr &= ~mask;
	*hsrr |= function;

	return (0);
}


/**
 * Sets a value in the channel priority register
 *
 * @param channel The desired TPU channel
 * @param priority The priority value
 * @return Always returns 0
 */
int tpu_set_cpr(short channel, short priority)
{
	short *cpr, mask;

	/* position priority bits and mask */
	priority = priority <<((channel % 8) * 2);
	mask = 0x03 <<((channel % 8) * 2);

	/* pointer to correct cpr register */
	cpr = (short *)((TPU_BASE + 0x1E) - ((channel / 8) * 2));

	/* clear and set function */
	*cpr &= ~mask;
	*cpr |= priority;

	return (0);
}


/**
 * Get channel CPR status
 *
 * @param channel The desired TPU channel
 * @return CPR value of specified channel
 */
short tpu_get_cpr(short channel)
{
	short cpr, mask;

	/* position mask */
	mask = 0x03 <<((channel % 8) * 2);

	/* pointer to correct cpr register */
	cpr = *(short *)((TPU_BASE + 0x1E) - ((channel / 8) * 2));

	/* clear and set function */
	return (cpr & mask);
}


/**
 * Sets channel interrupt enable register
 *
 * @param channel The desired TPU channel
 * @cie ???
 * @return Always returns 0
 */
int tpu_set_cier(short channel, short cie)
{
	TPU_CIER = TPU_CIER & (0xFFFF^(0x1<<channel));
	TPU_CIER = TPU_CIER | ( (0x1<<channel) & (cie<<channel) );

	return (0);
}


/**
 * Gets channel interrupt status register
 *
 * @param channel The desired TPU channel
 * @return Inerrupt status of specified TPU channel
 */
int tpu_get_cisr(short channel)
{
	return ((TPU_CISR & (0x1<<channel)) ? 1 : 0);
}


/**
 * Sets timer interrupt control register
 *
 * @param cirl ????
 * @param cibv ????
 */
void tpu_set_ticr(short cirl, short cibv)
{
  TPU_TICR = ( (0x0700 & cirl<<8) | (0x00F0 & cibv<<4) );
}

