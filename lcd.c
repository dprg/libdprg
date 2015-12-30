/**
 * \file lcd.c
 * \brief HD44780 LCD driver for Mini-Robominds 68332
 * \author David P. Anderson <dpa@io.isem.smu.edu>
 *
 * Drives the HD44780 LCD display from a queue serviced by the 1000 Hz 
 * interrupt. This gets rid of the waits needed to operate the display.
 * A first-in-first-out circular buffer to send characters to the display.
 *
 * lcd_service() should be run from the 1 kHz system clock interrupt. It 
 * tests each 1ms for FIFO EMPTY, and writes a char to LCD if available.
 *
 * Uses the queue structure and functions from "queue.h"
 *
 * <b>History:</b>
 *
 * 04 Oct 2004 dpa - Created
 *
 * 08 Oct 2004 dpa - Turn OFF lcd_service interrupt  when lcd queue is empty.
 *                   lcd_putc() turns it ON when needed.  Removed lcd_string
 *		     (not needed, just fprintf(stderr,"")).
 *
 * 27 Oct 2004 dpa - lcd_display_init() calls msleep() rather than hard_delay().
 *                   Call this after system startup, so that lcd does not delay 
 *	             other initializations (especially for motors).  Added
 *		     lcd_busy_wait() which calls defer();
 *
 * 27 Apr 2005 rsr - Removed unused version define
 *
 * 30 Dec 2006 rsr - Normalized identation
 *
 * \todo Should lcd_busy_wait() be type void?
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
#include "lcd_queue.h"

#define LCD_ROWS 2
#define LCD_COLUMNS 24
#define LCD_BUF_SIZE 100

/**
 * 64 byte LCD FIFO
 */
char lcd_buf[LCD_BUF_SIZE];

/**
 * LCD FIFO struct
 */
queue_struct lcdq;

/**
 * Number of characters output to LCD
 */
int lcd_count;


/**
 * Tests the LCD Busy line
 *
 * @return 0 = not busy, -1 = busy
 */
int lcd_busy()
{
	if (((*(unsigned char*)LCD_REG) & 0x80) == 0x80)
		return -1;

	return 0;
}


/**
 * Hangs in a defer() loop while LCD is busy
 */
int lcd_busy_wait()
{
	while (lcd_busy()) defer();
}


/**
 * Writes the passed value to the LCD command register.
 *
 * @return -1 if not ready, 0 if successful
 */
int lcd_write_reg(char val) {

	if (lcd_busy())
		return -1;

	*(unsigned char*)LCD_REG = val & 0xff;
	return 0;
}


/**
 * Writes the passed character data to the LCD data register.
 *
 * @return -1 if not ready, 0 if successful
 */
int lcd_write_data(char val) {

	if (lcd_busy())
		return -1;

	*(unsigned char*)LCD_DATA = val & 0xff;
	return 0;
}


/**
 * LCD state vector function pointer
 */
void (*lcd_state)();


/**
 * LCD display state machine service function. This function should be
 * run from the 1 kHz system clock interrupt. It never actually waits 
 * on busy state as LCD is always ready within 1ms (except during the
 * initialization period). When LCD is busy, the character just waits
 * in the FIFO for another 1ms interrupt.
 */
void lcd_service()
{
	if (lcd_state)
		(*lcd_state)();
}


/**
 * Output a character from FIFO to LCD.
 *
 * newline (\\n) = clear screen, home cursor
 *
 * tab (\\t) = set cursor to 2nd line
 *
 * \note Wrap will occur if character count >= LCD_COLUMNS, by
 * setting cursor to start of 2nd row and bumping char count (character
 * will be printed next time).
 */
