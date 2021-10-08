#include "TouchButton.h"
#include <Adafruit_SPITFT.h>
#include <Adafruit_ImageReader.h>

TouchButton::TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const char *label, Style idle, Style pressed,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, reader, x, y, w, h, label, false, idle, pressed) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const char *label, Style idle, Style pressed,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, nullptr, x, y, w, h, label, false, idle, pressed) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const char *label,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, reader, x, y, w, h, label) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const char *label,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, nullptr, x, y, w, h, label) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const __FlashStringHelper *label, Style idle, Style pressed,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, reader, x, y, w, h, (const char*)label, true, idle, pressed) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const __FlashStringHelper *label, Style idle, Style pressed,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, nullptr, x, y, w, h, (const char*)label, true, idle, pressed) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const __FlashStringHelper *label,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, reader, x, y, w, h, (const char*)label, true) { }

TouchButton::TouchButton(Adafruit_SPITFT *tft,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const __FlashStringHelper *label,
                         bool drawOnConstruct)
    : TouchButton(drawOnConstruct, tft, nullptr, x, y, w, h, (const char*)label, true) { }

TouchButton::TouchButton(bool drawOnConstruct, Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                         int16_t x, int16_t y, uint16_t w, uint16_t h,
                         const char *label, bool pgm, Style idle, Style pressed)
    : TouchRegion(x, y, w, h), _tft(tft), _reader(reader), _label(label), _pgm(pgm),
      _idle(idle), _pressed(pressed) {
  if (drawOnConstruct) {
    draw();
  }
}

void TouchButton::draw(bool pressed) {
  int16_t text_x, text_y;
  uint16_t text_w, text_h;

  Style *style = pressed ? &_pressed : &_idle;

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _tft->fillRoundRect(_x, _y, _w, _h, r, style->fill);
  _tft->drawRoundRect(_x, _y, _w, _h, r, style->outline);

  _tft->setTextSize(1);
  if (_pgm) {
    _tft->getTextBounds((const __FlashStringHelper*)_label, _x, _y + _h, &text_x, &text_y, &text_w, &text_h);
  } else {
    _tft->getTextBounds(_label, _x, _y + _h, &text_x, &text_y, &text_w, &text_h);
  }

  text_x = (_x + ((_w - text_w) / 2));
  text_y = (_y + ((_h + text_h) / 2)) - 1;

  if (text_w > 0) {
    text_x -= 1;
  }

  if (_reader != nullptr && style->icon != nullptr) {
    char icon[32];
    sprintf_P(icon, PSTR("/icons/%s.bmp"), style->icon);
    int32_t bmp_w, bmp_h;
    if (_reader->bmpDimensions(icon, &bmp_w, &bmp_h) == IMAGE_SUCCESS
      && _reader->drawBMP(icon, *_tft, text_x - (bmp_w / 2), text_y - (bmp_h / 2) - (text_h / 2) + 1, true) == IMAGE_SUCCESS)
    { // Only change `text_x` if the icon was loaded successfully
      text_x += (bmp_w / 2) + 2;
    }
  }

  _tft->setCursor(text_x, text_y);
  _tft->setTextColor(style->text);
  if (_pgm) {
    _tft->print((const __FlashStringHelper*)_label);
  } else {
    _tft->print(_label);
  }
}
