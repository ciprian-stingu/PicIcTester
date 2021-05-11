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
 * File:   keypad.c
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:11 PM
 */

#include "main.h"
#include "keypad.h"

void KeypadInit(void)
{
    TRISB7 = 1;
    TRISB6 = 1;
    TRISB5 = 1;
    TRISB4 = 1;
    
    RBIE = 1;
}

void Timer0Init(void)
{
    T0CS = 0; // Timer mode
    PSA = 0;  // prescaler for timer0
    
    // 1:256 prescaler
    PS0 = 1;
    PS1 = 1;
    PS2 = 1;    
}

void ResetKeypadData(void)
{
    uint8_t data = RB4 | RB5 | RB6 | RB7;
    
    T0IE = 0; // stop timer0
    //clear keypad data just to be sure
    KeypadDebounceCounter = 0;
    KeypadDataDebounced = KEY_NONE;
    KeypadData = KEY_NONE;
    
    TMR0IF = 0;
    RBIF = 0;
}
