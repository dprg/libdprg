/**
 * \file system_init.c
 * \brief Initialize libdprg
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Initialize MRM ibrary functions. Calls all inits() in this library
 *
 * <b>History:</b>
 *
 * 07 Oct 2004 dpa - Created
 *
 * 27 Oct 2004 dpa - Added call to tpu_init(0x06);
 *
 * 10 Feb 2005 dpa - Added call to analog_services();
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 * \todo system_init() should accept an initial clock speed value and
 * pass it to cpu_init()
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
 * Initialize the system function in the library and install the system
 * interrupt service routine on the 1kHz sysclock timer
 *
 * @return Always returns 0
 */
int system_init()
{
	void extern (*system_interrupt)();
	void extern system_services();
	void extern analog_services();

	cpu_init();
	sysclock_init();
	led_init();
	lcd_init(); 
	sci_init(); 
	tpu_init(0x06);   /* iarb = 6 */
	analog_service(); /* init analog read */

	/* set hook to 1Kz sysclock interrupt for LCD and A/D service */
	system_interrupt = &system_services;

	/* Enable level 6&7 interrupts */
	asm("move.w #0x2500,%sr"); 

	return 0;
}
	

/**
 * System 1kHz interrupt service routine. Services the LCD and A/D hardware
 */
void system_services()
{
	lcd_service(); 
	analog_service(); 
}

