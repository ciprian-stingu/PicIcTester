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
 * File:   eeprom.c
 * Author: Ciprian Stingu
 *
 * Created on July 31, 2020, 7:44 PM
 */


#include "main.h"
#include "eeprom.h"

uint8_t EepromSavedTestExists(void)
{
    uint8_t saveHeader1 = EepromReadByte(0);
    uint8_t saveHeader2 = EepromReadByte(1);
    
    return saveHeader1 == 0x69 && saveHeader2 == 0x96 ? TRUE : FALSE;
}

void EepromLoadTest(void)
{
    if(!EepromSavedTestExists()) {
        return;
    }
    
    DrawWindowOnInterface(16, 30, 66, 14, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
    DisplayStringOnInterface(19, 34, COLOR_BLACK, COLOR_WHITE, "Loading...");
                    
    uint8_t address = 2;
    
    // read test name
    ClearString(CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
    address = EepromReadBytes(address, (uint8_t *)&CommonTestName[0], SELECTED_DEVICE_NAME_MAX_LENGTH);
    
    // real test pins
    CommonTestPinCount = EepromReadByte(address++);
    
    // read test items count
    CommonTestCasesCount = EepromReadByte(address++);

    // read test items
    for(uint8_t i = 0; i < CommonTestCasesCount; i++) {
        address = EepromReadBytes(address, CommonTestCases[i], CommonTestCasesCount / 2);
    }
}

uint8_t EepromSaveTest(void)
{
    if(CommonTestPinCount == 0 || CommonTestCasesCount == 0 || CommonTestName[0] == NULL_CHAR) {
        return FALSE;
    }
 
    DrawWindowOnInterface(19, 30, 60, 14, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
    DisplayStringOnInterface(22, 34, COLOR_BLACK, COLOR_WHITE, "Saving...");
                    
    uint8_t address = 0;
    // write header
    EepromWriteByte(address++, 0x69);
    EepromWriteByte(address++, 0x96);
    
    //write test name
    address = EepromWriteBytes(address, (uint8_t *)&CommonTestName[0], SELECTED_DEVICE_NAME_MAX_LENGTH);
    
    // write test pins
    EepromWriteByte(address++, CommonTestPinCount);
    
    uint8_t savedCases = 0;
    // we have only 240 bytes of eeprom data available for test cases
    if((CommonTestPinCount / 2) * CommonTestCasesCount > 240) {
        savedCases = 240 / (CommonTestPinCount / 2);
    }
    else {
        savedCases = CommonTestCasesCount;
    }
    // write test items count
    EepromWriteByte(address++, savedCases);
    
    // write test items
    for(uint8_t i = 0; i < savedCases; i++) {
        address = EepromWriteBytes(address, CommonTestCases[i], CommonTestPinCount / 2);
    }
    
    return TRUE;
}

uint8_t EepromReadByte(uint8_t address)
{
    EEADR = address;	  // read data address
    EECON1bits.WREN = 0;  // Inhibits write to the EEPROM
    EECON1bits.EEPGD = 0; // Access Data EEPROM memory
    EECON1bits.RD = 1;    // Initiates an EEPROM read

    return EEDATA;
}

uint8_t EepromReadBytes(uint8_t address, uint8_t *data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++) {
        *data++ = EepromReadByte(address++);
    }
    
    return address;
}

void EepromWriteByte(uint8_t address, uint8_t data)
{
    EEADR = address;	  // write address
    EEDATA = data;	      // data to be written
    EECON1bits.EEPGD = 0; // Access Data EEPROM memory
    EECON1bits.CFGS = 0;  // Access FLASH Program or Data EEPROM memory
    EECON1bits.WREN = 1;  // Allows write cycles
    uint8_t INTCON_backup = INTCON; //Backup INCON interupt register
    INTCON = 0; //Disables the interrupt

    // Required sequence for write to internal EEPROM
    EECON2 = 0x55;
    EECON2 = 0xAA;
    
    EECON1bits.WR = 1; // Initiates a data EEPROM erase/write cycle
    INTCON = INTCON_backup; //Enables Interrupt
    EECON1bits.WREN = 0; // Inhibits write to the EEPROM
    
    while(PIR2bits.EEIF == 0); // Wait for write operation complete
    PIR2bits.EEIF = 0;	// Reset EEIF    
}

uint8_t EepromWriteBytes(uint8_t address, uint8_t *data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        EepromWriteByte(address, data[i]);
        address++;
    }
    
    return address;
}
