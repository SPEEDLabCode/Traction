/*
 * File:   CAM8MQ.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for the ublox CAM-8MQ GNSS navigation module
 * function library. 
 *
 * Language: C (CCI)
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependencies:
 * CAM8MQ.h
 * 
 * Compiler: Microchip XC16
 * Compiler Revision: 1.24
 *
 * Reference Document: u-blox M8 Reciever Description Including Protocol Specification
 * Document Number: UBX-13003221-R08
 * Revision / Date: R08 / 4 DEC 2014
 *
 * Created on 27 April 2015, 11:23 AM
 * Copyright (C) 2010-2015  Affinity Engineering pty. ltd.
 * <http://www.affinityengineering.com.au>
 * <http://github.com/SPEEDLabCode>

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
//! The first header byte of every message sent in UBX protocol
#define UBX_HEADER1 0xb5
//! The second header byte of every message sent in UBX protocol
#define UBX_HEADER2 0x62

// Classes
//! The class value for NAV-class messages
#define UBX_CLASS_NAV 0x01      // NAV class messages
#define UBX_CLASS_RXM 0x02    // Receiver Manager Messages
#define UBX_CLASS_INF 0x04    // Information Messages
#define UBX_CLASS_ACK 0x05    // ACK/NACK Messages
#define UBX_CLASS_CFG 0x06    // Configuration Input Messages
#define UBX_CLASS_UPD 0x09    // Firmware Update Messages
#define UBX_CLASS_MON 0x0A    // Monitoring Messages
#define UBX_CLASS_AID 0x0B    // AssistNow Aiding Messages
#define UBX_CLASS_TIM 0x0D    // Timing Messages
#define UBX_CLASS_MGA 0x13    // Multi-GNSS Assistance 
#define UBX_CLASS_LOG 0x21    // Logging Messages

// Ids

//NAV class message ID's
#define UBX_ID_NAV_POSLLH 0x02  // Geodetic Position Solution
#define UBX_ID_NAV_VELNED 0x12  // Velocity Solution in NED (North, East, Down)
#define UBX_ID_NAV_STATUS 0x03  // Reciever Navigation Status
#define UBX_ID_NAV_SVINFO 0x30
#define UBX_ID_NAV_RESETODO 0x10    // Reset trip odometer


// Sizes

//NAV class message lengths (bytes))
#define UBX_SIZE_NAV_POSLLH 28
#define UBX_SIZE_NAV_VELNED 36
#define UBX_SIZE_NAV_STATUS 16
#define UBX_SIZE_NAV_SVINFO 8   //see documentation
#define UBX_SIZE_NAV_TIMEUTC 20

//! Max number of channel to be used
#define UBX_MAX_CHANNEL 16

/* Structures */
/* This struct holds the variables for the UBX protocol packet
    in the order the packet is sent.*/
typedef enum
{
	UBX_STATE_HEADER1 = 0,
	UBX_STATE_HEADER2,
	UBX_STATE_CLASS,
	UBX_STATE_ID,
	UBX_STATE_LENGTH,
	UBX_STATE_CONTENT,
	UBX_STATE_CHECKSUM_A,
	UBX_STATE_CHECKSUM_B
}
ubx_State;

/* Variables */

//! The current state of the finite-state-machine for the decoding of incoming messages
ubx_State ubx_state = ubx_State.UBX_STATE_HEADER1;
//! The class of the current message
uint8_t ubx_currentMessageClass = 0;
//! The id of the current message
uint8_t ubx_currentMessageId = 0;
//! The size of the current message
unsigned short ubx_currentMessageSize = 0;
//! The current byte of the size being received
uint8_t ubx_currentMessageSizeByte = 0;
//! The checksum A calculated of the current message
uint8_t ubx_currentMessageCkA = 0;
//! The checksum B calculated of the current message
uint8_t ubx_currentMessageCkB = 0;
//! The checksum A read for the current message
uint8_t ubx_currentMessageCkARead = 0;
//! The checksum B read for the current message
uint8_t ubx_currentMessageCkBRead = 0;

