#ifndef KEYPAD_H
#define KEYPAD_H

#include <UI.h>
#include <Adafruit_SPITFT.h>
#include <TouchButton.h>

/**
 * @brief `KeyPad` buttons
 */
struct KeyPadButtonEnum {
  enum Buttons : uint8_t {
    CANCEL,
    ENTER
  };
};
typedef KeyPadButtonEnum::Buttons KeyPadButton;

class KeyPad : public UI {
  public:
    /**
     * @brief Construct a new `KeyPad` UI object with pointer to char array for title
     * 
     * @param tft 
     * @param title 
     * @param max 
     * @param min 
     */
    KeyPad(Adafruit_SPITFT *tft, const char *title, uint16_t max = INT16_MAX, uint16_t min = 0);
    /**
     * @brief Construct a new `KeyPad` UI object with pointer to PROGMEM char array for title
     * 
     * @param tft 
     * @param title 
     * @param max 
     * @param min 
     */
    KeyPad(Adafruit_SPITFT *tft, const __FlashStringHelper *title, uint16_t max = INT16_MAX, uint16_t min = 0);
    /**
     * @brief Destroy the `KeyPad` UI object
     * 
     */
    ~KeyPad();
    /**
     * @brief Handle a `KeyPad` press
     * 
     * @param x 
     * @param y 
     * @param touch 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touch);
    /**
     * @brief Get the current number
     * 
     * @return uint32_t 
     */
    uint32_t getNumber();
  private:
    /**
     * @brief `KeyPad` title
     */
    const char *_title;
    /**
     * @brief Max number allowed
     */
    uint16_t _max;
    /**
     * @brief Min number allowed
     */
    uint16_t _min;
    /**
     * @brief Cancel `TouchButton`
     */
    TouchButton *_cancel;
    /**
     * @brief Enter `TouchButton`
     */
    TouchButton *_enter;
    /**
     * @brief Delete `TouchButton`
     */
    TouchButton *_delete;
    /**
     * @brief Clear `TouchButton`
     */
    TouchButton *_clear;
    /**
     * @brief Number buffer, max 6 digits
     */
    char _numberBuf[6] = { 0 };
    /**
     * @brief `TouchButtons`s for 0-9
     */
    TouchButton *_numberBtns[10];
    /**
     * @brief Number button labels
     */
    const char _numberBtnLabels[10][2] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    /**
     * @brief Construct a new `KeyPad` UI object
     * 
     * @param tft 
     * @param title 
     * @param max 
     * @param min 
     * @param pgm 
     */
    KeyPad(Adafruit_SPITFT *tft, const char *title, uint16_t max, uint16_t min, bool pgm = false);
    /**
     * @brief Clear and print the number field
     */
    void printNumber();
};

#endif
