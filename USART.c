/*
 * File:   USART.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for onboard USART transciever functionality.
 *
 * Language: C
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependancies:
 * USART.h
 * xc.h
 * stddef.h
 * stdbool.h
 * stdint.h
 *
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 *
 * Created on 02 March 2014, 11:23 AM
 * Copyright (C) 2014-2015  Affinity Engineering pty ltd

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

/* Include Global Parameters */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "USART.h"


/********************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    unsigned char temp;
    while (U1STAbits.URXDA)
    {
        temp = U1RXREG;
        printSCN(temp);
        URXbuf[RXbufCnt++] = temp;
        if (RXbufCnt == 255) RXbufCnt = 0;
        //		if ((URXbuf[RXbufCnt-1] == 0x0A)&&(URXbuf[RXbufCnt-2] == 0x0D))
        //		{
        //		 	DataRXflag = 1;
        //			URXbuf[RXbufCnt-2] = 0;
        //			RXbufCnt -= 2;
        //		}
        DataRXflag = 1;
    }
    if (U1STAbits.OERR)
    {
        RXbufCnt = 0;
        URXbuf[0] = 0;
        U1STAbits.OERR = 0;
    }
    IFS0bits.U1RXIF = 0;
}

//********************************************************************

void initUSART(void)
{
    //UxBRG = ((FCY/Desired Baud Rate)/16) – 1
    //U1BRG 	= 25;//((4000000/9600)/16) – 1;
    U1BRG = 69;
    U1MODE = 0b1000000000001000;
    U1STAbits.UTXEN = 1;
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
}
//********************************************************************

void USARTTX(unsigned char DATA)
{
    //TODO change to a for loop over the length of the data
    while (U1STAbits.UTXBF);
    U1TXREG = DATA;
}
//********************************************************************

void USARTTXstg(unsigned char *DATA)
{
    //FIXME validate input!
    while (*DATA)
    {
        USARTTX(*DATA++);
    }
    USARTTX(0x0D); //<cr>
}

//********************************************************************

void Dis_USART(void)    //FIXME change name to Disable_UART
{
    U2MODE = 0x0000;
}
/**************************************************************/

/**********************************************/
void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    volatile unsigned char temp;
    extern unsigned int sizeG;
    extern char dataG[];
    extern unsigned char valid_data;
    //extern unsigned int GPSto;

    //GPSto = 0;
    IFS1bits.U2RXIF = 0;
    temp = 0;
    while ((U2STAbits.URXDA) && (temp != 0x0A))
    {
        temp = U2RXREG;
        if (temp == 36) sizeG = 0; //Wait for first $ of next NMEA data packet
        dataG[sizeG++] = temp; //Always align received string to first "$"
    }
    if ((temp == 0x0A) && (sizeG > 2))
    {
        valid_data = 2; //As the minimum number of chars is "$ <CR><LF>" flag indicates we have an NMEA string
        dataG[sizeG++] = 0; //Put a NULL on end of string so it can be handled with normal string routines
    }
    if (sizeG >= 255)
    { //It got to long with out a CR/LF so it must be crap start from the beginning of the buffer.
        sizeG = 0;
        valid_data = 0; //Set flag to zero nothing useful in the buffer
        dataG[0] = 0;
    }
    if (U2STAbits.OERR)
    {
        U2STAbits.OERR = 0;
        valid_data = 0;
        dataG[0] = 0;
    }
    dataG[255] = 0;
    
}

/******************************************************************************************/

char UART2GetChar(void)
{
    char Temp;  //FIXME change variable names as this is wrong
    extern int T1cnt;
    T1cnt = 10;
    if (U2STAbits.OERR) U2STAbits.OERR = 0;
    while ((U2STAbits.URXDA == 0) && T1cnt);
    Temp = U2RXREG;
    //    IFS1bits.U2RXIF = 0;
    return Temp;
}

/*******************************************************************************/
void UART2PutChar(char ch)
{
    //TODO validate input
    U2TXREG = ch;
    while (U2STAbits.TRMT == 0);
}

/*******************************************************************************/
void UART2PrintString(char *str)
{

    if(*str != NULL)
    {
        //TODO check length of string and convert while to for loop
    while (*str)
        UART2PutChar(*str++);
    }
}
/******************************************************************************************/