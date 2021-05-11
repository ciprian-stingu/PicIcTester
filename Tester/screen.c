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
 * File:   screen.c
 * Author: Ciprian Stingu
 *
 * Created on July 29, 2020, 5:57 PM
 */


#include "main.h"
#include "screen.h"

void DisplayMainScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    
    UpdateMainMenu();
}

void UpdateMainMenu(void)
{
    //write menu
#ifdef HARDWARE_DEBUG
    for(uint8_t i = 0; i <= MAIN_MENU_HWDEBUG; i++)
#else
    for(uint8_t i = 0; i <= MAIN_MENU_ABOUT; i++)
#endif
    {
        char *MenuItemText;
        if(i == 0) {
            MenuItemText = MAIN_MENU_ITEM_TEXT_0;
        }
        else if(i == 1) {
            MenuItemText = MAIN_MENU_ITEM_TEXT_1;
        }
        else if(i == 2) {
            MenuItemText = MAIN_MENU_ITEM_TEXT_2;
        }
        else if(i == 3) {
            MenuItemText = MAIN_MENU_ITEM_TEXT_3;
        }
        else {
            MenuItemText = MAIN_MENU_ITEM_TEXT_4;
        }
        
        if(MainMenuSelectedItem == i) 
        {
            DisplayStringOnInterface(3, i * 7 + 15, COLOR_RED, COLOR_YELLOW, ">");
            DisplayStringOnInterface(8, i * 7 + 15, COLOR_RED, COLOR_YELLOW, MenuItemText);
        }
        else
        {
            DisplayStringOnInterface(3, i * 7 + 15, COLOR_BLACK, COLOR_WHITE, " ");
            DisplayStringOnInterface(8, i * 7 + 15, COLOR_BLACK, COLOR_WHITE, MenuItemText);
        }
    }
}

void DisplaySearchScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);

    UpdateSearchMenu();
}

void UpdateSearchMenu(void)
{   
    if(SearchDeviceChanged) 
    {
        SearchDeviceInEeprom();
        //ClearRectangleOnInterface(0, 16, LCD_WIDTH, LCD_HEIGHT - 16, COLOR_WHITE);
        for(uint8_t i = 0; i < SEARCH_RESULTS; i++) {
            ClearRectangleOnInterface(3, i * 7 + 23, 94, 6, COLOR_WHITE);
        }
        SearchDeviceChanged = 0;
    }
    
    if(SearchMenuSelectedItem == SEARCH_MENU_SEARCH)
    {
        DrawRectangleOnInterface(3, 10, LCD_WIDTH - 6, 10, COLOR_BLUE, COLOR_WHITE);
        DisplayStringOnInterface(6, 12, COLOR_BLACK, COLOR_GREEN, SearchTerm);
    }
    else 
    {
        DrawRectangleOnInterface(3, 10, LCD_WIDTH - 6, 10, COLOR_BLACK, COLOR_WHITE);
        DisplayStringOnInterface(6, 12, COLOR_BLACK, COLOR_WHITE, SearchTerm);
    }
    
    for(uint8_t i = 0; i < SearchResultsCount; i++)
    {
        if(SearchMenuSelectedItem == i + 1)
        {
            DisplayStringOnInterface(3, i * 7 + 23, COLOR_RED, COLOR_YELLOW, ">");
            DisplayStringOnInterface(8, i * 7 + 23, COLOR_RED, COLOR_YELLOW, SearchResultNames[i]);
        }
        else 
        {
            DisplayStringOnInterface(3, i * 7 + 23, COLOR_BLACK, COLOR_WHITE, " ");
            DisplayStringOnInterface(8, i * 7 + 23, COLOR_BLACK, COLOR_WHITE, SearchResultNames[i]);
        }
    }
}

