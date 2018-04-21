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
#include <stdint.h>

class Nokia_LCD {
public:
    /**
     * Nokia_LCD constructor where the pins connected to the display are
     * defined.
     * @param clk_pin Clock pin (CLK)
     * @param din_pin Data input pin (MOSI)
     * @param dc_pin  Data or command pin (DC)
     * @param ce_pin  Chip select pin (CE)
     * @param rst_pin Reset pin (RST)
     */
    Nokia_LCD(const uint8_t clk_pin, const uint8_t din_pin,
              const uint8_t dc_pin, const uint8_t ce_pin,
              const uint8_t rst_pin);

    /**
     * Initializes the LCD screen, has to be called before usage or waking up
     * from deep sleep.
     */
    void begin();

    /**
     * Sets the contrast on the LCD screen
     * @param contrast Contrast value between 40 and 60 is usually good
     */
    void setContrast(uint8_t contrast);

    /**
     * Sets the display "cursor" at the specified position so we can draw from
     * that point. If an invalid cursor position is supplied, an out-of-bounds
     * error will be returned and the cursor position will not be updated.
     * @param  x Coordinates on the x-axis, can be between 0 and 83
     * @param  y Coordinates on the y-axis, can be between 0 and 5
     * @return   True if out of bounds error | False otherwise
     */
    bool setCursor(uint8_t x, uint8_t y);

    /**
     * Returns the current cursor position on the x-axis
     * @return The current cursor position on the x-axis
     */
    uint8_t getCursorX();

    /**
     * Returns the current cursor position on the y-axis
     * @return The current cursor position on the y-axis
     */
    uint8_t getCursorY();

    /**
     * Clears the screen with the supplied color (defaults to white). The cursor
     * shall return to position (0,0).
     * @param is_black The screen color after it is cleared. Default is white.
     */
    void clear(bool is_black = false);

    /**
     * Prints the supplied element starting at the current cursor location. The
     * text will overflow by starting from the beginning if it exceeds the size
     * of the display.
     * @param  string The string to be printed on the display
     * @return        True if out of bounds error | False otherwise
     */
    bool print(const char *string);
    bool print(int number);
    bool print(unsigned int number);
    bool print(long number);
    bool print(unsigned long number);

    /**
     * Prints the supplied element starting at the current cursor location and
     * goes to the next line. The text will overflow by starting from the
     * beginning if it exceeds the size of the display.
     * @param  string The string to be printed on the display
     * @return        True if out of bounds error | False otherwise
     */
    bool println(const char *string);
    bool println(int number);
    bool println(unsigned int number);
    bool println(long number);
    bool println(unsigned long number);

// To increase compatibility with different ATTiny cores, only the classic
// C-style strings are used by default. If you want to use the Arduino `String`
// class, then insert `#define STRING_CLASS_IS_IMPLEMENTED` in this file.
#ifdef STRING_CLASS_IS_IMPLEMENTED
    bool print(const String &string);
    bool println(const String &string);
#endif

    /**
     * Draws the supplied bitmap on the screen starting at the current cursor
     * location. The bitmap can contain up to 504 bits which is the amount of
     * pixels in the display. The bitmap will overflow by starting from the
     * beginning if it exceeds the size of the display.
     * If you want the bitmap to cover the whole screen, don't forget to set the
     * cursor at (0,0) before calling this function.
     * The *byte orientation* should be **vertical** for this function to
     * provide the desirable result. I got this to work using the
     * `LCD Assistant` utility. For more information and resources please refer
     * to the [Sparkfun Graphic LCD Hookup Guide](https://bit.ly/2IBTtGj).
     * @param  bitmap            The bitmap to be displayed
     * @param  bitmap_size       The size of the bitmap to be displayed up to
     *                           504 bits
     * @param read_from_progmem  Whether the bitmap is stored in flash memory
     *                           instead of SRAM. Default read from flash.
     * @return                   True if out of bounds error | False otherwise
     */
    bool draw(const unsigned char bitmap[], const unsigned int bitmap_size,
              const bool read_from_progmem = true);

    /**
     * Sends the specified byte as a command to the display.
     * @param command The byte to be sent as a command.
     */
    void sendCommand(const unsigned char command);

    /**
     * Sends the specified byte as (presentable) data to the display.
     * @param data  The byte to be sent as presentable data.
     * @return      True if out of bounds error | False otherwise
     */
    bool sendData(const unsigned char data);

private:
    /**
     * Sends the specified byte to the LCD via software SPI as data or a
     * command.
     * @param lcd_byte The byte to be send to the LCD
     * @param is_data  Whether the byte to be send is data (or a command)
     * @return         True if out of bounds error | False otherwise
     */
    bool send(const unsigned char lcd_byte, const bool is_data);

    /**
     * Prints the specified character
     * @param  character The character to be printed
     * @return           True if out of bounds error | False otherwise
     */
    bool printCharacter(const unsigned char character);

    const uint8_t kClk_pin, kDin_pin, kDc_pin, kCe_pin, kRst_pin;
    uint8_t mX_cursor, mY_cursor;
};
