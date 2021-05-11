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
 * File:   test.c
 * Author: Ciprian Stingu
 *
 * Created on July 31, 2020, 4:55 PM
 */


#include "main.h"
#include "test.h"


void ReadDatabaseInfo(void)
{
    // read test data db version
    DatabaseVersion = I2cReadDWord(EEPROM_ADDRESS, 0);
    
    //read number of devices
    TestDataDevicesCount = I2cReadDWord(EEPROM_ADDRESS, 2);
    
    //read number of tests
    TestDataRecordsCount = I2cReadDWord(EEPROM_ADDRESS, 4);
    if(TestDataRecordsCount > TEST_DATA_MAX_RECORDS) {
        TestDataRecordsCount = TEST_DATA_MAX_RECORDS;
    }
}

void SearchDeviceInEeprom(void)
{
    for(uint8_t i = 0; i < SEARCH_RESULTS; i++)
    {
        ClearString(SearchResultNames[i], SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
        SearchResultIndexes[i] = TEST_DATA_INVALID_RECORD;
    }
    SearchResultsCount = 0;
    
    for(uint16_t recordIndex = 0; recordIndex < TestDataRecordsCount; recordIndex++)
    {
        uint16_t indexAddress = recordIndex * 2 + TEST_DATA_INDEX_START;
        uint16_t recordAddress = I2cReadDWord(EEPROM_ADDRESS, indexAddress);
        
        I2cReadString(EEPROM_ADDRESS, recordAddress, I2C_DATA_MAX_LENGTH, '\n');        
        
        uint8_t found = DeviceNameMatch(I2cData, SearchTerm);
        if(found != INVALID_RESULT)
        {
            for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH && I2cData[found + i] != NULL_CHAR; i++) {
                SearchResultNames[SearchResultsCount][i] = I2cData[found + i];
            }
            SearchResultIndexes[SearchResultsCount++] = recordAddress;
        }
        
        if(SearchResultsCount >= SEARCH_RESULTS) {
            break;
        }
    }
}

uint8_t TransformTestData(uint8_t *test, char *data, uint8_t pinCount)
{
    uint8_t bytesCount = pinCount / 2, realPinCount = 0;
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++) {
        test[i] = 0x00;
    }

    for(uint8_t i = 0; i < bytesCount; i++)
    {
        if(*data == NULL_CHAR) {
            break;
        }
        uint8_t pin1 = GetPinFromChar(*data++);
        if(pin1 == PIN_NONE) {
            break;
        }
        realPinCount++;
        
        if(*data == NULL_CHAR) {
            break;
        }
        uint8_t pin2 = GetPinFromChar(*data++);
        if(pin2 == PIN_NONE) {
            break;
        }
        realPinCount++;
        
        test[i] = ((pin1 << 4) & 0xF0) | (pin2 & 0x0F); 
    }
    
    return realPinCount;
}

uint8_t GetPinFromChar(char data)
{
    uint8_t pin = PIN_NONE;
    
    switch(data)
    {
        case PIN_GND_CHAR:
            pin = PIN_GND;
            break;
        case PIN_VCC_CHAR:
            pin = PIN_VCC;
            break;
        case PIN_IN_LOW_CHAR:
            pin = PIN_IN_LOW;
            break;
        case PIN_IN_HIGH_CHAR:
            pin = PIN_IN_HIGH;
            break;
        case PIN_OUT_LOW_CHAR:
            pin = PIN_OUT_LOW;
            break;
        case PIN_OUT_HIGH_CHAR:
            pin = PIN_OUT_HIGH;
            break;
        case PIN_IGNORE_CHAR:
            pin = PIN_IGNORE;
            break;
        case PIN_CLOCK_CHAR:
            pin = PIN_CLOCK;
            break;
    }
    
    return pin;
}