void DisplaySearchIcInfoScreen(void)
{
    DrawWindowOnInterface(6, 13, LCD_WIDTH - 12, LCD_HEIGHT - 16, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);
    
    //address of selected item in search screen
    uint16_t address = SearchResultIndexes[SearchMenuSelectedItem - 1];
    char buffer[14];
    
    address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //ic name
    DisplayStringOnInterface(10, 17, COLOR_BLACK, COLOR_WHITE, "IC: ");
    ClearString(buffer, 14);
    CopyString(&I2cData[1], buffer, 9);
    DisplayStringOnInterface(34, 17, COLOR_BLACK, COLOR_WHITE, buffer);

    address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //description
    ClearString(buffer, 14);
    for(uint8_t i = 0; i < 13; i++) {
        buffer[i] = I2cData[i];
    }
    DisplayStringOnInterface(10, 24, COLOR_BLACK, COLOR_WHITE, buffer);
    ClearString(buffer, 14);
    for(uint8_t i = 13; i < 26; i++) {
        buffer[i - 13] = I2cData[i];
    }
    DisplayStringOnInterface(10, 31, COLOR_BLACK, COLOR_WHITE, buffer);
    
    address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n'); //pins
    DisplayStringOnInterface(10, 38, COLOR_BLACK, COLOR_WHITE, "Pins: ");
    DisplayStringOnInterface(46, 38, COLOR_BLACK, COLOR_WHITE, I2cData);
    
    //tests count
    uint8_t testCount = 0;
    do
    {
        address = I2cReadString(EEPROM_ADDRESS, address, I2C_DATA_MAX_LENGTH, '\n');
        testCount++;
    } while(I2cData[0] != '$' && I2cData[0] != NULL_CHAR);
    UnsignedWordToAsciiInteger(testCount - 1, buffer);
    DisplayStringOnInterface(10, 45, COLOR_BLACK, COLOR_WHITE, "Tests: ");
    DisplayStringOnInterface(52, 45, COLOR_BLACK, COLOR_WHITE, buffer);
        
    DisplayStringOnInterface(18, 54, COLOR_BLACK, COLOR_WHITE, "Press a key");
}

void DisplayTestScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    
    DisplayStringOnInterface(7, 10, COLOR_BLACK, COLOR_WHITE, "Current Device");
    DisplayStringOnInterface(1, 17, COLOR_BLACK, COLOR_WHITE, "[");
    DisplayStringOnInterface(8, 17, COLOR_BLACK, COLOR_WHITE, SelectedDeviceName);
    DisplayStringOnInterface(92, 17, COLOR_BLACK, COLOR_WHITE, "]");
    
    UpdateTestMenu();
}

void UpdateTestMenu(void)
{
    if(TestMenuSelectedItem == TEST_START)
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, "START");
        
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(56, 42, COLOR_BLACK, COLOR_WHITE, "CLOSE");
    }
    else 
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, "START");
        
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(56, 42, COLOR_BLACK, COLOR_GREEN, "CLOSE");
    }
}

void DisplayPcTestScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    
    if(PcTestStatus == PC_TEST_READY) 
    {
        DisplayStringOnInterface(10, 14, COLOR_BLACK, COLOR_WHITE, "Name: ");
        DisplayStringOnInterface(46, 14, COLOR_BLACK, COLOR_WHITE, CommonTestName);
        
        char buffer[6];
        UnsignedWordToAsciiInteger(CommonTestPinCount, buffer);
        DisplayStringOnInterface(10, 21, COLOR_BLACK, COLOR_WHITE, "Pins: ");
        DisplayStringOnInterface(46, 21, COLOR_BLACK, COLOR_WHITE, buffer);
        
        UnsignedWordToAsciiInteger(CommonTestCasesCount, buffer);
        DisplayStringOnInterface(10, 28, COLOR_BLACK, COLOR_WHITE, "Cases: ");
        DisplayStringOnInterface(52, 28, COLOR_BLACK, COLOR_WHITE, buffer);
    
        UpdatePcTestMenu();
    }
    else 
    {
        DisplayStringOnInterface(16, 20, COLOR_BLACK, COLOR_WHITE, "Waiting for");
        DisplayStringOnInterface(22, 27, COLOR_BLACK, COLOR_WHITE, "test data");
        
        DisplayStringOnInterface(10, 40, COLOR_BLACK, COLOR_WHITE, "Press any key");
        DisplayStringOnInterface(25, 47, COLOR_BLACK, COLOR_WHITE, "to abort");
        
        UartWriteMessageAndWait("Waiting for test data...");
    }
}

