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
 * File:   uart.c
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:17 PM
 */


#include "main.h"
#include "uart.h"

void UartInit(void)
{
    //set RX/TX pins as specified in manual
    TRISC6 = 0;
    TRISC7 = 1;
    
    SPBRG = 21; // Initialize SPBRG register for 115200 baud rate
    BRGH  = 1;  // set for high baud_rate
    
    SYNC  = 0;  // Asynchronous serial port
    SPEN  = 1;  // Enable serial port pins

    TX9   = 0;  // 8-bit reception selected
    RX9   = 0;  // 8-bit reception mode selected   
    
    TXIE = 0;   // no interrupts for send 
    RCIE = 1;   // Enable UART receive interrupt
    
    TXEN  = 1;  // enable transmission
    CREN  = 1;  // enable reception
}

// send a string to UART
void UartWriteString(const char *data)
{
    while(*data != NULL_CHAR) 
    {
        while(!TXIF);  // hold the program till TX buffer is free
        TXREG = *data++;
    }
}

void UartWriteMessageAndWait(const char *data)
{
    UartWriteString(data);
    UartWriteString("\n");
    __delay_ms(50);
}
