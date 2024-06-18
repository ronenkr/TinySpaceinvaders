/*
 * ST7586S - Interface with ST7586S (or compatible) LCDs.
 *
 * Copyright (c) 2024 R.K.
 * Ported to C for use with SDCC 2017 Michael Mayer <michael-mayer@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef ST7586S_H
#define ST7586S_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif


// Chip variants supported (ST7576 is experimental)...
#define CHIP_ST7586S 2

// plain C interface for use with SDCC

void ST7586S_connection(
    unsigned char sclk,	/* clock       (display pin 2) */
    unsigned char sdin,	/* data-in     (display pin 3) */
    unsigned char dc,	/* data select (display pin 4) */
    unsigned char reset,	/* reset       (display pin 8) */
    unsigned char sce);	/* enable      (display pin 5) */

// Display initialization (dimensions in pixels)...
#define ST7586S_begin()	PCD8544_begin_full(160,120, CHIP_ST7586S)
#define ST7586S_begin_wh(W,H)	PCD8544_begin_full(W,H, CHIP_ST7586S)
void ST7586S_begin_full(
    unsigned char width,
    unsigned char height,
    unsigned char model);
void ST7586S_stop();

// Erase everything on the display...
void ST7586S_clear();
void ST7586S_clearLine(unsigned char line);  // ...or just the current line

// Control the display's power state...
void ST7586S_setPower(bool on);

// For compatibility with the LiquidCrystal library...
void ST7586S_display();
void ST7586S_noDisplay();

// Place the cursor at position (column, line)...
void ST7586S_setCursor(unsigned char column, unsigned char line);

void ST7586S_putString(char *szString,unsigned char x,unsigned char y);

// Draw a bitmap ...
void ST7586S_drawBitmap(const unsigned char *data,unsigned char xpos, unsigned char ypos, unsigned char columns, unsigned char lines);

// Draw a chart element at the current cursor position...
void ST7586S_drawColumn(unsigned char lines, unsigned char value);

#endif  /* ST7586S_H */


/* vim: set expandtab ts=4 sw=4: */
