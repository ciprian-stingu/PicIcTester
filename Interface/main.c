/**
 * 
 * PicIcTester Interface
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
 * Created on July 7, 2020, 7:11 PM
 */


#include "main.h"

void main(void) 
{
    // Enable all unmasked global INT
    GIE = 1;
    // Enable Peripheral interrupt
    PEIE = 1;
    // disable comparators
    CM0 = 1;
    CM1 = 1;
    CM2 = 1;
    
    RelaysInit();
    KeypadInit();
    UartInit();
    LcdInit();
    Timer0Init();
    
    ShowBootingMessage();
    
    //memset call will make code bigger...
    for(uint8_t i = 0; i <= UART_MESSAGE_MAX_LENGTH; i++) {
        UartMessage[i] = 0x00;
    }
    UartMessageLength = 0;
    
    __delay_ms(2000);
    UartWriteString("_I OK\n"); //ready to accept messages
    
    ResetKeypadData();
    
    while(1)
    {
        if(UartMessageLength == UART_MESSAGE_MAX_LENGTH)
        {
            ProcessCommand();
            
            for(uint8_t i = 0; i <= UART_MESSAGE_MAX_LENGTH; i++) {
                UartMessage[i] = 0x00;
            }
            UartMessageLength = 0;
        }
        else if(KeypadData != KEY_NONE) //send key as long as no command is executed
        {
            // send key data
            // "_K key"
            char keyCommand[6] = {'_', 'K', ' ', KeypadData, '\n'};
            UartWriteString(keyCommand);
            KeypadData = KEY_NONE;
        }
    }
}

void ProcessCommand(void)
{
    if(UartMessage[0] != '_') {
        return;
    }
    
    // relay command
    // "_R no"
    // no - 1/2/3/4/5/6/7 in hex format
    if(UartMessage[1] == 'R' && UartMessage[2] == ' ') 
    {
        uint8_t relayNo = AsciiHexToByte(&UartMessage[3]);
        
        EnableRelay(relayNo);
        UartWriteString("_R OK\n");
    }
    // clear screen
    // "_C xxyywwhhbb"
    // all numbers in ascii hex format
    // xx - x origin, yy - y origin
    // ww - image width, hh - image height
    // bb - background color
    else if(UartMessage[1] == 'C' && UartMessage[2] == ' ')
    {
        uint8_t x = AsciiHexToByte(&UartMessage[3]);
        uint8_t y = AsciiHexToByte(&UartMessage[5]);
        uint8_t width = AsciiHexToByte(&UartMessage[7]);
        uint8_t height = AsciiHexToByte(&UartMessage[9]);
        uint8_t bgColor = AsciiHexToByte(&UartMessage[11]);
        
        LcdClear(x, y, width, height, bgColor);
        UartWriteString("_C OK\n");
    }
    // display graphic data
    // "_G xxyywwhhccbb graphic_data_bytes_raw
    // all numbers in ascii hex format
    // xx - x origin, yy - y origin
    // ww - image width, hh - image height
    // cc - foreground color, bb - background color
    // graphic data in ascii hex format
    // max of 31 hex bytes of graphic data
    else if(UartMessage[1] == 'G' && UartMessage[2] == ' ')
    {
        uint8_t x = AsciiHexToByte(&UartMessage[3]);
        uint8_t y = AsciiHexToByte(&UartMessage[5]);
        uint8_t width = AsciiHexToByte(&UartMessage[7]);
        uint8_t height = AsciiHexToByte(&UartMessage[9]);
        uint8_t color = AsciiHexToByte(&UartMessage[11]);
        uint8_t bgColor = AsciiHexToByte(&UartMessage[13]);
        
        LcdDisplayGraphic(x, y, width, height, color, bgColor, &UartMessage[16]);
        UartWriteString("_G OK\n");
    }
    // unknown command
    // so send error 
    else 
    {
        __delay_ms(5);
        UartWriteString("_E\n");
    }
}

void __interrupt() ISR(void)
{
    //new uart data
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
    
    //new keypad key
    if(RBIF)
    {   
        T0IE = 0; // stop timer0
        
        // compose keypad data from 4 bits 
        uint8_t data = ~(((RB4 & 0x01) << 3) | ((RB5 & 0x01) << 2) | ((RB6 & 0x01) << 1) | (RB7 & 0x01)) & 0x0F;
        //add it to debounced key data
        KeypadDataDebounced |= data;
            
        // reset timer0
        //60 - 10ms
        //21 - 12ms
        TMR0 = 0x3C;
        KeypadDebounceCounter = 0; // reset the delay
        T0IE = 1; // reenable timer0
        
        RBIF = 0;
    }
    
    //debounce keypad data
    if(TMR0IF && T0IE)
    {
        //if no pins active compose the keypad data
        if(RB4 && RB5 && RB6 && RB7 && KeypadDebounceCounter++ >= DEBOUNCER_DELAY && KeypadDataDebounced)
        {
            if(KeypadDataDebounced > 0 && KeypadDataDebounced < 0x0A) {
                KeypadData = KeypadDataDebounced + '0';
            }
            else if(KeypadDataDebounced == 0x0A) {
                KeypadData = '0';
            }
            else {
                KeypadData = (KeypadDataDebounced - 11) + 'A';
            }
            
            T0IE = 0; // stop timer0
            KeypadDataDebounced = KEY_NONE;
            KeypadDebounceCounter = 0;
        }
        
        TMR0IF = 0;
    }
}

uint8_t AsciiHexToByte(char *value)
{
    uint8_t result = (*value <= '9' ? *value - '0' : *value - 'A' + 10) << 4;
    value++;
    result |= (*value <= '9' ? *value - '0' : *value - 'A' + 10);
    
    return result;
}

void ShowBootingMessage(void)
{
    LcdDisplayGraphic(16, 26, 66, 1, 3, 255, "FFFFFFFFFFFFFFFFFF");
    LcdDisplayGraphic(16, 26, 1, 14, 3, 255, "8080808080808080808080808080");
    LcdDisplayGraphic(16, 39, 66, 1, 3, 255, "FFFFFFFFFFFFFFFFFF");
    LcdDisplayGraphic(81, 26, 1, 14, 3, 255, "8080808080808080808080808080");
    LcdDisplayGraphic(19, 40, 66, 1, 28, 255, "FFFFFFFFFFFFFFFFFF");
    LcdDisplayGraphic(19, 41, 66, 1, 28, 255, "FFFFFFFFFFFFFFFFFF");
    LcdDisplayGraphic(82, 29, 1, 13, 28, 255, "80808080808080808080808080");
    LcdDisplayGraphic(83, 29, 1, 13, 28, 255, "80808080808080808080808080");
    LcdDisplayGraphic(84, 29, 1, 13, 28, 255, "80808080808080808080808080");
    LcdDisplayGraphic(19, 30, 30, 6, 0, 255, "7000000048C31C3871248810492488104924881070C30838");
    LcdDisplayGraphic(49, 30, 30, 6, 0, 255, "0000000048E0000069000000796000005920000048C10410");
}
