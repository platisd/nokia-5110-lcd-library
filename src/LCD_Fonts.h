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

using GetFontCallback = const unsigned char* (*)(char);

class LcdFont
{
public:    
    LcdFont(GetFontCallback getFontCallback,
            uint8_t columnSize,
            const unsigned char* hSpace,
            uint8_t hSpaceSize)
        : hSpace{ hSpace }
        , hSpaceSize{ hSpaceSize }
        , columnSize{ columnSize } 
        , mGetFontCallback{ getFontCallback }
    {
    }
    const unsigned char* getFont(char character) const
    {   
        return mGetFontCallback(character);
    }
    const unsigned char* const hSpace;
    const uint8_t hSpaceSize;
    const uint8_t columnSize;
private:
    const GetFontCallback mGetFontCallback;
};