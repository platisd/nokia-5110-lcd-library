#include "Nokia_LCD.h"
#include <avr/pgmspace.h>
#include "Nokia_LCD_Fonts.h"

namespace {
const uint8_t kDisplay_max_width = 84;
const uint8_t kDisplay_max_height = 48;
const unsigned int kTotal_bits = kDisplay_max_width * kDisplay_max_height / 8;
}  // namespace

Nokia_LCD::Nokia_LCD(uint8_t clk_pin, uint8_t din_pin, uint8_t dc_pin,
                     uint8_t ce_pin, uint8_t rst_pin)
    : kClk_pin{clk_pin},
      kDin_pin{din_pin},
      kDc_pin{dc_pin},
      kCe_pin{ce_pin},
      kRst_pin{rst_pin} {}

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
    if (x >= kDisplay_max_width || y >= kDisplay_max_height) {
        return false;
    }

    sendCommand(0x80 | x);  // Column
    sendCommand(0x40 | y);  // Row

    return true;
}

void Nokia_LCD::clear(bool is_black) {
    setCursor(0, 0);
    unsigned char color = is_black ? 255 : 0;

    for (unsigned int i = 0; i < kTotal_bits; i++) {
        sendData(color);
    }
    setCursor(0, 0);
}

void Nokia_LCD::print(const char *string) {
    const char kNull_char = '\0';
    unsigned int index = 0;

    while (*(string + index) != kNull_char) {
        unsigned char character = *(string + index++);
        draw(Nokia_LCD_Fonts::ASCII[character - 0x20],
             Nokia_LCD_Fonts::kColumns_per_character, true);
    }
}

bool Nokia_LCD::print(String string) {}

bool Nokia_LCD::draw(const unsigned char bitmap[],
                     const unsigned int bitmap_size,
                     const bool read_from_progmem) {
    for (unsigned int i = 0; i < bitmap_size && i < kTotal_bits; i++) {
        unsigned char pixel =
            read_from_progmem ? pgm_read_byte_near(bitmap + i) : bitmap[i];
        sendData(pixel);
    }
    return bitmap_size >= kTotal_bits;
}

void Nokia_LCD::sendCommand(const unsigned char command) {
    send(command, false);
}

void Nokia_LCD::sendData(const unsigned char data) { send(data, true); }

void Nokia_LCD::send(const unsigned char lcd_byte, const bool is_data) {
    // Tell the LCD that we are writing either to data or a command
    digitalWrite(kDc_pin, is_data);

    // Send the byte
    digitalWrite(kCe_pin, LOW);
    shiftOut(kDin_pin, kClk_pin, MSBFIRST, lcd_byte);
    digitalWrite(kCe_pin, HIGH);
}
