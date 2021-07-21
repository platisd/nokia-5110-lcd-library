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
        LcdFont()
            : kFontTable( Nokia_LCD_Fonts::kDefault_font ) 
            , hSpace( Nokia_LCD_Fonts::hSpace )
            , hSpaceSize( Nokia_LCD_Fonts::hSpaceSize )
            , kCharacterOffset( 0x20 )
            , columnSize( Nokia_LCD_Fonts::kColumns_per_character )
            , rowSize( Nokia_LCD_Fonts::kRows_per_character )
        {
        }
        LcdFont(const unsigned char *hSpace,
                    uint8_t hSpaceSize,
                    uint8_t characterOffset,
                    uint8_t pColumnSize,
                    uint8_t pRowSize)
            : hSpace( hSpace )
            , hSpaceSize( hSpaceSize )
            , kCharacterOffset( characterOffset )
            , columnSize( pColumnSize )
            , rowSize( pRowSize ) 
        {
        }
        const unsigned char* getFont(char character) const
        {
            return *(kFontTable + (character - kCharacterOffset));
        }
        const uint8_t columnSize;
        const uint8_t rowSize;
        const unsigned char* hSpace;
        const uint8_t hSpaceSize;        
    private:
        const unsigned char (*kFontTable)[Nokia_LCD_Fonts::kColumns_per_character];  //<- not good, I need other sizes here not only 5
        const uint8_t kCharacterOffset;
};