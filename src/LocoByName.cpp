#include <LocoByName.h>
#include <Functions.h>
#include <ArduinoJson.h>

LocoByName::LocoByName(Adafruit_SPITFT *tft, SdFat *sd, bool groups, Selected selected)
    : UI(tft), _sd(sd), _selected(selected) {
  _tft->setCursor(0, 18);
  _tft->print(F("Select Loco"));

  if (groups) { // Load by groups
    FatFile json = _sd->open("groups.json");
    deserializeJson(_doc, json);
    json.close();

    _btnsDoc = _doc.as<JsonObject>();
  } else { // Enum locos directory
    _btnsDoc = _doc.to<JsonObject>();
    FatFile locoDir = _sd->open("/locos");
    FatFile loco;

    while (loco.openNext(&locoDir, O_READ)) {
      if (!loco.isSubDir() && !loco.isHidden()) {
        addLoco(loco);
      }
      loco.close();
    }
    locoDir.close();
    // _sd->cacheClear();
  }

  drawPagingAndButtons();
}

LocoByName::~LocoByName() {
  delete _paging;

  destroyButtons();
}

void LocoByName::destroyButtons() {
  for (uint8_t i = 0; i < _btnCount; i++) {
    delete _btns[i];
  }
  delete[] _btns;
}

void LocoByName::addLoco(FatFile loco) {
  StaticJsonDocument<16> filterDoc;
  filterDoc[F("name")] = true;
  StaticJsonDocument<64> locoDoc;

  deserializeJson(locoDoc, loco, DeserializationOption::Filter(filterDoc));
  char *name = locoDoc[F("name")];
  _btnsDoc[name] = getAddrFromFN(loco);
}

uint16_t LocoByName::getAddrFromFN(FatFile &loco) {
  char buf[14] = { 0 };
  loco.getName(buf, sizeof(buf));
  return (uint16_t)strtoul(buf, (char **)NULL, 10);
}

void LocoByName::drawPagingAndButtons() {
  _count = _btnsDoc.size();

  if (_count > 8) { // If there's more than 8 buttons we need paging
    uint8_t pages = divideAndCeil(_count, 7);
    _paging = new Paging(_tft, pages);
  } else {
    _paging = nullptr;
  }

  drawButtons();
}

void LocoByName::loadGroup(JsonArray locos) {
  _btnsDoc.clear();
  for (uint16_t address : locos) {
    char buf[32];
    sprintf_P(buf, PSTR("/locos/%d.json"), address);
    FatFile loco = _sd->open(buf, O_READ);
    addLoco(loco);
    loco.close();
  }

  delete _paging;
  _tft->fillRect(0, 288, 240, 32, ILI9341_BLACK); // Clear paging
  destroyButtons();
  drawPagingAndButtons();
}

void LocoByName::drawButtons() {
  _tft->fillRect(0, 30, 240, 254, ILI9341_BLACK); // Clear buttons

  if (_paging != nullptr) {
    _btnCount = min(_count - ((_paging->getPage() - 1) * 7), 7);
  } else {
    _btnCount = min(_count, 8);
  }

  _btns = new ValueButton*[_btnCount];

  uint8_t btn = 0;
  uint16_t y = 30;
  uint8_t i = 0;
  
  for (JsonPair pair : _btnsDoc) {
    if (_paging == nullptr || divideAndCeil(++i, 7) == _paging->getPage()) {
      _btns[btn++] = new ValueButton(_tft, 0, y, 240, 31, pair.key().c_str(), pair.value());
      y += 37;
    }
  }
}

int8_t LocoByName::touch(uint16_t x, uint16_t y, Touched touched) {
  for (uint8_t i = 0; i < _btnCount; i++) {
    if (_btns[i]->contains(x, y)) {
      _btns[i]->draw(true);
      while (touched()) {
        delay(50);
      }
      if (_btns[i]->value.is<JsonArray>()) { // Button is a group
        loadGroup(_btns[i]->value.as<JsonArray>());
      } else { // Button is a loco
        _selected(_btns[i]->value.as<uint16_t>());
      }
      return -1;
    }
  }
  if (_paging != nullptr && _paging->touch(x, y, touched)) {
    destroyButtons();
    drawButtons();
  }

  return -1;
}

void LocoByName::encoderChange(Rotation rotation) {
  if (_paging != nullptr) {
    _paging->encoderChange(rotation);
    destroyButtons();
    drawButtons();
  }
}