void UpdatePcTestMenu(void)
{
    if(PcTestMenuSelectedItem == PC_TEST_START)
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, "START");
    }
    else
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, "START");
    }
    if(PcTestMenuSelectedItem == PC_TEST_SAVE)
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_GREEN, "SAVE");
    }
    else 
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_WHITE, "SAVE");
    }
    if(PcTestMenuSelectedItem == PC_TEST_CLOSE)
    {
        DrawWindowOnInterface(31, 53, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(34, 55, COLOR_BLACK, COLOR_GREEN, "CLOSE");
    }
    else 
    {
        DrawWindowOnInterface(31, 53, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(34, 55, COLOR_BLACK, COLOR_WHITE, "CLOSE");
    }
}

void DisplayCustomTestScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    
    if(CustomTestStatus == CUSTOM_TEST_READY) 
    {
        DisplayStringOnInterface(10, 14, COLOR_BLACK, COLOR_WHITE, "Name: ");
        DisplayStringOnInterface(46, 14, COLOR_BLACK, COLOR_WHITE, CommonTestName);
        
        char buffer[6];
        UnsignedWordToAsciiInteger(CommonTestPinCount, buffer);
        DisplayStringOnInterface(10, 21, COLOR_BLACK, COLOR_WHITE, "Pins: ");
        DisplayStringOnInterface(46, 21, COLOR_BLACK, COLOR_WHITE, buffer);
        
        UnsignedWordToAsciiInteger(CommonTestCasesCount, buffer);
        DisplayStringOnInterface(10, 28, COLOR_BLACK, COLOR_WHITE, "Cases: ");
        DisplayStringOnInterface(52, 28, COLOR_BLACK, COLOR_WHITE, buffer);
        
        UpdateCustomTestMenu();
    }
    else 
    {
        if(CustomTestStatus == CUSTOM_TEST_INIT)
        {    
            if(EepromSavedTestExists())
            {
                DrawWindowOnInterface(4, 13, LCD_WIDTH - 8, LCD_HEIGHT - 18, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);

                DisplayStringOnInterface(10, 20, COLOR_BLACK, COLOR_WHITE, "Load previous");
                DisplayStringOnInterface(16, 27, COLOR_BLACK, COLOR_WHITE, "saved test?");

                CustomTestStatus = CUSTOM_TEST_EXISTS;
                CustomTestMenuSelectedItem = CUSTOM_TEST_PREVIOUS_YES;
                MenuChanged = TRUE;
            }
            else
            {
                CustomTestStatus = CUSTOM_TEST_FILL;
                CustomTestStep = CUSTOM_TEST_NAME;
                CustomTestMenuSelectedItem = CUSTOM_TEST_NAME_FILL;
                ClearString(CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
                ScreenChanged = TRUE;
            }
        }
        else
        {
            if(CustomTestStep == CUSTOM_TEST_NAME)
            {
                DisplayStringOnInterface(22, 14, COLOR_BLACK, COLOR_WHITE, "Test name");
                UpdateCustomTestNameStepMenu();
            }
            else if(CustomTestStep == CUSTOM_TEST_PINS)
            {
                DisplayStringOnInterface(22, 14, COLOR_BLACK, COLOR_WHITE, "Test pins");
                UpdateCustomTestPinsStepMenu();
            }
            else if(CustomTestStep == CUSTOM_TEST_CASE)
            {
                DisplayStringOnInterface(22, 14, COLOR_BLACK, COLOR_WHITE, "Test case");
                UpdateCustomTestItemStepMenu();
            }
        }
    }
}

void UpdateCustomTestNameStepMenu(void)
{
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_NAME_FILL)
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 10, COLOR_BLUE, COLOR_WHITE);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_GREEN, CommonTestName);
    }
    else 
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 10, COLOR_BLACK, COLOR_WHITE);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_WHITE, CommonTestName);
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_NAME_OK)
    {
        DrawWindowOnInterface(31, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(37, 42, COLOR_BLACK, COLOR_GREEN, " OK ");
    }
    else
    {
        DrawWindowOnInterface(31, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(37, 42, COLOR_BLACK, COLOR_WHITE, " OK ");
    }
}

void UpdateCustomTestPinsStepMenu(void)
{
    char buffer[6];
    UnsignedWordToAsciiInteger(CommonTestPinCount, buffer);
    
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_PINS_FILL)
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 10, COLOR_BLUE, COLOR_WHITE);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_GREEN, buffer);
    }
    else 
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 10, COLOR_BLACK, COLOR_WHITE);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_WHITE, buffer);
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_PINS_OK)
    {
        DrawWindowOnInterface(31, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(37, 42, COLOR_BLACK, COLOR_GREEN, " OK ");
    }
    else
    {
        DrawWindowOnInterface(31, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(37, 42, COLOR_BLACK, COLOR_WHITE, " OK ");
    }
}

void UpdateCustomTestItemStepMenu(void)
{
    char buffer[15];
    
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_FILL)
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 18, COLOR_BLUE, COLOR_WHITE);
        CopyString(CustomTestItemData, buffer, 14);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_GREEN, buffer);
        CopyString(&CustomTestItemData[14], buffer, 14);
        DisplayStringOnInterface(6, 31, COLOR_BLACK, COLOR_GREEN, buffer);
    }
    else 
    {
        DrawRectangleOnInterface(3, 22, LCD_WIDTH - 6, 18, COLOR_BLACK, COLOR_WHITE);
        CopyString(CustomTestItemData, buffer, 14);
        DisplayStringOnInterface(6, 24, COLOR_BLACK, COLOR_WHITE, buffer);
        CopyString(&CustomTestItemData[14], buffer, 14);
        DisplayStringOnInterface(6, 31, COLOR_BLACK, COLOR_WHITE, buffer);
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_NEXT)
    {
        DrawWindowOnInterface(9, 45, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(15, 47, COLOR_BLACK, COLOR_GREEN, "NEXT");
    }
    else
    {
        DrawWindowOnInterface(9, 45, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(15, 47, COLOR_BLACK, COLOR_WHITE, "NEXT");
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_DONE)
    {
        DrawWindowOnInterface(53, 45, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(59, 47, COLOR_BLACK, COLOR_GREEN, "DONE");
    }
    else 
    {
        DrawWindowOnInterface(53, 45, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(59, 47, COLOR_BLACK, COLOR_WHITE, "DONE");
    }
}

void UpdateCustomTestPreviousMenu(void)
{
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_PREVIOUS_YES)
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, " YES ");
    }
    else
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, " YES ");
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_PREVIOUS_NO)
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_GREEN, " NO ");
    }
    else 
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_WHITE, " NO ");
    }
}

