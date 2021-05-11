/**
 * 
 * PicIcTester Tester
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
 * File:   graph.c
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:19 PM
 */


#include "main.h"
#include "graph.h"

// graphicData size == (DWORD size * LINES_PER_CHARS * 2) + 1 == 49 bytes
uint8_t GetCharGraphicData(char *message, uint8_t messageLength, char *graphicData)
{
    if(messageLength >= MAX_CHARS_PER_GRAPHIC_DATA) {
        messageLength = MAX_CHARS_PER_GRAPHIC_DATA;
    }
    
    uint8_t ordonatedCharData[MAX_CHARS_PER_GRAPHIC_DATA * LINES_PER_CHARS];
    for(uint8_t i = 0; i < messageLength; i++) 
    {
        char value = message[i];
        
        char *charData;
        uint8_t index = 0;
        if(value >= '0' && value <= '9') 
        {
            charData = &CHAR_DIGITS[0];
            index = (value - '0') * 6;
        }
        else if(value >= 'A' && value <= 'Z') 
        {
            charData = &CHAR_AZ[0];
            index = (value - 'A') * 6;
        }
        else if(value >= 'a' && value <= 'z') 
        {
            charData = &CHAR_az[0];
            index = (value - 'a') * 6;
        }
        else
        {
            switch(value)
            {
//                case '<':
//                    charData = CHAR_LT;
//                    break;
//                case '=':
//                    charData = CHAR_EQ;
//                    break;
                case '>':
                    charData = CHAR_GT;
                    break;
                case ':':
                    charData = CHAR_DOTS;
                    break;
                case '.':
                    charData = CHAR_DOT;
                    break;
//                case '^':
//                    charData = CHAR_UP;
//                    break;
                case '*':
                    charData = CHAR_STAR;
                    break;
                case ' ':
                    charData = CHAR_SPACE;
                    break;
                case '[':
                    charData = CHAR_LSP;
                    break;
                case ']':
                    charData = CHAR_RSP;
                    break;
                case '-':
                    charData = CHAR_MINUS;
                    break;
                case '/':
                    charData = CHAR_SLASH;
                    break;
                case '+':
                    charData = CHAR_PLUS;
                    break;
                case '&':
                    charData = CHAR_AMP;
                    break;
                case '?':
                default:
                    charData = CHAR_QMARK;
            }
        }
        
        // arrange char bytes
        // from
        // char1 = {b1, b2, b3, b4, b5, b6 }
        // char2 = {b1, b2, b3, b4, b5, b6 }
        // char3 = {b1, b2, b3, b4, b5, b6 }
        // to
        // data = {ch1b1, ch2b1, ch3b1, ch1b2, ch2b2, ch3b2, ch1b3 ... }
        for(uint8_t j = 0; j < LINES_PER_CHARS; j++) {
            ordonatedCharData[i + messageLength * j] = charData[index + j];
        }
    }
    
    // compressed bytes based on chars used
    uint8_t dwordUsableIndexes = 1, widthInBits = 6;
    switch(messageLength)
    {
        //case 1: //default 1
        case 2:
            dwordUsableIndexes = 2;
            widthInBits = 12;
            break;
        case 3:
            dwordUsableIndexes = 3;
            widthInBits = 18;
            break;
        case 4:
            dwordUsableIndexes = 3;
            widthInBits = 24;
            break;
        case 5:
            dwordUsableIndexes = 4;
            widthInBits = 30;
            break;
    }
    
    for(uint8_t charLineIndex = 0, graphicByteCount = 0; charLineIndex < LINES_PER_CHARS; charLineIndex++)
    {
        uint32_t lineData = 0x00;
        // compress a line from charLength bytes to a DWORD
        for(uint8_t charIndex = 0; charIndex < messageLength; charIndex++)
        {
             //remove first and last bit
            uint8_t charByte = (ordonatedCharData[charLineIndex * messageLength + charIndex] << 1) & 0xFC;
            // based on char index find the shift amount needed to compress the data into a DWORD
            uint8_t shiftAmount = 0;
            switch(charIndex)
            {
                case 0:
                    shiftAmount = 24;
                    break;
                case 1:
                    shiftAmount = 18;
                    break;
                case 2:
                    shiftAmount = 12;
                    break;
                case 3:
                    shiftAmount = 6;
                    break;
                case 4:
                    shiftAmount = 0;
                    break;
            }
            // add each byte to DWORD
            lineData |= ((uint32_t)charByte) << shiftAmount;
        }
        
        char *charLineData = (char *)&lineData; //treat the dword as 4 bytes
        for(int8_t k = dwordUsableIndexes - 1; k >= 0; k--, graphicByteCount += 2 /* 2 bytes in ascii hex */) {
            ByteToAsciiHex(charLineData[k + 4 - dwordUsableIndexes], &graphicData[graphicByteCount]);
        }
    }
    
    return widthInBits;
}

