#include <Loco.h>
#include <Adafruit_ILI9341.h>
#include <Functions.h>

Loco::Loco(Adafruit_SPITFT *tft, SdFat *sd, DCCEx *dcc, LocoState *loco, Swiped swiped)
    : UI(tft), _sd(sd), _dcc(dcc), _loco(loco), _swiped(swiped) {
  char path[32];
  sprintf_P(path, PSTR("/locos/%d.json"), _loco->address);
  
  if (_sd->exists(path)) { // Check for loco config file
    FatFile json = _sd->open(path);
    deserializeJson(_locoDoc, json);
    json.close();
  }
  
  // Create an Adafruit Image Reader instance to handle buttons with icons
  _imageReader = new Adafruit_ImageReader(*_sd);

  // Print the loco name as provided by the config defaulting to `Unknown`
  _tft->setTextColor(ILI9341_WHITE);
  _tft->setCursor(0, 12);
  if (_locoDoc.containsKey(F("name"))) {
    _tft->println(_locoDoc[F("name")].as<const char*>());
  } else {
    _tft->println(F("Unknown"));
  }
  
  // Print the loco address
  _tft->setCursor(0, 32);
  _tft->print(F("Address: "));
  _tft->println(_loco->address);

  // Print the loco speed
  _tft->setCursor(0, 52);
  _tft->println(F("Speed:"));
  printSpeed();

  // Print the loco direction
  _tft->setCursor(118, 52);
  _tft->println(F("Direction:"));
  printDirection();

  if (_locoDoc[F("functions")].is<JsonArray>()) { // Function map array in loco config json
    _locoFunctions = _locoDoc[F("functions")].as<JsonArray>();
  } else if (_locoDoc[F("functions")].is<const char*>()) { // Name of function map file
    sprintf_P(path, PSTR("/fns/%s.json"), _locoDoc[F("functions")].as<const char*>());
    FatFile json = _sd->open(path);
    deserializeJson(_locoDoc, json);
    _locoFunctions = _locoDoc.as<JsonArray>();
    json.close();
  }

  uint8_t rows = _locoFunctions.size();
  if (rows == 0) { // Create 29 default functions if none were specified
    _locoFunctions = _locoDoc.createNestedArray();
    char buf[4];
    JsonArray row;
    for (uint8_t i = 0; i < 29; i++) {
      if (i % 3 == 0) { // New row
        row = _locoFunctions.createNestedArray();
      }
      JsonObject fn = row.createNestedObject();
      sprintf_P(buf, PSTR("F%d"), i);
      fn[F("label")] = buf;
      fn[F("fn")] = i;
      // TODO, which Fn's are by default non latching?
    }
    rows = 10;
  }

  if (rows > 7) { // More than 7 rows and we need paging
    uint8_t pages = divideAndCeil(rows, 6);
    _paging = new Paging(_tft, pages);
  }

  drawFunctionButtons();
}

Loco::~Loco() {
  delete _imageReader;
  delete _paging;

  destroyFunctionButtons();
}

void Loco::destroyFunctionButtons() {
  for (uint8_t i = 0; i < _locoFunctionCount; i++) {
    delete _locoFunctionBtns[i];
  }
  delete[] _locoFunctionBtns;
}

void Loco::printSpeed() {
  _tft->fillRect(60, 38, 40, 16, ILI9341_BLACK);
  _tft->setTextColor(ILI9341_WHITE);
  _tft->setCursor(60, 52);
  _tft->print(_loco->speed);
}

void Loco::printDirection() {
  _tft->fillRect(200, 38, 40, 16, ILI9341_BLACK);
  _tft->setTextColor(ILI9341_WHITE);
  _tft->setCursor(200, 52);
  _tft->print(_loco->direction == Direction::FORWARD ? F("FWD") : F("REV"));
}

