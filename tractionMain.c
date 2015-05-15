/*
 * File:   tractionMain.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the Main file for the Traction device.
 *
 *
 * Language: C
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
 * Created on 10 July 2012, 11:23 AM
 * Copyright (C) 2012-2015  Affinity Engineering pty ltd

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

/*********************************************************************

/** INCLUDES *******************************************************/
#include <xc.h>
#include <stddef.h>

//#include "usb_config.h"

//#include "./USB/usb_device.h"
//#include "./USB/usb.h"
//#include "./USB/usb_function_cdc.h"
//#include "globals.h"
#include "i2c.h"
#include "Timer1.h"
#include "OLED.h"
#include "USART.h"
//#include "USART2.h"
#include "CAM8MQ.h"
#include "SIM800.h"
#include "HardwareSIM900.h"


	_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx3 & FWDTEN_OFF & WINDIS_OFF);
	_CONFIG2( PLLDIV_DIV4 & PLL_96MHZ_ON & FNOSC_PRIPLL & IESO_ON & FCKSM_CSECMD & OSCIOFNC_ON & DISUVREG_ON & IOL1WAY_OFF & POSCMOD_HS);
	_CONFIG3(WPCFG_WPCFGDIS & WPDIS_WPDIS);

/** GLOBAL VARIABLES ******************************************************/
uint8_t timer1_flag = 0;
uint8_t out = 0, crtX = 0, crtY = 0;
uint8_t T1cnt = 0;

volatile char USB_Out_Buffer[CDC_DATA_OUT_EP_SIZE] = {0};
volatile char RS232_Out_Data[CDC_DATA_IN_EP_SIZE] = {0};
volatile char CMD_String[CDC_DATA_IN_EP_SIZE] = {0};

volatile unsigned char  NextUSBOut = 0;
//char RS232_In_Data;
volatile unsigned char    LastRS232Out = 0;  // Number of characters in the buffer
volatile unsigned char	 CMD_String_cp;

volatile char URXbuf[256];
volatile uint16_t RXbufCnt;
volatile uint8_t DataRXflag, WasDataRX, OKflg, ERRflg, SMScnt, SENT;

volatile char URXbuf2[256];
volatile unsigned int RXbuf2Cnt;
volatile uint8_t DataRX2flag;

volatile uint16_t tic1,tic2,tic3;

uint8_t RS232_Out_Data_Rdy = 0;
USB_HANDLE  lastTransmission = 0;

/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);
void ProcessIO(void);
//void USBDeviceTasks(void);
int8_t nibtohex(uint8_t, int8_t);

/********************************************************************/
void __attribute__((__interrupt__,no_auto_psv)) _DefaultInterrupt(void)
{
	Nop();
	Nop(); //Place 2nd breakpoint here
//asm volatile("RESET");	//Soft reset
				Nop();
				Nop();
				Nop();
	while(1);//{Sleep()}; //Trap
}

