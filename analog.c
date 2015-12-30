/**
 * \file analog.c
 * \brief Analog to digital conversion
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Provides the global variables and service routine for analog to digital
 * conversion. The service routine should be run from the 1kHz system 
 * interrupt.  It uses a counter to execute at 50 hz, every 20ms.
 * A/D values are returned in globals an0 thru an7. Only 7 channels are 
 * available; an7 is a dummy.
 *
 * <b>History</b>
 *
 * 01 Oct 2004 dpa - Created
 * 
 * 19 Feb 2005 dpa - Added dummy an8 channel in case loading pipeline is
 *                   corrupting an0. Use an8 to load pipeline instead.
 *
 * \todo Rename globals to new naming scheme
 * \todo Modify code to work at any clock rate
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

/* use 20 at 16MHz */
#define A2D_MOD 50

/**
 * Service interval counter
 */
int a2d_mod;

/**
 * Current A to D value
 */
volatile int an0,an1,an2,an3,an4,an5,an6,an7,an8;

/**
 * Loads the global A to D variables with the current values from the
 * analog sample buffer each time the modulo 20 counter reaches zero.
 */
void analog_service()
{
	if (--a2d_mod <= 0) {
		a2d_mod = A2D_MOD;

		an8 = *(unsigned char *)0xf00000;
		an8 = *(unsigned char *)0xf00001;
		an0 = *(unsigned char *)0xf00002;
		an1 = *(unsigned char *)0xf00003;
		an2 = *(unsigned char *)0xf00004;
		an3 = *(unsigned char *)0xf00005;
		an4 = *(unsigned char *)0xf00006;
		an5 = *(unsigned char *)0xf00007;
		an6 = *(unsigned char *)0xf00000;
		an7 = *(unsigned char *)0xf00000;
	} 
}