void lcd_state0()
{
	int byte;
	void extern lcd_state1();

	/* fetch the next byte from the queue */
	byte = qfetch(&lcdq);

	if (byte >= 0) {
		if (byte == 9) {                         /* Is this byte a tab? */
			if (lcd_write_reg(0xC0) == 0) {       /* set cursor to 2nd line */
				qincr_o(&lcdq);                    /* discard byte from fifo */
				lcd_count = LCD_COLUMNS+1;         /* and reset char count */
			}
		} else {
			if (byte == 10) {                     /* Is this byte a newline? */
				lcd_state = lcd_state1;            /* set state=1 for cls & home*/
				qincr_o(&lcdq);                    /* discard byte from fifo */
			} else {
				if (lcd_count == LCD_COLUMNS) {     /* at end of 1st line? */
					if (lcd_write_reg(0xC0) == 0) {  /* set cursor to second line*/
						lcd_count++;                  /* if status ok bump count */
					}
				} else {
					if (lcd_write_data(byte) == 0) { /* send char to lcd */
						qincr_o(&lcdq);               /* if status ok, incr fifo */
						lcd_count++;                  /* character count */
					}
				}
			}
		}
	} else {                                     /* buffer is empty */
		lcd_state = 0;                            /* turn off interrupt */
	}
}


/**
 * Clear screen, stage 1. Initiates cls and sets state to cls state 2.
 */
void lcd_state1() 
{
	void extern lcd_state2();
	int status;
	
	if (qfetch(&lcdq) >= 0) {           /* if we have a char */
		if (lcd_write_reg(0x01) == 0) {  /* initiate clear screen */
			lcd_state = lcd_state2;       /* and set next state if status ok */
		}
	}
}


/**
 * Clear screen, stage 2. Outputs set entry command 6 and sets state to cls
 * state 3.
 */
void lcd_state2()
{
	void extern lcd_state3();

	if (lcd_write_reg(0x06) == 0) {
		lcd_state = lcd_state3;
	}
}


/**
 * Clear screen, stage 3. Homes the cursor and resets state and count.
 */
void lcd_state3() 
{
	if (lcd_write_reg(0x02) == 0) {  /* home cursor */
		lcd_state = lcd_state0;       /* reset state */
		lcd_count = 0;                /* and count */
	}
}


/**
 * Initializes the LCD HD44780 driver chip. Initial state is display on,
 * no cursor, no blink.
 */
int lcd_display_init() {

	int i;

	lcd_write_reg(0x30);   /* function set 8-bit interface */
	msleep(5);             /* wait more than 4.1 ms */

	lcd_write_reg(0x30);   /* repeat function set */
	msleep(1);             /* wait more than 100 usecs */

	lcd_write_reg(0x30);   /* repeat function set one more time */
	msleep(1);             /* wait more than 100 usecs */

	lcd_write_reg(0x38);   /* 8 bit bus with two-line display */
	lcd_busy_wait();

	lcd_write_reg(0x08);   /* turn off display and cursor */
	lcd_busy_wait();

	lcd_write_reg(0x01);   /* clear display */
	lcd_busy_wait();

	lcd_write_reg(0x06);   /* entry mode increment by 1, no shift */
	lcd_busy_wait();

	lcd_write_reg(0x0c);   /* turn on display; no cursor; no blink */
	lcd_busy_wait();

	return 0;
}


/**
 * Global LCD availability flag. 1 = available.
 */
int lcd_available;


/**
 * LCD Initialization task for multi-tasking execution. This function
 * should be called with create_task() after multi-tasking is started
 */
void lcd_init_task(int x)
{
	lcd_display_init();
	lcd_available = 1;
	terminate();
}


/**
 * Initializes the LCD FIFO and creates the LCD hardware init task.
 */
int lcd_init()
{

	/* setup LCD output fifo */
	q_init(&lcdq, &lcd_buf[0], LCD_BUF_SIZE);

	/* create hardware init task for later execution by scheduler */
	create_task(lcd_init_task,0,256);

	return 0;
}


/**
 * Put a character in the LCD queue. LCD interrupt will be enabled
 * if needed.
 *
 * @return 0 if character is successfully queued. -1 if queue is full.
 */
int lcd_putc(char byte)
{
	if (qwrite(&lcdq,byte)) return (-1);

	if ((lcd_state == 0) && (lcd_available)) {
		lcd_state = lcd_state0;  /* turn interrupt on */
	}

	return(0);
}
