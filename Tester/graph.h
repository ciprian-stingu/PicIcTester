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
 * File:   graph.h
 * Author: Ciprian Stingu
 *
 * Created on July 23, 2020, 4:18 PM
 */

/**
 * The font used here is a modified version of TINYTYPE.pf
 * From https://www.min.at/prinz/o/software/pixelfont/ 
 */

#ifndef GRAPH_H
#define	GRAPH_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LCD_WIDTH  98
#define LCD_HEIGHT 67

#define CHAR_DIGITS "\x18\x24\x2C\x34\x24\x18\x10\x30\x10\x10\x10\x38\x18\x24\x04\x18\x20\x3C\x38\x04\x18\x04\x04\x38\x20\x28\x28\x3C\x08\x08"\
                    "\x3C\x20\x38\x04\x04\x38\x1C\x20\x38\x24\x24\x18\x3C\x04\x08\x10\x10\x10\x18\x24\x18\x24\x24\x18\x18\x24\x24\x1C\x04\x38"
#define CHAR_AZ     "\x18\x24\x24\x3C\x24\x24\x38\x24\x38\x24\x24\x38\x18\x24\x20\x20\x24\x18\x38\x24\x24\x24\x24\x38\x3C\x20\x38\x20\x20\x3C"\
                    "\x3C\x20\x38\x20\x20\x20\x18\x24\x20\x2C\x24\x18\x24\x24\x3C\x24\x24\x24\x38\x10\x10\x10\x10\x38\x04\x04\x04\x04\x24\x18"\
                    "\x28\x28\x30\x28\x24\x24\x20\x20\x20\x20\x20\x3C\x44\x6C\x54\x44\x44\x44\x24\x34\x34\x2C\x2C\x24\x18\x24\x24\x24\x24\x18"\
                    "\x38\x24\x24\x38\x20\x20\x18\x24\x24\x24\x24\x1C\x38\x24\x24\x38\x24\x24\x1C\x20\x18\x04\x04\x38\x38\x10\x10\x10\x10\x10"\
                    "\x24\x24\x24\x24\x24\x3C\x24\x24\x24\x24\x14\x0C\x44\x44\x44\x54\x6C\x44\x24\x24\x10\x08\x24\x24\x22\x22\x14\x08\x08\x08"\
                    "\x3C\x04\x08\x10\x20\x3C"
#define CHAR_az     "\x00\x18\x24\x3C\x24\x24\x00\x38\x24\x38\x24\x38\x00\x18\x24\x20\x24\x18\x00\x38\x24\x24\x24\x38\x00\x3C\x20\x38\x20\x3C"\
                    "\x00\x3C\x20\x38\x20\x20\x00\x1C\x20\x2C\x24\x18\x00\x24\x24\x3C\x24\x24\x00\x1C\x08\x08\x08\x1C\x00\x04\x04\x04\x24\x18"\
                    "\x00\x24\x28\x30\x28\x24\x00\x20\x20\x20\x20\x3C\x00\x42\x66\x5A\x42\x42\x00\x24\x34\x3C\x2C\x24\x00\x18\x24\x24\x24\x18"\
                    "\x00\x38\x24\x38\x20\x20\x00\x18\x24\x24\x24\x1C\x00\x38\x24\x38\x24\x24\x00\x1C\x20\x18\x04\x38\x00\x38\x10\x10\x10\x10"\
                    "\x00\x24\x24\x24\x24\x3C\x00\x24\x24\x24\x14\x0C\x00\x42\x42\x5A\x66\x42\x00\x44\x28\x10\x28\x44\x00\x44\x28\x10\x10\x10"\
                    "\x00\x7C\x08\x10\x20\x7C"
#define CHAR_STAR   "\x54\x38\x7C\x38\x54\x00"
#define CHAR_LT     "\x08\x10\x20\x10\x08\x00"
#define CHAR_EQ     "\x00\x7E\x00\x7E\x00\x00"
#define CHAR_GT     "\x20\x10\x08\x10\x20\x00"
#define CHAR_DOTS   "\x00\x10\x00\x00\x10\x00"
#define CHAR_DOT    "\x00\x00\x00\x00\x00\x08"
#define CHAR_UP     "\x00\x10\x38\x6C\xC6\x00"
#define CHAR_SPACE  "\x00\x00\x00\x00\x00\x00"
#define CHAR_QMARK  "\x38\x44\x08\x10\x00\x10"
#define CHAR_LSP    "\x1C\x10\x10\x10\x10\x1C"
#define CHAR_RSP    "\x70\x10\x10\x10\x10\x70"
#define CHAR_MINUS  "\x00\x00\x7E\x00\x00\x00"
#define CHAR_SLASH  "\x04\x08\x10\x20\x40\x80"
#define CHAR_PLUS   "\x00\x08\x08\x3E\x08\x08"
#define CHAR_AMP    "\x10\x28\x10\x2A\x44\x3A"

#define MAX_CHARS_PER_INTERFACE_MESSAGE 15
#define MAX_CHARS_PER_GRAPHIC_DATA 5
#define LINES_PER_CHARS 6

#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x03
#define COLOR_GREEN   0x1C
#define COLOR_CYAN    0x1F
#define COLOR_RED     0xE0
#define COLOR_MAGENTA 0xE3
#define COLOR_YELLOW  0xFC
#define COLOR_WHITE   0xFF
 
#define LINE_HORIZONTAL 0
#define LINE_VERTICAL   1
#define VERTICAL_LINE_MAX_HEIGHT 31
#define HORIZONTAL_LINE_MAX_WIDTH LCD_WIDTH

uint8_t GetCharGraphicData(char *message, uint8_t messageLength, char *graphicData);
void DisplayStringOnInterface(uint8_t x, uint8_t y, uint8_t color, uint8_t bgColor, char *message);
void ClearRectangleOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bgColor);
void DrawLineOnInterface(uint8_t x, uint8_t y, uint8_t type, uint8_t length, uint8_t color);
void DrawRectangleOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor);
void DrawWindowOnInterface(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color, uint8_t bgColor, uint8_t shadowColor);

#ifdef	__cplusplus
}
#endif

#endif	/* GRAPH_H */

