# A crappy version of Space Invaders using STM8S103F3 microcontroller using Sduino Core for Arduino and a LCD panel pulled from an HP Printer with some buttons using the LM1306B02-1A LCD.

for more info on LM1306B02-1A LCD see my other [repo](https://github.com/ronenkr/LM1306B02-1A)

![image](https://github.com/ronenkr/TinySpaceinvaders/blob/main/TinySpaceinvaders.png?raw=true)

Using:
1. Get the Sduino core
2. copy the ST7586S library to your arduino library location
3. copy the SPI.h over the existing Sduino implementation
4. Run

I also added an IP5306 Board with a battery to it and one of the pins connected to the key on the board to kick the PSU 
and prevent it from going to sleep, the module goes to sleep when in the Game Over screen after a while
