/*
 * File:   CAM8MQ.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for the ublox CAM-8MQ GNSS navigation module
 * function library. 
 *
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
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 *
 * Created on 27 April 2015, 11:23 AM
 * Copyright (C) 2010-2015  Affinity Engineering pty ltd

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



/* Includes */

#include <stdbool.h>
#include <stdint.h>
#include "CAM8MQ.h"
#include "USART.h"

/* Defines */
//UBX protocol class ID's
#define NAV 0x01    // Navigation results
#define RXM 0x02    // Receiver Manager Messages
#define INF 0x04    // Information Messages
#define ACK 0x05    // ACK/NACK Messages
#define CFG 0x06    // Configuration Input Messages
#define UPD 0x09    // Firmware Update Messages
#define MON 0x0A    // Monitoring Messages
#define AID 0x0B    // AssistNow Aiding Messages
#define TIM 0x0D    // Timing Messages
#define MGA 0x13    // Multi-GNSS Assistance 
#define LOG 0x21    // Logging Messages

/* Variables */

/* Structures */

struct UBXPacket
/* This struct holds the variables for the UBX protocol packet
    in the order the packet is sent.*/
//TODO structure this correctly!
{
    const uint8_t synchChar1 = 0xB5; //Per ublox protocol spec
    const uint8_t synchChar2 = 0x62; //Per ublox protocol spec
    uint8_t messageClass = 0;
    uint8_t messageID = 0;
    uint16_t payloadLength = 0;
};

/***************************************************************************************/

void gps_NMEA_comand(unsigned char* GPS_CMD)
/* FUNCTION: gps_NMEA_command()
 * PURPOSE : Send a command to the CAM-8 GNSS module using the NMEA protocol.
 * ARGUMENTS:
 *  Type    Name    Desc.
 *
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *
 *
 * NOTES :
 *
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 * SECTION:
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 13 MAY 2015
 *F*/
{
    unsigned long i;
    unsigned char check_sum = 0;
    unsigned char LastRS232Out;
    unsigned char *CMD;

    CMD = GPS_CMD;
    GPS_CMD++;
    for (i = 1; i < 20; i++)
    {
        check_sum = check_sum ^ *GPS_CMD++;
    }
    *(GPS_CMD++) = 42;
    *(GPS_CMD++) = (((check_sum >> 4) & 0x0F)+(((check_sum >> 4)&0x0F) / 0x0A)*0x07) + 0x30;
    *(GPS_CMD++) = ((check_sum & 0x0F)+((check_sum & 0x0F) / 0x0A)*0x07) + 0x30;
    *(GPS_CMD++) = 13;
    *(GPS_CMD++) = 10;
    LastRS232Out = 25;
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    for (i = 0; i < LastRS232Out; i++)
    {
        while (!IFS1bits.U2TXIF);
        UART2PutChar(*(CMD++));
    }
}

void gps_UBX_command(unsigned char* command)
/* FUNCTION: gps_UBX_command()
 * PURPOSE : Send a command to the CAM-8 GNSS module using the u-blox UBX
 * protocol.
 * ARGUMENTS:
 *  Type    Name    Desc.
 *
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *
 *
 * NOTES :
 *
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 * SECTION:
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 13 MAY 2015
 *F*/
{

}

void gps_chksum(unsigned char n, unsigned char* GPS_CMD)
/* FUNCTION: gps_chksum()
 * PURPOSE : Perform checksum checking on GNSS status words
 * ARGUMENTS:
 *  Type                    Name        Desc.
 *  unsigned char           n
 * unsigned char pointer   GPS_CMD
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *
 *
 * NOTES :
 *
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 * SECTION:
 *
 * VER: 1.0
 * PROGRAMMER:
 * E. Freund <eric@affinityengineering.com.au>
 * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 13 MAY 2015
 *F*/
// string always is 6 longer after this process
{
    unsigned long i;
    unsigned char check_sum = 0;

    GPS_CMD++;
    for (i = 1; i < n; i++)
    {
        check_sum = check_sum ^ *GPS_CMD++;
    }
    *(GPS_CMD++) = 42;
    *(GPS_CMD++) = (((check_sum >> 4) & 0x0F)+(((check_sum >> 4)&0x0F) / 0x0A)*0x07) + 0x30;
    *(GPS_CMD++) = ((check_sum & 0x0F)+((check_sum & 0x0F) / 0x0A)*0x07) + 0x30;
    *(GPS_CMD++) = 13;
    *(GPS_CMD++) = 10;
    *(GPS_CMD++) = 0;
}

void PollJammingDetection(void)
/* FUNCTION: PollJammingDetection()
 * PURPOSE : Query the CAM-8's onboard jamming and interference detection
 * system.
 * ARGUMENTS:
 *  Type                    Name        Desc.
 *
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *
 *
 * NOTES :
 *
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 * SECTION: 21.11.8.1
 *
 * VER: 1.0
 * PROGRAMMER:
  * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 13 MAY 2015
 *F*/
{

}

void ConfigJammingDetection(unsigned char *configWord)
/* FUNCTION: ConfigJammingDetection()
 * PURPOSE : Configure the CAM-8's onboard jamming and interference detection
 * system.
 * ARGUMENTS:
 *  Type                    Name        Desc.
 *  unsigned char pointer   configWord
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *
 *
 * NOTES :
 *
 * REFERENCE DOCUMENT: u-blox M8 Reciever Description Including Protocol Specification
 * DOCUMENT NUMBER: UBX-13003221-R08
 * REVISION / DATE: R08 / 4 DEC 2014
 * SECTION: 21.11.8.2
 *
 * VER: 1.0
 * PROGRAMMER:
  * W. Anthony <will@affinityengineering.com.au>
 *
 * DATE: 13 MAY 2015
 *F*/
{

}