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
 * File:   eeprom.h
 * Author: Ciprian Stingu
 *
 * Created on July 31, 2020, 7:44 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t EepromReadByte(uint8_t address);
uint8_t EepromReadBytes(uint8_t address, uint8_t *data, uint8_t length);
void EepromWriteByte(uint8_t address, uint8_t data);
uint8_t EepromWriteBytes(uint8_t address, uint8_t *data, uint8_t length);
uint8_t EepromSavedTestExists(void);
void EepromLoadTest(void);
uint8_t EepromSaveTest(void);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

