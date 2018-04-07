#pragma once
// If we are not building for AVR architectures ignore PROGMEM
#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#define pgm_read_byte_near *
#endif
#include <math.h>
#include "Nokia_LCD.h"
#include "Nokia_LCD_Fonts.h"

namespace {
const uint8_t kDisplay_max_width = 84;
const uint8_t kDisplay_max_height = 48;
// Each row is made of 8-bit columns
const unsigned int kTotal_rows =
    kDisplay_max_height / Nokia_LCD_Fonts::kRows_per_character;
const unsigned int kTotal_columns = kDisplay_max_width;
const unsigned int kTotal_bits = kDisplay_max_width * kTotal_rows;
const char kNull_char = '\0';
}  // namespace

Nokia_LCD::Nokia_LCD(const uint8_t clk_pin, const uint8_t din_pin,
                     const uint8_t dc_pin, const uint8_t ce_pin,
                     const uint8_t rst_pin)
    : kClk_pin{clk_pin},
      kDin_pin{din_pin},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin},
      mX_cursor{0},
      mY_cursor{0} {}

void Nokia_LCD::begin() {
    pinMode(kClk_pin, OUTPUT);
    pinMode(kDin_pin, OUTPUT);
    pinMode(kDc_pin, OUTPUT);
    pinMode(kCe_pin, OUTPUT);
    pinMode(kRst_pin, OUTPUT);

    // Reset the LCD to a known state
    digitalWrite(kRst_pin, LOW);
    digitalWrite(kRst_pin, HIGH);

    sendCommand(0x21);  // Tell LCD extended commands follow
    sendCommand(0xB0);  // Set LCD Vop (Contrast)
    sendCommand(0x04);  // Set Temp coefficent
    sendCommand(0x14);  // LCD bias mode 1:48 (try 0x13)
    // We must send 0x20 before modifying the display control mode
    sendCommand(0x20);
    sendCommand(0x0C);  // Set display control, normal mode.
}

void Nokia_LCD::setContrast(uint8_t contrast) {
    sendCommand(0x21);             // Tell LCD that extended commands follow
    sendCommand(0x80 | contrast);  // Set LCD Vop (Contrast)
    sendCommand(0x20);             // Set display mode
}

bool Nokia_LCD::setCursor(uint8_t x, uint8_t y) {
    if (x >= kDisplay_max_width || y >= kTotal_rows) {
        return false;
    }

    mX_cursor = x;
    mY_cursor = y;
    sendCommand(0x80 | x);  // Column
    sendCommand(0x40 | y);  // Row

    return true;
}

uint8_t Nokia_LCD::getCursorX() { return mX_cursor; }

uint8_t Nokia_LCD::getCursorY() { return mY_cursor; }

void Nokia_LCD::clear(bool is_black) {
    setCursor(0, 0);
    unsigned char color = is_black ? 255 : 0;

    for (unsigned int i = 0; i < kTotal_bits; i++) {
        sendData(color);
    }
    setCursor(0, 0);
}

bool Nokia_LCD::println(const char *string) {
    bool out_of_bounds = print(string);

    return print("\n") || out_of_bounds;
}

bool Nokia_LCD::print(const char *string) {
    unsigned int index = 0;

    bool out_of_bounds = false;
    while (*(string + index) != kNull_char) {
        unsigned char character = *(string + index++);
        out_of_bounds = printCharacter(character) || out_of_bounds;
    }

    return out_of_bounds;
}

#ifdef STRING_CLASS_IS_IMPLEMENTED
bool Nokia_LCD::print(const String &string) {
    bool out_of_bounds = false;
    for (unsigned char character : string) {
        out_of_bounds = printCharacter(character) || out_of_bounds;
    }

    return out_of_bounds;
}

bool Nokia_LCD::println(const String &string) {
    bool out_of_bounds = print(string);

    return print("\n") || out_of_bounds;
}
#endif

