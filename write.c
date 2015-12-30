/**
 * \file write.c
 * \brief Replacement for newlib write function
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Replaces write() from newlib with a similar function that write bytes
 * to an output device. Modified to call libdprg mrm_putc(), which blocks
 * using defer(). For the purposes of libdprg the stdout designation is
 * used for the serial UART and stderr is used for the LCD display. This 
 * code is based on code from libgloss in newlib.
 *
 * <b>History:</b>
 *
 * 09 Oct 2004 dpa - Created
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

 Portions of this code were derived from code in write.c of libgloss in
 newlib, originally Copyright (c) 1995 Cygnus Support.
 
 The authors hereby grant permission to use, copy, modify, distribute,
 and license this software and its documentation for any purpose, provided
 that existing copyright notices are retained in all copies and that this
 notice is included verbatim in any distributions. No written agreement,
 license, or royalty fee is required for any of the authorized uses.
 Modifications to this software may be copyrighted by their authors
 and need not follow the licensing terms described here, provided that
 the new terms are clearly indicated on the first page of each file where
 they apply. 		
*/

#include "glue.h"

/* extern int  _EXFUN (outbyte, (char x)); */
extern int  _EXFUN (mrm_putc, (int fd, char x));


/**
 * Writes bytes to the output FIFO.
 *
 * @param fd 1 = stdout = serial UART or 2 = stderr = LCD
 * @param buf Pointer to buffer containing bytes to written
 * @param nbytes The number of bytes to written
 * @return Number of bytes writeen
 */
int
_DEFUN (write, (fd, buf, nbytes),
       int fd _AND
       char *buf _AND
       int nbytes)
{
  int i;

  for (i = 0; i < nbytes; i++) {
    mrm_putc (fd, *(buf + i));
  }
  return (nbytes);
}
