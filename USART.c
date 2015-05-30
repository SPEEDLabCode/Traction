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
 * uart.h
 *
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
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

/* Include Global Parameters */
#include <xc.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <uart.h>
#include "USART.h"

/* INTERRUPT SERVICE ROUTINES */
/* NOTE: These ISR's are performing application specific functions, they 
 are not perfectly abstract.*/

/********************************************************************/

/* USART 2 Interrupts as per Microchip PIC24F peripheral function documentation */
void __attribute__ ((interrupt,no_auto_psv)) _U2TXInterrupt(void)
{
   static uint16_t i=0;
   U2TX_Clear_Intr_Status_Bit;
   if(Txdata[i]!='\0')
    {
      while(BusyUART2());
      WriteUART2((unsigned int)Txdata[i++]);
    }
   else
    {
      DisableIntU2TX;
      DataAvailable = 1;
    }
}

void __attribute__ ((interrupt,no_auto_psv)) _U2RXInterrupt(void)
{
   static uint16_t j=0;
   U2RX_Clear_Intr_Status_Bit;
   while(!DataRdyUART2());
   Rxdata[j++] = ReadUART2();
   if(j == 10)
      DataAvailable=1;
}

/* USART 1 interrupts as written */
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    uint8_t temp;
    while (U1STAbits.URXDA)
    {
        temp = U1RXREG;
        printSCN(temp);
        URXbuf[RXbufCnt++] = temp;
        if (RXbufCnt == 255)
        {
            RXbufCnt = 0;
        }
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

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{
    volatile uint8_t temp;
    extern uint16_t sizeG;
    extern char dataG[];
    extern uint8_t valid_data;

    IFS1bits.U2RXIF = 0; //Clear Interrupt flag
    temp = 0;
    while ((U2STAbits.URXDA) && (temp != 0x0A))
    {
        temp = U2RXREG;
        if (temp == 36)
        {
            sizeG = 0; //Wait for first $ of next NMEA data packet
        }
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

//********************************************************************

void initUSART(void)//TODO re-code so that both USART in use are configured
{
    //UxBRG = ((FCY/Desired Baud Rate)/16) – 1
    //U1BRG 	= 25;//((4000000/9600)/16) – 1;
    /* Configure UART1 module to transmit 8 bit data with one stopbit. */
    U1MODEvalue = UART_EN & UART_IDLE_CON & 
                  UART_DIS_WAKE & 
                  UART_EN_ABAUD & UART_NO_PAR_8BIT  &
                  UART_1STOPBIT;
    U1STAvalue  = UART_INT_TX_BUF_EMPTY  &  
                  UART_TX_PIN_NORMAL &
                  UART_TX_ENABLE & UART_INT_RX_3_4_FUL &
                  UART_ADR_DETECT_DIS &
                  UART_RX_OVERRUN_CLEAR;
    OpenUART1(U1MODEvalue, U1STAvalue, baudvalue);
    
    /* Configure UART2 module to transmit 8 bit data with one stopbit.  */
    U2BRG = 69;
    U2MODE = 0b1000000000001000;
    U2STAbits.UTXEN = 1;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 1;
    
}
//********************************************************************

void USARTTX(uint8_t number, uint8_t DATA)
{
    if (number > 0 && number <= USART_TRANSCIEVERS)
    {
        if (number == 1)
        {
    while (U1STAbits.UTXBF);
    U1TXREG = DATA;
        }
        if (number == 2)
        {
            while(U2STAbits.UTXBF);
            U2TXREG = DATA;
        }
    }
    
}
//********************************************************************

void USARTTXstg(uint8_t *DATA)//TODO re-code to take USART number argument
{
    //FIXME validate input!

    while (*DATA)
    {
        USARTTX(*DATA++);
    }
    USARTTX(0x0D); //<cr>
}

/**************************************************************/
void USART_Disable(uint8_t number)  //DEPRECATED
/* NOTE: Deprecated.  Use CloseUARTx() XC compiler built-in instead.*/
{
    //validate input
    if(number > 0 && number <= USART_TRANSCIEVERS)
    {
        if(number == 1)
        {
            CloseUART1();
        }
        if(number == 2)
        {
            CloseUART2();
        }
    }
    
}

/**********************************************/


char UART2GetChar(void)
{
    char Temp; //FIXME change variable names as this is wrong
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

    if (*str != NULL)
    {
        //TODO check length of string and convert while to for loop
        while (*str)
            UART2PutChar(*str++);
    }
}
/******************************************************************************************/