/********************************************************************/
int main(void)
{
	int16_t i=0,j,k,zz=99;
	char buf[5]="ATE0";
	char buf1[10]="AT+CMGF=1";				// Set text entry mode
	char buf2[23]="AT+CMGS= 0433884656 ";	// Number to send SMS to. note quotes are added manually
	buf2[8] = 0x22;									// We need to write a function that takes ph# and pointer to string
	buf2[19] = 0x22;								// Then constructs the command set and sends it.
	char buf3[23]="This is a test of SMS ";// Text for test SMS
	buf3[22] = 0x1A;
	char buf4[18]=	"AT+CNMI=1,2,2,0,1";			//Display all incoming SMS
	char SMSTXT [160] = {0};
	char SMSPHNO [13] = {0};
	char PriNum [13] = {0};
	char haveSMS = 0;
	char AlarmValHi = 99, AlarmH = 0, AlarmValLo = 99, AlarmL = 0;
	int Temperature;
	char TempStr[160];
	char OK = 0;

    InitializeSystem();
	crtX = 0;
	crtY = 0;
	Gotoxy(0,0);
	print_string_XY("system boot!\r\n",0,0);

	PWRON = 1;
	tic2 = 200;
	while(tic2);
	PWRON = 0;
	print_string_XY("Power\r\n",0,0);

//	while(!OK)
//	{
//		USARTTX(0x41);
//		USARTTX(0x54);
//		USARTTX(0x0D);
//		USARTTX(0x0A);
//	T1cnt = 30;
//	while(T1cnt);
//		if (URXbuf[RXbufCnt-1] == 0x0A)
//		{
//			if ((URXbuf[RXbufCnt-4] == 0x4F)&&(URXbuf[RXbufCnt-3] == 0x4B)){
//				OK = 1;
////				print_string_XY("SYNC\r\n",0,1);
//				RXbufCnt = 0;
//			}
//		}
//	}
//	ATcommand("+CMGF=1");
//	T1cnt = 30;
//	while(T1cnt);
//	ATcommand("&W");
//	T1cnt = 30;
//	while(T1cnt);
//	ATcommand("+CNMI=1,2,2,0,1");
//	T1cnt = 30;
//	while(T1cnt);
//	ATcommand("&W");
//	ATcommand("D122");
//	ATcommand("D*131*PIN#");//recharge SIM credit
//	ATcommand("DH");
//	sendreply("+61433884656","SYSTEM ACTIVE!");
//	ATcommand("+CMGL=\"ALL\"");
//	ATcommand("+CNUM");
//	ATcommand("+BTPOWER=1");


    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
	USBDeviceAttach();
	memset (CMD_String,0,CDC_DATA_IN_EP_SIZE);	//Clear command string

//	while (CMD_String[CMD_String_cp-1] !=0x47) ProcessIO();


   while(1){
		ProcessIO();
		i =0;
		while (i<CMD_String_cp) USARTTX(CMD_String[i++]);
		CMD_String_cp = 0;
	}
}
/********************************************************************/
static void InitializeSystem(void)
{
	TRISB = 0b0000001001000001;
	TRISC = 0;
	TRISD = 0b0000000000000000;
	TRISE = 0;
	TRISF = 0b0000000000010000;
	TRISG = 0b0000000010000000;	//Dont forget to TRIS the USART RX line
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	PORTF = 0;
	PORTG = 0;

	_CLKLOCK = 0;
	_SOSCEN = 0;
	_NSTDIS = 1;
	_ROSEL = 1;
	_ROON = 1;
	_TRISB8 = 1;
	_LATB9 = 1;	//GPS ON

  	RCONbits.VREGS = 0;    						/* Disable regulator in sleep mode */
	AD1PCFGL = 0xFFFF;
	AD1PCFGH = 0xFFFF;

        initOLED();
	initTimmer1();
	initUSART();
	initUSART2();

}//end InitializeSystem

/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{
	unsigned char i,NumBytesIn;
	char USB_In_Buffer[CDC_DATA_OUT_EP_SIZE];
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

//	if(mDataRdyUSART())
//	{
//		USB_Out_Buffer[NextUSBOut] = getcUSART();
//		++NextUSBOut;
//		USB_Out_Buffer[NextUSBOut] = 0;
//	}
	NumBytesIn = getsUSBUSART(USB_In_Buffer,CDC_DATA_IN_EP_SIZE);
	if (NumBytesIn > 0){
		for (i = 0; i < NumBytesIn; ++i){
			if ((CMD_String_cp == CDC_DATA_IN_EP_SIZE) || (USB_In_Buffer[i] == 36)) CMD_String_cp = 0;
			CMD_String[CMD_String_cp++] = USB_In_Buffer[i];
//			USB_Out_Buffer[NextUSBOut++] = USB_In_Buffer[i];
			}
	}

	if((USBUSARTIsTxTrfReady()) && (NextUSBOut > 0))
	{
		putUSBUSART(&USB_Out_Buffer, NextUSBOut);
		NextUSBOut = 0;
	}

    CDCTxService();
}		//end ProcessIO


/********************************************************************/
int8_t nibtohex(uint8_t value ,int8_t nibble)
{
	int8_t temp;
	if (nibble == 0)
	{
		temp = value&0x0F;
		if (temp > 9) temp +=7;
		temp += 48;
	}else if (nibble == 1){
		temp = value>>4;
		if (temp > 9) temp +=7;
		temp += 48;
	}
	return temp;
}

// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:

	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
	//things to not work as intended.


    #if defined(__C30__)
    #if 0
        U1EIR = 0xFFFF;
        U1IR = 0xFFFF;
        U1OTGIR = 0xFFFF;
        IFS5bits.USB1IF = 0;
        IEC5bits.USB1IE = 1;
        U1OTGIEbits.ACTVIE = 1;
        U1OTGIRbits.ACTVIF = 1;
        Sleep();
    #endif
    #endif
}


