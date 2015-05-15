/*
 * File:   USART.h
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for onboard USART transciever functionality.
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


#ifndef USART_H
#define USART_H
#ifdef	__cplusplus
extern "C" {
#endif

/*********************************************************************
Defines
********************************************************************/

/*********************************************************************
Globals
********************************************************************/
extern volatile unsigned char DataRXflag;
extern volatile char URXbuf[];
extern volatile unsigned int RXbufCnt;
extern volatile unsigned char DataRXflag, WasDataRX, OKflg, ERRflg, SMScnt, SENT;
extern volatile unsigned int tic1, tic2, tic3;
/*********************************************************************
Function prototypes
********************************************************************/
void initUSART();
void USARTTX(unsigned char);
void USARTTXstg(unsigned char*);
void USARTRX(unsigned char*);
void Dis_USART(void); 
void UART2PutChar(char); 
char UART2GetChar(void);
void UART2PrintString(char*);

//********************************************************************

#ifdef	__cplusplus
}
#endif

#endif /* USART_H */
