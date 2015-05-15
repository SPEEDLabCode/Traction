/*
 * File:   SIM800.c
 * Author:
 * E. Freund <eric@affinityengineering.com.au
 * W. Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for the SIM800 multi-band
 * GSM Module.  This library contains functions to automate many of the
 * onboard commands and common usage scenarios for the module.
 *
 * Language: C (CCI)
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependancies:
 * SIM800.h
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 *
 * REFERENCE DOCUMENT: SIM800 Series AT Command Manual
 * DOCUMENT NUMBER: SIM800Series_AT_CommandManual_V1.05
 * REVISION / DATE: 1.05 / 31 JUL 2014
 *
 * Created on 16 April 2015, 11:43 PM
 * Copyright (C) 2015  Affinity Engineering pty ltd

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

#include <stdbool.h>
#include <stdint.h>
#include "SIM800.h"
#include "USART.h"

/* Variables */

/* Structures */
struct channel
{
    uint8_t SNR = 255; // Initialize to known-bad values that the module will not return
    uint8_t BER = 255;
};

void ATcommand(char *DATA)
/* FUNCTION: ATcommand()
 * PURPOSE : Automate the USART transmission of AT commands
 * ARGUMENTS:
 *  Type    Name    Desc.
 *  string  DATA    characters to transmit
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/

{
    //FIXME validate input!
    USARTTX(0x41); //A
    USARTTX(0x54); //T
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++);
    }
    USARTTX(0x0D); //<cr>
}
//********************************************************************

void SMSnumber(char *DATA)
/* FUNCTION: SMSnumber()
 * PURPOSE : Automate the USART transmission of the phone number string
 * ARGUMENTS:
 *  Type                Name    Desc.
 *  character string    DATA    phone number
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * ATstg();
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    //FIXME validate input!
    ATstg("AT+CMGS=");
    USARTTX(0x22); //"
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++); //FIXME should be USARTTXstg()?
    }
    USARTTX(0x22); //"
    USARTTX(0x0D); //<cr>
}
//********************************************************************

void SMStext(char* DATA)
/* FUNCTION: SMStext()
 * PURPOSE : Automate the USART transmission of SMS message content
 * ARGUMENTS:
 *  Type                Name    Desc.
 *  character string    DATA    characters to transmit
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    //FIXME validate input!
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++); //FIXME should be USARTTXstg()?
    }
    USARTTX(0x1A); //<^Z>
}
//********************************************************************

void ATstg(char* DATA)
/* FUNCTION: ATstg()
 * PURPOSE : Automate the USART transmission of unformatted strings
 * ARGUMENTS:
 *  Type                Name    Desc.
 *  character string    DATA    characters to transmit
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    //FIXME validate input!
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++); //FIXME should be USARTTXstg()?
    }
}

//********************************************************************

char SendReply(char *NUM, char *DATA) //TODO check on the logic and flow control of this and ensure that there isn't some wasted stuff in here!
/* FUNCTION: SendReply()
 * PURPOSE : Return a reply SMS message
 * ARGUMENTS:
 *  Type                Name    Desc.
 *  character string    NUM     Phone number to contact
 *  character string    DATA    Contents of text message
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  true/false indication of function success //FIXME set this instead of char?
 *
 * DEPENENCIES:
 * ATcommand();
 * SMSnumber();
 * SMStext();
 *
 * NOTES :
 * REFERENCE DOCUMENT: SIM800 Series AT Command Manual
 * DOCUMENT NUMBER: SIM800Series_AT_CommandManual_V1.05
 * REVISION / DATE: 1.05 / 31 JUL 2014
 * SECTION:
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    //FIXME validate input!
    OKflg = 0xFF;
    while (OKflg == 0xFF)
    {
        ATcommand("+CMGF=1");
        tic1 = 500;
        while (tic1 && (OKflg == 0xFF));
    }
    OKflg = 0xFF;
    //				while (OKflg == 0xFF) {SMSnumber(*&NUM);tic1 = 500;while (tic1&&(OKflg == 0xFF));}
    SMSnumber(*&NUM);
    tic1 = 100;
    while (tic1);
    //print_string(*&DATA);
    //				OKflg = 0xFF;
    SMStext(*&DATA);
    ERRflg = 0xFF;
    SENT = 0xFF;
    tic1 = 2000;
    while ((tic1) || ((ERRflg == 0xDD) || (SENT == 0xFF)));
    return SENT;
}
//********************************************************************

bool BTpower(bool Test, bool Power)
/* FUNCTION: BTpower()
 * PURPOSE : Set the Bluetooth module power on or off and conduct self-test
 * ARGUMENTS:
 *  Type    Name    Desc.
 *  bool    Test    Sets device test mode
 *  bool    Power   Set BT module on or off
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{

    bool status = false; /* return variable for success or failure of operation*/

    if (Test == 0 && Power == 0) //Switch off module
    {
        return status;
    }

    if (Test == 1 && Power == 0) //In the case where the user does something stupid and asks it for status while turning it off
    {
        return status;
    }

    if (Test == 0 && Power == 1) //Turn module on
    {
        return status;
    }

    if (Test == 1 && Power == 1) //Get module Status
    {
        return status;
    }

}
//********************************************************************

