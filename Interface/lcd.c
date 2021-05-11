/**
 * 
 * PicIcTester Interface
 * 
 * Copyright (C) 2020-2021 Ciprian Stingu
 * ciprian dot stingu at gmail dot com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 * or see <http://www.gnu.org/licenses/>
 *
 * File:   lcd.c
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:07 PM
 */


/**
 * This file borrows most of code from http://www.toughdev.com/content/2011/03/interfacing-nokia-3510i-and-5110-lcd-with-pic-microcontroller/
 * Original copyrights to ToughDev
 */

#include "main.h"
#include "lcd.h"

void LcdInit(void)
{
    //set as outputs
    TRISA0 = 0;
	TRISA1 = 0;
	TRISA2 = 0;
	TRISA3 = 0;
    
    // LCD Hardware Reset
    LCD_RESET = 1;
	LCD_CS = 1;
	LCD_SDATA = 0;
	LCD_SCLK = 1;
	LCD_RESET = 0;
	__delay_ms(50);
	LCD_RESET = 1;
	__delay_ms(50);
	
    // NOP
    LcdSendData(0x00, LCD_CMD); 
    __delay_ms(5); 
    
    // LCD Software Reset 
    LcdSendData(0x01, LCD_CMD);      
    __delay_ms(5); 
    
    // Initial Escape
    LcdSendData(0xC6, LCD_CMD);       
    
    // Refresh set 
    LcdSendData(0xB9, LCD_CMD);
    LcdSendData(0x00, LCD_DATA); 
    
//    LcdVersion = LcdGetVersion();
    
    // Gamma Curve Set
    LcdSendData(0xB5, LCD_CMD);
    LcdSendData(0x01, LCD_DATA);
    
    //common driver output select
    LcdSendData(0xBD, LCD_CMD);
    if(LcdVersion == 'D') {
        LcdSendData(0x04, LCD_DATA);
    }
    else {
        LcdSendData(0x00, LCD_DATA);
    }
    
    // Power Control
    LcdSendData(0xBE, LCD_CMD); 
    LcdSendData(0x03, LCD_DATA);
    
    // Sleep out
    LcdSendData(0x11, LCD_CMD);
    
    // Power Control
    LcdSendData(0xBA, LCD_CMD);
    LcdSendData(0x7F, LCD_DATA); 
    LcdSendData(0x03, LCD_DATA);
    
    // RGB
    LcdSendData(0x36, LCD_CMD);
    if(LcdVersion == 'A') {
        LcdSendData(0x08, LCD_DATA);
    }
    else {
        LcdSendData(0x00, LCD_DATA);
    }
    
    // Temperature gradient set
    LcdSendData(0xB7, 0);
    for(uint8_t i = 0; i < 14; i++) { 
        LcdSendData(0, LCD_DATA); 
    } 
    
    //display ON
    LcdSendData(0x29, LCD_CMD);
    
    // Booster Voltage ON
    LcdSendData(0x03, LCD_CMD);
    
    // Booster Voltage stability 
    __delay_ms(20);     
    
    //display inversion OFF
    LcdSendData(0x20, LCD_CMD);
    
    // Write contrast
    LcdSendData(0x25, LCD_CMD);
    switch(LcdVersion)							   
    {
   		case 'A':		
			LcdSendData(70, LCD_DATA);  
			break;
   		case 'B':	  
			LcdSendData(67, LCD_DATA);  
			break;
		case 'C':
		   	LcdSendData(66, LCD_DATA);  
			break;
		case 'D':		  
			LcdSendData(49, LCD_DATA); 
			break;	
    }
    
    //interface pixel format
    LcdSendData(0x3A, LCD_CMD);
    LcdSendData(0x02, LCD_DATA);
    LcdSendData(0x2D, LCD_CMD);

    // lcd color set
    if(LcdVersion == 'B' || LcdVersion == 'C')
    {
        //red
        LcdSendData(~0x00, LCD_DATA);
        LcdSendData(~0x02, LCD_DATA);
        LcdSendData(~0x03, LCD_DATA);
        LcdSendData(~0x04, LCD_DATA);
        LcdSendData(~0x05, LCD_DATA);
        LcdSendData(~0x06, LCD_DATA);
        LcdSendData(~0x08, LCD_DATA);
        LcdSendData(~0x0F, LCD_DATA);

        //green
        LcdSendData(~0x00, LCD_DATA);
        LcdSendData(~0x02, LCD_DATA);
        LcdSendData(~0x03, LCD_DATA);
        LcdSendData(~0x04, LCD_DATA);
        LcdSendData(~0x05, LCD_DATA);
        LcdSendData(~0x06, LCD_DATA);
        LcdSendData(~0x08, LCD_DATA);
        LcdSendData(~0x0F, LCD_DATA);

        //blue
        LcdSendData(~0x00, LCD_DATA);
        LcdSendData(~0x03, LCD_DATA);
        LcdSendData(~0x06, LCD_DATA);
        LcdSendData(~0x0F, LCD_DATA);
    }
    else
    {
        //red
        LcdSendData(0x00, LCD_DATA);
        LcdSendData(0x02, LCD_DATA);
        LcdSendData(0x03, LCD_DATA);
        LcdSendData(0x04, LCD_DATA);
        LcdSendData(0x05, LCD_DATA);
        LcdSendData(0x06, LCD_DATA);
        LcdSendData(0x08, LCD_DATA);
        LcdSendData(0x0F, LCD_DATA);

        //green
        LcdSendData(0x00, LCD_DATA);
        LcdSendData(0x02, LCD_DATA);
        LcdSendData(0x03, LCD_DATA);
        LcdSendData(0x04, LCD_DATA);
        LcdSendData(0x05, LCD_DATA);
        LcdSendData(0x06, LCD_DATA);
        LcdSendData(0x08, LCD_DATA);
        LcdSendData(0x0F, LCD_DATA);

        //blue
        LcdSendData(0x00, LCD_DATA);
        LcdSendData(0x03, LCD_DATA);
        LcdSendData(0x06, LCD_DATA);
        LcdSendData(0x0F, LCD_DATA);
    }
    
    //clear screen
    LcdClear(0, 0, LCD_WIDTH, LCD_HEIGHT, 255);
}