void UpdateCustomTestMenu(void)
{
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_START)
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, "START");
    }
    else
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, "START");
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_SAVE)
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_GREEN, "SAVE");
    }
    else 
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_WHITE, "SAVE");
    }
    if(CustomTestMenuSelectedItem == CUSTOM_TEST_CLOSE)
    {
        DrawWindowOnInterface(31, 53, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(34, 55, COLOR_BLACK, COLOR_GREEN, "CLOSE");
    }
    else 
    {
        DrawWindowOnInterface(31, 53, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(34, 55, COLOR_BLACK, COLOR_WHITE, "CLOSE");
    }
}

void DisplayTestResultScreen(void)
{
    DrawWindowOnInterface(4, 13, LCD_WIDTH - 8, LCD_HEIGHT - 18, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);
    
    DisplayStringOnInterface(16, 16, COLOR_BLACK, COLOR_WHITE, "TEST RESULT");
    
    if(LastTestCaseResult == INVALID_RESULT && TestedCases == INVALID_RESULT) // invalid configuration
    {
        DisplayStringOnInterface(28, 27, COLOR_BLACK, COLOR_WHITE, "Invalid");
        DisplayStringOnInterface(22, 34, COLOR_BLACK, COLOR_WHITE, "test data");
    }
    else if(LastTestCaseResult > 0) // error
    {
        DisplayStringOnInterface(31, 25, COLOR_BLACK, COLOR_WHITE, "FAILED");
        
        char buffer[6];
        UnsignedWordToAsciiInteger(TestedCases + 1, buffer);
        DisplayStringOnInterface(20, 35, COLOR_BLACK, COLOR_WHITE, "Case: ");
        DisplayStringOnInterface(56, 35, COLOR_BLACK, COLOR_WHITE, buffer);
        UnsignedWordToAsciiInteger(LastTestCaseResult, buffer);
        DisplayStringOnInterface(20, 42, COLOR_BLACK, COLOR_WHITE, "Pin: ");
        DisplayStringOnInterface(50, 42, COLOR_BLACK, COLOR_WHITE, buffer);
    }
    else {
        DisplayStringOnInterface(34, 27, COLOR_BLACK, COLOR_WHITE, "IC OK");
    }
    
    DisplayStringOnInterface(16, 53, COLOR_BLACK, COLOR_WHITE, "Press a key");
}


void DisplayIdentifyIcScreen(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    
    DisplayStringOnInterface(16, 16, COLOR_BLACK, COLOR_WHITE, "Identify IC");
    DisplayStringOnInterface(10, 28, COLOR_BLACK, COLOR_WHITE, "Pins: ");
    
    UpdateIdentifyIcMenu();
}

void UpdateIdentifyIcMenu(void)
{
    char buffer[6];
    if(IdentifyIcPinsCount > 0) {
        UnsignedWordToAsciiInteger(IdentifyIcPinsCount, buffer);
    }
    else {
        buffer[0] = NULL_CHAR;
    }
    
    if(IdentifyIcMenuSelectedItem == IDENTIFY_IC_PINS)
    {
        DrawRectangleOnInterface(46, 26, 46, 10, COLOR_BLUE, COLOR_WHITE);
        DisplayStringOnInterface(48, 28, COLOR_BLACK, COLOR_GREEN, buffer);
    }
    else
    {
        DrawRectangleOnInterface(46, 26, 46, 10, COLOR_BLACK, COLOR_WHITE);
        DisplayStringOnInterface(48, 28, COLOR_BLACK, COLOR_WHITE, buffer);
    }
    
    if(IdentifyIcMenuSelectedItem == IDENTIFY_IC_START)
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, "START");
    }
    else 
    {
        DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, "START");
    }
    
    if(IdentifyIcMenuSelectedItem == IDENTIFY_IC_CLOSE)
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(56, 42, COLOR_BLACK, COLOR_GREEN, "CLOSE");
    }
    else
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(56, 42, COLOR_BLACK, COLOR_WHITE, "CLOSE");
    }
}

