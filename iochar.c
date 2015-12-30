/**
 * \file iochar.c
 * \brief Character I/O function to support read() and write()
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * File handle value assumptions:
 *
 * fd = 0 = stdin  is serial port
 * fd = 1 = stdout is serial port
 * fd = 2 = stderr is lcd display
 *
 * Notes:		
 *
 * mrm_putc() and mrm_getc() call defer() while blocking. If multi-tasking
 * not enabled (run_level == 0) then defer() just returns, else defer() 
 * polls round-robin linked-list queue for tasks ready to execute, while
 * it waits for fifo_empty or char available.
 *
 * <b>History:</b>
 *
 * 08 Oct 2004 dpa - Created
 *
 * 30 Dec 2006 rsr - Normalize identation
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
 * Writes a character to serial port or LCD depending on fd.
 * 
 * @param fd Destination: 1 = stdout = SCI or 2 = stderr = LCD
 * @param x Character to written
 * @return Zero when a character is successfully written
 */
int mrm_putc(int fd, char x) 
{
	if (fd == 2) {          /* fd=stderr = LCD */
		while (lcd_putc(x)) defer();
	} else {                /* fd=stdout = SCI */
		if (x == 10)
			while(sci_putc(13)) defer();
		while(sci_putc(x)) defer();
	}
	return 0;
}

/**
 * Reads a character from the serial port
 * 
 * @param fd Port to read from. This value is ignored at present as only the
 * serial port can be read.
 * @return The character read from the serial port
 */
char mrm_getc(int fd) 
{
	int c;

	while (c = sci_getc() < 0)
		defer();
	return (char)(c);
}

