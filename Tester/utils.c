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
 * File:   utils.c
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:23 PM
 */


#include "main.h"
#include "utils.h"

void UnsignedWordToAsciiInteger(uint16_t value, char *buffer)
{
 	if(value)
 	{
        uint16_t divisor;
        uint8_t i, printed = FALSE;
        
 		for(i = 0, divisor = 10000; i < 5; i++)
 		{
 			uint16_t digit = value / divisor;
 			if(digit || printed)
 			{
 				*buffer++ = '0' + digit;
 				value -= digit * divisor;
 				printed = TRUE;
 			}
 			divisor /= 10;
 		}
 	}
 	else {
 		*buffer++ = '0';
 	}
 
 	*buffer = NULL_CHAR;
}

void ByteToAsciiHex(uint8_t value, char *buffer)
{
    uint8_t lsb = value & 0x0F;
    uint8_t msb = (value >> 4) & 0x0F;
    
    *buffer++ = msb <= 9 ? msb + '0' : (msb - 10) + 'A';
    *buffer++ = lsb <= 9 ? lsb + '0' : (lsb - 10) + 'A';
    *buffer = NULL_CHAR;
}

//uint8_t AsciiHexToByte(char *value)
//{
//    uint8_t result = (*value <= '9' ? *value - '0' : *value - 'A' + 10) << 4;
//    value++;
//    result |= (*value <= '9' ? *value - '0' : *value - 'A' + 10);
//    
//    return result;
//}

void ClearString(char *data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++) {
        data[i] = NULL_CHAR;
    }
}

uint8_t IsSameString(char *src, char *test, uint8_t length)
{
    uint8_t sameString = TRUE;
    
    for(uint8_t i = 0; i < length; i++)
    {
        if(src[i] != test[i]) 
        {
            sameString = FALSE;
            break;
        }
    }
    
    return sameString;
}

uint8_t CopyString(char *src, char *dst, uint8_t length)
{
    uint8_t i = 0;
    
    for(; i < length && src[i] != NULL_CHAR; i++) {
        dst[i] = src[i];
    }
    dst[i] = NULL_CHAR;
    
    return i;
}

uint8_t DeviceNameMatch(char *src, char *test)
{
    if(test[0] == '*') {
        return 1;
    }
    
	uint8_t i = 1, j = 0;
    
    while(src[i] != NULL_CHAR && test[j] != NULL_CHAR)
	{
        if(src[i] != test[j]) 
        {
            while(TRUE) 
            { 
                i++;
                if(src[i] == NULL_CHAR) {
                    return INVALID_RESULT;
                }
                else if(src[i] == '/') {
                    break;
                }
            }
            j = 0;
        }
        else 
        {
            if(test[j + 1] == '*') {
                return i - j;
            }
            j++;
        }
		i++;
	}

	return INVALID_RESULT;
}
