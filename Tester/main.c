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
 * File:   main.c
 * Author: Ciprian Stingu
 *
 * Created on July 17, 2020, 2:00 PM
 */


#include "main.h"

void main(void) 
{
    // Enable all unmasked global INT
    GIE = 1;
    // Enable Peripheral interrupt
    PEIE = 1;
    //disable comparators
    CCP1M0 = 0;
    CCP1M1 = 0;
    CCP1M2 = 0;
    CCP1M3 = 0;
    CCP2M0 = 0;
    CCP2M1 = 0;
    CCP2M2 = 0;
    CCP2M3 = 0;
    // A/D Port Configuration Control bits
    // 011x - Digital I/O
    PCFG3 = 0;
    PCFG2 = 1;
    PCFG1 = 1;
    PCFG0 = 0;
    // shut-off A/D converter
    ADON = 0;
    // General purpose I/O mode
    PSPMODE = 0;
    
    I2cInit();
    UartInit();
    
    ClearString(UartMessage, UART_MESSAGE_MAX_LENGTH + 1);
    UartMessageLength = 0;
    
    ClearString(I2cData, I2C_DATA_MAX_LENGTH + 1);
    I2cDataLength = 0;
    
    ClearString(SelectedDeviceName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
    
    __delay_ms(250);
    ReadDatabaseInfo();
    
    //wait for interface to init
    while(InterfaceCmdStatus != INTERFACE_CMD_DONE) {
        __delay_ms(1);
    }
    InitInterfaceScreen();
    KeypadData = KEY_NONE;
    
    ResetHardware(TRUE);
    
    while(TRUE)
    {
        if(UartMessageLength == UART_MESSAGE_MAX_LENGTH)
        {
            ProcessCommand();
            
            ClearString(UartMessage, UART_MESSAGE_MAX_LENGTH + 1);
            UartMessageLength = 0;
        }
        else if(ScreenChanged)
        {
            ScreenChanged = FALSE;
            MenuChanged = FALSE;
            
            if(ActiveScreen == MAIN_SCREEN) {
                DisplayMainScreen();
            }
            else if(ActiveScreen == SEARCH_SCREEN) {
                DisplaySearchScreen();
            }
            else if(ActiveScreen == PC_TEST_SCREEN) {
                DisplayPcTestScreen();
            }
            else if(ActiveScreen == CUSTOM_TEST_SCREEN) {
                DisplayCustomTestScreen();
            }
            else if(ActiveScreen == IDENTIFY_IC_SCREEN) 
            {
                if(IdentifyIcStatus == IDENTIFY_IC_STATUS_RUN) 
                {
                    if(IdentifyIcTestIndex < TestDataRecordsCount) 
                    {
                        uint8_t identifyResult = IdentifyIcTest(IdentifyIcTestIndex++, IdentifyIcPinsCount, IdentifyIcDeviceName);
                        if(identifyResult != INVALID_RESULT) {
                            ActiveScreen = IDENTIFY_IC_RESULT_SCREEN;
                        }
                    }
                    else {
                        ActiveScreen = IDENTIFY_IC_RESULT_SCREEN;
                    }
                    ScreenChanged = TRUE;
                }
                else {
                    DisplayIdentifyIcScreen();
                }
            }
#ifdef HARDWARE_DEBUG
            else if(ActiveScreen == HWDEBUG_SCREEN) {
                DisplayHwDebug();
            }
#else
            else if(ActiveScreen == ABOUT_SCREEN) {
                DisplayAboutScreen();
            }
#endif
            else if(ActiveScreen == SEARCH_IC_INFO_SCREEN) {
                DisplaySearchIcInfoScreen();
            }
            else if(ActiveScreen == TEST_SCREEN) {
                DisplayTestScreen();
            }
            else if(ActiveScreen == TEST_RESULT_SCREEN) {
                DisplayTestResultScreen();
            }
            else if(ActiveScreen == IDENTIFY_IC_RESULT_SCREEN) 
            {
                ResetHardware(FALSE);
                DisplayIdentifyIcResultScreen();
            }
        }
        else if(MenuChanged)
        {
            MenuChanged = FALSE;
            
            if(ActiveScreen == MAIN_SCREEN) {
                UpdateMainMenu();
            }
            else if(ActiveScreen == SEARCH_SCREEN) {
                UpdateSearchMenu();
            }
            else if(ActiveScreen == PC_TEST_SCREEN) {
                UpdatePcTestMenu();
            }
            else if(ActiveScreen == CUSTOM_TEST_SCREEN) 
            {
                if(CustomTestStatus == CUSTOM_TEST_READY) {
                    UpdateCustomTestMenu();
                }
                else if(CustomTestStatus == CUSTOM_TEST_EXISTS) {
                    UpdateCustomTestPreviousMenu();
                }
                else 
                {
                    if(CustomTestStep == CUSTOM_TEST_NAME) {
                        UpdateCustomTestNameStepMenu();
                    }
                    else if(CustomTestStep == CUSTOM_TEST_PINS) {
                        UpdateCustomTestPinsStepMenu();
                    }
                    else if(CustomTestStep == CUSTOM_TEST_CASE) {
                        UpdateCustomTestItemStepMenu();
                    }
                }
            }
            else if(ActiveScreen == IDENTIFY_IC_SCREEN) {
                UpdateIdentifyIcMenu();
            }
#ifdef HARDWARE_DEBUG
            else if(ActiveScreen == HWDEBUG_SCREEN)
            {
                UpdateHwDebugMenu();
            }
#endif
            else if(ActiveScreen == TEST_SCREEN) {
                UpdateTestMenu();
            }
        }
        else if(KeypadData != KEY_NONE)
        {            
            OnKeyUpdateScreen();
            KeypadData = KEY_NONE;
        }
    }
}

void __interrupt() ISR(void)
{
    if(RCIF)
    {
        if(OERR) // check for overrun error and reset
        {
            CREN = 0; 
            CREN = 1;
        }
        // RCREG contain UART data
        if(RCREG == 0x0A) {
            UartMessageLength = UART_MESSAGE_MAX_LENGTH;
        }
        else if(UartMessageLength < UART_MESSAGE_MAX_LENGTH) {
            UartMessage[UartMessageLength++] = RCREG;
        }
        RCIF = 0;
    }
    
    //process interface messages
    if(UartMessageLength == UART_MESSAGE_MAX_LENGTH && UartMessage[0] == '_')
    {
        // all cmds _I/_C/_G/_R OK mesage
        if(UartMessage[2] == ' ' && UartMessage[3] == 'O' && UartMessage[4] == 'K') {
            InterfaceCmdStatus = INTERFACE_CMD_DONE;
        }
        //key message
        else if(UartMessage[1] == 'K' && UartMessage[2] == ' ') {
            InterfaceCmdStatus = INTERFACE_CMD_DONE; // @TODO see WTF is here
            KeypadData = UartMessage[3];
        }
        //error message
        else if(UartMessage[1] == 'E') { // @TODO error handling
            InterfaceCmdStatus = INTERFACE_CMD_DONE;
        }
        
//        ClearString(UartMessage, UART_MESSAGE_MAX_LENGTH + 1);
        UartMessage[0] = NULL_CHAR;
        UartMessageLength = 0;
    }
}

void ProcessCommand(void)
{
    if(PcTestStatus == PC_TEST_WAITING && IsSameString(UartMessage, "TEST ", 5))
    {
        if(IsSameString(&UartMessage[5], "ABORT", 5)) 
        {
            ActiveScreen = MAIN_SCREEN;
            MainMenuSelectedItem = MAIN_MENU_PC_TEST;
            PcTestStatus = PC_TEST_INIT;
            ScreenChanged = TRUE;
        }
        else if(PcTestCurrentStep == PC_TEST_STEP_START && IsSameString(&UartMessage[5], "BEGIN", 5)) 
        {
            PcTestCurrentStep = PC_TEST_STEP_NAME;
        }
        else if(PcTestCurrentStep == PC_TEST_STEP_NAME && IsSameString(&UartMessage[5], "NAME", 4))
        {
            ClearString(CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
            uint8_t labelLength = CopyString(&UartMessage[10], CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH);
            
            if(labelLength > 0) {
                PcTestCurrentStep = PC_TEST_STEP_PINS;
            }
            else {
                UartWriteMessageAndWait(PC_TEST_ERROR_NO_LABEL);
            }
        }
        else if(PcTestCurrentStep == PC_TEST_STEP_PINS && IsSameString(&UartMessage[5], "PINS", 4))
        {
            uint8_t pinsCount = 0;
            
            if(UartMessage[10] != NULL_CHAR && UartMessage[10] >= '0' && UartMessage[10] <= '9' && UartMessage[11] != NULL_CHAR && UartMessage[11] >= '0' && UartMessage[11] <= '9') {
                pinsCount = 10 * (UartMessage[10] - '0') + (UartMessage[11] - '0');
            }
            
            CommonTestPinCount = 0;
            if(pinsCount > 0 && pinsCount <= COMMON_TEST_MAX_PINS && (pinsCount % 2) == 0) 
            {
                CommonTestPinCount = pinsCount;
                PcTestCurrentStep = PC_TEST_STEP_CASE;
                CommonTestCasesCount = 0;
            }
            else {
                UartWriteMessageAndWait(PC_TEST_ERROR_INVALID_NUMBER_OF_PINS);
            }
        }
        else if(PcTestCurrentStep == PC_TEST_STEP_CASE)
        {
            if(IsSameString(&UartMessage[5], "CASE", 4)) 
            {
                if(CommonTestCasesCount < COMMON_TEST_MAX_CASES) 
                {
                    uint8_t testPins = TransformTestData(CommonTestCases[CommonTestCasesCount], &UartMessage[10], CommonTestPinCount);
                    if(testPins == CommonTestPinCount) {
                        CommonTestCasesCount++;
                    }
                    else {
                        UartWriteMessageAndWait(PC_TEST_ERROR_TEST_INVALID_NUMBER_OF_PINS);
                    }
                }
            }
            else if(IsSameString(&UartMessage[5], "END", 3)) 
            {
                if(CommonTestCasesCount == 0) {
                    UartWriteMessageAndWait(PC_TEST_ERROR_NO_TESTS_FOUND);
                }
                else 
                {
                    UartWriteMessageAndWait("Test data complete");
                    PcTestStatus = PC_TEST_READY;
                    ScreenChanged = TRUE;
                }
            }
        }
        else {
            UartWriteMessageAndWait(UART_UNKNOWN_COMMAND);
        }
    }
    else {
        UartWriteMessageAndWait(UART_UNKNOWN_COMMAND);
    }
}

void InitInterfaceScreen(void)
{
    //display app info
    ClearRectangleOnInterface(0, 0, LCD_WIDTH, 8, COLOR_BLACK);
    DisplayStringOnInterface(7, 1, COLOR_YELLOW, COLOR_BLACK, TESTER_VERSION);
    
    DisplayMainScreen();
    
    UartWriteString(TESTER_VERSION);
    UartWriteString(" ready\n");
    
    __delay_ms(100);
}

void OnKeyUpdateScreen(void)
{
    uint8_t key = KeypadData;
    KeypadData = KEY_NONE;
    
    if(ActiveScreen == MAIN_SCREEN)
    {
#ifdef HARDWARE_DEBUG
        if(key == 'A' && MainMenuSelectedItem < MAIN_MENU_HWDEBUG) // down key
#else
        if(key == 'A' && MainMenuSelectedItem < MAIN_MENU_ABOUT) // down key
#endif
        {
            MainMenuSelectedItem++;
            MenuChanged = TRUE;
        }
        else if(key == 'C' && MainMenuSelectedItem > MAIN_MENU_SEARCH) // up key
        {
            MainMenuSelectedItem--;
            MenuChanged = TRUE;
        }
        else if(key == 'E') // enter key
        {
            if(MainMenuSelectedItem == MAIN_MENU_SEARCH) 
            {
                ClearString(SearchTerm, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
                SearchTerm[0] = '*';
                SearchMenuSelectedItem = SEARCH_MENU_SEARCH;
                SearchDeviceChanged = TRUE;
                
                ActiveScreen = SEARCH_SCREEN;
                ScreenChanged = TRUE;
            }
            else if(MainMenuSelectedItem == MAIN_MENU_PC_TEST)
            {
                ActiveScreen = PC_TEST_SCREEN;
                PcTestMenuSelectedItem = PC_TEST_START;
                PcTestStatus = PC_TEST_WAITING;
                PcTestCurrentStep = PC_TEST_STEP_START;
                ScreenChanged = TRUE;
            }
            else if(MainMenuSelectedItem == MAIN_MENU_CUSTOM_TEST)
            {
                ActiveScreen = CUSTOM_TEST_SCREEN;
                CustomTestStatus = CUSTOM_TEST_INIT;
                CustomTestStep = CUSTOM_TEST_NAME;
                CustomTestMenuSelectedItem = CUSTOM_TEST_START;
                ScreenChanged = TRUE;
            }
            else if(MainMenuSelectedItem == MAIN_MENU_IDENTIFY_IC)
            {
                ActiveScreen = IDENTIFY_IC_SCREEN;
                IdentifyIcMenuSelectedItem = IDENTIFY_IC_PINS;
                IdentifyIcPinsCount = 0;
                IdentifyIcStatus = IDENTIFY_IC_STATUS_INIT;
                ScreenChanged = TRUE;
            }
#ifdef HARDWARE_DEBUG
            else if(MainMenuSelectedItem == MAIN_MENU_HWDEBUG)
            {
                ActiveScreen = HWDEBUG_SCREEN;
                ScreenChanged = TRUE;
                HwDebugMenuSelectedItem = HWDEBUG_START;
                HwDebugStep = HWDEBUG_STEP_INIT;
                ClearString(HwDebugMessage, HWDEBUG_MESSAGE_LENGTH + 1);
                CopyString("Ready...", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
            }
#else
            else if(MainMenuSelectedItem == MAIN_MENU_ABOUT)
            {
                ActiveScreen = ABOUT_SCREEN;
                ScreenChanged = TRUE;
            }
#endif
        }
    }
    else if(ActiveScreen == SEARCH_SCREEN) 
    {
        if(key == 'A' && SearchMenuSelectedItem < SearchResultsCount) // down key
        {
            SearchMenuSelectedItem++;
            MenuChanged = TRUE;
        }
        else if(key == 'C' && SearchMenuSelectedItem > SEARCH_MENU_SEARCH) // up key
        {
            SearchMenuSelectedItem--;
            MenuChanged = TRUE;
        }
        else if(key == 'E' && SearchMenuSelectedItem > SEARCH_MENU_SEARCH) //select an device when not in search box
        {
            if(SearchResultIndexes[SearchMenuSelectedItem - 1] != TEST_DATA_INVALID_RECORD) // is valid device
            {
                ClearString(SelectedDeviceName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
                for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH; i++) {
                    SelectedDeviceName[i] = SearchResultNames[SearchMenuSelectedItem - 1][i];
                }
                SelectedDeviceIndex = SearchResultIndexes[SearchMenuSelectedItem - 1];
                
                ActiveScreen = TEST_SCREEN;
                TestMenuSelectedItem = TEST_START;
                ScreenChanged = TRUE;
            }
        }
        else if(key == 'B' && SearchMenuSelectedItem == SEARCH_MENU_SEARCH) //delete last char from search box
        {
            uint8_t index = 0;
            for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH; i++)
            {
                if(SearchTerm[i] != NULL_CHAR) {
                    index++;
                }
            }
            if(index > 1)
            {
                SearchTerm[index - 2] = '*';
                SearchTerm[index - 1] = NULL_CHAR;
                SearchDeviceChanged = TRUE;
                MenuChanged = TRUE;
            }
        }
        else if(key == 'B' && SearchMenuSelectedItem > SEARCH_MENU_SEARCH) //return to previous screen
        {
            ActiveScreen = MAIN_SCREEN;
            MainMenuSelectedItem = MAIN_MENU_SEARCH;
            ScreenChanged = TRUE;
        }
        else if(key == 'D' && SearchMenuSelectedItem > SEARCH_MENU_SEARCH) //display ic test info
        {
            ActiveScreen = SEARCH_IC_INFO_SCREEN;
            ScreenChanged = TRUE;
        }
        else if(key >= '0' && key <= '9' && SearchMenuSelectedItem == SEARCH_MENU_SEARCH) //add search chars
        {
            uint8_t index = 0;
            for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH; i++)
            {
                if(SearchTerm[i] != NULL_CHAR) {
                    index++;
                }
            }
            if(index > 0 && index < SELECTED_DEVICE_NAME_MAX_LENGTH)
            {
                SearchTerm[index - 1] = key;
                SearchTerm[index] = '*';
                SearchDeviceChanged = TRUE;
                MenuChanged = TRUE;
            }
        }
    }
    else if(ActiveScreen == PC_TEST_SCREEN) 
    {
        if(PcTestStatus == PC_TEST_READY) // test received from pc
        {
            if(key == 'A' && PcTestMenuSelectedItem < PC_TEST_CLOSE) // down key
            {
                PcTestMenuSelectedItem++;
                MenuChanged = TRUE;
            }
            else if(key == 'C' && PcTestMenuSelectedItem > PC_TEST_START) // up key
            {
                PcTestMenuSelectedItem--;
                MenuChanged = TRUE;
            }
            else if(key == 'E') // enter key
            {
                if(PcTestMenuSelectedItem == PC_TEST_START) // test start
                {
                    StartTest(PC_TEST_SCREEN);
                }
                else if(PcTestMenuSelectedItem == PC_TEST_SAVE) // save test in eeprom
                {
                    EepromSaveTest();
                    ScreenChanged = TRUE;
                }
                else // back to main screen
                {
                    ActiveScreen = MAIN_SCREEN;
                    MainMenuSelectedItem = MAIN_MENU_PC_TEST;
                    PcTestStatus = PC_TEST_INIT;
                    PcTestCurrentStep = PC_TEST_STEP_START;
                    ScreenChanged = TRUE;
                }
            }
        }
        else // abort receiving the test data from pc
        {
            ActiveScreen = MAIN_SCREEN;
            MainMenuSelectedItem = MAIN_MENU_PC_TEST;
            PcTestStatus = PC_TEST_INIT;
            PcTestCurrentStep = PC_TEST_STEP_START;
            ScreenChanged = TRUE;
        }
    }
    else if(ActiveScreen == CUSTOM_TEST_SCREEN) 
    {
        if(CustomTestStatus == CUSTOM_TEST_READY) // custom test is ready
        {
            if(key == 'A' && CustomTestMenuSelectedItem < CUSTOM_TEST_CLOSE) // down key
            {
                CustomTestMenuSelectedItem++;
                MenuChanged = TRUE;
            }
            else if(key == 'C' && CustomTestMenuSelectedItem > CUSTOM_TEST_START) // up key
            {
                CustomTestMenuSelectedItem--;
                MenuChanged = TRUE;
            }
            else if(key == 'E') // enter key
            {
                if(CustomTestMenuSelectedItem == CUSTOM_TEST_START) // start test
                {
                    StartTest(CUSTOM_TEST_SCREEN);
                }
                else if(CustomTestMenuSelectedItem == CUSTOM_TEST_SAVE) // save test in eeprom
                {
                    EepromSaveTest();
                    ScreenChanged = TRUE;
                }
                else // return to main screen
                {
                    ActiveScreen = MAIN_SCREEN;
                    MainMenuSelectedItem = MAIN_MENU_CUSTOM_TEST;
                    CustomTestStep = CUSTOM_TEST_NAME;
                    ScreenChanged = TRUE;
                }
            }
        }
        else if(CustomTestStatus == CUSTOM_TEST_EXISTS) // a test is already present in eeprom
        {
            if(key == 'A' && CustomTestMenuSelectedItem < CUSTOM_TEST_PREVIOUS_NO)  // down key
            {
                CustomTestMenuSelectedItem++;
                MenuChanged = TRUE;
            }
            else if(key == 'C' && CustomTestMenuSelectedItem > CUSTOM_TEST_PREVIOUS_YES) // up key
            {
                CustomTestMenuSelectedItem--;
                MenuChanged = TRUE;
            }
            else if(key == 'E') // enter key
            {
                if(CustomTestMenuSelectedItem == CUSTOM_TEST_PREVIOUS_YES) // load test from eeprom
                {
                    EepromLoadTest();
                    CustomTestStatus = CUSTOM_TEST_READY;
                    CustomTestMenuSelectedItem = CUSTOM_TEST_START;
                    ScreenChanged = TRUE;
                }
                else // ignore the test from eeprom an continue to fill a new test
                {
                    CustomTestStatus = CUSTOM_TEST_FILL;
                    CustomTestMenuSelectedItem = CUSTOM_TEST_NAME_FILL;
                    ClearString(CommonTestName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
                    ScreenChanged = TRUE;
                }
            }
        }
        else if(CustomTestStatus == CUSTOM_TEST_FILL) // fill a new test
        {
            if(CustomTestStep == CUSTOM_TEST_NAME) // test name
            {
                if(key == 'A' && CustomTestMenuSelectedItem < CUSTOM_TEST_NAME_OK) // down key
                {
                    CustomTestMenuSelectedItem++;
                    MenuChanged = TRUE;
                }
                else if(key == 'C' && CustomTestMenuSelectedItem > CUSTOM_TEST_NAME_FILL) // up key
                {
                    CustomTestMenuSelectedItem--;
                    MenuChanged = TRUE;
                }
                else if(key == 'E' && CustomTestMenuSelectedItem == CUSTOM_TEST_NAME_OK) // finish test name
                {
                    if(CommonTestName[0] != NULL_CHAR)
                    {
                        CommonTestPinCount = 0;
                        CustomTestStep = CUSTOM_TEST_PINS;
                        CustomTestMenuSelectedItem = CUSTOM_TEST_PINS_FILL;
                        ScreenChanged = TRUE;
                    }
                }
                else if(key >= '0' && key <= '9' && CustomTestMenuSelectedItem == CUSTOM_TEST_NAME_FILL) // add new chars to test name
                {
                    uint8_t index = 0;
                    for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH; i++)
                    {
                        if(CommonTestName[i] != NULL_CHAR) {
                            index++;
                        }
                    }
                    if(index < SELECTED_DEVICE_NAME_MAX_LENGTH)
                    {
                        CommonTestName[index] = key;
                        MenuChanged = TRUE;
                    }
                }
                else if(key == 'B' && CustomTestMenuSelectedItem == CUSTOM_TEST_NAME_FILL) // delete last char from test name
                {
                    uint8_t index = 0;
                    for(uint8_t i = 0; i < SELECTED_DEVICE_NAME_MAX_LENGTH; i++)
                    {
                        if(CommonTestName[i] != NULL_CHAR) {
                            index++;
                        }
                    }
                    if(index > 0)
                    {
                        CommonTestName[index - 1] = NULL_CHAR;
                        MenuChanged = TRUE;
                    }
                }
            }
            else if(CustomTestStep == CUSTOM_TEST_PINS)
            {
                if(key == 'A' && CustomTestMenuSelectedItem < CUSTOM_TEST_PINS_OK) // down key
                {
                    CustomTestMenuSelectedItem++;
                    MenuChanged = TRUE;
                }
                else if(key == 'C' && CustomTestMenuSelectedItem > CUSTOM_TEST_PINS_FILL) // up key
                {
                    CustomTestMenuSelectedItem--;
                    MenuChanged = TRUE;
                }
                else if(key == 'E' && CustomTestMenuSelectedItem == CUSTOM_TEST_PINS_OK) // finish test pins
                {
                    if(CommonTestPinCount > 0 && (CommonTestPinCount % 2) == 0)
                    {
                        CustomTestStep = CUSTOM_TEST_CASE;
                        CustomTestMenuSelectedItem = CUSTOM_TEST_CASE_FILL;
                        ClearString(CustomTestItemData, COMMON_TEST_MAX_PINS + 1); // up to 28 pins in tests
                        CommonTestCasesCount = 0;
                        ScreenChanged = TRUE;
                    }
                }
                else if(key >= '0' && key <= '9' && CustomTestMenuSelectedItem == CUSTOM_TEST_PINS_FILL) // add new pins
                {
                    uint8_t pins = CommonTestPinCount * 10 + (key - '0');
                    if(pins <= COMMON_TEST_MAX_PINS) 
                    {
                        CommonTestPinCount = pins;
                        MenuChanged = TRUE;
                    }
                }
                else if(key == 'B' && CustomTestMenuSelectedItem == CUSTOM_TEST_PINS_FILL) // delete last pins
                {
                    CommonTestPinCount = CommonTestPinCount / 10;
                    MenuChanged = TRUE;
                }
            }
            else if(CustomTestStep == CUSTOM_TEST_CASE)
            {
                if(key == 'A' && CustomTestMenuSelectedItem < CUSTOM_TEST_CASE_DONE) // down key
                {
                    CustomTestMenuSelectedItem++;
                    MenuChanged = TRUE;
                }
                else if(key == 'C' && CustomTestMenuSelectedItem > CUSTOM_TEST_CASE_FILL) // up key
                {
                    CustomTestMenuSelectedItem--;
                    MenuChanged = TRUE;
                }
                else if(key == 'E' && CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_DONE) // finish test tests
                {
                    if(CommonTestCasesCount > 0)
                    {
                        CustomTestStatus = CUSTOM_TEST_READY;
                        CustomTestMenuSelectedItem = CUSTOM_TEST_START;
                        ScreenChanged = TRUE;
                    }
                }
                else if(key == 'E' && CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_NEXT) // add a new test
                {
                    if(CustomTestItemData[0] != NULL_CHAR)
                    {
                        uint8_t testPins = TransformTestData(CommonTestCases[CommonTestCasesCount], CustomTestItemData, CommonTestPinCount);
                        if(testPins == CommonTestPinCount) 
                        {
                            CommonTestCasesCount++;
                            CustomTestMenuSelectedItem = CUSTOM_TEST_CASE_FILL;
                            ClearString(CustomTestItemData, COMMON_TEST_MAX_PINS + 1);
                            MenuChanged = TRUE;
                        }
                    }
                }
                else if(key == 'B' && CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_FILL) // delete last pin from current test
                {
                    uint8_t index = 0;
                    for(uint8_t i = 0; i < COMMON_TEST_MAX_PINS; i++)
                    {
                        if(CustomTestItemData[i] != NULL_CHAR) {
                            index++;
                        }
                    }
                    if(index > 0)
                    {
                        CustomTestItemData[index - 1] = NULL_CHAR;
                        MenuChanged = TRUE;
                    }
                }
                else if(key >= '0' && key <= '7' && CustomTestMenuSelectedItem == CUSTOM_TEST_CASE_FILL) // add pins to current test
                {
                    uint8_t index = 0;
                    for(uint8_t i = 0; i < COMMON_TEST_MAX_PINS; i++)
                    {
                        if(CustomTestItemData[i] != NULL_CHAR) {
                            index++;
                        }
                    }
                    if(index < COMMON_TEST_MAX_PINS)
                    {
                        char pin = NULL_CHAR;
                        switch(key)
                        {
                            case '0':
                            case '1':
                                pin = key;
                                break;
                            case '2':
                                pin = 'L';
                                break;
                            case '3':
                                pin = 'H';
                                break;
                            case '4':
                                pin = 'G';
                                break;
                            case '5':
                                pin = 'V';
                                break;
                            case '6':
                                pin = 'X';
                                break;
                            case '7':
                                pin = 'C';
                                break;
                        }
                        
                        CustomTestItemData[index] = pin;
                        MenuChanged = TRUE;
                    }
                }
            }
        }
    }
    else if(ActiveScreen == IDENTIFY_IC_SCREEN)
    {
        if(IdentifyIcStatus == IDENTIFY_IC_STATUS_INIT)
        {
            if(key == 'A' && IdentifyIcMenuSelectedItem < IDENTIFY_IC_CLOSE) // down key
            {
                IdentifyIcMenuSelectedItem++;
                MenuChanged = TRUE;
            }
            else if(key == 'C' && IdentifyIcMenuSelectedItem > IDENTIFY_IC_PINS) // up key
            {
                IdentifyIcMenuSelectedItem--;
                MenuChanged = TRUE;
            }
            else if(key == 'E' && IdentifyIcMenuSelectedItem == IDENTIFY_IC_CLOSE) // back to main menu
            {
                ActiveScreen = MAIN_SCREEN;
                MainMenuSelectedItem = MAIN_MENU_IDENTIFY_IC;
                ScreenChanged = TRUE;
            }
            else if(key == 'E' && IdentifyIcMenuSelectedItem == IDENTIFY_IC_START) // finish test pins
            {
                if(IdentifyIcPinsCount > 0 && (IdentifyIcPinsCount % 2) == 0)
                {
                    IdentifyIcStatus = IDENTIFY_IC_STATUS_RUN;
                    IdentifyIcTestIndex = 0;
                    ClearString(IdentifyIcDeviceName, SELECTED_DEVICE_NAME_MAX_LENGTH + 1);
                    ResetHardware(TRUE);
                    __delay_ms(100);
                    DisplayIdentifyIcRunScreen();
                    ScreenChanged = TRUE;
                }
            }
            else if(key >= '0' && key <= '9' && IdentifyIcMenuSelectedItem == IDENTIFY_IC_PINS) // add new pins
            {
                uint8_t pins = IdentifyIcPinsCount * 10 + (key - '0');
                if(pins <= COMMON_TEST_MAX_PINS) 
                {
                    IdentifyIcPinsCount = pins;
                    MenuChanged = TRUE;
                }
            }
            else if(key == 'B' && IdentifyIcMenuSelectedItem == IDENTIFY_IC_PINS) // delete last pins
            {
                IdentifyIcPinsCount = IdentifyIcPinsCount / 10;
                MenuChanged = TRUE;
            }
        }
        else
        {
            ActiveScreen = IDENTIFY_IC_RESULT_SCREEN;
            ScreenChanged = TRUE;
        }
    }
#ifdef HARDWARE_DEBUG
    else if(ActiveScreen == HWDEBUG_SCREEN) // back to main screen
    {
        if(key == 'A' && HwDebugMenuSelectedItem < HWDEBUG_STOP) // down key
        {
            HwDebugMenuSelectedItem++;
            MenuChanged = TRUE;
        }
        else if(key == 'C' && HwDebugMenuSelectedItem > HWDEBUG_START) // up key
        {
            HwDebugMenuSelectedItem--;
            MenuChanged = TRUE;
        }
        else if(key == 'E' && HwDebugMenuSelectedItem == HWDEBUG_START) 
        {
            switch(HwDebugStep)
            {
                case HWDEBUG_STEP_INIT:
                    HwDebugStep = HWDEBUG_STEP_OUT_ALL_LOW;
                    CopyString("OUT ALL LOW ", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugSetOutAllLow();
                    break;
                case HWDEBUG_STEP_OUT_ALL_LOW:
                    HwDebugStep = HWDEBUG_STEP_OUT_ALL_HIGH;
                    CopyString("OUT ALL HIGH", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugSetOutAllHigh();
                    break;
                case HWDEBUG_STEP_OUT_ALL_HIGH:
                    HwDebugStep = HWDEBUG_STEP_OUT_ALT_HL;
                    CopyString("OUT ALL HL  ", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugSetOutAllHL();
                    break;
                case HWDEBUG_STEP_OUT_ALT_HL:
                    HwDebugStep = HWDEBUG_STEP_OUT_ALT_LH;
                    CopyString("OUT ALL LH  ", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugSetOutAllLH();
                    break;
                case HWDEBUG_STEP_OUT_ALT_LH:
                    HwDebugStep = HWDEBUG_STEP_RELAY_1;
                    CopyString("7 GND/14 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay1();
                    break;
                case HWDEBUG_STEP_RELAY_1:
                    HwDebugStep = HWDEBUG_STEP_RELAY_2;
                    CopyString("8 GND/16 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay2();
                    break;
                case HWDEBUG_STEP_RELAY_2:
                    HwDebugStep = HWDEBUG_STEP_RELAY_3;
                    CopyString("9 GND/18 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay3();
                    break;
                case HWDEBUG_STEP_RELAY_3:
                    HwDebugStep = HWDEBUG_STEP_RELAY_4;
                    CopyString("12 GND/5 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay4();
                    break;
                case HWDEBUG_STEP_RELAY_4:
                    HwDebugStep = HWDEBUG_STEP_RELAY_5;
                    CopyString("10 GND/20 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay5();
                    break;
                case HWDEBUG_STEP_RELAY_5:
                    HwDebugStep = HWDEBUG_STEP_RELAY_6;
                    CopyString("12 GND/24 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay6();
                    break;
                case HWDEBUG_STEP_RELAY_6:
                    HwDebugStep = HWDEBUG_STEP_RELAY_7;
                    CopyString("14 GND/28 VCC", HwDebugMessage, HWDEBUG_MESSAGE_LENGTH);
                    HwDebugRelay7();
                    break;
                case HWDEBUG_STEP_RELAY_7:
                    HwDebugStep = HWDEBUG_STEP_END;
                    ResetHardware(TRUE);
                    break;
            }
            MenuChanged = TRUE;
        }
        else if(key == 'E' && HwDebugMenuSelectedItem == HWDEBUG_STOP) // finish debug
        {
            ActiveScreen = MAIN_SCREEN;
            MainMenuSelectedItem = MAIN_MENU_HWDEBUG;
            ScreenChanged = TRUE;
        }
    }
#else
    else if(ActiveScreen == ABOUT_SCREEN) // back to main screen
    {
        ActiveScreen = MAIN_SCREEN;
        MainMenuSelectedItem = MAIN_MENU_ABOUT;
        ScreenChanged = TRUE;
    }
#endif
    else if(ActiveScreen == SEARCH_IC_INFO_SCREEN) // back to search screen
    {
        ActiveScreen = SEARCH_SCREEN;
        ScreenChanged = TRUE;
    }
    else if(ActiveScreen == TEST_SCREEN) 
    {
        if(key == 'A' && TestMenuSelectedItem < TEST_CLOSE) // down key
        {
            TestMenuSelectedItem++;
            MenuChanged = TRUE;
        }
        else if(key == 'C' && TestMenuSelectedItem > TEST_START) // up key
        {
            TestMenuSelectedItem--;
            MenuChanged = TRUE;
        }
        else if(key == 'E') // enter key
        {
            if(TestMenuSelectedItem == TEST_START) { // can start the tests
                StartTest(TEST_SCREEN);
            }
            else // return to main screen
            {
                ActiveScreen = MAIN_SCREEN;
                MainMenuSelectedItem = MAIN_MENU_SEARCH;
                ScreenChanged = TRUE;
            }
        }
    }
    else if(ActiveScreen == TEST_RESULT_SCREEN)
    {
        ActiveScreen = ReturnToTestScreen;
        ScreenChanged = TRUE;
    }
    else if(ActiveScreen == IDENTIFY_IC_RESULT_SCREEN) // back to search screen
    {
        ActiveScreen = MAIN_SCREEN;
        MainMenuSelectedItem = MAIN_MENU_IDENTIFY_IC;
        ScreenChanged = TRUE;
    }
}
