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
 * File:   keypad.h
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:11 PM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

#ifdef	__cplusplus
extern "C" {
#endif

#define KEY_NONE 0x00
char KeypadData = KEY_NONE;

void KeypadInit(void);
void ResetKeypadData(void);

uint8_t KeypadDataDebounced = KEY_NONE;
uint8_t KeypadDebounceCounter = 0;
#define DEBOUNCER_DELAY 3

void Timer0Init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* KEYPAD_H */

