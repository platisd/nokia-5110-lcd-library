/**
 * A library to control the monochrome LCD displays found on Nokia 5110.
 *
 * It is a derivative of Jim Lindblom's work, from Sparkfun. The goal
 * was to decrease its size so it can be ran on ATTiny microcontrollers
 * that have limited RAM size. Therefore, the main difference is that a
 * framebuffer, which would consume all dynamic memory, is not used. Get the
 * original library [here](https://github.com/sparkfun/GraphicLCD_Nokia_5110).
 *
 * On how to use this library, please refer to the `examples/` folder.`
 */
#pragma once
#include <Arduino.h>

struct CustomFont {
    virtual const unsigned char* getFont(char character) = 0;
    virtual const uint8_t colSize() = 0;
    virtual const uint8_t rowSize() = 0;
    virtual const unsigned char* HSpacingChar() = 0;
    virtual const uint8_t HSpacingSize() = 0;
};
