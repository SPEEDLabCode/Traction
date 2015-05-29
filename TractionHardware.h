/* 
 * File:   TractionHardware.h
 * Author: Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This header file contains all of the hardware specific 
 * definitions for the SPEEDLab Traction module.  In conjunction with the 
 * schematic documentation, this will provide developers with the information 
 * required to modify the Traction configuration or troubleshoot problems should
 * they arise.
 *
 * Language: C
 * Coding Standard: NASA JPL DOCID D-60411
 * Style: NASA SEL-94-003
 * License: GNU GPL version 3.0
 * Version: 1.0
 *
 * Dependencies:
 * 
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

/* Control Lines */
#define SIM800PWR _LATB0
#define NRESET _LATB4
#define GPSPWR _LATB9

/* For OLED Screen operation */
#define HVEN	_LATC13     //Screen PSU Enable
#define	OLEDVDD _LATC14
#define BS1		_LATD1
#define BS2		_LATD2
#define CS		_LATD3
#define	RES		_LATD4
#define D_C		_LATD5
#define R_W		_LATD6
#define E_R		_LATD7
