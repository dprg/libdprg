/**
 * \file queue.c
 * \brief FIFO implementation to support serial and LCD I/O
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Provides low-level FIFO functionality for use by the serial and LCD
 * input and output functions.
 *
 * <b>History:</b>
 *
 * 04 Oct 2004 dpa - Created
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

#include "queue.h"


/**
 * Initalizes a FIFO queue structure
 *
 * @param q Pointer to the queue_struct to initialize
 * @param buffer Pointer to the FIFO buffer
 * @param size Buffer size n bytes
 * @return Always returns 0
 */
int q_init(queue_struct *q, unsigned char *buffer, int size) 
{
	q->buf = buffer;
	q->end = buffer + size;
	q->size = size;
	q->in = q->out = q->buf;
	return 0;
}


/**
 * Increments the queue output pointer, wrapping if needed
 *
 * @param q Pointer to the queue_struct to be incremented
 * @return New position of queue output pointer
 */
int qincr_o(queue_struct *q)
{
	q->out++; if (q->out >= q->end) q->out = q->buf;
	return (int)q->out;
}


/**
 * Increments the queue input pointer, wrapping if needed
 *
 * @param q Pointer to the queue_struct to be incremented
 * @return New position of queue input pointer
 */
int qincr_i(queue_struct *q)
{
	q->in++; if (q->in >= q->end) q->in = q->buf;
	return (int)q->in;
}


/**
 * Fetches the next byte from a FIFO without incrementing the output pointer
 * 
 * @param q Pointer to the queue_struct to be read
 * @return The byte read or -1 if FIFO is empty
 */
int qfetch(queue_struct *q)
{
	int byte;
	if (q->in != q->out) {
		byte = *q->out;
	} else {
		byte = -1;
	}
	return byte;
}


/**
 * Reads a byte from a FIFO and increments the pointer
 *
 * @param q Pointer to the queue_struct to be read and incremented
 * @return The byte read or -1 if FIFO is empty
 */
int qread(queue_struct *q)
{
	int byte;

	byte = qfetch(q);
	if (byte != -1) qincr_o(q);
	return byte;
}


/**
 * Writes a byte to a FIFO
 * 
 * @param q Pointer to the queue_struct to be written to
 * @return 0 if byte is successfully written or -1 if FIFO is full
 */
int qwrite(queue_struct *q, unsigned char byte)
{
	unsigned char *p;

	p = q->in;
	p++; if (p >= q->end) p = q->buf;

	if (p == q->out) {	
		return -1;
	} else {
		*q->in = byte;
		q->in = p;
		return 0;
	}
}


/**
 * Check status of a FIFO
 * 
 * @param q Pointer to a queue_struct
 * @return The number of characters in the queue
 */
int qstatus(queue_struct *q)
{
	int s;

	s = q->in - q->out;
	if (q->in < q->out) s += q->size;

	return s;
}

