/*
 * File:   CAM8MQ.h
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the header file for the ublox CAM-8MQ GNSS navigation module.
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


#ifndef CAM8MQ_H
#define	CAM8MQ_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef GPS_H
#define GPS_H



    void gps_comand(unsigned char*);    //TODO document this properly
     /* FUNCTION: gps_command()
     * PURPOSE : Query the internal jamming detection system of the CAM-8M
     * ARGUMENTS:
     *  Type    Name    Desc.
     *
     *
     * RETURN :
     *  Type    Name    Desc.
     *
     *
     * DEPENENCIES:
     * USARTTX();
     *
     * VARIABLES:
     *
     *
     * NOTES :
     * This function is only really required if using a user-interactive terminal session,
     * otherwise this can create additional bus congestion for no benefit.
     *
     * VER: 1.0
     * PROGRAMMER:
     * W. Anthony <will@affinityengineering.com.au>
     *
     * DATE: 13 MAY 2015
     *F*/


    void gps_chksum(unsigned char, unsigned char*); //TODO document properly
     /* FUNCTION: PollJammingDetection()
     * PURPOSE : Query the internal jamming detection system of the CAM-8M
     * ARGUMENTS:
     *  Type    Name    Desc.
     *
     *
     * RETURN :
     *  Type    Name    Desc.
     *
     *
     * DEPENENCIES:
     * USARTTX();
     *
     * VARIABLES:
     *
     *
     * NOTES :
     * This function is only really required if using a user-interactive terminal session,
     * otherwise this can create additional bus congestion for no benefit.
     *
     * VER: 1.0
     * PROGRAMMER:
     * W. Anthony <will@affinityengineering.com.au>
     *
     * DATE: 13 MAY 2015
     *F*/


#endif



#ifdef	__cplusplus
}
#endif

#endif	/* CAM8MQ_H */

