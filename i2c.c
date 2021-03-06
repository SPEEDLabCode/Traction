
/*
 * File:   i2c.c.c
 * Author:
 * E. Freund <eric@affinityengineering.com.au
 * W. Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for the built-in I2C transciever.
 *
 * Language: C (CCI)
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependancies:
 * SIM800.h
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 *
 * Created on 16 April 2015, 11:43 PM
 * Copyright (C) 2015  Affinity Engineering pty ltd

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <xc.h>
#include "i2c.h"

void Init_I2C1(void)
{
_TRISD9 = 1;
_TRISD10 = 1;
  I2C1BRG = 0x9C;				//sets Fscl ~100khz or lower
  I2C1STAT = 0;
  I2C1CONbits.A10M = 0;
  I2C1CONbits.I2CEN = 1;
  _MI2C1IF = 0;
  _MI2C1IE = 1;
}

void DisI2C1(void)
{
    I2C1CONbits.I2CEN = 0;
  	_MI2C1IE = 0;
}

/** Send an I2C START condition.
    \note Do not use this for a repeated start, use i2c_rstart() instead. */
void i2c_start1()
{
  I2C1CONbits.SEN = 1;
  /* FIXME why is this commented out */
  //if((I2C1STATbits.S == 0) && (I2C1STATbits.P == 1)){
  /* wait until START is complete */
  while (I2C1CONbits.SEN == 1);
//    Idle();
  // }
}

/** Send an I2C repeated START condition. */
void i2c_rstart1()
{
  I2C1CONbits.RSEN = 1;
  /* wait until repeated START is complete */
  while (I2C1CONbits.RSEN == 1);
//    Idle();
}

/** Send an I2C STOP condition. */
void i2c_stop1()
{
  I2C1CONbits.PEN = 1;
  /* wait until STOP is complete */
  while (I2C1CONbits.PEN == 1);
//    Idle();
}

/** Send a byte on the I2C bus. 
    \param data The byte to send. */
unsigned char i2c_sbyte1(unsigned char data)
{
  /* put data in transmit buffer */
  I2C1TRN = data;

  /* wait until transmit + ack timeslots have been used */
  while (I2C1STATbits.TRSTAT == 1);
//    Idle();

  /* return 0 if slave doesn't ack */
  return !I2C1STATbits.ACKSTAT;
}

/** Receive a byte from the I2C bus.
    \param data The location to store the received byte.
    \param ack_or_nack See enum i2c_ack. */
unsigned char i2c_rbyte1(enum i2c_ack ack_or_nack)
{
	char data;
	  /* enable receive */
	  I2C1CONbits.RCEN = 1;
	
	  /* wait for receive to finish */
	  while (I2C1CONbits.RCEN == 1);
//	    Idle();
	
	  /* save data */
	  data = I2C1RCV;
	
	  /* send ack or nack */
	  I2C1CONbits.ACKDT = ack_or_nack;
	  I2C1CONbits.ACKEN = 1;
	  
	  /* wait for ack/nack to be sent */
	  while (I2C1CONbits.ACKEN == 1);
//	  	      Idle();
  return data;
}

/** Interrupt handler for master I2C, only used to get us out of Idle. */
void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
//	I2C1STATbits.BCL = 0;
  	_MI2C1IF = 0;
}

