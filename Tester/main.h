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
 * File:   main.h
 * Author: Ciprian Stingu
 *
 * Created on July 17, 2020, 2:00 PM
 */

#ifndef MAIN_H
#define	MAIN_H

//#define HARDWARE_DEBUG

#include <xc.h>
#include <pic18f452.h> 
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#pragma config OSC     = HSPLL
#pragma config OSCS    = OFF
#pragma config PWRT    = OFF
#pragma config BOR     = ON
#pragma config BORV    = 25
#pragma config WDT     = OFF
//#pragma config WDTPS = 128
#pragma config CCP2MUX = OFF
#pragma config STVR    = ON
#pragma config LVP     = OFF
#pragma config DEBUG   = OFF
#pragma config CP0     = OFF
#pragma config CP1     = OFF
#pragma config CP2     = OFF
#pragma config CP3     = OFF
#pragma config CPB     = OFF
#pragma config CPD     = OFF
#pragma config WRT0    = OFF
#pragma config WRT1    = OFF
#pragma config WRT2    = OFF
#pragma config WRT3    = OFF
#pragma config WRTB    = OFF
#pragma config WRTC    = OFF
#pragma config WRTD    = OFF
#pragma config EBTR0   = OFF
#pragma config EBTR1   = OFF
#pragma config EBTR2   = OFF
#pragma config EBTR3   = OFF
#pragma config EBTRB   = OFF
    
#define _XTAL_FREQ 40000000
#define TESTER_VERSION "IC Tester v1.0"
#define FALSE 0
#define TRUE 1
#define NULL_CHAR 0x00

#include "i2c.h"
#include "uart.h"
#include "graph.h"
#include "utils.h"
#include "screen.h"
#include "test.h"
#include "eeprom.h"

void __interrupt() ISR(void);

#define INTERFACE_CMD_READY 0
#define INTERFACE_CMD_DONE  1
uint8_t InterfaceCmdStatus = INTERFACE_CMD_READY;

void ProcessCommand(void);

#define KEY_NONE 0x00
char KeypadData = KEY_NONE;

void InitInterfaceScreen(void);
void OnKeyUpdateScreen(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

