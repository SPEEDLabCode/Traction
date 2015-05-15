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

/*Include Global Parameters*/
#include "SIM800.h"
#include "USART.h"
#include <stdbool.h>
#include <stdint.h>

/* Variables */

/* Structures */
struct channel
{
    uint8_t SNR = 255; // Initialize to known-bad values that the module will not return
    uint8_t BER = 255;
};

void ATcommand(char *DATA)

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

{
    //FIXME validate input!
    ATstg("AT+CMGS=");
    USARTTX(0x22); //"
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++);   //FIXME should be USARTTXstg()?
    }
    USARTTX(0x22); //"
    USARTTX(0x0D); //<cr>
}
//********************************************************************

void SMStext(char* DATA)

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

{
    //FIXME validate input!
    while (*DATA) //FIXME should determine length of data and use for.  Provides pre-defined exit.
    {
        USARTTX(*DATA++); //FIXME should be USARTTXstg()?
    }
}

//********************************************************************

char SendReply(char *NUM, char *DATA) //TODO check on the logic and flow control of this and ensure that there isn't some wasted stuff in here!

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
{
    return false;

}
//********************************************************************

bool CMDecho(bool echo)
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
{
    bool status = false;
    ATcommand("H"); //FIXME ensure we are passing the letter "H" properly

    //TODO Check to see if hangup was performed

    //TODO If hangup was performed then change status

    return status;
}
//********************************************************************

bool SetSMSFormat(bool text)
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
{
    bool status = false;
    
    ATcommand("+CSQ");  //send channel status command

    //TODO read in response

    //TODO if response is +CME ERROR then status = false, return false

    //TODO assign first value +CSQ: <rssi>,<ber> to channel.SNR
    //TODO assign second value +CSQ: <rssi>,<ber> to channel.BER
    //TODO status = true

    return status;
}
//********************************************************************
