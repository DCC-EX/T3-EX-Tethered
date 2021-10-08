#include <LocoByAddress.h>

LocoByAddress::LocoByAddress(Adafruit_SPITFT *tft, Selected selected)
    : KeyPad(tft, F("Enter Address"), 10293, 1), _selected(selected) { }

int8_t LocoByAddress::touch(uint16_t x, uint16_t y, Touched touch) {
  int8_t btn = KeyPad::touch(x, y, touch);
  if (btn != -1) {
    _selected(btn == KeyPadButton::ENTER ? getNumber() : 0);
  }

  return -1;
}