void LcdSendData(uint8_t data, uint8_t operation) 
{
	LCD_CS = 0;
	LCD_SCLK = 0;
	LCD_SDATA = operation == LCD_CMD ? 0 : 1;
	LCD_SCLK = 1;

	for(uint8_t i = 0; i < 8; i++) 
    {
		LCD_SCLK = 0;
        LCD_SDATA = data & 0x80 ? 1 : 0;
		LCD_SCLK = 1;
		data = data << 1;
	}
    LCD_CS = 1;
}

// Call this to get access to a particular region on the LCD before writing the pixel value
void LcdAddSet(uint8_t topX, uint8_t topY, uint8_t bottomX, uint8_t bottomY)
{
    //column address set
    LcdSendData(0x2A, LCD_CMD);
    LcdSendData(topX, LCD_DATA);
    LcdSendData(bottomX, LCD_DATA);
    //page address set
    LcdSendData(0x2B, LCD_CMD);
    LcdSendData(topY, LCD_DATA);
    LcdSendData(bottomY, LCD_DATA);
    //memory write
    LcdSendData(0x2C, LCD_CMD);
}

//Fill the entire LCD screen with the given color value
void LcdClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bgColor)
{
    LcdAddSet(x, y, x + width - 1, y + height - 1);

    for(uint8_t i = 0; i < height; i++)
    {
        for(uint8_t j = 0; j < width; j++) {
            LcdSendData(bgColor, LCD_DATA);
        }
    }
}

void LcdDisplayGraphic(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor, char *data)
{
    if(x + width > LCD_WIDTH || y + height > LCD_HEIGHT) { //sanity check
        return;
    }
    
    LcdAddSet(x, y, x + width - 1, y + height - 1);

    uint16_t dataIndex = 0;
    for(uint8_t yIndex = 0; yIndex < height; yIndex++)
    {
        for(uint8_t xIndex = 0; xIndex < width; xIndex += 8) //8 bits per byte!!!
        {
            uint8_t graphData = AsciiHexToByte(&data[dataIndex]);
            uint8_t bitMax = 0;
            
            if(xIndex + 8 >= width) {
                bitMax = 8 - (width - xIndex);
            }
            
            for(int8_t bitIndex = 7; bitIndex >= bitMax; bitIndex--)
            {
                if(graphData & (1 << bitIndex)) {
                    LcdSendData(color, LCD_DATA);
                }
                else {
                    LcdSendData(bgColor, LCD_DATA);
                }
            }
            
            dataIndex += 2;
        }
    }
}

//uint8_t LcdGetVersion(void)  
//{    
//    // Read display status 
//    LcdSendData(0x09, 0);
//    
//    TRISA1 = 1; //set CS as input
//    LCD_CS = 0;
//    LCD_SCLK = 0;
//    
//    // will read 8 bits of dummy data
//    // then
//    // will read 32 bits of data
//    uint8_t lcdStatus[5];
//    for(uint8_t i = 0; i < 5; i++)
//    {
//        lcdStatus[i] = 0x00;
//        for(uint8_t j = 0; j <= 7; j++)
//        {
//            LCD_SCLK = 1;
//            if(LCD_SDATA == 0) {
//                lcdStatus[i] |= (1 << (7 - j)) & 0xFF;
//            }
//            LCD_SCLK = 0;
//        }
//    }
//    LCD_SCLK = 1;
//    TRISA1 = 0;
//    LCD_CS = 1;
//    
//    if(lcdStatus[0] == 0xFF) {
//        return 'A';
//    }
//    else if(lcdStatus[0] == 0x7F) {
//        return 'B';
//    }
//    else
//    {
//        if(lcdStatus[1] == 0 && lcdStatus[2] == 0 && lcdStatus[3]) {
//            return 'D';
//        }
//        else {
//            return 'C';
//        }
//    }
//}
