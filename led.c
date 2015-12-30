/**
 * \file led.c
 * \brief LED support for Mini-Robominds board
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Provides function to turn the MRM board LEDs on and off.
 *
 * <b>History:</b>
 *
 * 09 Oct 2004 dpa - Created
 *
 * 30 Dec 2006 rsr - Normalized indentation
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

#include "sim_reg.h"
#include "led.h"


/**
 * Initializes the 68332 SIM Port C data registers for LED use.
 */
void led_init(void) {
	SIM_CSPDR  = SIM_CSPDR  | 0x30;  /* SIM Port C data register, both leds off */
	SIM_CSPAR1 = SIM_CSPAR1 & 0xffc3;/* SIM Chip-Select Pin Assign, clr 2 bits ?*/
}


/**
 * Turns on the specified LED
 *
 * @param color The color (RED_LED or GREEN_LED) of the LED to turn on.
 */
void led_on(int color) {
	SIM_CSPDR &= ~color;
}


/**
 * Turns off the specified LED
 *
 * @param color The color (RED_LED or GREEN_LED) of the LED to turn off.
 */
void led_off(int color) {
	SIM_CSPDR |= color;
}


/**
 * Toggles the state of the specified LED
 *
 * @param color The color (RED_LED or GREEN_LED) of the LED to toggle.
 */
void led_toggle(int color) {
	SIM_CSPDR = SIM_CSPDR ^ color;
}
