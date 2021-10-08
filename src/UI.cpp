#include <UI.h>

UI::UI(Adafruit_SPITFT *tft)
    : _tft(tft) { }

UI::~UI() { }

int8_t UI::touch(uint16_t x, uint16_t y, Touched touched) {
  return -1;
}

void UI::encoderChange(Rotation rotation) { }

void UI::encoderPress(bool emergency) { }
