#include <Nokia_LCD.h>
#include "fonts/Small_LCD_Fonts.h"
#include "fonts/Bold_LCD_Fonts.h"
#include "fonts/Glyphs_LCD_Fonts.h"

Nokia_LCD lcd(13 /* CLK */, 12 /* DIN */, 11 /* DC */, 10 /* CE */, 9 /* RST */, 9 /* BL */);
/**
 * Or, if you would like to control the backlight on your own, init the lcd without the last argument
 * Nokia_LCD lcd(13, 12, 11, 10, 9);
*/

/**
 * To change the font, create an object that represents it. 
 * Do it in the main file or in the header font files like SmallFont and GlyphFont.
 **/
const LcdFont BoldFont { 
  [](char c) { return Bold_LCD_Fonts::kFont_Table[c - 0x20]; },  // method to retrieve the character
  Bold_LCD_Fonts::kColumns_per_character,                        // width of each char
  Bold_LCD_Fonts::hSpace,                                        // horizontal spacing array
  1                                                              // size of horizontal spacing array
};


const unsigned char TOOLBAR[] = {
  0x0E,  // musical note
  0x0F,  // solar
  0x1D,  // key  
  0xF9,  // hourglass
  0x20,  // space
  0x1A,  // rewind  
  0x20,  // space
  0x1B,  // pause
  0x20,  // space
  0x19,  // fast foward
  0x20,  // space
  0x01,  // battery 
  0x02,  // battery 
  0x03   // battery 
};

void setup() {
  // Initialize the screen
  lcd.begin();
  // Turn on the backlight
  lcd.setBacklight(true);
  // Set the contrast
  lcd.setContrast(60);  // Good values are usualy between 40 and 60
  // Clear the screen by filling it with black pixels
  lcd.clear();
  
  
  // Glyph font example
  lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
  // Draw the toolbar
  for (int i=0; i<14; i++) {
    lcd.print((char) TOOLBAR[i]);
  }
  // Draw the pattern
  lcd.setCursor(0, 1);  
  for (int i=0; i<14; i++) {
    lcd.print((char) (0xD4+(int)i/5));
  }

  // Small font example
  lcd.setCursor(0, 2);  
  lcd.setFont(&Small_LCD_Fonts::SmallFont);
  lcd.print("This is an example\nof small font, it\nuses 3px width");
  
  // Bold font example
  lcd.setCursor(0, 5);
  lcd.setFont(&BoldFont);
  lcd.print("I'M BOLD!");  
}

void loop() {
  // put your main code here, to run repeatedly
}
