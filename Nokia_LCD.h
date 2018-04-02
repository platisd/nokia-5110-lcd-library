/**
 * This work is heavily inspired by Jim Lindblom's work, from Sparkfun. The main
 * goal was to decrease its size so it can be ran on ATTiny microcontrollers
 * that have limited RAM size. The original library can be found on Sparkfun's
 * GitHub repository: https://github.com/sparkfun/GraphicLCD_Nokia_5110
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
    Nokia_LCD(uint8_t clk_pin, uint8_t din_pin, uint8_t dc_pin, uint8_t ce_pin,
              uint8_t rst_pin);

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
     * that point.
     * @param  x Coordinates on the x-axis, can be between 0 and 84
     * @param  y Coordinates on the y-axis, can be between 0 and 48
     * @return   True if out of bounds error | False otherwise
     */
    bool setCursor(uint8_t x, uint8_t y);

    /**
     * Gets the position of the cursor (as supplied by the user) on the x-axis.
     * @return Position of the cursor on the x-axis
     */
    uint8_t getX();

    /**
     * Gets the position of the cursor (as supplied by the user) on the y-axis.
     * @return Position of the cursor on the y-axis
     */
    uint8_t getY();

    /**
     * Clears the screen with the supplied color (defaults to white). The cursor
     * shall return to position (0,0).
     * @param is_black The screen color after it is cleared. Default is white.
     */
    void clear(bool is_black = false);

    /**
     * Prints the supplied string starting at the current cursor location.
     * @param  string The string to be printed on the display
     */
    void print(const char *string);
    void print(String string);

    /**
     * Draws the supplied bitmap on the screen starting at the current cursor
     * location. The bitmap can contain up to 504 bits which is the amount of
     * pixels in the display. The bitmap will be printed out as long as it does
     * not exceed the size of the display.
     * If you want the bitmap to cover the whole screen, don't forget to set the
     * cursor at (0,0) before calling this function.
     * @param  bitmap            The bitmap to be displayed
     * @param  bitmap_size       The size of the bitmap to be displayed up to
     *                           504 bits
     * @param read_from_progmem  Whether the bitmap is stored in flash memory
     *                           instead of SRAM
     * @return                   True if out of bounds error | False otherwise
     */
    bool draw(const unsigned char bitmap[], const unsigned int bitmap_size,
              const bool read_from_progmem = false);

    /**
     * Sends the specified byte as a command to the display.
     * @param command The byte to be sent as a command.
     */
    void sendCommand(const unsigned char command);

    /**
     * Sends the specified byte as (presentable) data to the display.
     * @param data  The byte to be sent as presentable data.
     */
    void sendData(const unsigned char data);

private:
    /**
     * Sends the specified byte to the LCD via software SPI as data or a
     * command.
     * @param lcd_byte The byte to be send to the LCD
     * @param is_data  Whether the byte to be send is data (or a command)
     */
    void send(const unsigned char lcd_byte, const bool is_data);

    const uint8_t kClk_pin, kDin_pin, kDc_pin, kCe_pin, kRst_pin;
    uint8_t mXcursor, mYcursor;
};
