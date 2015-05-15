/*
 * File:   i2c.h
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
 * 
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


#ifndef I2C_H
#define I2C_H
/*********************************************************************
Defines
********************************************************************/
/** The \p ack_or_nack argument for i2c_byte().
    The values of this enum currently match the value we pass to ACKDT. */
enum i2c_ack {
  /** Don't acknowledge. */
  I2C_NACK = 1,
  /** Do acknowledge. */
  I2C_ACK = 0
  };
/*********************************************************************
Globals
********************************************************************/

/*********************************************************************
Function prototypes
********************************************************************/

void Init_I2C1(void);
void DisI2C1(void);
void i2c_start1(void);
void i2c_rstart1(void);
void i2c_stop1(void);
unsigned char  i2c_sbyte1(unsigned char);
unsigned char i2c_rbyte1(enum i2c_ack);

void Init_I2C2(void);
void DisI2C2(void);
void i2c_start2(void);
void i2c_rstart2(void);
void i2c_stop2(void);
unsigned char  i2c_sbyte2(unsigned char);
unsigned char i2c_rbyte2(enum i2c_ack);

void Init_I2C3(void);
void DisI2C3(void);
void i2c_start3(void);
void i2c_rstart3(void);
void i2c_stop3(void);
unsigned char  i2c_sbyte3(unsigned char);
unsigned char i2c_rbyte3(enum i2c_ack);


#endif


