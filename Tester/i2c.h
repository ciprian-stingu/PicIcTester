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
 * File:   i2c.h
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:12 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EEPROM_ADDRESS 0x00
#define I2C_DATA_MAX_LENGTH 126
char I2cData[I2C_DATA_MAX_LENGTH + 1];
uint8_t I2cDataLength = 0;

void I2cInit(void);
void I2cStart(void);
void I2cRestart(void);
void I2cStop(void);
void I2cSendAck(void);
void I2cSendNack(void);
__bit I2cWriteByte(uint8_t data);
uint8_t I2cReadByte(void);
void I2cSendControlByte(uint8_t address, uint8_t rw);
uint16_t I2cReadString(uint8_t deviceAddress, uint16_t dataAddress, uint8_t maxLength, uint8_t terminator);
uint16_t I2cReadDWord(uint8_t deviceAddress, uint16_t dataAddress);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

