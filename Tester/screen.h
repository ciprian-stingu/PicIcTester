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
 * File:   screen.h
 * Author: Ciprian Stingu
 *
 * Created on July 29, 2020, 5:56 PM
 */

#ifndef SCREEN_H
#define	SCREEN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAIN_SCREEN               0
#define SEARCH_SCREEN             1
#define PC_TEST_SCREEN            2
#define CUSTOM_TEST_SCREEN        3
#define IDENTIFY_IC_SCREEN        4
#ifdef HARDWARE_DEBUG
#define HWDEBUG_SCREEN            5    
#else
#define ABOUT_SCREEN              5
#endif
#define SEARCH_IC_INFO_SCREEN     6
#define TEST_SCREEN               7
#define TEST_RESULT_SCREEN        8
#define IDENTIFY_IC_RESULT_SCREEN 9
uint8_t ActiveScreen = MAIN_SCREEN;
uint8_t ScreenChanged = FALSE;
uint8_t MenuChanged = FALSE;

#define MAIN_MENU_SEARCH      0
#define MAIN_MENU_PC_TEST     1
#define MAIN_MENU_CUSTOM_TEST 2
#define MAIN_MENU_IDENTIFY_IC 3
#ifdef HARDWARE_DEBUG
#define MAIN_MENU_HWDEBUG     4
#else
#define MAIN_MENU_ABOUT       4
#endif

#define MAIN_MENU_ITEM_TEXT_0 "Search & Test "
#define MAIN_MENU_ITEM_TEXT_1 "PC Test       "
#define MAIN_MENU_ITEM_TEXT_2 "Custom Test   "
#define MAIN_MENU_ITEM_TEXT_3 "Identify IC   "
#ifdef HARDWARE_DEBUG
#define MAIN_MENU_ITEM_TEXT_4 "Hardware Test "
#else
#define MAIN_MENU_ITEM_TEXT_4 "About         "
#endif
uint8_t MainMenuSelectedItem = MAIN_MENU_SEARCH;

void DisplayMainScreen(void);
void UpdateMainMenu(void);

#define SELECTED_DEVICE_NAME_MAX_LENGTH 14
#define SEARCH_MENU_SEARCH 0
#define SEARCH_RESULTS     6
uint8_t SearchMenuSelectedItem = SEARCH_MENU_SEARCH;
uint8_t SearchDeviceChanged = FALSE;
char SearchTerm[SELECTED_DEVICE_NAME_MAX_LENGTH + 1];
char SearchResultNames[SEARCH_RESULTS][SELECTED_DEVICE_NAME_MAX_LENGTH + 1];
uint16_t SearchResultIndexes[SEARCH_RESULTS];
uint8_t SearchResultsCount = 0;

void DisplaySearchScreen(void);
void UpdateSearchMenu(void);
void DisplaySearchIcInfoScreen(void);

#define TEST_START 0
#define TEST_CLOSE 1
uint8_t TestMenuSelectedItem = TEST_START;

void DisplayTestScreen(void);
void UpdateTestMenu(void);

#define PC_TEST_START 0
#define PC_TEST_SAVE  1
#define PC_TEST_CLOSE 2
#define PC_TEST_INIT    0
#define PC_TEST_WAITING 1
#define PC_TEST_READY   2
#define PC_TEST_STEP_START 0
#define PC_TEST_STEP_NAME  1
#define PC_TEST_STEP_PINS  2
#define PC_TEST_STEP_CASE  3
uint8_t PcTestMenuSelectedItem = PC_TEST_START;
uint8_t PcTestStatus = PC_TEST_INIT;
uint8_t PcTestCurrentStep = PC_TEST_STEP_START;

void DisplayPcTestScreen(void);
void UpdatePcTestMenu(void);

#define CUSTOM_TEST_START 0
#define CUSTOM_TEST_SAVE  1
#define CUSTOM_TEST_CLOSE 2

#define CUSTOM_TEST_PREVIOUS_YES 0
#define CUSTOM_TEST_PREVIOUS_NO  1

#define CUSTOM_TEST_NAME   0
#define CUSTOM_TEST_PINS   1
#define CUSTOM_TEST_CASE   3

#define CUSTOM_TEST_INIT   0
#define CUSTOM_TEST_EXISTS 1
#define CUSTOM_TEST_FILL   2
#define CUSTOM_TEST_READY  3

#define CUSTOM_TEST_NAME_FILL 0
#define CUSTOM_TEST_NAME_OK   1

#define CUSTOM_TEST_PINS_FILL 0
#define CUSTOM_TEST_PINS_OK   1

#define CUSTOM_TEST_CASE_FILL 0
#define CUSTOM_TEST_CASE_NEXT 1
#define CUSTOM_TEST_CASE_DONE 2

uint8_t CustomTestMenuSelectedItem = CUSTOM_TEST_START;
uint8_t CustomTestStep = CUSTOM_TEST_NAME;
uint8_t CustomTestStatus = CUSTOM_TEST_INIT;

void DisplayCustomTestScreen(void);
void UpdateCustomTestMenu(void);
void UpdateCustomTestPreviousMenu(void);
void UpdateCustomTestNameStepMenu(void);
void UpdateCustomTestPinsStepMenu(void);
void UpdateCustomTestItemStepMenu(void);

void DisplayTestResultScreen(void);

#define IDENTIFY_IC_PINS  0
#define IDENTIFY_IC_START 1
#define IDENTIFY_IC_CLOSE 2

#define IDENTIFY_IC_STATUS_INIT 0
#define IDENTIFY_IC_STATUS_RUN  1

uint8_t IdentifyIcMenuSelectedItem = IDENTIFY_IC_PINS;
uint8_t IdentifyIcPinsCount = 0;
uint8_t IdentifyIcStatus = IDENTIFY_IC_STATUS_INIT;
uint16_t IdentifyIcTestIndex = 0;
char IdentifyIcDeviceName[SELECTED_DEVICE_NAME_MAX_LENGTH + 1];

void DisplayIdentifyIcScreen(void);
void UpdateIdentifyIcMenu(void);
void DisplayIdentifyIcRunScreen(void);
void DisplayIdentifyIcResultScreen(void);

#ifdef HARDWARE_DEBUG

#define HWDEBUG_START 0
#define HWDEBUG_NEXT  0
#define HWDEBUG_STOP  1

#define HWDEBUG_STEP_INIT         0
#define HWDEBUG_STEP_OUT_ALL_LOW  1
#define HWDEBUG_STEP_OUT_ALL_HIGH 2
#define HWDEBUG_STEP_OUT_ALT_HL   3
#define HWDEBUG_STEP_OUT_ALT_LH   4
#define HWDEBUG_STEP_RELAY_1      5
#define HWDEBUG_STEP_RELAY_2      6
#define HWDEBUG_STEP_RELAY_3      7
#define HWDEBUG_STEP_RELAY_4      8
#define HWDEBUG_STEP_RELAY_5      9
#define HWDEBUG_STEP_RELAY_6      10
#define HWDEBUG_STEP_RELAY_7      11
#define HWDEBUG_STEP_END          12

#define HWDEBUG_MESSAGE_LENGTH 15

uint8_t HwDebugMenuSelectedItem = HWDEBUG_START;
uint8_t HwDebugStep = HWDEBUG_STEP_INIT;
char HwDebugMessage[HWDEBUG_MESSAGE_LENGTH + 1];

void DisplayHwDebug(void);
void UpdateHwDebugMenu(void);
#else
void DisplayAboutScreen(void);
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* SCREEN_H */

