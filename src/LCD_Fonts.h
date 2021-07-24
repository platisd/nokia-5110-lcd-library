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
#include "Nokia_LCD_Fonts.h"
class LcdFont
{
public:
    LcdFont(const unsigned char* fontTable, 
            uint8_t columnSize,
            const unsigned char* hSpace,
            uint8_t hSpaceSize,            
            uint8_t rowSize = 8,
            uint8_t characterOffset = 0x20)
        : hSpace{ hSpace }
        , hSpaceSize{ hSpaceSize }
        , columnSize{ columnSize }
        , rowSize{ rowSize }
        , kFontTable{ fontTable }
        , kCharacterOffset{ characterOffset }
    {
    }
    const unsigned char* getFont(char character) const
    {   
        return (kFontTable + (character - kCharacterOffset) * columnSize);
    }
    const unsigned char* const hSpace;
    const uint8_t hSpaceSize;
    const uint8_t columnSize;
    const uint8_t rowSize;
private:
    const unsigned char *kFontTable;
    const uint8_t kCharacterOffset;
};