bool Nokia_LCD::printCharacter(const unsigned char character) {
    const unsigned char new_line = '\n';

    // If there is a new line character, we only need to change row
    if (character == new_line) {
        mX_cursor = 0;  // Go back to the beginning of the columns
        mY_cursor = (mY_cursor + 1) % kTotal_rows;
        setCursor(mX_cursor, mY_cursor);  // Set the new cursor position

        // If we went back to row 0, return an out-of-bounds error
        return mY_cursor == 0;
    }

    return draw(Nokia_LCD_Fonts::kDefault_font[character - 0x20],
                Nokia_LCD_Fonts::kColumns_per_character, true);
}

bool Nokia_LCD::draw(const unsigned char bitmap[],
                     const unsigned int bitmap_size,
                     const bool read_from_progmem) {
    bool out_of_bounds = false;
    for (unsigned int i = 0; i < bitmap_size; i++) {
        unsigned char pixel =
            read_from_progmem ? pgm_read_byte_near(bitmap + i) : bitmap[i];
        out_of_bounds = sendData(pixel) || out_of_bounds;
    }

    return out_of_bounds;
}

void Nokia_LCD::sendCommand(const unsigned char command) {
    send(command, false);
}

bool Nokia_LCD::sendData(const unsigned char data) { return send(data, true); }

bool Nokia_LCD::send(const unsigned char lcd_byte, const bool is_data) {
    // Tell the LCD that we are writing either to data or a command
    digitalWrite(kDc_pin, is_data);

    // Send the byte
    digitalWrite(kCe_pin, LOW);
    shiftOut(kDin_pin, kClk_pin, MSBFIRST, lcd_byte);
    digitalWrite(kCe_pin, HIGH);

    // If we just sent the command, there was no out-of-bounds error
    // and we don't have to calculate the new cursor position
    if (!is_data) {
        return false;
    }

    // Calculate the cursor position after the byte being sent
    mX_cursor = (mX_cursor + 1) % kTotal_columns;  // Column
    if (mX_cursor == 0) {
        // If the column just became 0, this means the row should change
        mY_cursor = (mY_cursor + 1) % kTotal_rows;  // Row
        if (mY_cursor == 0) {
            // If we are back to row 0 again, then we just went out of bounds
            return true;
        }
    }

    return false;
}

bool Nokia_LCD::print(int number) {
    // Handle the negative numbers
    if (number < 0) {
        print("-");
        // We took care of the sign, so now we can treat the number as unsigned
        number = -number;
    }
    return print(static_cast<unsigned long>(number));
}

bool Nokia_LCD::print(unsigned int number) {
    return print(static_cast<unsigned long>(number));
}

bool Nokia_LCD::print(long number) {
    // Handle the negative numbers
    if (number < 0) {
        print("-");
        // We took care of the sign, so now we can treat the number as unsigned
        number = -number;
    }
    return print(static_cast<unsigned long>(number));
}

bool Nokia_LCD::print(unsigned long number) {
    const uint8_t base = 10;  // We shall treat all numbers as decimals
    // The log base 10 of the number increased by 1 indicates how many digits
    // there are in a number.
    const uint8_t length_as_string =
        static_cast<uint8_t>(log10(number)) + 1 + sizeof(kNull_char);

    char number_as_string[length_as_string] = {0};
    int8_t string_index = length_as_string - 1;   // Start from the end
    number_as_string[string_index] = kNull_char;  // Null terminated string
    while (--string_index >= 0) {
        const char digit = static_cast<char>(number % base);
        number /= base;
        number_as_string[string_index] = '0' + digit;
    }

    return print(number_as_string);
}

bool Nokia_LCD::println(int number) {
    bool out_of_bounds = print(number);

    return print("\n") || out_of_bounds;
}

bool Nokia_LCD::println(unsigned int number) {
    bool out_of_bounds = print(number);

    return print("\n") || out_of_bounds;
}

bool Nokia_LCD::println(long number) {
    bool out_of_bounds = print(number);

    return print("\n") || out_of_bounds;
}

bool Nokia_LCD::println(unsigned long number) {
    bool out_of_bounds = print(number);

    return print("\n") || out_of_bounds;
}
