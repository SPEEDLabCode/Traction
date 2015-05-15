/*********************************************************************

 ********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   10/07/2012   Initial release

 ********************************************************************/
/** INCLUDES *******************************************************/
#include "HardwareSIM900.h"
#include "OLED.h"
#include "USART.h"
#include "Timer1.h"
#include <stdint.h>
#include <xc.h>
#include "SIM800.h"


_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx3 & FWDTEN_OFF & WINDIS_OFF);
_CONFIG2(PLLDIV_DIV2 & PLL_96MHZ_ON & FNOSC_PRIPLL & IESO_OFF & FCKSM_CSECMD & OSCIOFNC_OFF & DISUVREG_ON & IOL1WAY_OFF & POSCMOD_HS);
_CONFIG3(WPCFG_WPCFGDIS & WPDIS_WPDIS);

/** GLOBAL VARIABLES ******************************************************/
volatile uint8_t URXbuf[255]; //USART RX buffer
volatile uint8_t UTXbuf[255]; //USART TX buffer
volatile uint8_t DataRXflag = 0, WasDataRX = 0;
volatile uint16_t RXbufCnt = 0;
volatile uint16_t tic1 = 0, tic2 = 0, tic3 = 0;
volatile uint8_t  OKflg = 0, ERRflg = 0, SMScnt = 0, SENT = 0;
volatile uint8_t crtX = 0, crtY = 0;

/********************************************************************/
/** PROTOTYPE FUNCTIONS ******************************************************/
void InitializeSystem(void);
unsigned char ScanKey(void);

/********************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _DefaultInterrupt(void)
{
    Nop();
    Nop(); //Place 2nd breakpoint here
    //asm volatile("RESET");	//Soft reset
    Nop();
    Nop();
    Nop();
    while (1); //{Sleep()}; //Trap
}

/********************************************************************/
int main(void)
{
    //int i = 0, j, k, zz = 99;
    unsigned char buf[5] = "ATE0";
    unsigned char buf1[10] = "AT+CMGF=1"; // Set text entry mode
    unsigned char buf2[23] = "AT+CMGS= 0433884656 "; // Number to send SMS to. note quotes are added manually
    //	unsigned char buf2[23]="AT+CMGS= 0403326236 ";
    buf2[8] = 0x22; // We need to write a function that takes ph# and pointer to string
    buf2[19] = 0x22; // Then constructs the command set and sends it.
    unsigned char buf3[23] = "This is a test of SMS "; // Text for test SMS
    //	unsigned char buf3[24]="HEY LOVE YOU HEAPS XXX ";
    buf3[22] = 0x1A;
    unsigned char buf4[18] = "AT+CNMI=1,2,2,0,1"; //Display all incoming SMS

    //	unsigned int avgcnt =0;

    InitializeSystem();
    crtX = 0;
    crtY = 0;
    Gotoxy(0, 0);
    //	for (i = 32; i< (96+32);i++) printSCN (i);
    print_string_XY("system boot!", 0, 0);
    tic2 = 100;
    while (1)
    {
        //		Gotoxy(0,0);
        //		for (j=0;j<30000;j++)for (i=0;i<20;i++);
        //		PWRON = 1;
        DataRXflag = 0;
        //		for (j=0;j<30000;j++)for (i=0;i<20;i++);

        while (1)
        {
            //			if(DataRXflag)// || ((RXbufCnt)&&(!tic1)))
            //			{
            //				print_string_XY(URXbuf,0,0);
            //				RXbufCnt = 0;
            //				for (i=0;i<255;i++) URXbuf[i] = 0;
            //				DataRXflag = 0;
            //				tic1 = 100;
            //			}
            if (ScanKey() == 0x44)
            {
                //				ClearOLED();
                //				USARTTXstg(buf1);
                ATcommand("+CMGF=1");
                while (ScanKey() == 0x44);
            }
            if (ScanKey() == 0x43)
            {
                //				ClearOLED();
                //				USARTTXstg(buf2);
                SMSnumber("+61433884656");
                while (ScanKey() == 0x43);
            }
            if (ScanKey() == 0x42)
            {
                //				ClearOLED();
                //				USARTTXstg(buf3);
                SMStext("Ok this is a new test string");
                while (ScanKey() == 0x42);
            }
            if (ScanKey() == 0x41)
            {
                //				ClearOLED();
                //				USARTTXstg(buf4);
                ATcommand("+CNMI=1,2,2,0,1");
                while (ScanKey() == 0x41);
            }
            if (ScanKey() == 0x33)
            {
                ClearOLED();
                Gotoxy(0, 0);
                crtX = 0;
                crtY = 0;
                while (ScanKey() == 0x33);
            }
            if (ScanKey() == 0x2A)
            {
                PWRON = 1;
                tic2 = 100;
                while (tic2);
                PWRON = 0;

                while (ScanKey() == 0x2A);
            }
            //			if(!tic2)
            //			{
            //				print_big_num(zz/10, 0, 5);
            //				print_big_num(zz%10, 14, 5);
            //				zz--;
            //				if( zz<0)zz = 99;
            //				tic2 =100;
            //			}
        }
    }
}

