#ifndef MENU_H
#define MENU_H

#include <UI.h>
#include <SdFat.h>
#include <TouchButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ImageReader.h>

// Rotate icon, generated using http://javl.github.io/image2cpp/
const unsigned char rotateGlyphBitmap [] PROGMEM = {
  0x00, 0x3c, 0x18, 0x3c, 0x3c, 0x3c, 0x7e, 0x3c, 0xff, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 
	0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0xff, 0x3c, 0x7e, 0x3c, 0x3c, 0x3c, 0x18, 0x3c, 0x00
};

const GFXglyph rotateBitmapGlyphs [] PROGMEM = {
	{ 0, 16, 16, 0, 1, -15 }
};

const GFXfont RotateBitmapFont PROGMEM = {
	(uint8_t*)rotateGlyphBitmap,
	(GFXglyph*)rotateBitmapGlyphs,
	0x20, 0x20, 1
};

/**
 * @brief Menu buttons
 */
struct MenuButtonEnum {
  enum Buttons : uint8_t {
    ROTATE,
    LOCO_LOAD_BY_ADDRESS,
    LOCO_LOAD_BY_NAME,
    LOCO_LOAD_BY_GROUP,
    LOCO_RELEASE,
    LOCO_PROGRAM,
    POWER_OFF_ALL,
    POWER_ON_ALL,
    POWER_OFF_MAIN,
    POWER_ON_MAIN,
    POWER_OFF_PROG,
    POWER_ON_PROG,
    POWER_JOIN,
    COUNT // Always at end
  };
};
typedef MenuButtonEnum::Buttons MenuButton;

class Menu : public UI {
  public:
    /**
     * @brief Lambda declaration
     */
    using Selected = void(*)(uint8_t);
    /**
     * @brief Construct a new `Menu` UI object
     * 
     * @param tft 
     * @param selected 
     */
    Menu(Adafruit_SPITFT *tft, Selected selected);
    /**
     * @brief Destroy the `Menu` UI object
     */
    ~Menu();
    /**
     * @brief Handle UI touch events
     * 
     * @param x 
     * @param y 
     * @param touched 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touched);
  private:
    Selected _selected;
    /**
     * @brief `Menu` buttons
     */
    TouchButton *_menuBtns[MenuButton::COUNT];
};

#endif
