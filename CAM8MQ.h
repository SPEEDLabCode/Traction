/*
 * File:   CAM8MQ.h
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for the ublox CAM-8MQ GNSS navigation module.
 * Function documentation can be found in the source file.
 *
 * Language: C (CCI)
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependencies:
 * 
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


#ifndef CAM8MQ_H
#define	CAM8MQ_H

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
    void gps_NMEA_comand(uint8_t*);
    void gps_UBX_command(uint8_t*);

    void gps_chksum(uint8_t, uint8_t*);
    
    void ubx_ReceiveDataByte(uint8_t);
    uint8_t ubx_CheckTimeout();
    uint8_t ubx_GetTimeout();
    
    void ubx_GetGPSData(gps_Data*);   //FIXME data type comes from a type enum?

    void PollJammingDetection(void);
    void ConfigJammingDetection(uint8_t*);


#ifdef	__cplusplus
}
#endif

#endif	/* CAM8MQ_H */