void DisplayIdentifyIcRunScreen(void)
{
    DrawWindowOnInterface(4, 13, LCD_WIDTH - 8, LCD_HEIGHT - 18, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);
    
    DisplayStringOnInterface(16, 16, COLOR_BLACK, COLOR_WHITE, "Identifying");
    
    DisplayStringOnInterface(8, 32, COLOR_BLACK, COLOR_WHITE, "IC: ");
    
    DisplayStringOnInterface(16, 46, COLOR_BLACK, COLOR_WHITE, "Press a key");
    DisplayStringOnInterface(25, 53, COLOR_BLACK, COLOR_WHITE, "to abort");
}

void DisplayIdentifyIcResultScreen(void)
{
    DrawWindowOnInterface(4, 13, LCD_WIDTH - 8, LCD_HEIGHT - 18, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);
    
    DisplayStringOnInterface(22, 16, COLOR_BLACK, COLOR_WHITE, "ID RESULT");
    
    if(IdentifyIcDeviceName[0] != NULL_CHAR) 
    {
        DisplayStringOnInterface(16, 30, COLOR_BLACK, COLOR_WHITE, "Match found");
        DisplayStringOnInterface(16, 38, COLOR_BLACK, COLOR_WHITE, IdentifyIcDeviceName);
    }
    else {
        DisplayStringOnInterface(25, 36, COLOR_BLACK, COLOR_WHITE, "No match");
    }
    
    DisplayStringOnInterface(16, 53, COLOR_BLACK, COLOR_WHITE, "Press a key");
}

