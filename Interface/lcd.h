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
 * File:   lcd.h
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:07 PM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LCD_RESET  RA0
#define LCD_CS     RA1
#define LCD_SCLK   RA2
#define LCD_SDATA  RA3
#define LCD_CMD    0
#define LCD_DATA   1
#define LCD_WIDTH  98
#define LCD_HEIGHT 67
uint8_t LcdVersion = 'D';
    
void LcdInit(void);
void LcdSendData(uint8_t data, uint8_t operation);
void LcdAddSet(uint8_t topX, uint8_t topY, uint8_t bottomX, uint8_t bottomY);
void LcdClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bgColor);
void LcdDisplayGraphic(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor, char *data);
//uint8_t LcdGetVersion(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

