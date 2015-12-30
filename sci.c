/**
 * \file sci.c
 * \brief Serial Communications Interface support
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Provide Serial Communications Interface (SCI) support. Buffered serial
 * I/O using the QSM UART.
 *
 * <b>History:</b>
 *
 * 07 Oct 2004 dpa - Created
 *
 * 30 Dec 2006 rsr - Normalized indentation
 *
 * \todo inbyte() needs better description
 * \todo outbyte() needs better description
 * \todo interrupt handler routine needs better description
 * \todo replace cryptic address=value statements with descriptive defines
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
#include "queue.h"

/**
 * Size of SCI transmit and receive buffers
 */
#define SCI_BUF_SIZE 128

/**
 * SCI transmit FIFO buffer
 */
char tx_buf[SCI_BUF_SIZE];

/**
 * SCI receive FIFO buffer
 */
char rx_buf[SCI_BUF_SIZE];


/**
 * queue struct thingies? need better desc
 */
queue_struct txq, rxq;


/**
 * SCI status from QSM_SCSR status register
 */
int sci_status;


/**
 * SCI data value from the QSM_SCDR UART register
 */
int sci_data;


/**
 * SCI interrupt service.
 * QSM_SCSR status register
 * & 0x0040 for receive interrupt (RDRF)
 * & 0x0100 for transmit interrupt (TDRE)
 */
__attribute__((interrupt_handler)) void sci_int(void) 
{
	sci_status = QSM_SCSR;

	if (sci_status & 0x0040) {	/* RDRF */
		sci_data = QSM_SCDR;	/* read incoming byte from UART */

		if (qwrite(&rxq,sci_data) != 0) {  /* and write to fifo */
			qread(&rxq);		/* discard oldest if no room */
			qwrite(&rxq,sci_data);	/* and try again */
		}
	}

	if (sci_status & 0x0100) {	/* TDRE */
		sci_data = qread(&txq);
		if (sci_data >= 0) {
			QSM_SCDR = sci_data;
		} else {
			QSM_SCCR1 = 0x002c;	/* TDRE interrupt off */
		}
	}
}
		

/**
 * User assigned exception vector for SCI interrupts
 * this may need to go in sci.h
 */
#define SCIVEC 0x56


/**
 * Initializes the QSM SCI interface and FIFOs
 */
void sci_init()
{
	extern int vbraddr;

	q_init(&txq, &tx_buf[0], SCI_BUF_SIZE);
	q_init(&rxq, &rx_buf[0], SCI_BUF_SIZE);

	*(long*)(vbraddr+(SCIVEC+0)*4) = (long)sci_int;	/* SCI interrupt vector */
	*(long*)(vbraddr+(SCIVEC+1)*4) = (long)sci_int;	/* why this one also? */

	QSM_MCR = 0x0087;			/* QSM configuration register */
						/* 0x0080 is supervisor bit */
						/* low 4 bits are arbitration # (7) */

	QSM_QILR_QIVR = SCIVEC + 0x0600;	/* low 8 bits are vec #, hi 3 are priority */
						/* so vec# 0x56, priority 6 */


	QSM_SCCR0 = 41; /* 27 = 19200 baud at 16.778 MHz clock. */
                   /* 55 = 9600, 109 = 4800 */

	QSM_SCCR1 = 0x002c;	/* Trans int ena(80) +Rcv int ena(20) +Tran ena (08) +Rcv ena (04) */
				/* start with trans int enable off (0x80) */
}


/**
 * Send one byte via the SCI UART. Complement to lcd_putc() from
 * lcd_queue.c
 *
 * @param byte The byte to be sent
 * @return 0 on success or -1 on failure
 */
int sci_putc(char byte)
{
	if (qwrite(&txq,byte)) return (-1);

	QSM_SCCR1 = 0x00ac;	/* TDRE interrupt on */

	return(0);
}


/**
 * Read one byte from SCI UART receive buffer 
 *
 * @return Next char or -1 if buffer empty
 */
int sci_getc()
{
	return (qread(&rxq));
}


/**
 * hook for libdprg.a debug() I/O
 */
void outbyte(char byte)
{
	qwrite(&txq,byte);	/* write byte to fifo */
	QSM_SCCR1 = 0x00ac;	/* TDRE interrupt on */
}


/**
 * hook for libmrm.a debug() I/O
 */
char inbyte(void)
{
	return (sci_getc());
}