void Loco::drawFunctionButtons() {
  _tft->fillRect(0, 60, 240, 228, ILI9341_BLACK); // Clear buttons
  
  uint8_t i = 0;
  _locoFunctionCount = 0;
  // Get the function button count
  for (JsonArrayConst const& row : _locoFunctions) {
    if (_paging == nullptr || divideAndCeil(++i, 6) == _paging->getPage()) {
      _locoFunctionCount += row.size();
    }
  }

  _locoFunctionBtns = new FunctionButton*[_locoFunctionCount];

  uint8_t btn = 0;
  uint16_t y = 60; // Start at 90
  i = 0;
  for (JsonArrayConst const& row : _locoFunctions) {
    if (_paging == nullptr || divideAndCeil(++i, 6) == _paging->getPage()) {
      uint8_t cols = row.size();
      uint8_t width = (240 - ((cols - 1) * 6)) / cols;

      uint8_t x = 0;
      uint8_t col = 0;
      for (JsonObjectConst const& fn : row) {
        JsonObjectConst idle = fn[F("btn")][F("idle")];
        JsonObjectConst pressed = fn[F("btn")][F("pressed")];
        // Needed for 4 button rows as it divides to a half pixel so the two inner buttons are 1 pixel wider
        uint8_t extra = cols == 4 && (col == 1 || col == 2) ? 1 : 0;
        _locoFunctionBtns[btn] = new FunctionButton(_tft, _imageReader, x, y, width + extra, 32, fn[F("label")], {
          ILI9341_WHITE,
          idle[F("fill")] | (uint16_t)ILI9341_BLACK,
          idle[F("text")] | (uint16_t)ILI9341_WHITE,
          idle[F("icon")]
        }, {
          ILI9341_WHITE,
          pressed[F("fill")] | (uint16_t)ILI9341_WHITE,
          pressed[F("text")] | (uint16_t)ILI9341_BLACK,
          pressed[F("icon")]
        }, fn[F("fn")], fn[F("latching")] | true);

        uint32_t funcmask = (1UL << _locoFunctionBtns[btn]->fn);
        _locoFunctionBtns[btn]->draw(_loco->functions & funcmask);

        x += width + 6 + extra;
        btn++;
        col++;
      }
      y += 38;
    }
  }
}

int8_t Loco::touch(uint16_t x, uint16_t y, Touched touched) {
  for (uint8_t i = 0; i < _locoFunctionCount; i++) {
    if (_locoFunctionBtns[i]->contains(x, y)) {
      uint32_t funcmask = (1UL << _locoFunctionBtns[i]->fn);
      _locoFunctionBtns[i]->draw(!(_loco->functions & funcmask));
      if (_locoFunctionBtns[i]->latching) { // Toggle state for latching functions
        if (_loco->functions & funcmask) {
          _loco->functions &= ~funcmask;
        } else {
          _loco->functions |= funcmask;
        }
        _dcc->setFn(_loco->address, _locoFunctionBtns[i]->fn, _loco->functions & funcmask);
      } else { // Set non latching function to on
        _dcc->setFn(_loco->address, _locoFunctionBtns[i]->fn, true);
        delay(250);
        _dcc->setFn(_loco->address, _locoFunctionBtns[i]->fn, false);
      }
      while (touched()) {
        delay(50);
      }
      if (!_locoFunctionBtns[i]->latching) { // Set non latching function button to off
        _locoFunctionBtns[i]->draw();
      }
    }
  }
  if (_paging != nullptr && _paging->touch(x, y, touched)) {
    destroyFunctionButtons();
    drawFunctionButtons();
  }

  return -1;
}

void Loco::swipe(Swipe swipe) {
  _swiped(swipe);
}

void Loco::encoderChange(Rotation rotation) {
  // TODO, steps??
  if (rotation == CW) {
    if (_loco->speed < 127) {
      _loco->speed++;
    }
  } else {
    if (_loco->speed > 0) {
      _loco->speed--;
    }
  }

  _dcc->setThrottle(_loco->address, _loco->speed, _loco->direction);
  printSpeed();
}

void Loco::encoderPress(bool emergency) {
  if (emergency) {
    printSpeed();
  } else {
    _loco->direction = !_loco->direction;
    _dcc->setThrottle(_loco->address, _loco->speed, _loco->direction);
    printDirection();
  }
}