void StartTest(uint8_t screenSource)
{
    DrawWindowOnInterface(16, 30, 66, 14, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
    DisplayStringOnInterface(19, 34, COLOR_BLACK, COLOR_WHITE, "Testing...");
    
    TestedCases = 0;
    LastTestCaseResult = 0;
    
    uint16_t testCasesAddress = 0;
    if(screenSource == TEST_SCREEN) // load test data
    {
        uint16_t address = SelectedDeviceIndex;

        address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //ic name
        ClearString(CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
        CopyString(SelectedDeviceName, CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH);

        address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //ignore description

        address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //pins
        CommonTestPinCount = (I2cData[0] - '0') * 10 + (I2cData[1] - '0');
        
        testCasesAddress = address; // save test cases start address
        
        //tests count
        CommonTestCasesCount = 0;
        do
        {
            address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n');
            CommonTestCasesCount++;
        } while(I2cData[0] != '$' && I2cData[0] != NULL_CHAR); //no limit in test cases when reading from eeprom
    }
    
    if(CommonTestCasesCount == 0 || CommonTestPinCount == 0 || (CommonTestPinCount % 2) != 0 || CommonTestPinCount > COMMON_TEST_MAX_PINS) 
    {
        TestedCases = INVALID_RESULT;
        LastTestCaseResult = INVALID_RESULT;
        ActiveScreen = TEST_RESULT_SCREEN;
        ReturnToTestScreen = screenSource;
        ScreenChanged = TRUE;
        return;
    }
    
    if(screenSource == TEST_SCREEN) //load first test case
    {
        I2cReadString(EEPROM_ADDRESS, testCasesAddress, I2C_DATA_MAX_LENGTH, '\n');
        TransformTestData(CommonTestCases[0], I2cData, CommonTestPinCount);
    }
    
    uint8_t powerConfig = GetPowerPinsConfig(CommonTestCases[0], CommonTestPinCount);
    //set power config
    SetPowerConfig(powerConfig);

    __delay_ms(100);
    
    for(; TestedCases < CommonTestCasesCount; TestedCases++)
    {
        uint8_t *caseData;
        if(screenSource == TEST_SCREEN)
        {
            testCasesAddress = I2cReadString(EEPROM_ADDRESS, testCasesAddress, I2C_DATA_MAX_LENGTH, '\n');
            TransformTestData(CommonTestCases[0], I2cData, CommonTestPinCount);
            caseData = CommonTestCases[0];
        }
        else {
            caseData = CommonTestCases[TestedCases];
        }
        
        LastTestCaseResult = RunTestCase(caseData, CommonTestPinCount);
        if(LastTestCaseResult > 0) { // error
            break;
        }
        
        __delay_ms(50);
    }
    
    
    // disable the relays
    ResetHardware(TRUE);
    __delay_ms(100);
    
    ActiveScreen = TEST_RESULT_SCREEN;
    ReturnToTestScreen = screenSource;
    ScreenChanged = TRUE;
}

uint8_t RunTestCase(uint8_t *caseData, uint8_t pinCount)
{    
    // set ic pin function (GND/VCC/IN/OUT/CLK)
    for(uint8_t i = 0, j = pinCount / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, (caseData[i] & 0xF0) >> 4, i * 2 + 1, pinCount);
        PinIoOperation(IO_OP_SET_FUNCTION, caseData[i] & 0x0F, i * 2 + 2, pinCount);
    }

    __delay_ms(5);

    // increment the clock if needed
    for(uint8_t i = 0, j = pinCount / 2; i < j; i++)
    {
        if((caseData[i] & 0xF0) >> 4 == PIN_CLOCK) 
        {
            PinIoOperation(IO_OP_CLOCK, 1, i * 2 + 1, pinCount);
            __delay_ms(10);
            PinIoOperation(IO_OP_CLOCK, 0, i * 2 + 1, pinCount);
        }
        if((caseData[i] & 0x0F) == PIN_CLOCK)
        {
            PinIoOperation(IO_OP_CLOCK, 1, i * 2 + 2, pinCount);
            __delay_ms(10);
            PinIoOperation(IO_OP_CLOCK, 0, i * 2 + 2, pinCount);
        }
    }

    __delay_ms(5);

    // read the ic output
    uint8_t caseResult = 0;
    
    for(uint8_t i = 0, j = pinCount / 2; i < j; i++)
    {
        uint8_t outPin = (caseData[i] & 0xF0) >> 4, result = 0;
        if(outPin == PIN_OUT_LOW)
        {
            result = PinIoOperation(IO_OP_READ_PIN, 0, i * 2 + 1, pinCount);
            if(result != 0) 
            {
                caseResult = i * 2 + 1;
                break;
            }
        }
        else if(outPin == PIN_OUT_HIGH) 
        {
            result = PinIoOperation(IO_OP_READ_PIN, 0, i * 2 + 1, pinCount);
            if(result != 1) 
            {
                caseResult = i * 2 + 1;
                break;
            }
        }

        outPin = caseData[i] & 0x0F;
        if(outPin == PIN_OUT_LOW)
        {
            result = PinIoOperation(IO_OP_READ_PIN, 0, i * 2 + 2, pinCount);
            if(result != 0) 
            {
                caseResult = i * 2 + 2;
                break;
            }
        }
        else if(outPin == PIN_OUT_HIGH) 
        {
            result = PinIoOperation(IO_OP_READ_PIN, 0, i * 2 + 2, pinCount);
            if(result != 1) 
            {
                caseResult = i * 2 + 2;
                break;
            }
        }
    }
    
    return caseResult;
}

uint8_t PinIoOperation(uint8_t type, uint8_t param, uint8_t pin, uint8_t pinCount)
{
    uint16_t configPin = pinCount << 8 | pin;
    uint8_t result = 0;
    
    switch(configPin)
    {
        case 0x1C01: // 1/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD3 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD3;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD3 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD3 = 0;
                    }
                    else {
                        PORTDbits.RD3 = 1;
                    }
                }
                else {
                    TRISD3 = 1;
                }
            } 
            break;
        case 0x1C02: // 2/28
            if(type == IO_OP_CLOCK) {
                PORTCbits.RC5 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTCbits.RC5;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISC5 = 0;
                    if(param < PIN_VCC) {
                        PORTCbits.RC5 = 0;
                    }
                    else {
                        PORTCbits.RC5 = 1;
                    }
                }
                else {
                    TRISC7 = 1;
                }
            }
            break;
        case 0x1801: // 1/24
        case 0x1C03: // 3/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD4 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD4;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD4 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD4 = 0;
                    }
                    else {
                        PORTDbits.RD4 = 1;
                    }
                }
                else {
                    TRISD4 = 1;
                }
            }
            break;
        case 0x1802: // 2/24
        case 0x1C04: // 4/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD5 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD5;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD5 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD5 = 0;
                    }
                    else {
                        PORTDbits.RD5 = 1;
                    }
                }
                else {
                    TRISD5 = 1;
                }
            }
            break;
        case 0x1401: // 1/20
        case 0x1803: // 3/24
        case 0x1C05: // 5/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD6 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD6;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD6 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD6 = 0;
                    }
                    else {
                        PORTDbits.RD6 = 1;
                    }
                }
                else {
                    TRISD6 = 1;
                }
            }
            break;
        case 0x1201: // 1/18
        case 0x1402: // 2/20
        case 0x1804: // 4/24
        case 0x1C06: // 6/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD7 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD7;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD7 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD7 = 0;
                    }
                    else {
                        PORTDbits.RD7 = 1;
                    }
                }
                else {
                    TRISD7 = 1;
                }
            }
            break;
        case 0x1001: // 1/16
        case 0x1202: // 2/18
        case 0x1403: // 3/20
        case 0x1805: // 5/24
        case 0x1C07: // 7/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB0 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB0;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB0 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB0 = 0;
                    }
                    else {
                        PORTBbits.RB0 = 1;
                    }
                }
                else {
                    TRISB0 = 1;
                }
            }
            break;
        case 0x0E01: // 1/14
        case 0x1002: // 2/16
        case 0x1203: // 3/18
        case 0x1404: // 4/20
        case 0x1806: // 6/24
        case 0x1C08: // 8/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB1 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB1;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB1 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB1 = 0;
                    }
                    else {
                        PORTBbits.RB1 = 1;
                    }
                }
                else {
                    TRISB1 = 1;
                }
            }
            break;
        case 0x0E02: // 2/14
        case 0x1003: // 3/16
        case 0x1204: // 4/18
        case 0x1405: // 5/20
        case 0x1807: // 7/24
        case 0x1C09: // 9/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB2 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB2;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB2 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB2 = 0;
                    }
                    else {
                        PORTBbits.RB2 = 1;
                    }
                }
                else {
                    TRISB2 = 1;
                }
            }
            break;
        case 0x0E03: // 3/14
        case 0x1004: // 4/16
        case 0x1205: // 5/18
        case 0x1406: // 6/20
        case 0x1808: // 8/24
        case 0x1C0A: // 10/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB3 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB3;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB3 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB3 = 0;
                    }
                    else {
                        PORTBbits.RB3 = 1;
                    }
                }
                else {
                    TRISB3 = 1;
                }
            }
            break;
        case 0x0801: // 1/8    
        case 0x0E04: // 4/14
        case 0x1005: // 5/16
        case 0x1206: // 6/18
        case 0x1407: // 7/20
        case 0x1809: // 9/24
        case 0x1C0B: // 11/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB4 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB4;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB4 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB4 = 0;
                    }
                    else {
                        PORTBbits.RB4 = 1;
                    }
                }
                else {
                    TRISB4 = 1;
                }
            }
            break;
        case 0x0802: // 2/8
        case 0x0E05: // 5/14
        case 0x1006: // 6/16
        case 0x1207: // 7/18
        case 0x1408: // 8/20
        case 0x180A: // 10/24
        case 0x1C0C: // 12/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB5 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB5;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB5 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB5 = 0;
                    }
                    else {
                        PORTBbits.RB5 = 1;
                    }
                }
                else {
                    TRISB5 = 1;
                }
            }
            break;
        case 0x0803: // 3/8
        case 0x0E06: // 6/14
        case 0x1007: // 7/16
        case 0x1208: // 8/18
        case 0x1409: // 9/20
        case 0x180B: // 11/24
        case 0x1C0D: // 13/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB6 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB6;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB6 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB6 = 0;
                    }
                    else {
                        PORTBbits.RB6 = 1;
                    }
                }
                else {
                    TRISB6 = 1;
                }
            }
            break;
        case 0x0804: // 4/8
        case 0x0E07: // 7/14
        case 0x1008: // 8/16
        case 0x1209: // 9/18
        case 0x140A: // 10/20
        case 0x180C: // 12/24
        case 0x1C0E: // 14/28
            if(type == IO_OP_CLOCK) {
                PORTBbits.RB7 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTBbits.RB7;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISB7 = 0;
                    if(param < PIN_VCC) {
                        PORTBbits.RB7 = 0;
                    }
                    else {
                        PORTBbits.RB7 = 1;
                    }
                }
                else {
                    TRISB7 = 1;
                }
            }
            break;
        
        // second row of pins
        case 0x0805: // 5/8
        case 0x0E08: // 8/14
        case 0x1009: // 9/16
        case 0x120A: // 10/18
        case 0x140B: // 11/20
        case 0x180D: // 13/24
        case 0x1C0F: // 15/28
            if(type == IO_OP_CLOCK) {
                PORTAbits.RA0 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTAbits.RA0;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISA0 = 0;
                    if(param < PIN_VCC) {
                        PORTAbits.RA0 = 0;
                    }
                    else {
                        PORTAbits.RA0 = 1;
                    }
                }
                else {
                    TRISA0 = 1;
                }
            }
            break;
        case 0x0806: // 6/8
        case 0x0E09: // 9/14
        case 0x100A: // 10/16
        case 0x120B: // 11/18
        case 0x140C: // 12/20
        case 0x180E: // 14/24
        case 0x1C10: // 16/28
            if(type == IO_OP_CLOCK) {
                PORTAbits.RA1 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTAbits.RA1;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISA1 = 0;
                    if(param < PIN_VCC) {
                        PORTAbits.RA1 = 0;
                    }
                    else {
                        PORTAbits.RA1 = 1;
                    }
                }
                else {
                    TRISA1 = 1;
                }
            }
            break;
        case 0x0807: // 7/8
        case 0x0E0A: // 10/14
        case 0x100B: // 11/16
        case 0x120C: // 12/18
        case 0x140D: // 13/20
        case 0x180F: // 15/24
        case 0x1C11: // 17/28
            if(type == IO_OP_CLOCK) {
                PORTAbits.RA2 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTAbits.RA2;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISA2 = 0;
                    if(param < PIN_VCC) {
                        PORTAbits.RA2 = 0;
                    }
                    else {
                        PORTAbits.RA2 = 1;
                    }
                }
                else {
                    TRISA2 = 1;
                }
            }
            break;
        case 0x0808: // 8/8
        case 0x0E0B: // 11/14
        case 0x100C: // 12/16
        case 0x120D: // 13/18
        case 0x140E: // 14/20
        case 0x1810: // 16/24
        case 0x1C12: // 18/28
            if(type == IO_OP_CLOCK) {
                PORTAbits.RA3 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTAbits.RA3;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISA3 = 0;
                    if(param < PIN_VCC) {
                        PORTAbits.RA3 = 0;
                    }
                    else {
                        PORTAbits.RA3 = 1;
                    }
                }
                else {
                    TRISA3 = 1;
                }
            }
            break;
        case 0x0E0C: // 12/14
        case 0x100D: // 13/16
        case 0x120E: // 14/18
        case 0x140F: // 15/20
        case 0x1811: // 17/24
        case 0x1C13: // 19/28
            if(type == IO_OP_CLOCK) {
                PORTAbits.RA5 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTAbits.RA5;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISA5 = 0;
                    if(param < PIN_VCC) {
                        PORTAbits.RA5 = 0;
                    }
                    else {
                        PORTAbits.RA5 = 1;
                    }
                }
                else {
                    TRISA5 = 1;
                }
            }
            break;
        case 0x0E0D: // 13/14
        case 0x100E: // 14/16
        case 0x120F: // 15/18
        case 0x1410: // 16/20
        case 0x1812: // 18/24
        case 0x1C14: // 20/28
            if(type == IO_OP_CLOCK) {
                PORTEbits.RE0 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTEbits.RE0;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISE0 = 0;
                    if(param < PIN_VCC) {
                        PORTEbits.RE0 = 0;
                    }
                    else {
                        PORTEbits.RE0 = 1;
                    }
                }
                else {
                    TRISE0 = 1;
                }
            }
            break;
        case 0x0E0E: // 14/14
        case 0x100F: // 15/16
        case 0x1210: // 16/18
        case 0x1411: // 17/20
        case 0x1813: // 19/24
        case 0x1C15: // 21/28
            if(type == IO_OP_CLOCK) {
                PORTEbits.RE1 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTEbits.RE1;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISE1 = 0;
                    if(param < PIN_VCC) {
                        PORTEbits.RE1 = 0;
                    }
                    else {
                        PORTEbits.RE1 = 1;
                    }
                }
                else {
                    TRISE1 = 1;
                }
            }
            break;
        case 0x1010: // 16/16
        case 0x1211: // 17/18
        case 0x1412: // 18/20
        case 0x1814: // 20/24
        case 0x1C16: // 22/28
            if(type == IO_OP_CLOCK) {
                PORTEbits.RE2 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTEbits.RE2;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISE2 = 0;
                    if(param < PIN_VCC) {
                        PORTEbits.RE2 = 0;
                    }
                    else {
                        PORTEbits.RE2 = 1;
                    }
                }
                else {
                    TRISE2 = 1;
                }
            }
            break;
        case 0x1212: // 18/18
        case 0x1413: // 19/20
        case 0x1815: // 21/24
        case 0x1C17: // 23/28
            if(type == IO_OP_CLOCK) {
                PORTCbits.RC0 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTCbits.RC0;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISC0 = 0;
                    if(param < PIN_VCC) {
                        PORTCbits.RC0 = 0;
                    }
                    else {
                        PORTCbits.RC0 = 1;
                    }
                }
                else {
                    TRISC0 = 1;
                }
            }
            break;
        case 0x1414: // 20/20
        case 0x1816: // 22/24
        case 0x1C18: // 24/28
            if(type == IO_OP_CLOCK) {
                PORTCbits.RC1 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTCbits.RC1;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISC1 = 0;
                    if(param < PIN_VCC) {
                        PORTCbits.RC1 = 0;
                    }
                    else {
                        PORTCbits.RC1 = 1;
                    }
                }
                else {
                    TRISC1 = 1;
                }
            }
            break;
        case 0x1817: // 23/24
        case 0x1C19: // 25/28
            if(type == IO_OP_CLOCK) {
                PORTCbits.RC2 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTCbits.RC2;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISC2 = 0;
                    if(param < PIN_VCC) {
                        PORTCbits.RC2 = 0;
                    }
                    else {
                        PORTCbits.RC2 = 1;
                    }
                }
                else {
                    TRISC2 = 1;
                }
            }
            break;
        case 0x1818: // 24/24
        case 0x1C1A: // 26/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD0 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD0;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD0 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD0 = 0;
                    }
                    else {
                        PORTDbits.RD0 = 1;
                    }
                }
                else {
                    TRISD0 = 1;
                }
            }
            break;
        case 0x1C1B: // 27/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD1 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD1;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD1 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD1 = 0;
                    }
                    else {
                        PORTDbits.RD1 = 1;
                    }
                }
                else {
                    TRISD1 = 1;
                }
            }
            break;
        case 0x1C1C: // 28/28
            if(type == IO_OP_CLOCK) {
                PORTDbits.RD2 = param;
            }
            else if(type == IO_OP_READ_PIN) {
                result = PORTDbits.RD2;
            }
            else
            {
                if(param < PIN_OUT_LOW) // set as output
                {
                    TRISD2 = 0;
                    if(param < PIN_VCC) {
                        PORTDbits.RD2 = 0;
                    }
                    else {
                        PORTDbits.RD2 = 1;
                    }
                }
                else {
                    TRISD2 = 1;
                }
            }
            break;
    }
    
    return result;
}

