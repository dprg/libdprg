/**
 * \file read.c
 * \brief Replacement for newlib read function
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Replaces read() from newlib with a similar function that reads bytes
 * from an input device. Modified to call libmrm mrm_getc(), which blocks
 * using defer(). This code is based on code from libgloss in newlib.
 *
 * <b>History:</b>
 *
 * 09 Oct 2004 dpa - Created
 *
 * 30 Dec 2006 rsr - Normalize indentation
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

 Portions of this code were derived from code in read.c of libgloss in
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

/* extern char _DEFUN_VOID (inbyte); */
extern char _EXFUN (mrm_getc, (int fd));

/**
 * Reads bytes from the serial port. fd = 0 for stdin.  We really only
 * have stdin.  Pass it to mrm_getc() anyway.
 */
int
_DEFUN (read, (fd, buf, nbytes),
	int fd _AND
	char *buf _AND
	int nbytes)
{
	int i = 0;

	for (i = 0; i < nbytes; i++) {
		*(buf + i) = mrm_getc(fd);
	}
	return (i);
}
