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
 * File:   relay.h
 * Author: Ciprian Stingu
 *
 * Created on July 26, 2020, 1:13 PM
 */

#ifndef RELAY_H
#define	RELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RELAY_NONE 0

void RelaysInit(void);
void EnableRelay(uint8_t relay);

#ifdef	__cplusplus
}
#endif

#endif	/* RELAY_H */

