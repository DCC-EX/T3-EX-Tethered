#ifndef LOCO_H
#define LOCO_H

#include <UI.h>
#include <Arduino.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_ImageReader.h>
#include <SdFat.h>
#include <ArduinoJson.h>
#include <Paging.h>
#include <TouchButton.h>
#include <DCCEx.h>

/**
 * @brief Loco directions
 */
struct DirectionEnum {
  enum Direction : uint8_t {
    REVERSE,
    FORWARD
  };
};
typedef DirectionEnum::Direction Direction;

/**
 * @brief Loco state
 */
struct LocoState {
  LocoState()
      : speed(0), direction(Direction::FORWARD) { }
  uint16_t address = 0; // Loco DCC address
  uint32_t functions = 0; // Latching function states, uses bit shift
  uint8_t speed : 7;
  uint8_t direction : 1;
};

/**
 * @brief Extended `TouchButton` class that has `fn` and `latching` properties
 */
class FunctionButton : public TouchButton {
  public: 
    uint8_t fn;
    bool latching;
    FunctionButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                   int16_t x, int16_t y, uint16_t w, uint16_t h,
                   const char *label, Style idle, Style pressed, uint8_t fn, bool latching)
        : TouchButton(tft, reader, x, y, w, h, label, idle, pressed, false),
          fn(fn), latching(latching) { }
};

class Loco : public UI {
  public:
    using Swiped = void(*)(Swipe);
    /**
     * @brief Construct a new `Loco` UI object
     * 
     * @param tft 
     * @param sd 
     * @param dcc 
     * @param loco
     * @param swiped
     */
    Loco(Adafruit_SPITFT *tft, SdFat *sd, DCCEx *dcc, LocoState *loco, Swiped swiped);
    /**
     * @brief Destroy the `Loco` UI object
     */
    ~Loco();
    /**
     * @brief Handle UI touch events
     * 
     * @param x 
     * @param y 
     * @param touched 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touched);
    /**
     * @brief Handle UI swipe
     * 
     * @param swipe 
     */
    void swipe(Swipe swipe);
    /**
     * @brief Handle UI encoder rotation
     * 
     * @param rotation 
     */
    void encoderChange(Rotation rotation);
    /**
     * @brief Hanlde UI encoder presses
     * 
     * @param emergency 
     */
    void encoderPress(bool emergency);
  private:
    /**
     * @brief Pointer to `SdFat` object
     */
    SdFat *_sd;
    /**
     * @brief Pointer to `DCCEx` object
     */
    DCCEx *_dcc;
    /**
     * @brief Pointer to `LocoState` object
     */
    LocoState *_loco;
    /**
     * @brief 
     */
    Swiped _swiped;
    /**
     * @brief Loco config JSON document
     */
    StaticJsonDocument<4096> _locoDoc;
    /**
     * @brief Array of loco function objects
     */
    JsonArray _locoFunctions;
    /**
     * @brief Pointer to `Adafruit_ImageReader` object, used for button icons
     */
    Adafruit_ImageReader *_imageReader;
    /**
     * @brief Dynamic array of loco function buttons, will only contain those currently in use
     */
    FunctionButton **_locoFunctionBtns;
    /**
     * @brief How many loco functions are currently in use
     */
    uint8_t _locoFunctionCount = 0;
    /**
     * @brief Pointer to `Paging` object, only used if needed
     */
    Paging *_paging = nullptr;
    /**
     * @brief Print current loco speed
     */
    void printSpeed();
    /**
     * @brief Print current loco direction
     */
    void printDirection();
    /**
     * @brief Draw loco function buttons
     */
    void drawFunctionButtons();
    /**
     * @brief Destroy loco function buttons
     */
    void destroyFunctionButtons();
};

#endif