void DisplayStringOnInterface(uint8_t x, uint8_t y, uint8_t color, uint8_t bgColor, char *message)
{
    uint8_t messageLength = 0;
    while(messageLength < MAX_CHARS_PER_INTERFACE_MESSAGE && message[messageLength] != NULL_CHAR) {
        messageLength++;
    }
    
    for(uint8_t chunk = 0; chunk < messageLength; chunk += MAX_CHARS_PER_GRAPHIC_DATA)
    {
        uint8_t currentChunkLength = chunk + MAX_CHARS_PER_GRAPHIC_DATA < messageLength ? MAX_CHARS_PER_GRAPHIC_DATA : messageLength - chunk;
        char hexBuffer[3], graphicData[49] /* (DWORD size * LINES_PER_CHARS * 2) + 1 */;
        
        ClearString(graphicData, 49);
        
        uint8_t graphWidth = GetCharGraphicData(&message[chunk], currentChunkLength, graphicData);
        
        UartWriteString("_G ");
        ByteToAsciiHex(x, hexBuffer);
        UartWriteString(hexBuffer);
        ByteToAsciiHex(y, hexBuffer);
        UartWriteString(hexBuffer);
        ByteToAsciiHex(graphWidth, hexBuffer);
        UartWriteString(hexBuffer);
        ByteToAsciiHex(LINES_PER_CHARS, hexBuffer);
        UartWriteString(hexBuffer);
        ByteToAsciiHex(color, hexBuffer);
        UartWriteString(hexBuffer);
        ByteToAsciiHex(bgColor, hexBuffer);
        UartWriteString(hexBuffer);
        UartWriteString(" ");
        UartWriteString(graphicData);
        
        InterfaceCmdStatus = INTERFACE_CMD_READY;
        UartWriteString("\n");        
        while(InterfaceCmdStatus != INTERFACE_CMD_DONE) {
            __delay_us(25);
        }
        
        x += graphWidth;
    }
}

void ClearRectangleOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bgColor)
{
    char hexBuffer[3];
    
    UartWriteString("_C ");
    ByteToAsciiHex(x, hexBuffer);
    UartWriteString(hexBuffer);
    ByteToAsciiHex(y, hexBuffer);
    UartWriteString(hexBuffer);
    ByteToAsciiHex(width, hexBuffer);
    UartWriteString(hexBuffer);
    ByteToAsciiHex(height, hexBuffer);
    UartWriteString(hexBuffer);
    ByteToAsciiHex(bgColor, hexBuffer);
    UartWriteString(hexBuffer);
    
    InterfaceCmdStatus = INTERFACE_CMD_READY;
    UartWriteString("\n");
    while(InterfaceCmdStatus != INTERFACE_CMD_DONE) {
        __delay_us(25);
    }
}