uint8_t GetPowerPinsConfig(uint8_t *test, uint8_t pinCount)
{
    switch(pinCount)
    {
        case 8:
            // 4 - GND, 8 - VCC
            break;
        case 14:
            // 7 - GND, 14 - VCC
            if(((test[3] & 0xF0) >> 4) == PIN_GND && (test[6] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_14_7_14;
            } 
            // 11 - GND, 4 - VCC
            // 10 - GND, 4 - VCC
            // 10 - GND, 5 - VCC
            break;
        case 16:
            // 8 - GND, 16 - VCC
            if((test[3] & 0x0F) == PIN_GND && (test[7] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_16_8_16;
            }
            // 12 - GND, 5 - VCC
            if((test[5] & 0x0F) == PIN_GND && ((test[2] & 0xF0) >> 4) == PIN_VCC) {
                return POWER_CONFIG_16_12_5;
            }
            // 13 - GND, 5 - VCC
            break;
        case 18:
            // 9 - GND, 18 - VCC
            if(((test[4] & 0xF0) >> 4) == PIN_GND && (test[8] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_18_9_18;
            }
            break;
        case 20:
            // 10 - GND, 20 - VCC
            if((test[4] & 0x0F) == PIN_GND && (test[9] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_20_10_20;
            }
            break;
        case 24:
            // 12 - GND, 24 - VCC
            if((test[5] & 0x0F) == PIN_GND && (test[11] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_24_12_24;
            }
            break;
        case 28:
            // 14 - GND, 28 - VCC
            if((test[6] & 0x0F) == PIN_GND && (test[13] & 0x0F) == PIN_VCC) {
                return POWER_CONFIG_28_14_28;
            }
            break;
    }
    
    return POWER_CONFIG_UNKNOWN;
}

void SetPowerConfig(uint8_t powerConfig)
{
    UartWriteString("_R ");

    char hexBuffer[3];
    ByteToAsciiHex(powerConfig, hexBuffer);
    UartWriteString(hexBuffer);

    InterfaceCmdStatus = INTERFACE_CMD_READY;
    UartWriteString("\n");        
    while(InterfaceCmdStatus != INTERFACE_CMD_DONE) {
        __delay_us(25);
    }
}

void ResetHardware(uint8_t fullReset)
{
    if(fullReset) {
        SetPowerConfig(POWER_CONFIG_UNKNOWN);
    }
    
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 1, COMMON_TEST_MAX_PINS);
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 2, COMMON_TEST_MAX_PINS);
    }
}

uint8_t IdentifyIcTest(uint16_t index, uint8_t pinCount, char *deviceName)
{
    uint16_t indexAddress = index * 2 + TEST_DATA_INDEX_START;
    uint16_t icTestAddress = I2cReadDWord(EEPROM_ADDRESS, indexAddress);
    
    icTestAddress = I2cReadString(EEPROM_ADDRESS, icTestAddress, I2C_DATA_MAX_LENGTH, '\n'); //ic name
    char icTestDeviceName[10];
    ClearString(icTestDeviceName, 10);
    CopyString(&I2cData[1], icTestDeviceName, 9);

    icTestAddress = I2cReadString(EEPROM_ADDRESS, icTestAddress, I2C_DATA_MAX_LENGTH, '\n'); //ignore description

    icTestAddress = I2cReadString(EEPROM_ADDRESS, icTestAddress, I2C_DATA_MAX_LENGTH, '\n'); //pins
    uint8_t icTestPinCount = (I2cData[0] - '0') * 10 + (I2cData[1] - '0');
    if(icTestPinCount != pinCount) { // pins don't match
        return INVALID_RESULT;
    }
    
    ClearRectangleOnInterface(32, 32, 54, 6, COLOR_WHITE); // clear previous ic name...
    DisplayStringOnInterface(32, 32, COLOR_BLACK, COLOR_WHITE, icTestDeviceName);
    
    ResetHardware(FALSE);
    __delay_ms(50);
    
    uint8_t icTestCase[COMMON_TEST_MAX_PINS / 2], icTestCaseResult = 0;
    
    while(TRUE)
    {
        icTestAddress = I2cReadString(EEPROM_ADDRESS, icTestAddress, I2C_DATA_MAX_LENGTH, '\n');
        if(I2cData[0] == '$' || I2cData[0] == NULL_CHAR) {
            break;
        }
        
        TransformTestData(icTestCase, I2cData, icTestPinCount);
        
        icTestCaseResult = RunTestCase(icTestCase, icTestPinCount);
        if(icTestCaseResult > 0) { // error
            break;
        }
        
        __delay_ms(10);
    }
    
    if(icTestCaseResult == 0) 
    {
        CopyString(icTestDeviceName, deviceName, 9);
        return 0;
    }
    return INVALID_RESULT;
}

#ifdef HARDWARE_DEBUG

void HwDebugSetOutAllLow(void)
{
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 1, COMMON_TEST_MAX_PINS);
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 2, COMMON_TEST_MAX_PINS);
    }
}

