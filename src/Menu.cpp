#include <Menu.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSans9pt7b.h>

Menu::Menu(Adafruit_SPITFT *tft, Selected selected)
    : UI(tft), _selected(selected) {
  // For the rotate icon we use a custom font so the throttle can be used without an SD card
  _tft->setFont(&RotateBitmapFont);
  _menuBtns[MenuButton::ROTATE] = new TouchButton(_tft, 0, 0, 26, 26, " ");

  // Need to reset the font after
  _tft->setFont(&FreeSans9pt7b);
  _tft->setTextColor(ILI9341_WHITE);
  _tft->drawFastHLine(0, 36, 94, ILI9341_WHITE);
  _tft->drawFastHLine(146, 36, 94, ILI9341_WHITE);
  _tft->setCursor(100, 40);
  _tft->print(F("Loco"));
  
  _menuBtns[MenuButton::LOCO_LOAD_BY_ADDRESS] = new TouchButton(_tft, 0, 52, 117, 32, F("By Address"));
  _menuBtns[MenuButton::LOCO_LOAD_BY_NAME] = new TouchButton(_tft, 123, 52, 117, 32, F("By Name"));

  _menuBtns[MenuButton::LOCO_LOAD_BY_GROUP] = new TouchButton(_tft, 0, 90, 76, 32, F("Groups"));
  _menuBtns[MenuButton::LOCO_RELEASE] = new TouchButton(_tft, 82, 90, 76, 32, F("Release"));
  _menuBtns[MenuButton::LOCO_PROGRAM] = new TouchButton(_tft, 164, 90, 76, 32, F("Program"));

  _tft->drawFastHLine(0, 140, 90, ILI9341_WHITE);
  _tft->drawFastHLine(150, 140, 90, ILI9341_WHITE);
  _tft->setCursor(94, 144);
  _tft->print(F("Power"));

  _menuBtns[MenuButton::POWER_OFF_ALL] = new TouchButton(_tft, 0, 156, 117, 32, F("Off All"));
  _menuBtns[MenuButton::POWER_ON_ALL] = new TouchButton(_tft, 123, 156, 117, 32, F("On All"));
  _menuBtns[MenuButton::POWER_OFF_MAIN] = new TouchButton(_tft, 0, 194, 117, 32, F("Off Main"));
  _menuBtns[MenuButton::POWER_ON_MAIN] = new TouchButton(_tft, 123, 194, 117, 32, F("On Main"));
  _menuBtns[MenuButton::POWER_OFF_PROG] = new TouchButton(_tft, 0, 232, 117, 32, F("Off Prog"));
  _menuBtns[MenuButton::POWER_ON_PROG] = new TouchButton(_tft, 123, 232, 117, 32, F("On Prog"));
  _menuBtns[MenuButton::POWER_JOIN] = new TouchButton(_tft, 0, 270, 240, 32, F("Join Tracks"));
}

Menu::~Menu() {
  for (uint8_t i = 0; i < MenuButton::COUNT; i++) {
    delete _menuBtns[i];
  }
}

int8_t Menu::touch(uint16_t x, uint16_t y, Touched touched) {
  for (uint8_t i = 0; i < MenuButton::COUNT; i++) {
    if (_menuBtns[i]->contains(x, y)) {
      _menuBtns[i]->draw(true);
      while (touched()) {
        delay(50);
      }
      _menuBtns[i]->draw();
      _selected(i);
      break;
    }
  }

  return -1;
}
