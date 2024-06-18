/*
 * PCD8544 - Interface with ST7586S (or compatible) LCDs.
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


#include "ST7586S.h"

#include <SPI.h>

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif


#define ST7586S_CMD  LOW
#define ST7586S_DATA HIGH

#define SPI_HARD 1
//#define SPI_SOFT 1

/*
 * If this was a ".h", it would get added to sketches when using
 * the "Sketch -> Import Library..." menu on the Arduino IDE...
 */
#include "charset.inc"


// private data and functions
#ifdef SPI_SOFT
static unsigned char ST7586S_pin_sclk;
static unsigned char ST7586S_pin_sdin;
#endif
static unsigned char ST7586S_pin_sce;
static unsigned char ST7586S_pin_dc;
static unsigned char ST7586S_pin_reset;

// Chip variant in use...
static unsigned char ST7586S_model;

// The size of the display, in pixels...
static unsigned char ST7586S_width;
static unsigned char ST7586S_height;

// Current cursor position...
static unsigned char ST7586S_column;
static unsigned char ST7586S_line;

// Send a command or data to the display...
static void ST7586S_send(unsigned char type, unsigned char data);


// --- public functions: --------------------------------------------------

void ST7586S_connection(unsigned char sclk, unsigned char sdin,
                 unsigned char dc, unsigned char reset,
                 unsigned char sce)
{
#ifdef SPI_SOFT
    ST7586S_pin_sclk	= sclk;
    ST7586S_pin_sdin	= sdin;
    
#endif
    ST7586S_pin_sce	= sce;
    ST7586S_pin_dc	= dc;
    ST7586S_pin_reset	= reset;

}


void ST7586S_begin_full(unsigned char width, unsigned char height, unsigned char model)
{
    ST7586S_width = width;
    ST7586S_height = height;

    // Only two chip variants are currently known/supported...
    ST7586S_model = CHIP_ST7586S;

    ST7586S_column = 0;
    ST7586S_line = 0;

    // All pins are outputs (these displays cannot be read)...
#ifdef SPI_SOFT
    pinMode(ST7586S_pin_sclk, OUTPUT);
    pinMode(ST7586S_pin_sdin, OUTPUT);
    pinMode(ST7586S_pin_sce, OUTPUT);
#endif
    pinMode(ST7586S_pin_dc, OUTPUT);
    pinMode(ST7586S_pin_reset, OUTPUT);



    // Reset the controller state...
    digitalWrite(ST7586S_pin_reset, HIGH);

    digitalWrite(ST7586S_pin_sce, HIGH);

    digitalWrite(ST7586S_pin_reset, LOW);
    delay(100);
    digitalWrite(ST7586S_pin_reset, HIGH);

#ifndef SPI_SOFT
    SPI_begin();
    SPI_beginTransaction(0);
#endif
    ST7586S_send(ST7586S_CMD, 0x01);  // Soft reset
    delay(60);

    ST7586S_send(ST7586S_CMD, 0x11);  // Sleep Out

    ST7586S_send(ST7586S_CMD, 0x28);  // Display OFF
    delay(25);

    ST7586S_send(ST7586S_CMD, 0x12); // No partial mode

    ST7586S_send(ST7586S_CMD,  0xC0); // Vop = 120h data sheet suggested 0x0145 but this caused streaks
    ST7586S_send(ST7586S_DATA, 0x20); //
    ST7586S_send(ST7586S_DATA, 0x01); //
  
    ST7586S_send(ST7586S_CMD,  0xC3);  // BIAS = 1/14
    ST7586S_send(ST7586S_DATA, 0x00); //
  
    ST7586S_send(ST7586S_CMD,  0xC4);  // Booster = x8
    ST7586S_send(ST7586S_DATA, 0x07); //

    ST7586S_send(ST7586S_CMD,  0xD0);  // Enable Analog Circuit
    ST7586S_send(ST7586S_DATA, 0x1D); //

    ST7586S_send(ST7586S_CMD,  0xB3); // Set FOSC divider
    ST7586S_send(ST7586S_DATA, 0x00); 

    ST7586S_send(ST7586S_CMD,  0xB5);  // N-Line = 0
    ST7586S_send(ST7586S_DATA, 0x00); 

    delay(25);

    ST7586S_send(ST7586S_CMD,  0x39);  // 0x39 Monochrome mode. 0x38 - gray Mode

    ST7586S_send(ST7586S_CMD,  0x3A);  // Enable DDRAM Interface
    ST7586S_send(ST7586S_DATA, 0x02); 

    ST7586S_send(ST7586S_CMD,  0x36); // Scan Direction Setting
    ST7586S_send(ST7586S_DATA, 0x40); // COM:C159->C0   SEG: SEG383->SEG0

    delay(25);

    ST7586S_send(ST7586S_CMD,  0xB1); // First output COM
    ST7586S_send(ST7586S_DATA, 0x00); 

    ST7586S_send(ST7586S_CMD,  0xB0); // Duty Setting (num rows - 1)
    ST7586S_send(ST7586S_DATA, 0x77); 

    ST7586S_send(ST7586S_CMD,  0x20); // Display inversion off

    ST7586S_send(ST7586S_CMD,  0x2A); // Column Address Setting
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x4F); // 80*3=240 pixels in the RAM but effective only 160

    delay(25);

    ST7586S_send(ST7586S_CMD,  0x2B); // Row Address Setting
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x00); 
    ST7586S_send(ST7586S_DATA, 0x78); // 120 pixels
  
    delay(25);

    ST7586S_send(ST7586S_CMD,  0x29); // Display ON
