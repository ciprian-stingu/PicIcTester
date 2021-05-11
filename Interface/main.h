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
 * File:   main.h
 * Author: Ciprian Stingu
 *
 * Created on July 7, 2020, 7:12 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <pic16f628a.h> 
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#pragma config CP    = OFF
#pragma config CPD   = OFF
#pragma config LVP   = OFF
#pragma config BOREN = ON
#pragma config MCLRE = OFF
#pragma config PWRTE = OFF
#pragma config WDTE  = OFF
#pragma config FOSC  = HS

#define _XTAL_FREQ 20000000

#include "lcd.h"
#include "uart.h"
#include "keypad.h"
#include "relay.h"

void __interrupt() ISR(void);

//command processor
void ProcessCommand(void);

//helper function
uint8_t AsciiHexToByte(char *value);

// booting message
void ShowBootingMessage(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

