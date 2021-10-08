#ifndef LOCO_BY_ADDRESS_H
#define LOCO_BY_ADDRESS_H

#include <KeyPad.h>

class LocoByAddress : public KeyPad {
  public:
    /**
     * @brief Lambda declaration
     */
    using Selected = void(*)(uint16_t);
    /**
     * @brief Construct a new `LocoByAddress` UI object
     * 
     * @param tft 
     * @param selected 
     */
    LocoByAddress(Adafruit_SPITFT *tft, Selected selected);
    /**
     * @brief Handle UI touch events
     * 
     * @param x 
     * @param y 
     * @param touch 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touch);
  private:
    Selected _selected;
};

#endif
