What is it?
===========

How it Works
============

To use this library, you must first connect your LCD to the proper pins on the Arduino.
For a Nokia 3310 display the connections would be the following:

Display Pin       | Arduino Pin
------------------|------------
Pin 1             | +3.3V Pin
Pin 2 (SCLK)      | Digital Pin 3
Pin 3 (SDIN/MOSI) | Digital Pin 4
Pin 4 (D/C)       | Digital Pin 5
Pin 5 (SCE)       | Digital Pin 7
Pin 6             | Ground Pin
Pin 7             | 10uF capacitor to Ground Pin
Pin 8 (RST)       | Digital Pin 6

