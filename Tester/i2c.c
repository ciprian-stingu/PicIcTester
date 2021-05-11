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
 * File:   i2c.c
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:13 PM
 */

#include "main.h"
#include "i2c.h"

void I2cInit(void)
{
    // set I2C SCL/SDA as inputs
    TRISC3 = 1;
    TRISC4 = 1;
    
    // 1000 = I2C Master mode, clock = FOSC / (4 * (SSPADD + 1))
    SSPM3 = 1;
    SSPM2 = 0;
    SSPM1 = 0;
    SSPM0 = 0;

    SSPEN = 1; //Synchronous Serial Port Enable bit
    
    // Baud rate clock divider
    // 24 = 400k 
    // 99 = 100k
    SSPADD = 24; 
    
    // Slew Rate Control
    // 1 disabled for 100k & 1m
    // 0 enabled for 400k
    SMP = 0;
    
    __delay_ms(10);
}

void I2cStart(void)
{
    SSPIF = 0;
    SEN = 1; // Send start bit
    while(!SSPIF);
    //SSPIF = 0;
}

void I2cRestart(void)
{
    SSPIF = 0;
    RSEN = 1; // Send Restart bit
    while(!SSPIF);
    //SSPIF = 0;
}

void I2cStop(void)
{
    SSPIF = 0;
    PEN = 1; // Send stop bit
    while(!SSPIF);
    //SSPIF = 0;
}

void I2cSendAck(void)
{
    SSPIF = 0;
    ACKDT = 0; // 0 means ACK
    ACKEN = 1; // Send ACKDT value
    while(!SSPIF);
    //SSPIF = 0;
}

void I2cSendNack(void)
{
    SSPIF = 0;
    ACKDT = 1; // 1 means NACK
    ACKEN = 1; // Send ACKDT value
    while(!SSPIF);
    //SSPIF = 0;
}

__bit I2cWriteByte(uint8_t data)
{
    SSPIF = 0;
    SSPBUF = data; // Send data value
    while(!SSPIF);
    //SSPIF = 0;
    return ACKSTAT; // Return ACK/NACK from slave
}

uint8_t I2cReadByte(void)
{
    SSPIF = 0;
    RCEN = 1; // Enable reception of 8 bits
    while(!SSPIF);
    //SSPIF = 0;
    return SSPBUF; // Return received byte
}

void I2cSendControlByte(uint8_t address, uint8_t rw)
{
    SSPIF = 0;
    SSPBUF = (((0b1010 << 4) | (address << 1)) | (rw & 0x01));  // send the control byte
    while(!SSPIF);
    //SSPIF = 0;
}

uint16_t I2cReadString(uint8_t deviceAddress, uint16_t dataAddress, uint8_t maxLength, uint8_t terminator)
{
    if(maxLength == 0) {
        return dataAddress;
    }
    
    ClearString(I2cData, I2C_DATA_MAX_LENGTH + 1);
    I2cDataLength = 0;
    
    I2cStart();
    //select device & address
    I2cSendControlByte(deviceAddress, 0); //write
    I2cWriteByte((dataAddress >> 8) & 0xFF); // Write Address upper byte
    I2cWriteByte(dataAddress & 0xFF); // Write Address lower byte
    I2cRestart();
    
    //read data
    I2cSendControlByte(deviceAddress, 1); //read
    
    uint8_t count = 0;
    do
    {
        char data = I2cReadByte();
        //if the terminator char matches data
        //and address is > 0 or not first char read
        //or data indicates end of eeprom
        if((terminator != NULL_CHAR && data == terminator) || data == '&') 
        {
            I2cDataLength = I2C_DATA_MAX_LENGTH;
            I2cSendNack();
            if(data == '&') //here we have the end of eeprom data
            {
                I2cStop();
                return 0xFFFF;
            }
        }
        else
        {
            I2cData[count++] = data;
            if(count >= maxLength || count >= I2C_DATA_MAX_LENGTH) //just to be sure
            {
                I2cDataLength = I2C_DATA_MAX_LENGTH;
                I2cSendNack();
            }
            else {
                I2cSendAck();
            }
        }
    } while(I2cDataLength != I2C_DATA_MAX_LENGTH);
    
    I2cStop();
    
    return dataAddress + count + 1; //add 1 to the end to count the terminator char
}

uint16_t I2cReadDWord(uint8_t deviceAddress, uint16_t dataAddress)
{
    I2cStart();
    //select device & address
    I2cSendControlByte(deviceAddress, 0); //write
    I2cWriteByte((dataAddress >> 8) & 0xFF); // Write Address upper byte
    I2cWriteByte(dataAddress & 0xFF); // Write Address lower byte
    I2cRestart();
    
    //read data
    I2cSendControlByte(deviceAddress, 1); //read
    
    uint16_t result = I2cReadByte();
    I2cSendAck();
    result |= (I2cReadByte() << 8);
    I2cSendNack();
    
    I2cStop();
    
    return result;
}