#ifndef SPI_SOFT
    SPI_endTransaction();
#endif
}


void ST7586S_stop()
{
    ST7586S_clear();
    ST7586S_setPower(false);
}


void ST7586S_clear()
{
#ifndef SPI_SOFT
    //SPI_beginTransaction(SPISettings(SPI_CLOCK_DIV256,MSBFIRST,SPI_MODE0));
    SPI_beginTransaction(0);
#endif

    for(unsigned short j = 0; j<ST7586S_height;j++){
        ST7586S_setCursor(0, j);
        for (unsigned short i = 0; i < (ST7586S_width/3)+1 ; i++) {
            ST7586S_send(ST7586S_DATA, 0x00);
        }
    }

    ST7586S_setCursor(0, 0);
#ifndef SPI_SOFT
    SPI_endTransaction();
#endif    
}


void ST7586S_clearLine(unsigned char line)
{
#ifndef SPI_SOFT
    SPI_beginTransaction(0);
#endif
    ST7586S_setCursor(0, line);
#ifdef SPI_SOFT
     for (unsigned char i = 0; i < (ST7586S_width/3)+1; i++) {
         ST7586S_send(ST7586S_DATA, 0x00);
     }
#else
    digitalWrite(ST7586S_pin_dc, ST7586S_DATA);
    digitalWrite(ST7586S_pin_sce, LOW);
    SPI_SendNulls((ST7586S_width/3)+1);
    digitalWrite(ST7586S_pin_sce, HIGH);
#endif    


#ifndef SPI_SOFT
    SPI_endTransaction();
#endif    
    // ST7586S_setCursor(0, ST7586S_line);
}


void ST7586S_setPower(bool on)
{
    ST7586S_send(ST7586S_CMD, on ? 0x10 : 0x11);
    if(!on){
        delay(50);
    }
}


inline void ST7586S_display()
{
    ST7586S_setPower(true);
}


inline void ST7586S_noDisplay()
{
    ST7586S_setPower(false);
}

void ST7586S_setCursor(unsigned char column, unsigned char line)
{
    /*ST7586S_column = (column % ST7586S_width);
    ST7586S_line = (line % (ST7586S_height/9 + 1));

    ST7586S_send(ST7586S_CMD, 0x80 | ST7586S_column);
    ST7586S_send(ST7586S_CMD, 0x40 | ST7586S_line);*/

    ST7586S_send(ST7586S_CMD, 0x2B); //row
    ST7586S_send(ST7586S_DATA, 0);
    ST7586S_send(ST7586S_DATA, line);
    ST7586S_send(ST7586S_DATA, 0);
    ST7586S_send(ST7586S_DATA, ST7586S_height-1);

    ST7586S_send(ST7586S_CMD, 0x2A); //column
    ST7586S_send(ST7586S_DATA, 0);
    ST7586S_send(ST7586S_DATA, column);
    ST7586S_send(ST7586S_DATA, 0);
    ST7586S_send(ST7586S_DATA, 0x4F);

    ST7586S_send(ST7586S_CMD,  0x2C); /* cmd write display data to ram */

}

void ST7586S_drawBitmap(const unsigned char *data,unsigned char xpos, unsigned char ypos, unsigned char columns, unsigned char lines)
{
    unsigned char scolumn = ST7586S_column;
    unsigned char sline = ST7586S_line;

    // The bitmap will be clipped at the right/bottom edge of the display...
    //unsigned char mx = (scolumn + columns > ST7586S_width) ? (ST7586S_width - scolumn) : columns;
    //unsigned char my = (sline + lines > ST7586S_height/8) ? (ST7586S_height/8 - sline) : lines;
#ifndef SPI_SOFT
    //SPI_beginTransaction(SPISettings(SPI_CLOCK_DIV2,MSBFIRST,SPI_MODE0));
    SPI_beginTransaction(0);
#endif
    for (unsigned char y = 0; y < lines; y++) {
        ST7586S_setCursor(scolumn+xpos, sline + y+ypos);

        for (unsigned char x = 0; x < columns; x++) {
            ST7586S_send(ST7586S_DATA, data[y * columns + x]);
        }
    }
#ifndef SPI_SOFT
    SPI_endTransaction();
#endif
    // Leave the cursor in a consistent position...
    //ST7586S_setCursor(scolumn + columns, sline);
}

static void ST7586S_send(unsigned char type, unsigned char data)
{
    digitalWrite(ST7586S_pin_dc, type);

    digitalWrite(ST7586S_pin_sce, LOW);
#ifdef SPI_SOFT
    shiftOut(ST7586S_pin_sdin, ST7586S_pin_sclk, MSBFIRST, data);
#else
    SPI_Send(data);
#endif    
    digitalWrite(ST7586S_pin_sce, HIGH);

}

void ST7586S_putString(char *szString,unsigned char x,unsigned char y)
{
    unsigned char len =strlen(szString);
    for(int i=0;i<len;i++){
        ST7586S_drawBitmap(charset[szString[i]-' '],x+i*2,y,2,5);
    }

}


/* vim: set expandtab ts=4 sw=4: */
