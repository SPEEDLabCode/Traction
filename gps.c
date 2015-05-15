/**************************************************************
*	GPS and UART functions
*
*	(C) 2010 Eric Freund <eric@tecy.com.au>
*
*	Functions to work on PIC24F and UMC-430 GPS module
*
**************************************************************/

#include "gps.h"
#include "P24FJ256GB106.h"
#include "HardwareProfile - UAVrevA.h"
/***************************************************************************************/
void Init_USART(void)
{
	_TRISB8	= 1;
	U2MODE =	0x8010;
	U2STA =		0x0400;	//TX pin enabled
	U2BRG = 	207;	//4800 Baud
	IFS1bits.U2RXIF = 0;
	IEC1bits.U2RXIE = 1;
}
/***************************************************************************************/
void Dis_USART(void)
{
	U2MODE =	0x0000;
}
/**************************************************************/
void gps_comand(unsigned char* GPS_CMD)
{
	unsigned long i;
	unsigned char check_sum = 0;
	unsigned char LastRS232Out;
	unsigned char *CMD;

	CMD = GPS_CMD;
	GPS_CMD++;
	for (i =1; i<20; i++){
	check_sum = check_sum ^ *GPS_CMD++;
	}
	*(GPS_CMD++) = 42;
	*(GPS_CMD++) = (((check_sum>>4) & 0x0F)+(((check_sum>>4)&0x0F)/0x0A)*0x07) + 0x30;
	*(GPS_CMD++) = ((check_sum & 0x0F)+((check_sum&0x0F)/0x0A)*0x07) + 0x30;
	*(GPS_CMD++) = 13;
	*(GPS_CMD++) = 10;
	LastRS232Out = 25;
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
			Nop();
	for (i=0; i < LastRS232Out; i++)
	{
		while (!IFS1bits.U2TXIF);
		UART2PutChar(*(CMD++));
	}
}
/*******************************************************************************
Function: UART2GetChar()

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine waits for a byte to be received.  It then returns that byte.

Input: None.

Output: Byte received.

*******************************************************************************/
char UART2GetChar(void)
{
    char Temp;
	extern int T1cnt;
	T1cnt = 10;
	if ( U2STAbits.OERR ) U2STAbits.OERR = 0;
    while((U2STAbits.URXDA == 0) && T1cnt);
    Temp = U2RXREG;
//    IFS1bits.U2RXIF = 0;
    return Temp;
}
/*******************************************************************************/
void UART2PutChar( char ch )
{
    U2TXREG = ch;
    while(U2STAbits.TRMT == 0);
}
/*******************************************************************************/
void UART2PrintString( char *str )
{
    while(*str)
        UART2PutChar(*str++);
}
/******************************************************************************************/
void gps_chksum(unsigned char n , unsigned char* GPS_CMD)
// string always is 6 longer after this process
{
	unsigned long i;
	unsigned char check_sum = 0;

	GPS_CMD++;
	for (i =1; i<n; i++){
	check_sum = check_sum ^ *GPS_CMD++;
	}
	*(GPS_CMD++) = 42;
	*(GPS_CMD++) = (((check_sum>>4) & 0x0F)+(((check_sum>>4)&0x0F)/0x0A)*0x07) + 0x30;
	*(GPS_CMD++) = ((check_sum & 0x0F)+((check_sum&0x0F)/0x0A)*0x07) + 0x30;
	*(GPS_CMD++) = 13;
	*(GPS_CMD++) = 10;
	*(GPS_CMD++) = 0;
}
/**********************************************/
void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt(void)
{
	volatile unsigned char temp;
	extern unsigned int sizeG;
	extern char dataG[];
	extern unsigned char valid_data;
	extern unsigned int GPSto;

//	REDBL = ~REDBL;
	GPSto = 0;
    IFS1bits.U2RXIF = 0;
	temp = 0;
	while ((U2STAbits.URXDA) && (temp != 0x0A)){
    temp = U2RXREG;
	if(temp == 36) sizeG = 0;		//Wait for first $ of next NMEA data packet
	dataG[sizeG++] = temp;			//Always align received string to first "$" 
	}
	if((temp == 0x0A) && (sizeG > 2)) {
		valid_data = 2;				//As the minimum number of chars is "$ <CR><LF>" flag indicates we have an NMEA string
		dataG[sizeG++] = 0;			//Put a NULL on end of string so it can be handled with normal string routines
		}
	if(sizeG >= 255){				//It got to long with out a CR/LF so it must be crap start from the beginning of the buffer.
		sizeG = 0;
		valid_data = 0;				//Set flag to zero nothing useful in the buffer
		dataG[0] = 0;
		}
	if(U2STAbits.OERR) {
		U2STAbits.OERR = 0;
		valid_data = 0;
		dataG[0] = 0;
	}	
		dataG[255] = 0;
//strncpy(dataG,"$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,*10",68);
//dataG[68] = 13;
//dataG[69] = 10;
//sizeG = 70;
//valid_data = 2;
}
/******************************************************************************************/
