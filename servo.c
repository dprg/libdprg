/**
 * \file servo.c
 * \brief TPU related functions for using R/C servos
 * \author R. Steven Rainwater <steve@ncc.com>
 *
 * This is not pretty code at the moment but once I grok the meaning of 
 * the TPU initialization math, I'll clean it up, make it work for any
 * CPU speed, and comment it. It does actually function to the extent 
 * that you can move a servo around with it as-is.
 *
 * History
 * 27 Dec 2004 rsr - Created
 *
 * \todo Make code timing independent of CPU speed
 * \todo Comment on the meaning of timing values
 *
 */

/*
 Copyright (C) 2004-2007 R. Steven Rainwater <steve@ncc.com>

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

#include "tpu.h"

static long tmp_clock = 25165820;
// static long tmp_clock = 16777220;
// static long tmp_clock =  8388688;

    /* TCR1 = Clock speed / 4  --- 16.788MHz / 4 */
    /* TCR1 = clock speed / 4 ---  25.166MHz / 4 */

/**
 * The stored default servo position timings for all initialized TPU channels
 */
volatile int gServo_default[16];

/**
 * Moves a servo by setting the pulse time of the specified TPU channel to
 * the select number of microseconds. The overall pulse cycle time is always
 * kept at exactly 20ms. Note that must do your own range checking to
 * insure that you don't try to position a given servo outside of the range
 * it can safely handle. 
 *
 * @param channel A TPU channel (0-15)
 * @param usec Desired servo position pulse width in microseconds (0 - 3000)
 * A negative value will cause the pulse width to assume the default time
 * specified when this TPU channel was initialized.
 */
void servo_move(short channel, int usec)
{
    struct tpu_qom_ram *tpu_params;    
    if(usec<0) usec = gServo_default[channel];
    int tpm = (int) tmp_clock / 4000;    
    int hi = (int) (usec * tpm) / 1000;
    int lo = (5 * tpm) - hi;
    
    /* TPU_RAM + (channel * 16) = address parameter ram for this channel */
    tpu_params = (struct tpu_qom_ram *)(TPU_RAM + (channel << 4));
    
    tpu_params->offset_1 = (lo << 1 | 1);
    tpu_params->offset_2 = (hi << 1 | 0);
}


/**
 * Initializes the specified TPU channel to output a pulse with an overall
 * cycle time of 20ms. The timing will be adjusted to compensate for the
 * current CPU clock speed.
 *
 * @param channel A TPU channel (0-15)
 * @param usec Desired servo default position pulse width in microseconds
 * (0 - 3000)
 */
void servo_init(short channel, int usec)
{
    /* Set the default pulse width for this channel */
    gServo_default[channel] = usec;
	
    struct tpu_qom_ram *tpu_params;    
    tpu_params = (struct tpu_qom_ram *)(TPU_RAM + (channel << 4));
    
    tpu_set_cpr(channel, 0);    // turn off the TPU channel
    tpu_set_cfsr(channel, 0xE); // set function code for QOM

    /* start TPU immediately */
    tpu_params->ref_addr_b = 0;
    
    /* (offset of first parameter) + number of bytes of used parameters = address of last parameter used */
    /* 0xc = 12  = (5 params + 1) * 2*/
    tpu_params->last_off_addr_a = (channel << 4) | 0xc;

    /* set loop_cnt to zero and write the other zero bit needed to start TPU immediately */
    tpu_params->off_ptr_c = 0;

    /* TCR1 = Clock speed / 4  --- 16.788MHz / 4 = 4.1945MHz */
    /* TCR1 = clock speed / 4 ---  25.166MHz / 4 = 6291500 */
    /* TCR1 counts (4194500/sec) */
    /* 4194.5 ticks / ms */
    /* "normal range"   (120 degree) is 1000 us - 2000 ms center = 1500 us */
    /* "extended range" (180 degree) is  500 us - 2500 us center = 1500 us */

    /* tmp_clock / 4000  = ticks per millisecond */
    int tpm = (int) tmp_clock / 4000;    
    int dt = (int) (tpm * usec) / 1000;

    /* Set up five TPU parameters to create a 20 ms pulse cycle */
    /* 1.  5ms - 0ms lo	*/    
    /* 2.  0ms - 3ms hi	*/
    /* 3.  5ms lo	*/
    /* 4.  5ms lo	*/
    /* 5.  5ms lo	*/
    tpu_params->offset_1 = (((5 * tpm) - dt) << 1 | 1);
    tpu_params->offset_2 = (dt << 1 | 0);
    tpu_params->offset_3 = ((5 * tpm) << 1 | 0);
    tpu_params->offset_4 = ((5 * tpm) << 1 | 0);
    tpu_params->offset_5 = ((5 * tpm) << 1 | 0);
    
    tpu_set_hsqr(channel, 2);
    tpu_set_hsrr(channel, 2);
    tpu_set_cpr(channel, 1);  // turn on the TPU channel to low priority
    
    msleep(500);  // wait for the tpu to warm up...
}