/******************************************************************************
 * Function:        void _USB1Interrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the USB interrupt bit is set
 *					In this example the interrupt is only used when the device
 *					goes to sleep when it receives a USB suspend command
 *
 * Note:            None
 *****************************************************************************/
#if 0
void __attribute__ ((interrupt)) _USB1Interrupt(void)
{
    #if !defined(self_powered)
        if(U1OTGIRbits.ACTVIF)
        {
            IEC5bits.USB1IE = 0;
            U1OTGIEbits.ACTVIE = 0;
            IFS5bits.USB1IF = 0;

            //USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
            USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);
            //USBSuspendControl = 0;
        }
    #endif
}
#endif

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *
 *					This call back is invoked when a wakeup from USB suspend
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// a few milliseconds of wakeup time, after which the device must be
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.

	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckCDCRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This
 *					callback function should initialize the endpoints
 *					for the device's usage according to the current
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()

 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;

    USBResumeControl = 1;                // Start RESUME signaling

    delay_count = 1800U;                // Set RESUME line for 1-13 ms
    do
    {
        delay_count--;
    }while(delay_count);
    USBResumeControl = 0;
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *
 * Note:            None
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_CONFIGURED:
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER:
            Nop();
            break;
        default:
            break;
    }
    return TRUE;
}
/********************************************************************
This routine scans the keyboard and returns a value showing key pressed
********************************************************************/
unsigned char ScanKey(void)
{
	unsigned int pressed = 0;
	unsigned int OLDTRISE,OLDTRISF;
	unsigned char i = 0;
	unsigned char button[17] = {0x43,0x39,0x38,0x37,0x44,0x23,0x30,0x2A,0x42,0x36,0x35,0x34,0x41,0x33,0x32,0x31,0x20}; //key mapping V

	OLDTRISE = TRISE;
	OLDTRISF = TRISF;
	TRISE		=	0b0000000000111010;
	TRISF		=	0b0000000000000001;

	_LATD0 = 1;
	Nop();
	pressed = ((PORTE&0x0038)>>1)+(PORTE&0x0002)+((PORTF&0x0002)>>1);
	_LATD0 = 0;
	_LATD11 = 1;
	pressed = pressed << 5;
	pressed += ((PORTE&0x0038)>>1)+(PORTE&0x0002)+((PORTF&0x0002)>>1);
	_LATD11 = 0;
	_LATD8 = 1;
	pressed = pressed << 5;
	pressed += ((PORTE&0x0038)>>1)+(PORTE&0x0002)+((PORTF&0x0002)>>1);
	_LATD8 = 0;

	while(!(pressed&0x0001)&&(i<15))
	{
		i++;
		pressed = pressed >> 1;
	}

	return button[i];
}//end ScanKey
/********************************************************************/
void clrBUF(char * DATA,unsigned char count)
{
unsigned char i;
for (i = 0;i<count;i++) *DATA = 0;
}
/********************************************************************/
void extractSMS(char * DATA)
{
unsigned char i;
	i = SMScnt;
	while(URXbuf[i+47]!= 0x0D) *DATA++ = URXbuf[47+i++];	//47 is the SMS text location offset
	*DATA = 0;
}
/********************************************************************/
void extractNUM(char * DATA)
{
unsigned char i;
	i = SMScnt;
	while(URXbuf[i+6]!= 0x22) *DATA++ = URXbuf[6+i++];	//48 is the SMS text location offset
	*DATA = 0;
}
/********************************************************************/
void itostr(int i, char * p)
{
    char const digit[] = "0123456789";
    if(i<0)
	{
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    while(shifter)
	{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }
    *p = '\0';
    while(i)
	{ //Move back, inserting digits as u go
        *--p = digit[i%10];//(i%10)+0x20;//
        i = i/10;
    }
}
/********************************************************************/
void addstr(char * p, char * q)	//p destination string pointer needs to be an array q string to add can be from quotes
{
while(*p) p++;
while(*q)
{
	*p++ = *q++;
}
	*p++ = *q++;
}
/********************************************************************/

/** EOF main.c ***************************************************************/
