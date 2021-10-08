#ifndef TOUCH_REGION_H
#define TOUCH_REGION_H

#include <Arduino.h>
#include <Adafruit_FT6206.h>

class TouchRegion {
  protected:
    /**
     * @brief Touch region x & y
     */
    uint16_t _x, _y;
    /**
     * @brief Touch region width and height
     */
    uint16_t _w, _h;
  public:
    /**
     * @brief Construct a new `TouchRegion` object
     * 
     * @param x 
     * @param y 
     * @param w 
     * @param h 
     */
    TouchRegion(int16_t x, int16_t y, uint16_t w, uint16_t h);
    /**
     * @brief Is `x` and `y` within the touch region
     * 
     * @param x 
     * @param y 
     * @return true 
     * @return false 
     */
    bool contains(uint16_t x, uint16_t y);
    /**
     * @brief Is `TS_Point` within the touch region
     * 
     * @param p 
     * @return true 
     * @return false 
     */
    bool contains(TS_Point p);
};

#endif
