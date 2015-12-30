/**
 * \file cpu_init.c
 * \brief Housekeeping for 68332 startup
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Set VBR exception vector register, copy CPU32Bug vectors, clear ram
 * wait states, set clock speed.
 *
 * <b>History:</b>
 *
 * 01 Oct 2004 dpa - Created
 *
 * \todo Rename globals to new naming scheme
 * \todo cpu_init() should accept an initial clock speed value
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

#include "qsm_reg.h"

/**
 * Vector base table. 68332 VBR register should be set to this address
 */
long myvector[256];

/**
 * pointer to vector base table
 */
int vbraddr = (int)&myvector[0];

/**
 * Sets up the vector base table and hardware vector base register. 
 * Initializes vectors to CPU32BUG defaults. Sets Flash and RAM wait states.
 * Sets CPU speed.
 */
int cpu_init(void)
{
	int j;

	/* ---------------------------------------------------------- */
	/* setup vector base table and hardware vector base register */

	vbraddr = (int)&myvector[0];	/* get address of our vector table */

	/* Initialize all interrrupt vectors to the CPU32BUG defaults */
	/* copy $0000-$0400 to myvector pointed by vbraddr */

	for(j=2;j<256;j++) *(long*)(vbraddr+j*4) = *(long*)(j*4);

	/* set address of myvector contained in vbraddr to the hardware vbr */

	asm("move %d0,-(%a7);move.l vbraddr,%d0;movec %d0,%vbr;move (%a7)+,%d0");

	/* --------------------------------------------------------------- */
	/* Set wait states to zero Flash and RAM --- special MRM magic foo */
	/* From the SIM reference manual p1-8, these SIM registers are  */
	/* "unassigned" and they are also labled "Chip Select" as below. */
	/* Not sure what Mark is doing here. */

	*(unsigned short int*)0xfffa4a = 0x7830;	/* "Chip Select Option Boot (CSORBT)" */
	*(unsigned short int*)0xfffa66 = 0x6830;	/* "Chip Select Option 6 (CSOR6)" */
	*(unsigned short int*)0xfffa4e = 0x7830;	/* "Chip Select Option 0 (CSOR0)" */

	/* ---------------------------------------------------------- */
	/* Write SIM_SYNCR to set CPU clock speed (default is 8.389 MHz) */

	/* the options are: */
	/* 0x3f00;	// for 8.389 mHz */
	/* 0x7f00;	// Set clock speed to 16.778Mhz */
	/* 0xd700;	// Set clock speed to 25.166Mhz */

	*(unsigned short int*)(0xfffa04) = 0xd700; /* Set clock speed to 25.166 mHz */
	while((*(unsigned short int*)(0xfffa04) & 0x8) == 0){} ; /* and wait for "VCO locked" */

	return 0;
}
