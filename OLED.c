/*
 * File:   OLED.c
 * Author:  Eric Freund <eric@affinityengineering.com.au>
 *          Will Anthony <will@affinityengineering.com.au>
 * Description: This is the source file for the display module.
 * 
 *
 * Language: C (CCI)
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
 * Reference Document: 
 * Document Number: 
 * Revision / Date: 
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

/** INCLUDES *******************************************************/

#include "OLED.h"

//*******************************************************
extern volatile uint8_t crtX, crtY;
//*******************************************************

void initOLED(void)
{
    uint16_t i = 0;
    OLEDVDD = 1;
    BS1 = 0; //6800 parallel mode
    BS2 = 1; //6800 parallel mode
    CS = 1;
    D_C = 1;
    R_W = 1;
    E_R = 1;

    RES = 0;
    for (i = 0; i < 1000; i++);
    RES = 1;
    for (i = 0; i < 1000; i++);

    OLEDcmd(0xAE); //Display off
    OLEDcmd(0xD5);
    OLEDcmd(0xA0);
    OLEDcmd(0xA8);
    OLEDcmd(0x3F);
    OLEDcmd(0xD3);
    OLEDcmd(0x00);
    OLEDcmd(0xA0);
    OLEDcmd(0xAD);
    OLEDcmd(0x8E);
    OLEDcmd(0xD8);
    OLEDcmd(0x05);
    OLEDcmd(0xA1);
    OLEDcmd(0xC8);
    OLEDcmd(0xDA);
    OLEDcmd(0x12);
    OLEDcmd(0x91);
    OLEDcmd(0x3F);
    OLEDcmd(0x3F);
    OLEDcmd(0x3F);
    OLEDcmd(0x3F);
    OLEDcmd(0x81);
    OLEDcmd(0x80);
    OLEDcmd(0xD9);
    OLEDcmd(0xD2);
    OLEDcmd(0xDB);
    OLEDcmd(0x10);
    OLEDcmd(0x20);
    OLEDcmd(0x00);
    OLEDcmd(0xA4);
    OLEDcmd(0xA6);
    ClearOLED();
    OLEDcmd(0xAF);
    HVEN = 1;

}
//*******************************************************

void OLEDcmd(uint8_t DATA)
{
    //int i;
    D_C = 0; //Set Data or command line accordingly D = 1 C = 0
    R_W = 0; //We are writting
    E_R = 1; //E line low
    CS = 0;
    LATE = 0x00FF & DATA;
    CS = 1;
    D_C = 1;
    R_W = 1;
    E_R = 0;
}
//*******************************************************

void OLEDdata(uint8_t DATA)
{
    //int i;
    D_C = 1; //Set Data or command line accordingly D = 1 C = 0
    R_W = 0; //We are writting
    E_R = 1; //E line low
    CS = 0;
    LATE = 0x00FF & DATA;
    CS = 1;
    D_C = 1;
    R_W = 1;
    E_R = 0;
}
//*******************************************************

void ClearOLED(void)
{
    uint8_t i, j;

    for (i = 0; i < 8; i++)
    {
        OLEDcmd(0xB0 | i);
        OLEDcmd(0x02);
        OLEDcmd(0x11);

        for (j = 0; j < 96; j++)
        {
            OLEDdata(0x00);
        }
    }
}
//*******************************************************

void print_char(char c)// 6 x 8 pixels blank top and right
{
    uint8_t i = 0;
    for (i = 0; i < 5; i++)
    {
        OLEDdata(FontLookup[c - 32][i] << 1);
    }
    OLEDdata(0x00);

}
//*******************************************************

void print_string(char *message) // Write message to LCD (C string type) this will overun the edge
{
    while (*message) // Look for end of string
    {
        printSCN(*message++);
    }
}
//*******************************************************

void print_string_XY(char *message, uint8_t X, uint8_t Y) // Write message to LCD this will wrap at the edge
{
    //	Y = Y%8;
    //	if (X > (15 * 6)) {X = 0; Y++;}
    //	X = (X/6)*6;
    //	Gotoxy(X,Y);
    //	while (*message)			// Look for end of string
    //	{
    //		if(*message==0x0D) {Y++; X=0;Gotoxy(X,Y); *message++;}
    //		if(*message==0x0A) *message++;
    //		print_char(*message++);
    //		X += 6;
    //		if (X > 95) {X = 0; Y++;Gotoxy(X,Y);}
    //	}
    crtX = X,
            crtY = Y;
    Gotoxy(6 * crtX, crtY);
    while (*message) printSCN(*message++);
}
//*******************************************************

void Gotoxy(int8_t x, int8_t y)
{
    x = x + 0x12;
    OLEDcmd((0xB0 | (y & 0x0F))); // Y axis initialisation: 0100 yyyy			
    OLEDcmd((0x00 | (x & 0x0F))); // X axis initialisation: 0000 xxxx ( x3 x2 x1 x0)
    OLEDcmd((0x10 | ((x >> 4)&0x07))); // X axis initialisation: 0010 0xxx  ( x6 x5 x4)
}
//*******************************************************

void print_big_num(uint16_t num, uint16_t x, uint16_t y)
{
    /*******************************************************
    This routine only prints 14x24 numbers,
    when you print each number the x location must be incremented
    by 14 for each digit.
     *******************************************************/
    uint8_t i = 0, j = 0;

    for (j = 0; j < 3; j++)
    {
        Gotoxy(x, y + j);
        //		OLEDdata(0);
        OLEDdata(0);
        for (i = 0; i < 12; ++i)
        {
            OLEDdata(bignum[j][i + (num * 12)]);
        }
        //		OLEDdata(0);
        OLEDdata(0);
    }
}
//*******************************************************

void printSCN(char DATA) // Write char to LCD this will wrap at the edge and at the bottom
{
    uint8_t i = 0;
    if (DATA == 0x0A)
    {
        if (crtX < 15) for (i = crtX; i < 16; i++) print_char(0x20);
        crtX = 0;
        crtY++;
        if (crtY > 7) crtY = 0;
        Gotoxy(6 * crtX, crtY);
    }
    if ((DATA != 0x0A)&&(DATA != 0x0D))
    {
        print_char(DATA);
        crtX++;
    };
    if (crtX > 15)
    {
        crtX = 0;
        crtY++;
        if (crtY > 7) crtY = 0;
        Gotoxy(6 * crtX, crtY);
    }
}