//! If a timeout occurred on the GPS (GPS not connected or not working)
uint8_t ubx_timeout = 0;

//! The current size of the current message (message full if currentMessageCurrentByte == currentMessageSize)
unsigned short ubx_currentMessageCurrentByte = 0;
//! The pointer to the pointer to the structure of the current message to fill
unsigned char ** ubx_currentMessage = 0;    //FIXME pointers to pointers are dangerous
//! The pointer to the pointer to the structure of the last message received of the same type than the current one being received (for exchange at the end)
unsigned char ** ubx_lastMessage = 0;   //FIXME pointers to pointers are dangerous
//! The pointer to the number to increment when a message of the type has been received
unsigned short *ubx_validMessage = 0;

// We are using two buffers for each message, one for the last message received, the other for the message being received (not complete)
//! The Posllh message buffer
ubx_MessagePosllh ubx_posllhMessage[2];
//! The Velned message buffer
ubx_MessageVelned ubx_velnedMessage[2];
//! The Status message buffer
ubx_MessageStatus ubx_statusMessage[2];
//! The SVInfo message buffer
ubx_MessageSVInfo ubx_svInfoMessage[2];

// NAV-POSLLH
//! The pointer to the Posllh message that is being filled (not usable)
ubx_MessagePosllh * ubx_currentPosllhMessage = &ubx_posllhMessage[0];
//! The pointer to the last Posllh message that was completed
ubx_MessagePosllh * ubx_lastPosllhMessage = &ubx_posllhMessage[1];
//! Number of valid Posllh message received
unsigned short ubx_numberOfValidPosllhMessage = 0;
// NAV-VELNED
//! The pointer to the Velned message that is being filled (not usable)
ubx_MessageVelned * ubx_currentVelnedMessage = &ubx_velnedMessage[0];
//! The pointer to the last Velned message that was completed
ubx_MessageVelned * ubx_lastVelnedMessage = &ubx_velnedMessage[1];
//! Number of valid Velned message received
unsigned short ubx_numberOfValidVelnedMessage = 0;
// NAV-STATUS
//! The pointer to the Status message that is being filled (not usable)
ubx_MessageStatus * ubx_currentStatusMessage = &ubx_statusMessage[0];
//! The pointer to the last Status message that was completed
ubx_MessageStatus * ubx_lastStatusMessage = &ubx_statusMessage[1];
//! Number of valid Status message received
unsigned short ubx_numberOfValidStatusMessage = 0;
// NAV-SVINFO
//! The pointer to the Status message that is being filled (not usable)
ubx_MessageSVInfo * ubx_currentSVInfoMessage = &ubx_svInfoMessage[0];
//! The pointer to the last Status message that was completed
ubx_MessageSVInfo * ubx_lastSVInfoMessage = &ubx_svInfoMessage[1];
//! Number of valid Status message received
unsigned short ubx_numberOfValidSVInfoMessage = 0;


//! Satellite strength buffer
uint8_t ubx_satelliteStrength[UBX_MAX_CHANNEL];
//! Number of satellite used for navigation
uint8_t ubx_satelliteCount;

// Statistics
//! The number of bytes dropped in header1 state
unsigned short ubx_errorHeader1 = 0;
//! The number of bytes dropped in header2 state
unsigned short ubx_errorHeader2 = 0;
//! The number of messages dropped due to checksum error
unsigned short ubx_errorChecksum = 0;
//! The number of messages dropped due to unknown message class (or not taken into account)
unsigned short ubx_errorUnknownClass = 0;
//! The number of messages dropped due to unknown message id or wrong size read
unsigned short ubx_errorUnknownIdSize = 0;
//! The ID of the last message that was dropped due to unknown id or wrong size
uint8_t ubx_errorWrongId = 0;
//! The size of the last message that was dropped due to unknown id or wrong size
unsigned short ubx_errorWrongSize = 0;
//! The number of major errors that occurred (should always be 0)
unsigned short ubx_errorMajor = 0;



