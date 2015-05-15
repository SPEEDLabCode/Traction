/*********************************************************************

 File Description:

 Change History:
  Rev   Date         Description
  1.0   2/03/2014   Initial release

********************************************************************/


/** INCLUDES *******************************************************/

#include "Timmer1.h"
#include "USART.h"

extern volatile unsigned int tic1,tic2,tic3;
extern volatile unsigned int RXbufCnt;
extern volatile unsigned char DataRXflag, WasDataRX, OKflg, ERRflg, SMScnt, SENT;
extern volatile char URXbuf[255];	//USART RX buffer
extern volatile char UTXbuf[255];	//USART TX buffer
//******************************************************************
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
{
	char *ptr;
	if(tic1) tic1--;	// Decrement tic1
	if(tic2) tic2--;	// Decrement tic2
	if(tic3)
	{
		 tic3--;
	}else{
		if (WasDataRX) //Then it has been 100ms since data arrived. Should be ok to process
		{
//			print_string(URXbuf);
			ptr = strstr((char*)URXbuf,"OK\r\n");
			if( ptr != NULL) OKflg = ptr - URXbuf +1; else OKflg = 0xFF;
			ptr = strstr((char*)URXbuf,"ERROR\r\n");
			if (ptr != NULL) ERRflg = ptr - URXbuf +1; else ERRflg = 0xFF;
			ptr = strstr((char*)URXbuf,"+CMT:");		
			if (ptr != NULL) SMScnt = ptr - URXbuf +1; else SMScnt = 0xFF;
			ptr = strstr((char*)URXbuf,"+CMGS:");		
			if (ptr != NULL) SENT = ptr - URXbuf +1; else SENT = 0xFF;

			WasDataRX = 0;
			RXbufCnt = 0;
		}
		if(DataRXflag) {DataRXflag = 0; WasDataRX = 1;}
		tic3 = 20;
	}
	IFS0bits.T1IF = 0;
}
//********************************************************************
void initTimmer1(void)
{
	T1CON 	= 0b1000000000110000;
	PR1		= 625;	//Should be 10ms int
	_T1IP = 0x07;
	_T1IF = 0;	
	_T1IE = 1;
}
//********************************************************************