void DrawLineOnInterface(uint8_t x, uint8_t y, uint8_t type, uint8_t length, uint8_t color)
{
    //we have only 31 bytes of graphic data available
    if((type == LINE_VERTICAL && length > VERTICAL_LINE_MAX_HEIGHT) || (type == LINE_HORIZONTAL && length > HORIZONTAL_LINE_MAX_WIDTH)) {
        return;
    }
    
    char hexBuffer[3];
    
    UartWriteString("_G ");
    ByteToAsciiHex(x, hexBuffer);
    UartWriteString(hexBuffer);
    ByteToAsciiHex(y, hexBuffer);
    UartWriteString(hexBuffer);
    if(type == LINE_HORIZONTAL)
    {
        ByteToAsciiHex(length, hexBuffer);
        UartWriteString(hexBuffer);
        UartWriteString("01");
    }
    else
    {
        UartWriteString("01");
        ByteToAsciiHex(length, hexBuffer);
        UartWriteString(hexBuffer);
    }
    ByteToAsciiHex(color, hexBuffer);
    UartWriteString(hexBuffer);
    UartWriteString("FF ");

    if(type == LINE_HORIZONTAL)
    {
        for(uint8_t i = 0; i <= length; i += 8) { //max of 26 bytes of data will be sent
            UartWriteString("FF");
        }
    }
    else
    {
        for(uint8_t i = 0; i < length; i++) { //max of 62 bytes of data will be sent
            UartWriteString("80");
        }
    }
    
    InterfaceCmdStatus = INTERFACE_CMD_READY;
    UartWriteString("\n");
    while(InterfaceCmdStatus != INTERFACE_CMD_DONE) {
        __delay_us(25);
    }
}

void DrawRectangleOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor)
{
    ClearRectangleOnInterface(x, y, width, height, bgColor);
    
    DrawLineOnInterface(x, y, LINE_HORIZONTAL, width, color);
    if(height > VERTICAL_LINE_MAX_HEIGHT) 
    {
        DrawLineOnInterface(x, y, LINE_VERTICAL, VERTICAL_LINE_MAX_HEIGHT, color);
        DrawLineOnInterface(x, y + VERTICAL_LINE_MAX_HEIGHT, LINE_VERTICAL, height - VERTICAL_LINE_MAX_HEIGHT, color);
    }
    else {
        DrawLineOnInterface(x, y, LINE_VERTICAL, height, color);
    }
    DrawLineOnInterface(x, y + height - 1, LINE_HORIZONTAL, width, color);
    if(height > VERTICAL_LINE_MAX_HEIGHT) 
    {
        DrawLineOnInterface(x + width - 1, y, LINE_VERTICAL, VERTICAL_LINE_MAX_HEIGHT, color);
        DrawLineOnInterface(x + width - 1, y + VERTICAL_LINE_MAX_HEIGHT, LINE_VERTICAL, height - VERTICAL_LINE_MAX_HEIGHT, color);
    }
    else {
        DrawLineOnInterface(x + width - 1, y, LINE_VERTICAL, height, color);
    }
}

void DrawWindowOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor, uint8_t shadowColor)
{
    DrawRectangleOnInterface(x, y, width, height, color, bgColor);
    
    for(uint8_t i = 0; i < 2; i++) {
        DrawLineOnInterface(x + 3, y + height + i, LINE_HORIZONTAL, width, shadowColor);
    }
    
    for(uint8_t i = 0; i < 3; i++) 
    {
        if(height > VERTICAL_LINE_MAX_HEIGHT)
        {
            DrawLineOnInterface(x + width + i, y + 3, LINE_VERTICAL, VERTICAL_LINE_MAX_HEIGHT, shadowColor);
            DrawLineOnInterface(x + width + i, y + 3 + VERTICAL_LINE_MAX_HEIGHT, LINE_VERTICAL, height - VERTICAL_LINE_MAX_HEIGHT - 1, shadowColor);
        }
        else {
            DrawLineOnInterface(x + width + i, y + 3, LINE_VERTICAL, height - 1, shadowColor);
        }
    }
}
