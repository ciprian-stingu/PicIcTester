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
 * File:   relay.c
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:13 PM
 */


#include "main.h"
#include "relay.h"

void RelaysInit(void)
{
    TRISA4 = 0;
    TRISB0 = 0;
    TRISB3 = 0;
    
    EnableRelay(RELAY_NONE);
}

void EnableRelay(uint8_t relay)
{
    //disable relays
    RA4 = 0;
    RB0 = 0;
    RB3 = 0;
    
    if(relay & 0x1) {
        RA4 = 1;
    }
    if(relay & 0x2) {
        RB0 = 1;
    }
    if(relay & 0x4) {
        RB3 = 1;
    }
    
    __delay_ms(5);
}
