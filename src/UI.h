#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Adafruit_SPITFT.h>

/**
 * @brief UI encoder rotation
 */
enum Rotation : uint8_t {
  CCW,
  CW
};

class UI {
  protected:
    /**
     * @brief Pointer to the TFT instance
     */
    Adafruit_SPITFT *_tft;
  public:
    /**
     * @brief Lambda declaration
     */
    using Touched = uint8_t(*)();
    /**
     * @brief Construct a new `UI` object
     * 
     * @param tft 
     */
    UI(Adafruit_SPITFT *tft);
    /**
     * @brief Destroy the `UI` object
     */
    virtual ~UI();
    /**
     * @brief `UI` touch
     * 
     * @param x 
     * @param y 
     * @param touched 
     * @return int8_t 
     */
    virtual int8_t touch(uint16_t x, uint16_t y, Touched touched);
    /**
     * @brief Encoder rotated
     * 
     * @param rotation 
     */
    virtual void encoderChange(Rotation rotation);
    /**
     * @brief Encoder pressed
     * 
     * @param emergency 
     */
    virtual void encoderPress(bool emergency = false);
};

#endif
