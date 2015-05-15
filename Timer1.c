/*
 * File:   Timer1.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for the onboard Timer1 module.
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



/** INCLUDES *******************************************************/

#include "Timer1.h"
#include "USART.h"

//******************************************************************

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    char *ptr;
    if (tic1) tic1--; // Decrement tic1
    if (tic2) tic2--; // Decrement tic2
    if (tic3)
    {
        tic3--;
    }
    else
    {
        if (WasDataRX) //Then it has been 100ms since data arrived. Should be ok to process
        {
            //			print_string(URXbuf);
            ptr = strstr((char*) URXbuf, "OK\r\n");
            if (ptr != NULL) OKflg = ptr - URXbuf + 1;
            else OKflg = 0xFF;
            ptr = strstr((char*) URXbuf, "ERROR\r\n");
            if (ptr != NULL) ERRflg = ptr - URXbuf + 1;
            else ERRflg = 0xFF;
            ptr = strstr((char*) URXbuf, "+CMT:");
            if (ptr != NULL) SMScnt = ptr - URXbuf + 1;
            else SMScnt = 0xFF;
            ptr = strstr((char*) URXbuf, "+CMGS:");
            if (ptr != NULL) SENT = ptr - URXbuf + 1;
            else SENT = 0xFF;

            WasDataRX = 0;
            RXbufCnt = 0;
        }
        if (DataRXflag)
        {
            DataRXflag = 0;
            WasDataRX = 1;
        }
        tic3 = 20;
    }
    IFS0bits.T1IF = 0;
}
//********************************************************************

void initTimer1(void)
{
    T1CON = 0b1000000000110000;
    PR1 = 625; //Should be 10ms int
    _T1IP = 0x07;
    _T1IF = 0;
    _T1IE = 1;
}
//********************************************************************

