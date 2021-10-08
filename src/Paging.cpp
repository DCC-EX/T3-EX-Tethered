#include <Paging.h>
#include <Adafruit_ILI9341.h>

Paging::Paging(Adafruit_SPITFT *tft, uint8_t pages)
    : UI(tft), _pages(pages) {
  _prev = new TouchButton(_tft, 0, 288, 76, 32, "<", {
    ILI9341_WHITE,
    ILI9341_DARKGREY,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_DARKGREY
  });

  _next = new TouchButton(_tft, 164, 288, 76, 32, ">", {
    ILI9341_WHITE,
    ILI9341_DARKGREY,
    ILI9341_WHITE
  }, {
    ILI9341_WHITE,
    ILI9341_WHITE,
    ILI9341_DARKGREY
  });

  update();
}

Paging::~Paging() {
  delete _prev;
  delete _next;
}

void Paging::nextPage() {
  _page++;
  if (_page > _pages) {
    _page = 1;
  }

  update();
}

void Paging::prevPage() {
  _page--;
  if (_page == 0) {
    _page = _pages;
  }

  update();
}

void Paging::update() {
  int16_t text_x, text_y;
  uint16_t text_w, text_h;

  char label[6];
  sprintf_P(label, PSTR("%d\\%d"), _page, _pages);

  _tft->fillRect(82, 288, 76, 32, ILI9341_BLACK);
  _tft->setTextSize(1);
  _tft->getTextBounds(label, 82, 288, &text_x, &text_y, &text_w, &text_h);
  _tft->setCursor(120 - (text_w / 2), 309);
  _tft->setTextColor(ILI9341_WHITE);
  _tft->print(label);
}

int8_t Paging::touch(uint16_t x, uint16_t y, Touched touched) {
  if (_prev->contains(x, y)) {
    _prev->draw(true);
    while (touched()) {
      delay(50);
    }
    _prev->draw();

    prevPage();
    
    return true;
  } else if (_next->contains(x, y)) {
    _next->draw(true);
    while (touched()) {
      delay(50);
    }
    _next->draw();

    nextPage();

    return true;
  }

  return false;
}

void Paging::encoderChange(Rotation rotation) {
  if (rotation == CW) {
    nextPage();
  } else if (rotation == CCW) {
    prevPage();
  }
}

uint8_t Paging::getPage() {
  return _page;
}