void HwDebugSetOutAllHigh(void)
{
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_HIGH, i * 2 + 1, COMMON_TEST_MAX_PINS);
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_HIGH, i * 2 + 2, COMMON_TEST_MAX_PINS);
    }
}

void HwDebugSetOutAllHL(void)
{
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_HIGH, i * 2 + 1, COMMON_TEST_MAX_PINS);
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 2, COMMON_TEST_MAX_PINS);
    }
}

void HwDebugSetOutAllLH(void)
{
    for(uint8_t i = 0, j = COMMON_TEST_MAX_PINS / 2; i < j; i++)
    {
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_LOW, i * 2 + 1, COMMON_TEST_MAX_PINS);
        PinIoOperation(IO_OP_SET_FUNCTION, PIN_IN_HIGH, i * 2 + 2, COMMON_TEST_MAX_PINS);
    }
}

void HwDebugRelay1(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 7 - GND, 14 - VCC
    SetPowerConfig(POWER_CONFIG_14_7_14);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 7, 14);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 14, 14);
}

void HwDebugRelay2(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 8 - GND, 16 - VCC
    SetPowerConfig(POWER_CONFIG_16_8_16);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 8, 16);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 16, 16);
}

void HwDebugRelay3(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 9 - GND, 18 - VCC
    SetPowerConfig(POWER_CONFIG_18_9_18);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 9, 18);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 18, 18);
}

void HwDebugRelay4(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 12 - GND, 5 - VCC
    SetPowerConfig(POWER_CONFIG_16_12_5);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 12, 16);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 5, 16);
}

void HwDebugRelay5(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 10 - GND, 20 - VCC
    SetPowerConfig(POWER_CONFIG_20_10_20);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 10, 20);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 20, 20);
}

void HwDebugRelay6(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 12 - GND, 24 - VCC
    SetPowerConfig(POWER_CONFIG_24_12_24);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 12, 24);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 24, 24);
}

void HwDebugRelay7(void)
{
    SetPowerConfig(POWER_CONFIG_UNKNOWN);
    
    HwDebugSetOutAllLow();
    // 14 - GND, 28 - VCC
    SetPowerConfig(POWER_CONFIG_28_14_28);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_GND, 14, 28);
    PinIoOperation(IO_OP_SET_FUNCTION, PIN_VCC, 28, 28);
}

#endif