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
 * File:   test.h
 * Author: Ciprian Stingu
 *
 * Created on July 31, 2020, 4:55 PM
 */

#ifndef TEST_H
#define	TEST_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TEST_DATA_INDEX_START 6
#define TEST_DATA_MAX_RECORDS 0x7FFF
#define TEST_DATA_INVALID_RECORD 0xFFFF
uint16_t DatabaseVersion = 0;
uint16_t TestDataDevicesCount = 0;
uint16_t TestDataRecordsCount = 0;

void ReadDatabaseInfo(void);
void SearchDeviceInEeprom(void);

char SelectedDeviceName[SELECTED_DEVICE_NAME_MAX_LENGTH + 1];
uint16_t SelectedDeviceIndex = TEST_DATA_INVALID_RECORD;

#define PIN_NONE     0
// set as output pins
#define PIN_IGNORE   1
#define PIN_GND      2
#define PIN_IN_LOW   3
#define PIN_CLOCK    4
#define PIN_VCC      5
#define PIN_IN_HIGH  6
// set as input pins
#define PIN_OUT_LOW  7
#define PIN_OUT_HIGH 8

#define PIN_GND_CHAR      'G'
#define PIN_VCC_CHAR      'V'
#define PIN_IN_LOW_CHAR   '0'
#define PIN_IN_HIGH_CHAR  '1'
#define PIN_OUT_LOW_CHAR  'L'
#define PIN_OUT_HIGH_CHAR 'H'
#define PIN_IGNORE_CHAR   'X'
#define PIN_CLOCK_CHAR    'C'

#define POWER_CONFIG_UNKNOWN  0
#define POWER_CONFIG_14_7_14  1
#define POWER_CONFIG_16_8_16  2
#define POWER_CONFIG_18_9_18  3
#define POWER_CONFIG_16_12_5  4
#define POWER_CONFIG_20_10_20 5
#define POWER_CONFIG_24_12_24 6
#define POWER_CONFIG_28_14_28 7

#define PC_TEST_ERROR_NO_LABEL "No label specified"
#define PC_TEST_ERROR_INVALID_NUMBER_OF_PINS "Ic has an invalid number of pins"
#define PC_TEST_ERROR_TEST_INVALID_NUMBER_OF_PINS "Test has an invalid number of pins"
#define PC_TEST_ERROR_NO_TESTS_FOUND "No tests were found"

#define COMMON_TEST_MAX_CASES 48
#define COMMON_TEST_MAX_PINS  28
uint8_t CommonTestCasesCount = 0;
uint8_t CommonTestCases[COMMON_TEST_MAX_CASES][COMMON_TEST_MAX_PINS / 2];
uint8_t CommonTestPinCount = 0;
char CommonTestName[SELECTED_DEVICE_NAME_MAX_LENGTH + 1];
char CustomTestItemData[COMMON_TEST_MAX_PINS + 1];

#define IO_OP_SET_FUNCTION 0
#define IO_OP_CLOCK        1
#define IO_OP_READ_PIN     2

uint8_t TestedCases = 0;
uint8_t LastTestCaseResult = 0;
uint8_t ReturnToTestScreen = 0;

uint8_t TransformTestData(uint8_t *test, char *data, uint8_t pinCount);
uint8_t GetPinFromChar(char data);
void StartTest(uint8_t source);
uint8_t GetPowerPinsConfig(uint8_t *test, uint8_t pinCount);
uint8_t PinIoOperation(uint8_t type, uint8_t param, uint8_t pin, uint8_t pinCount);
uint8_t RunTestCase(uint8_t *caseData, uint8_t pinCount);
void SetPowerConfig(uint8_t powerConfig);
void ResetHardware(uint8_t fullReset);

uint8_t IdentifyIcTest(uint16_t index, uint8_t pinCount, char *deviceName);

#ifdef HARDWARE_DEBUG
void HwDebugSetOutAllLow(void);
void HwDebugSetOutAllHigh(void);
void HwDebugSetOutAllHL(void);
void HwDebugSetOutAllLH(void);
void HwDebugRelay1(void);
void HwDebugRelay2(void);
void HwDebugRelay3(void);
void HwDebugRelay4(void);
void HwDebugRelay5(void);
void HwDebugRelay6(void);
void HwDebugRelay7(void);
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* TEST_H */