/***************************************************************************************/

void gps_NMEA_comand(uint8_t *GPS_CMD)
/* FUNCTION: gps_NMEA_command()
 * PURPOSE : Send a command to the CAM-8 GNSS module using the NMEA protocol.
 * ARGUMENTS:
 *  Type                    Name        Desc.
 *  unsigned char pointer   *GPS_CMD    NMEA command
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 *
 * VARIABLES:
 *  Type                      Name            Scope       Initialized To
 * unsigned long            i               local       0
 * unsigned char            check_sum       local       0
 * unsigned char            LastRS232Out    local       0
 * unsigned char pointer    *CMD            local       
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
    uint32_t i = 0;
    uint8_t check_sum = 0;
    uint8_t LastRS232Out = 0;
    uint8_t *CMD;   

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

void gps_chksum(unsigned char n, unsigned char *GPS_CMD)
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
    uint32_t i = 0; //FIXME why is this counter so large when we're passing in an 8 bit value for n?
    uint8_t check_sum = 0;

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
 *  none
 *
 * RETURN :
 *  Type    Name    Desc.
 *  void
 *
 * DEPENENCIES:
 * USARTTX();
 * gps_UBX_command();
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
 * gps_UBX_command();
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


/*!
*	This function receives bytes from the communication
*	It runs a state machine which fills decodes the messages and put their content into structures
*
*	@param data The data byte that arrived through communication
*/
void ubx_ReceiveDataByte(unsigned char data)
{
	// the 
	unsigned char * temporaryMessageForSwaping;
	
	// read header 1
	if (ubx_state == UBX_STATE_HEADER1)
	{
		if (data == UBX_HEADER1)
		{
			ubx_state = UBX_STATE_HEADER2;	
		}
		else
		{
			++ubx_errorHeader1;
		}	
	}
	// read header 2
	else if (ubx_state == UBX_STATE_HEADER2)
	{
		if (data == UBX_HEADER2)
		{
			// set up next state
			ubx_currentMessageCkA = 0;
			ubx_currentMessageCkB = 0;
			ubx_state = UBX_STATE_CLASS;	
		}
		else if (data == UBX_HEADER1)
		{
			// stay in this state	
		}
		else
		{
			++ubx_errorHeader2;
			// go back to first state
			ubx_state = UBX_STATE_HEADER1;	
		}	
	}
	// read the class of the message
	else if (ubx_state == UBX_STATE_CLASS)
	{
		// read class
		ubx_currentMessageClass = data;
		// checksum
		ubx_currentMessageCkA += data;
		ubx_currentMessageCkB += ubx_currentMessageCkA;
		// go to next state: id
		ubx_state = UBX_STATE_ID;
	}
	// read the id of the message
	else if (ubx_state == UBX_STATE_ID)
	{
		// read id
		ubx_currentMessageId = data;
		// checksum
		ubx_currentMessageCkA += data;
		ubx_currentMessageCkB += ubx_currentMessageCkA;
		// set up next state: length
		ubx_currentMessageSizeByte = 0;
		ubx_currentMessageSize = 0;
		ubx_state = UBX_STATE_LENGTH;
	}
	// read the size of the message
	else if (ubx_state == UBX_STATE_LENGTH)
	{
		((unsigned char*)(&ubx_currentMessageSize))[ubx_currentMessageSizeByte++] = data;
		// checksum
		ubx_currentMessageCkA += data;
		ubx_currentMessageCkB += ubx_currentMessageCkA;
		
		// size read completely
		if (ubx_currentMessageSizeByte == 2)
		{
			
			// select structure to fill
			// NAV class messages
			if (ubx_currentMessageClass == UBX_CLASS_NAV)
			{
				// NAV-POSLLH
				if (ubx_currentMessageId == UBX_ID_NAV_POSLLH && ubx_currentMessageSize == UBX_SIZE_NAV_POSLLH)
				{
					ubx_currentMessage = (unsigned char**)&ubx_currentPosllhMessage;
					ubx_lastMessage = (unsigned char**)&ubx_lastPosllhMessage;
					ubx_validMessage = &ubx_numberOfValidPosllhMessage;
					
					ubx_state = UBX_STATE_CONTENT;
				}
				// NAV-VELNED
				else if	(ubx_currentMessageId == UBX_ID_NAV_VELNED && ubx_currentMessageSize == UBX_SIZE_NAV_VELNED)
				{
					ubx_currentMessage = (unsigned char**)&ubx_currentVelnedMessage;
					ubx_lastMessage = (unsigned char**)&ubx_lastVelnedMessage;
					ubx_validMessage = &ubx_numberOfValidVelnedMessage;
					
					ubx_state = UBX_STATE_CONTENT;
				}
				// NAV-STATUS
				else if (ubx_currentMessageId == UBX_ID_NAV_STATUS && ubx_currentMessageSize == UBX_SIZE_NAV_STATUS)
				{
					ubx_currentMessage = (unsigned char**)&ubx_currentStatusMessage;
					ubx_lastMessage = (unsigned char**)&ubx_lastStatusMessage;
					ubx_validMessage = &ubx_numberOfValidStatusMessage;
					
					ubx_state = UBX_STATE_CONTENT;
				}
				// NAV-SVINFO
				else if (ubx_currentMessageId == UBX_ID_NAV_SVINFO)
				{
					ubx_currentMessage = (unsigned char**)&ubx_currentSVInfoMessage;
					ubx_lastMessage = (unsigned char**)&ubx_lastSVInfoMessage;
					ubx_validMessage = &ubx_numberOfValidSVInfoMessage;
					
					ubx_state = UBX_STATE_CONTENT;
				}
				// not taken into account, so wait for next message
				else
				{
					++ubx_errorUnknownIdSize;
					ubx_errorWrongId = ubx_currentMessageId;
					ubx_errorWrongSize = ubx_currentMessageSize;
					
					ubx_state = UBX_STATE_HEADER1;
				}		
			}	
			// not taken into account, so wait for next message
			else
			{
				++ubx_errorUnknownClass;
				ubx_state = UBX_STATE_HEADER1;
			}		
			
			// reset byte count anyway
			ubx_currentMessageCurrentByte = 0;
		}	
	}	
	// read the content of the message
	else if (ubx_state == UBX_STATE_CONTENT)
	{
		// read byte and put it in the structure
		(*ubx_currentMessage)[ubx_currentMessageCurrentByte++] = data;
	
		// checksum
		ubx_currentMessageCkA += data;
		ubx_currentMessageCkB += ubx_currentMessageCkA;
		
		// message is full, go to next state
		if (ubx_currentMessageCurrentByte == ubx_currentMessageSize)
		{
			ubx_state = UBX_STATE_CHECKSUM_A;
		}
	}
	// read the checksum byte A
	else if (ubx_state == UBX_STATE_CHECKSUM_A)	
	{
		// read checksum A
		ubx_currentMessageCkARead = data;
		// go to last state: checksum B
		ubx_state = UBX_STATE_CHECKSUM_B;
	}	
	// read the checksum byte B
	else if (ubx_state == UBX_STATE_CHECKSUM_B)
	{
		// read checksum B
		ubx_currentMessageCkBRead = data;
		
		// checksum is correct ?
		if (ubx_currentMessageCkA == ubx_currentMessageCkARead && ubx_currentMessageCkB == ubx_currentMessageCkBRead)
		{
			// this type of message has been received successfully
			++(*ubx_validMessage);
			// message received, set reset timeout
			ubx_timeout = 0;
			
			// swap message buffers, old message is discarded and becomes incoming buffer, new message become valid message (=old)
			temporaryMessageForSwaping = *ubx_currentMessage;
			*ubx_currentMessage = *ubx_lastMessage;
			*ubx_lastMessage = temporaryMessageForSwaping;
			
		}	
		// checksum error
		else
		{
			++ubx_errorChecksum;
			// keep same buffers as data is corrupted
		}	
		// go back to state header1 for next message
		ubx_state = UBX_STATE_HEADER1;
	}	
	// major error, should never happen, the state is unknown !
	else
	{
		++ubx_errorMajor;
		ubx_state = UBX_STATE_HEADER1;
	}			
}	


