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
 * Dependencies:
 * USART.h
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 *
 * Created on 16 April 2015, 11:43 PM
 * Copyright (C) 2015  Affinity Engineering pty. ltd.
 * http://www.affinityengineering.com.au
 * http://github.com/SPEEDLabCode

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

/*********************************************************************
Defines
********************************************************************/

/*********************************************************************
Globals
********************************************************************/

/*********************************************************************
Function prototypes
********************************************************************/

    /* Housekeeping Functions */
    bool CMDecho(bool); 
    bool SetSMSFormat(bool);
   
    /* Operational Functions */
    void ATcommand(char*);
    void SMSnumber(char*);
    void SMStext(char*);
    void ATstg(char*);
    char SendReply(char *, char *);
    bool Hangup(void);
    bool ChannelQual(void);
    void extractSMS(char);
    void extractNUM(char);
   
    /* Bluetooth Functions */
    bool BTpower(bool, bool);
    bool BTstatus(uint8_t);
    
    

#ifdef	__cplusplus
}
#endif

#endif	/* SIM800_H */

