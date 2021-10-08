#include <Program.h>

Program::Program(Adafruit_SPITFT *tft, DCCEx *dcc)
    : UI(tft), _dcc(dcc) {
  _programBtns[ProgramButton::WRITE_ADDRESS] = new TouchButton(_tft, 0, 30, 240, 32, F("Write Address"));
  _programBtns[ProgramButton::WRITE_BYTE] = new TouchButton(_tft, 0, 68, 240, 32, F("Write Byte"));
  _programBtns[ProgramButton::WRITE_BIT] = new TouchButton(_tft, 0, 106, 240, 32, F("Write Bit"));

  _programBtns[ProgramButton::READ_ADDRESS] = new TouchButton(_tft, 0, 144, 240, 32, F("Read Address"));
  _programBtns[ProgramButton::READ_BYTE] = new TouchButton(_tft, 0, 182, 240, 32, F("Read Byte"));
}

Program::~Program() {
  for (uint8_t i = 0; i < ProgramButton::COUNT; i++) {
    delete _programBtns[i];
  }

  delete _ok;
  destroyKeyPad();
}

void Program::destroyKeyPad() {
  delete _keyPad;
  _keyPad = nullptr;
}

int8_t Program::touch(uint16_t x, uint16_t y, Touched touched) {
  if (_keyPad != nullptr) { // If we have a KeyPad then only process touch events for it
    uint8_t key = _keyPad->touch(x, y, touched);
    if (key != -1) {
      if (key == KeyPadButton::ENTER) {
        keyPadEnter();
      } else if (key == KeyPadButton::CANCEL) {
        destroyKeyPad();
        drawButtons();
      }
    }
  } else if (_ok != nullptr) { // If the Ok button is shown only process touch events for it
    if (_ok->contains(x, y)) {
      _ok->draw(true);
      while (touched()) {
        delay(50);
      }
      delete _ok;
      _ok = nullptr;
      drawButtons();
    }
  } else { // Program button press
    for (uint8_t i = 0; i < ProgramButton::COUNT; i++) {
      if (_programBtns[i]->contains(x, y)) {
        _programBtns[i]->draw(true);
        while (touched()) {
          delay(50);
        }
        programButtonPress(i);
      }
    }
  }

  return -1;
}

void Program::clearUI() {
  _tft->fillRect(0, 0, 207, 22, ILI9341_BLACK);
  _tft->fillRect(0, 22, 240, 298, ILI9341_BLACK);
}

void Program::drawButtons() {
  clearUI();

  for (uint8_t i = 0; i < ProgramButton::COUNT; i++) {
    _programBtns[i]->draw();
  }
}

void Program::programButtonPress(uint8_t btn) {
  switch (btn) {
    case ProgramButton::WRITE_ADDRESS: {
      newStep(ProgramStep::WRITE_ADDRESS_GET_ADDRESS, F("Enter Address"), 10293, 1);
    } break;
    case ProgramButton::WRITE_BYTE: {
      newStep(ProgramStep::WRITE_CV_BYTE_GET_CV, F("Enter CV Address"), 1024, 1);
    } break;
    case ProgramButton::WRITE_BIT: {
      newStep(ProgramStep::WRITE_CV_BIT_GET_CV, F("Enter CV Address"), 1024, 1);
    } break;
    case ProgramButton::READ_ADDRESS: {
      readResult(_dcc->readAddress());
    } break;
    case ProgramButton::READ_BYTE: {
      newStep(ProgramStep::READ_CV_BYTE_GET_CV, F("Enter CV Address"), 1024, 1);
    } break;
  }
}

void Program::newStep(uint8_t step, const __FlashStringHelper *title, uint16_t max, uint8_t min) {
  clearUI();
  _step = step;
  _keyPad = new KeyPad(_tft, title, max, min);
}

void Program::keyPadEnter() {
  switch (_step) {
    case ProgramStep::WRITE_ADDRESS_GET_ADDRESS: {
      bool result = _dcc->writeAddress(_keyPad->getNumber());
      destroyKeyPad();
      writeResult(result);
    } break;
    case ProgramStep::WRITE_CV_BYTE_GET_CV: {
      _stepData[0] = _keyPad->getNumber();
      destroyKeyPad();
      newStep(ProgramStep::WRITE_CV_BYTE_GET_VALUE, F("Enter Byte Value"), 255, 0);
    } break;
    case ProgramStep::WRITE_CV_BYTE_GET_VALUE: {
      bool result = _dcc->writeCVByte(_stepData[0], _keyPad->getNumber());
      destroyKeyPad();
      writeResult(result);
    } break;
    case ProgramStep::WRITE_CV_BIT_GET_CV: {
      _stepData[0] = _keyPad->getNumber();
      destroyKeyPad();
      newStep(ProgramStep::WRITE_CV_BIT_GET_BIT, F("Enter Bit"), 7, 0);
    } break;
    case ProgramStep::WRITE_CV_BIT_GET_BIT: {
      _stepData[1] = _keyPad->getNumber();
      destroyKeyPad();
      newStep(ProgramStep::WRITE_CV_BIT_GET_VALUE, F("Enter Value"), 1, 0);
    } break;
    case ProgramStep::WRITE_CV_BIT_GET_VALUE: {
      bool result = _dcc->writeCVBit(_stepData[0], _stepData[1], _keyPad->getNumber());
      destroyKeyPad();
      writeResult(result);
    } break;
    case ProgramStep::READ_CV_BYTE_GET_CV: {
      int16_t result = _dcc->readCVByte(_keyPad->getNumber());
      destroyKeyPad();
      readResult(result);
    } break;
  }
}

void Program::writeResult(bool result) {
  clearUI();
  if (result) {
    _tft->setCursor(62, 120);
    _tft->setTextColor(ILI9341_GREEN);
    _tft->println(F("Write Success"));
  } else {
    _tft->setCursor(70, 120);
    _tft->setTextColor(ILI9341_RED);
    _tft->println(F("Write Failed!"));
  }
  _ok = new TouchButton(_tft, 0, 148, 240, 32, F("Ok"));
}

void Program::readResult(int16_t result) {
  clearUI();
  if (result != -1) {
    char buf[6] = { 0 };
    sprintf_P(buf, PSTR("%d"), result);
    int16_t text_x, text_y;
    uint16_t text_w, text_h;
    _tft->getTextBounds(buf, 0, 0, &text_x, &text_y, &text_w, &text_h);

    _tft->setTextColor(ILI9341_GREEN);
    _tft->setCursor(114 - (text_w / 2), 120);
    _tft->println(buf);
  } else {
    _tft->setCursor(70, 120);
    _tft->setTextColor(ILI9341_RED);
    _tft->println(F("Read Failed!"));
  }
  _ok = new TouchButton(_tft, 0, 148, 240, 32, F("Ok"));
}