/*!
*	This function calculates the number of time that it has been called since the last GPS message was received.
*	If this number goes higher than #UBX_TIMEOUT_CYCLES, then there is a timeout.
*
*	@return 1 if there is a timeout, 0 otherwise.
*/
unsigned char ubx_CheckTimeout()
{
	if (ubx_timeout < UBX_TIMEOUT_CYCLES)
	{
		++ubx_timeout;
	}	
	
	return (ubx_timeout == UBX_TIMEOUT_CYCLES);
}

/*!
*	This function returns if there is a timeout on the GPS.
*	It is passive and does not modifies the timeout value.
*/
unsigned char ubx_GetTimeout()
{
	return (ubx_timeout == UBX_TIMEOUT_CYCLES);
}	


/*!
*	This function gets data using the UBX binary protocol
*	The reference indicated in the data structure returned (itow) is the smallest
*	reference of all messages
*/
void ubx_GetGPSData(gps_Data * data)
{
	ubx_MessagePosllh * gpsPosllh;
	ubx_MessageVelned * gpsVelned;
	ubx_MessageStatus * gpsStatus;
	
	// The reference of each messages
	unsigned long gpsPosllhReference = 0;
	unsigned long gpsVelnedReference = 0;
	unsigned long gpsStatusReference = 0;
	
	unsigned char gpsFix = 0;
 	unsigned long hAcc = 0;
	unsigned long vAcc = 0;
	unsigned long sAcc = 0;
	unsigned long cAcc = 0;

	unsigned char timeout = ubx_GetTimeout();
	
	// Get Posllh message
	gpsPosllh = ubx_GetPosllh();
	if (gpsPosllh)
	{
		gpsPosllhReference = gpsPosllh->itow;
		data->longitude = gpsPosllh->lon;
		data->latitude = gpsPosllh->lat;	
		data->altitude = ((float)gpsPosllh->height) / 1000.;
		hAcc = gpsPosllh->hAcc;
		vAcc = gpsPosllh->vAcc;
	}	

	// Get Velned message	
	gpsVelned = ubx_GetVelned();
	if (gpsVelned)
	{
		gpsVelnedReference = gpsVelned->itow;
		data->speed = ((float)gpsVelned->speed) / 100.;
		data->groundSpeed = ((float)gpsVelned->gSpeed) / 100.;
		data->course = ((float)gpsVelned->heading) / 100000.;
		data->northSpeed = ((float)gpsVelned->velN) / 100.;
		data->eastSpeed = ((float)gpsVelned->velE) / 100.;
		data->verticalSpeed = -((float)gpsVelned->velD) / 100.;
		sAcc = gpsVelned->sAcc;
		cAcc = gpsVelned->cAcc;
	}	
	
	// Get Status message
	gpsStatus = ubx_GetStatus();
	if (gpsStatus)
	{
		gpsStatusReference = gpsStatus->itow;
		gpsFix = gpsStatus->gpsFix; // 2 is ok for 2D, 3 or more means fix is good for 3D
	}	
	// take smallest reference
	data->itow = (gpsPosllhReference <= gpsVelnedReference) ? gpsPosllhReference : gpsVelnedReference;
	data->itow = (data->itow <= gpsStatusReference) ? data->itow : gpsStatusReference;
	
	// update status
	// position and altitude
	if (!gpsPosllh || timeout)
	{
		data->latitudeStatus = TIMEOUT;
		data->longitudeStatus = TIMEOUT;
		data->altitudeStatus = TIMEOUT;
	}
	else
	{
		if (gpsFix >= 2 && hAcc < UBX_POSITION_PRECISION)
		{
			data->latitudeStatus = CORRECT;
			data->longitudeStatus = CORRECT;
		}
		else
		{
			data->latitudeStatus = INACCURATE;
			data->longitudeStatus = INACCURATE;
		}
		
		if (gpsFix >= 3 && vAcc < UBX_ALTITUDE_PRECISION)
		{
			data->altitudeStatus = CORRECT;
		}
		else
		{
			data->altitudeStatus = INACCURATE;
		}		
	}	
	// speed and heading
	if (!gpsVelned || timeout)
	{
		data->speedStatus = TIMEOUT;
		data->groundSpeedStatus = TIMEOUT;
		data->northSpeedStatus = TIMEOUT;
		data->eastSpeedStatus = TIMEOUT;
		data->verticalSpeedStatus = TIMEOUT;
		data->courseStatus = TIMEOUT;
	}
	else 
	{
		if (sAcc < UBX_SPEED_PRECISION)
		{
			data->speedStatus = CORRECT;
			data->groundSpeedStatus = CORRECT;
			data->northSpeedStatus = CORRECT;
			data->eastSpeedStatus = CORRECT;
			data->verticalSpeedStatus = CORRECT;
		}
		else
		{
			data->speedStatus = INACCURATE;
			data->groundSpeedStatus = INACCURATE;
			data->northSpeedStatus = INACCURATE;
			data->eastSpeedStatus = INACCURATE;
			data->verticalSpeedStatus = INACCURATE;
		}
		if (cAcc < UBX_HEADING_PRECISION)
		{
			data->courseStatus = CORRECT;
		}
		else
		{
			data->courseStatus = INACCURATE;
		}		
	}
	
	// FIXME: we write the satelite strength to the buffer at this time instead of in the interrupt
	ubx_MessageSVInfo *svInfo = ubx_GetSVInfo();
	if (svInfo)
	{
		int i;
		ubx_satelliteCount = 0;
		for (i = 0; i < UBX_MAX_CHANNEL; i++)
		{
			ubx_satelliteStrength[i] = svInfo->channelData[i].cno;
			
			if (svInfo->channelData[i].flags & 0x1)
				ubx_satelliteCount++;
		}	
	}	 
}

