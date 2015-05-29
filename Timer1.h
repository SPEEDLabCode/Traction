/*
 * File:   USART.h
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for the onboard Timer1 module.
 *
 * Language: C (CCI)
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependancies:
 * xc.h
 * stddef.h
 * stdbool.h
 * stdint.h
 *
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 * 
 * Reference Document: PIC24FJ256GB110 Family Data Sheet
 * Document Number: DS39897C
 * Revision / Date: C / 16 DEC 2009
 *
 * Created on 02 March 2014, 11:23 AM
 * Copyright (C) 2014-2015  Affinity Engineering pty. ltd.
 * <http://www.affinityengineering.com.au>
 * <http://github.com/SPEEDLabCode>

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

#ifndef TMR1_H
#define TMR1_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <string.h>

    /*********************************************************************
    Defines
     ********************************************************************/

    /*********************************************************************
    Globals
     ********************************************************************/
    extern volatile uint16_t tic1, tic2, tic3;
    extern volatile uint16_t RXbufCnt;
    extern volatile uint8_t DataRXflag, WasDataRX, OKflg, ERRflg, SMScnt, SENT;
    extern volatile char URXbuf[255]; //USART RX buffer
    extern volatile char UTXbuf[255]; //USART TX buffer

    /*********************************************************************
    Function prototypes
     ********************************************************************/
    void initTimer1(void);

#ifdef	__cplusplus
}
#endif

#endif /* TMR1_H */
