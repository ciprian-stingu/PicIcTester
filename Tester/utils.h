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
 * File:   utils.h
 * Author: ciprian
 *
 * Created on July 23, 2020, 4:22 PM
 */

#ifndef UTILS_H
#define	UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define INVALID_RESULT 0xFF

void UnsignedWordToAsciiInteger(uint16_t value, char *buffer);
void ByteToAsciiHex(uint8_t value, char *buffer);
void ClearString(char *data, uint8_t length);
uint8_t IsSameString(char *src, char *test, uint8_t length);
uint8_t CopyString(char *src, char *dst, uint8_t length);
uint8_t DeviceNameMatch(char *src, char *test);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

