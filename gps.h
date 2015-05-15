/**************************************************************
*	GPS and UART functions
*
*	(C) 2010 Eric Freund <eric@tecy.com.au>
*
*	Functions to work on PIC24F and UMC-430 GPS module
*
**************************************************************/

#ifndef GPS_H
#define GPS_H

void gps_comand(unsigned char* );
void gps_chksum(unsigned char, unsigned char* );
void UART2PutChar(char );
char UART2GetChar(void);
void UART2PrintString(char*);
void Init_USART(void);
void Dis_USART(void);

#endif