#ifdef HARDWARE_DEBUG

void DisplayHwDebug(void)
{
    //clear the screen
    ClearRectangleOnInterface(0, 8, LCD_WIDTH, LCD_HEIGHT - 8, COLOR_WHITE);
    DisplayStringOnInterface(27, 16, COLOR_BLACK, COLOR_WHITE, "HW TEST");
    
    UpdateHwDebugMenu();
}

void UpdateHwDebugMenu(void)
{
    DisplayStringOnInterface(3, 23, COLOR_BLACK, COLOR_WHITE, HwDebugMessage);
    
    if(HwDebugStep == HWDEBUG_STEP_INIT)
    {
        if(HwDebugMenuSelectedItem == HWDEBUG_START)
        {
            DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
            DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_GREEN, "START");
        }
        else
        {
            DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
            DisplayStringOnInterface(12, 42, COLOR_BLACK, COLOR_WHITE, "START");
        }
    }
    else
    {
        if(HwDebugMenuSelectedItem == HWDEBUG_NEXT)
        {
            DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
            DisplayStringOnInterface(15, 42, COLOR_BLACK, COLOR_GREEN, "NEXT");
        }
        else
        {
            DrawWindowOnInterface(9, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
            DisplayStringOnInterface(15, 42, COLOR_BLACK, COLOR_WHITE, "NEXT");
        }
    }
    
    if(HwDebugMenuSelectedItem == HWDEBUG_STOP)
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLUE, COLOR_WHITE, COLOR_GREEN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_GREEN, "STOP");
    }
    else 
    {
        DrawWindowOnInterface(53, 40, 36, 10, COLOR_BLACK, COLOR_WHITE, COLOR_CYAN);
        DisplayStringOnInterface(59, 42, COLOR_BLACK, COLOR_WHITE, "STOP");
    }
}

#else

void DisplayAboutScreen(void)
{
    DrawWindowOnInterface(4, 13, LCD_WIDTH - 8, LCD_HEIGHT - 18, COLOR_BLACK, COLOR_WHITE, COLOR_GREEN);
    
    DisplayStringOnInterface(29, 16, COLOR_BLACK, COLOR_WHITE, "ABOUT");
    DisplayStringOnInterface(6, 26, COLOR_BLACK, COLOR_WHITE, "Ciprian Stingu");
    
    DisplayStringOnInterface(16, 36, COLOR_BLACK, COLOR_WHITE, "DB ver: ");
    char buffer[6];
    UnsignedWordToAsciiInteger(DatabaseVersion, buffer);
    DisplayStringOnInterface(64, 36, COLOR_BLACK, COLOR_WHITE, buffer);
    
    DisplayStringOnInterface(16, 43, COLOR_BLACK, COLOR_WHITE, "ICs: ");
    UnsignedWordToAsciiInteger(TestDataDevicesCount, buffer);
    DisplayStringOnInterface(46, 43, COLOR_BLACK, COLOR_WHITE, buffer);
    
    DisplayStringOnInterface(16, 53, COLOR_BLACK, COLOR_WHITE, "Press a key");
}

#endif