/*! 
*	This function returns a pointer to the last NAV-POSLLH message that was received
*	Warning: the values of the message must be read very quickly after the call to this function as buffer may be swapped in an interruption
*
*	@return A pointer to the last valid posllh message, or 0.
*/
ubx_MessagePosllh * ubx_GetPosllh()
{
	if (ubx_numberOfValidPosllhMessage)
		return ubx_lastPosllhMessage;
	else
		return 0;
}
	
/*! 
*	This function returns a pointer to the last NAV-VELNED message that was received
*	Warning: the values of the message must be read very quickly after the call to this function as buffer may be swapped in an interruption
*
*	@return A pointer to the last valid velned message, or 0.
*/
ubx_MessageVelned * ubx_GetVelned()
{
	if (ubx_numberOfValidVelnedMessage)
		return ubx_lastVelnedMessage;
	else
		return 0;
}

/*!
*	This function returns a pointer to the last NAV-STATUS message that was received
*	Warning: the values of the message must be read very quickly after the call to this function as buffer may be swapped in an interruption
*
*	@return A pointer to the last valid status message, or 0.
*/
ubx_MessageStatus * ubx_GetStatus()
{
	if (ubx_numberOfValidStatusMessage)
		return ubx_lastStatusMessage;
	else
		return 0;	
}

/*!
*	This function returns a pointer to the last NAV-SVINFO message that was received
*	Warning: the values of the message must be read very quickly after the call to this function as buffer may be swapped in an interruption
*
*	@return A pointer to the last valid status message, or 0.
*/
ubx_MessageSVInfo * ubx_GetSVInfo()
{
	if (ubx_numberOfValidSVInfoMessage)
		return ubx_lastSVInfoMessage;
	else
		return 0;	
}

