# Nokia 5110 LCD library
An Arduino library for driving the monochrome Nokia 5110 LCD. It has a rather small footprint and is therefore able to run on many ATTiny microcontrollers.

## Why
There are many nice libraries for the inexpensive Nokia 5110 display out there which are probably more feature-complete. The main advantage of this one is using neither a framebuffer nor the SPI library. As a result it is lightweight enough and keeps dependencies to a minimum, thus suitable to run on various Arduino-compatible microcontrollers.

## How
The library should be simple to use and the [API](https://github.com/platisd/nokia-5110-lcd-library/blob/master/src/Nokia_LCD.h) is well documented. Demo sketches can be found in the [examples/](https://github.com/platisd/nokia-5110-lcd-library/tree/master/examples) directory. To start using this library, [install it manually](https://www.arduino.cc/en/Guide/Libraries) or look it up in the library manager of your Arduino IDE.
