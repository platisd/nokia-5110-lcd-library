// If we are not building for AVR architectures ignore PROGMEM
#if __has_include(<avr/pgmspace.h>)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#define pgm_read_byte_near *
#endif
#include <SPI.h>
#include <math.h>
#include <stdio.h>

#include "Nokia_LCD.h"
#include "Nokia_LCD_Fonts.h"

namespace {
// Instantiate the default font
const LcdFont nokiaFont{
    [](char c) { return Nokia_LCD_Fonts::kDefault_font[c - 0x20]; },
    Nokia_LCD_Fonts::kColumns_per_character, Nokia_LCD_Fonts::hSpace, 1};

// Each row is made of 8-bit columns
const unsigned int kTotal_rows =
    nokia_lcd::kDisplay_max_height / Nokia_LCD_Fonts::kRows_per_character;
const unsigned int kTotal_columns = nokia_lcd::kDisplay_max_width;
const unsigned int kTotal_bits = nokia_lcd::kDisplay_max_width * kTotal_rows;
const char kNull_char = '\0';
const uint8_t kMax_number_length = 11;  // Size of unsigned long (10) + null
}  // namespace

Nokia_LCD::Nokia_LCD(const uint8_t clk_pin, const uint8_t din_pin,
                     const uint8_t dc_pin, const uint8_t ce_pin,
                     const uint8_t rst_pin)
    : kClk_pin{clk_pin},
      kDin_pin{din_pin},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin},
      kBl_pin{255},
      kUsingBacklight{false},
      kUsingHardwareSPI{false},
      mX_cursor{0},
      mY_cursor{0},
      mCurrentFont{&nokiaFont} {}

Nokia_LCD::Nokia_LCD(const uint8_t dc_pin, const uint8_t ce_pin,
                     const uint8_t rst_pin)
    : kClk_pin{0},
      kDin_pin{0},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin},
      kBl_pin{255},
      kUsingBacklight{false},
      kUsingHardwareSPI{true},
      mX_cursor{0},
      mY_cursor{0},
      mCurrentFont{&nokiaFont} {}

Nokia_LCD::Nokia_LCD(const uint8_t clk_pin, const uint8_t din_pin,
                     const uint8_t dc_pin, const uint8_t ce_pin,
                     const uint8_t rst_pin, const uint8_t bl_pin)
    : kClk_pin{clk_pin},
      kDin_pin{din_pin},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin},
      kBl_pin{bl_pin},
      kUsingBacklight{true},
      kUsingHardwareSPI{false},
      mX_cursor{0},
      mY_cursor{0},
      mCurrentFont{&nokiaFont} {}

Nokia_LCD::Nokia_LCD(const uint8_t dc_pin, const uint8_t ce_pin,
                     const uint8_t rst_pin, const uint8_t bl_pin)
    : kClk_pin{0},
      kDin_pin{0},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin},
      kBl_pin{bl_pin},
      kUsingBacklight{true},
      kUsingHardwareSPI{true},
      mX_cursor{0},
      mY_cursor{0},
      mCurrentFont{&nokiaFont} {}

void Nokia_LCD::begin() {
    pinMode(kClk_pin, OUTPUT);
    pinMode(kDin_pin, OUTPUT);
    pinMode(kDc_pin, OUTPUT);
    pinMode(kCe_pin, OUTPUT);
    pinMode(kRst_pin, OUTPUT);
    if (kUsingBacklight) {
        pinMode(kBl_pin, OUTPUT);
    }

    // Reset the LCD to a known state
    digitalWrite(kRst_pin, LOW);
    digitalWrite(kRst_pin, HIGH);

    if (kUsingHardwareSPI) {
        SPI.begin();
    }

    sendCommand(0x21);  // Tell LCD extended commands follow
    sendCommand(0xB0);  // Set LCD Vop (Contrast)
    sendCommand(0x04);  // Set Temp coefficent
    sendCommand(0x14);  // LCD bias mode 1:48 (try 0x13)
    // We must send 0x20 before modifying the display control mode
    sendCommand(0x20);
    sendCommand(0x0C);  // Set display control, normal mode.
}

void Nokia_LCD::couple() {
    digitalWrite(kCe_pin, LOW);
    mCoupled = true;
}
void Nokia_LCD::uncouple() {
    digitalWrite(kCe_pin, HIGH);
    mCoupled = false;
}

void Nokia_LCD::setContrast(uint8_t contrast) {
    sendCommand(0x21);             // Tell LCD that extended commands follow
    sendCommand(0x80 | contrast);  // Set LCD Vop (Contrast)
    sendCommand(0x20);             // Set display mode
}

void Nokia_LCD::setInverted(bool invert) { mInverted = invert; }

void Nokia_LCD::setFont(const LcdFont *font) {
    if (!font) {
        return;
    }
    mCurrentFont = font;
}

void Nokia_LCD::setDefaultFont() { mCurrentFont = &nokiaFont; }

void Nokia_LCD::setBacklight(bool enabled) {
    if (!kUsingBacklight) {
        return;
    }
    digitalWrite(kBl_pin, enabled);
}

