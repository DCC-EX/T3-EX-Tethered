#include <KeyPad.h>

KeyPad::KeyPad(Adafruit_SPITFT *tft, const char *title, uint16_t max, uint16_t min)
    : KeyPad(tft, title, max, min, false) { }

KeyPad::KeyPad(Adafruit_SPITFT *tft, const __FlashStringHelper *title, uint16_t max, uint16_t min)
    : KeyPad(tft, (const char *)title, max, min, true) { }

KeyPad::KeyPad(Adafruit_SPITFT *tft, const char *title, uint16_t max, uint16_t min, bool pgm)
    : UI(tft), _title(title), _max(max), _min(min) {
  // Title
  _tft->setTextColor(ILI9341_WHITE);
  _tft->setCursor(0, 18);
  if (pgm) {
    _tft->print((const __FlashStringHelper*)_title);
  } else {
    _tft->print(_title);
  }

  // Round rect to simulate input field
  _tft->drawRoundRect(0, 30, 240, 40, 5, ILI9341_WHITE);

  // Range label
  _tft->setCursor(0, 94);
  _tft->print(F("Range "));
  _tft->print(min);
  _tft->print(F(" - "));
  _tft->print(max);

  _cancel = new TouchButton(_tft, 0, 106, 117, 32, F("Cancel"), {
    ILI9341_WHITE,
    ILI9341_LIGHTGREY,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_LIGHTGREY
  });

  _enter = new TouchButton(_tft, 123, 106, 117, 32, F("Enter"), {
    ILI9341_WHITE,
    ILI9341_DARKGREEN,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_DARKGREEN
  });

  // Number buttons
  uint16_t x = 0;
  uint16_t y = 144;
  for (uint8_t i = 1; i < 10; i++) {
    _numberBtns[i] = new TouchButton(_tft, x, y, 76, 32, _numberBtnLabels[i]);
    if (i % 3 == 0) { // New row
      x = 0;
      y += 38;
    } else {
      x += 82;
    }
  }
  _numberBtns[0] = new TouchButton(_tft, 82, 258, 76, 32, _numberBtnLabels[0]);
  
  _delete = new TouchButton(_tft, 0, 258, 76, 32, F("Del"), {
    ILI9341_WHITE,
    ILI9341_ORANGE,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_ORANGE
  });

  _clear = new TouchButton(_tft, 164, 258, 76, 32, F("Clr"), {
    ILI9341_WHITE,
    ILI9341_ORANGE,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_ORANGE
  });
}

KeyPad::~KeyPad() {
  delete _cancel;
  delete _enter;
  delete _delete;
  delete _clear;

  for (uint8_t i = 0; i < 10; i++) {
    delete _numberBtns[i];
  }
}

int8_t KeyPad::touch(uint16_t x, uint16_t y, Touched touch) {
  if (_cancel->contains(x, y)) { // Cancel
    _cancel->draw(true);
    while (touch()) {
      delay(50);
    }
    return KeyPadButton::CANCEL;
  } else if (_enter->contains(x, y)) { // Enter
    _enter->draw(true);
    while (touch()) {
      delay(50);
    }
    uint16_t valid = getNumber();
    if (valid >= _min && valid <= _max) {
      return KeyPadButton::ENTER;
    }
    _enter->draw();
  } else if (_delete->contains(x, y)) { // Delete
    _delete->draw(true);
    while (touch()) {
      delay(50);
    }
    _delete->draw();

    uint8_t len = strlen(_numberBuf);
    if (len > 0) {
      _numberBuf[len - 1] = '\0';
      printNumber();
    }
  } else if (_clear->contains(x, y)) { // Clear
    _clear->draw(true);
    while (touch()) {
      delay(50);
    }
    _clear->draw();

    memset(_numberBuf, 0, sizeof(_numberBuf));
    printNumber();
  } else { // Number buttons
    for (uint8_t i = 0; i < 10; i++) {
      if (_numberBtns[i]->contains(x, y)) {
        _numberBtns[i]->draw(true);
        while (touch()) {
          delay(50);
        }
        _numberBtns[i]->draw();

        uint8_t len = strlen(_numberBuf);
        if (len < sizeof(_numberBuf) - 1) {
          _numberBuf[len] = _numberBtnLabels[i][0];
        }
 
        uint32_t valid = getNumber();
        if (valid >= _min && valid <= _max) {
          printNumber();
        } else {
          _numberBuf[len] = '\0';
        }
        break;
      }
    }
  }

  return -1;
}

uint32_t KeyPad::getNumber() {
  return strtoul(_numberBuf, (char **)NULL, 10);
}

void KeyPad::printNumber() {
  _tft->fillRect(2, 32, 236, 36, ILI9341_BLACK);
  _tft->setCursor(10, 56);
  _tft->print(_numberBuf);
}