bool BTstatus(uint8_t command)
/* FUNCTION: BTstatus()
 * PURPOSE : Set the Bluetooth module power on or off and conduct self-test
 * ARGUMENTS:
 *  Type    Name    Desc.
 *  bool  Test    Sets device test mode
 *  bool    Power   Set BT module on or off
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{

    bool status = false;
    //TODO if statement that validates input command and performs query to BT module

    //TODO if statement that parses BT module response, and if correct sets return status variable == true for success

    return status;

}
//********************************************************************

bool CMDecho(bool echo)
/* FUNCTION: CMDecho()
 * PURPOSE : Set the command echo output of the module on or off
 * ARGUMENTS:
 *  Type    Name    Desc.
 *  bool    echo    true sets module to echo all command strings
 *
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 * This function is only really required if using a user-interactive terminal session,
 * otherwise this can create additional bus congestion for no benefit.
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    if (echo == true)
    {
        ATcommand("E1");
    }
    else
    {
        ATcommand("E0");
    }

}
//********************************************************************

bool Hangup(void)
/* FUNCTION: Hangup()
 * PURPOSE : Terminate a call session
 * ARGUMENTS:
 *  Type    Name    Desc.
 *
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    bool status = false;
    ATcommand("H"); //FIXME ensure we are passing the letter "H" properly

    //TODO Check to see if hangup was performed

    //TODO If hangup was performed then change status

    return status;
}
//********************************************************************

bool SetSMSFormat(bool text)
/* FUNCTION: SetSMSFormat()
 * PURPOSE : Switch between data and text format for SMS messages
 * ARGUMENTS:
 *  Type    Name    Desc.
 *  bool    text    true for text mode, false for data mode
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * ATcommand();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    bool status = false;

    if (text = true)
    {
        //set SMS text format
        ATcommand("+CMGF=1");
        //if set is successful set status = true
    }
    else
    {
        //set SMS numerical format
        ATcommand("CMGF=0");
        //if set is successful set status = true
    }

    return status;
}
//********************************************************************

bool ChannelQual(void)
/* FUNCTION: ChannelQual()
 * PURPOSE : Query the quality of the mobile network connection
 * ARGUMENTS:
 *  Type    Name    Desc.
 *
 * RETURN :
 *  Type    Name    Desc.
 *  bool    status  function error return, true == success
 *
 * DEPENENCIES:
 * ATcommand();
 *
 * NOTES :
 *
 * VER: 1.0
 * PROGRAMMER:
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 18 APR 2015
 *F*/
{
    bool status = false;

    ATcommand("+CSQ"); //send channel status command

    //TODO read in response

    //TODO if response is +CME ERROR then status = false, return false

    //TODO assign first value +CSQ: <rssi>,<ber> to channel.SNR
    //TODO assign second value +CSQ: <rssi>,<ber> to channel.BER
    //TODO status = true

    return status;
}
//********************************************************************
