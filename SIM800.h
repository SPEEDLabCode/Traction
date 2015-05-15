/* 
 * File:   SIM800.h
 * Author: Will Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for the SIM800 multi-band
 * GSM Module.  This library contains functions to automate much of the
 * commands used and common usage scenarios for the module.
 *
 * Language: C
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependancies:
 * USART.h
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

#ifndef SIM800_H
#define	SIM800_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* Housekeeping Functions */
    bool CMDecho(bool);
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

    bool SetSMSFormat(bool);
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

    /* Operational Functions */


    void ATcommand(char*);
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


    void SMSnumber(char*);
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

    void SMStext(char*);
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

    void ATstg(char*);
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

    char SendReply(char *, char *);
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
     *
     * VER: 1.0
     * PROGRAMMER:
     * E. Freund <eric@affinityengineering.com.au>
     * W. Anthony <will@affinityengineering.com.au>
     *
     * DATE: 18 APR 2015
     *F*/

    bool Hangup(void);
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

    bool ChannelQual(void);
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

    //Bluetooth Functions
    bool BTpower(bool);
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


    bool BTstatus(bool);
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





#ifdef	__cplusplus
}
#endif

#endif	/* SIM800_H */