/********************************************************************/
void InitializeSystem(void)
{
    TRISB = 0b1111111111101110;
    TRISC = 0xFFFF;
    TRISD = 0b1111111100000001;
    TRISE = 0b1111111100000000;
    TRISF = 0b1111111111111100;
    TRISG = 0xFEFF;
    LATB = 0;
    LATC = 0;
    LATD = 0b0000000100000000;
    LATE = 0;
    LATF = 0b0000000000111000;
    LATG = 0x0100;

    _CN12PDE = 1; //Enable week pulldowns for switches
    _CN30PDE = 1;
    _CN31PDE = 1;
    _CN32PDE = 1;

    _CLKLOCK = 0;

    SRbits.IPL = 0x00;
    CORCONbits.IPL3 = 0;

    RCONbits.VREGS = 0; /* Disable regulator in sleep mode */
    AD1PCFGL = 0xFFFF;
    AD1PCFGH = 0xFFFF;

    //	__builtin_write_OSCCONH(0x07);				//8MHz clock
    __builtin_write_OSCCONL(0x03);
    //	CLKDIVbits.CPDIV = 2;
    initOLED();

    //	RPINR18bits.U1RXR = 27;	//USART RX on RP29
    _U1RXR = 27;
    RPOR9bits.RP19R = 3; //USART TX on RP27
    initUSART();
    initTimmer1();

}//end InitializeSystem

/********************************************************************
This routine scans the keyboard and returns a value showing key pressed
 ********************************************************************/
unsigned char ScanKey(void)
{
    unsigned int pressed = 0;
    unsigned char i = 0;
    //	unsigned char button[17] = {0x33,0x36,0x39,0x23,0x41,0x42,0x43,0x44,0x32,0x35,0x38,0x30,0x31,0x34,0x37,0x2A,0x00}; //key mapping H
    unsigned char button[17] = {0x43, 0x39, 0x38, 0x37, 0x44, 0x23, 0x30, 0x2A, 0x42, 0x36, 0x35, 0x34, 0x41, 0x33, 0x32, 0x31, 0x20}; //key mapping V

    TRISF = 0b1111111111000100;
    _TRISD8 = 0;
    _LATF5 = 1;
    Nop();
    pressed = (PORTB & 0xF000);
    _LATF5 = 0;
    _LATF4 = 1;
    Nop();
    pressed += ((PORTB >> 4)&0x0F00);
    _LATF4 = 0;
    _LATF3 = 1;
    Nop();
    pressed += ((PORTB >> 8)&0x00F0);
    _LATF3 = 0;
    _LATD8 = 1;
    Nop();
    pressed += ((PORTB >> 12)&0x000F);
    _LATD8 = 0;
    while (!(pressed & 0x8000) && (i < 16))
    {
        i++;
        pressed = pressed << 1;
    }

    return button[i];
}//end ScanKey
/********************************************************************/