bool Nokia_LCD::setCursor(uint8_t x, uint8_t y) {
    if (x >= nokia_lcd::kDisplay_max_width || y >= kTotal_rows) {
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

bool Nokia_LCD::println(char character) {
    bool out_of_bounds = print(character);

    return print("\n") || out_of_bounds;
}

bool Nokia_LCD::print(char character) { return printCharacter(character); }

bool Nokia_LCD::printCharacter(char character) {
    const unsigned char new_line = '\n';

    // If there is a new line character, we only need to change row
    if (character == new_line) {
        mX_cursor = 0;  // Go back to the beginning of the columns
        mY_cursor = (mY_cursor + 1) % kTotal_rows;
        setCursor(mX_cursor, mY_cursor);  // Set the new cursor position

        // If we went back to row 0, return an out-of-bounds error
        return mY_cursor == 0;
    }

    bool out_of_bounds =
        draw(mCurrentFont->getFont(character), mCurrentFont->columnSize, true);
    // Separate the characters with a vertical line so they don't appear too
    // close to each other
    return draw(mCurrentFont->hSpace, mCurrentFont->hSpaceSize, false) ||
           out_of_bounds;
}

bool Nokia_LCD::draw(const unsigned char bitmap[],
                     const unsigned int bitmap_size,
                     const bool read_from_progmem,
                     const unsigned int bitmap_width) {
    bool out_of_bounds = false;
    const unsigned int initialX = mX_cursor;
    for (unsigned int i = 0; i < bitmap_size; i++) {
        unsigned char pixel =
            read_from_progmem ? pgm_read_byte_near(bitmap + i) : bitmap[i];
        if (mInverted) {
            pixel = ~pixel;
        }
        sendData(pixel, false);
        out_of_bounds =
            updateCursorPosition(initialX, bitmap_width) || out_of_bounds;
    }

    return out_of_bounds;
}

void Nokia_LCD::sendCommand(const unsigned char command) {
    send(command, false);
}

bool Nokia_LCD::sendData(const unsigned char data) {
    return sendData(data, true);
}

bool Nokia_LCD::sendData(const unsigned char data, const bool update_cursor) {
    return send(data, true, update_cursor);
}

bool Nokia_LCD::updateCursorPosition(const unsigned int x_start_position,
                                     const unsigned int x_end_position) {
    bool out_of_bounds = false;

    mX_cursor =
        (mX_cursor + 1) %
        (x_start_position +
         x_end_position);  // Used to determine if X reached the right margin.
                           // E.g. starts drawing on column 10, an image of 25px
                           // width, X will reach the right margin at 35px, so
                           // we have to break line
    // Calculate the cursor position after the byte being sent
    if (mX_cursor == 0) {
        mX_cursor = x_start_position;  // return X to the initial position
        // If the column just became 0, this means the row should change
        mY_cursor = (mY_cursor + 1) % kTotal_rows;  // Row
        if (mY_cursor == 0) {
            // If we are back to row 0 again, then we just went out of bounds
            out_of_bounds = true;
        }
    }

    setCursor(mX_cursor, mY_cursor);  // updates the cursor position
    return out_of_bounds;
}

bool Nokia_LCD::send(const unsigned char lcd_byte, const bool is_data,
                     const bool update_cursor) {
    // Tell the LCD that we are writing either to data or a command
    digitalWrite(kDc_pin, is_data);

    // Send the byte
    if (!mCoupled) {
        digitalWrite(kCe_pin, LOW);
    }
    if (kUsingHardwareSPI) {
        constexpr uint32_t kSPiClockSpeed{F_CPU / 4U};
        SPI.beginTransaction(SPISettings{kSPiClockSpeed, MSBFIRST, SPI_MODE0});
        SPI.transfer(lcd_byte);
        SPI.endTransaction();
    } else {
        shiftOut(kDin_pin, kClk_pin, MSBFIRST, lcd_byte);
    }

    if (!mCoupled) {
        digitalWrite(kCe_pin, HIGH);
    }

    // If we just sent the command, there was no out-of-bounds error
    // and we don't have to calculate the new cursor position
    if (!is_data || !update_cursor) {
        return false;
    }

    return updateCursorPosition();
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
    // The log base 10 of the number increased by 1 indicates how many digits
    // there are in a number.
    const uint8_t length_as_string =
        static_cast<uint8_t>(log10(number)) + 1 + sizeof(kNull_char);
    char number_as_string[kMax_number_length] = {0};
    snprintf(number_as_string, length_as_string, "%lu", number);

    return print(number_as_string);
}

bool Nokia_LCD::print(double number, unsigned short decimals) {
    double integral = 0;
    long fractional = pow(10.0, decimals) * modf(number, &integral);
    bool out_of_bounds = false;

    // prints the left hand side of the dot
    out_of_bounds = out_of_bounds || print(static_cast<long>(number));
    out_of_bounds = out_of_bounds || print(".");

    // prints any leading 0s after the dot
    for (int d = decimals - 1; (0 <= d); d--) {
        int num = fractional / pow(10, d);
        if (num == 0) {
            out_of_bounds = out_of_bounds || print(num);
        } else {
            break;  // no more 0s to print so break
        }
    }

    if (fractional == 0) {
        // If the fractional part is 0, then we have already printed it
        return out_of_bounds;
    }
    // prints the rest of the fractional
    return (out_of_bounds || print(fractional));
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

bool Nokia_LCD::println(double number, unsigned short decimals) {
    bool out_of_bounds = print(number, decimals);

    return print("\n") || out_of_bounds;
}
