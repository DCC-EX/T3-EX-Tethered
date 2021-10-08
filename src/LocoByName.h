#ifndef LOCO_BY_NAME_H
#define LOCO_BY_NAME_H

#include <UI.h>
#include <SdFat.h>
#include <ArduinoJson.h>
#include <Paging.h>

/**
 * @brief Extended `TouchButton` that has `JsonVariant` property
 */
class ValueButton : public TouchButton {
  public:
    JsonVariant value;
    ValueButton(Adafruit_SPITFT *tft,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label, JsonVariant value)
        : TouchButton(tft, x, y, w, h, label), value(value) { }
};

class LocoByName : public UI {
  public:
    /**
     * @brief Lambda declaration
     */
    using Selected = void(*)(uint16_t);
    /**
     * @brief Construct a new `LocoByName` object
     * 
     * @param tft 
     * @param sd 
     * @param groups 
     * @param selected 
     */
    LocoByName(Adafruit_SPITFT *tft, SdFat *sd, bool groups, Selected selected);
    /**
     * @brief Destroy the `LocoByName` object
     */
    ~LocoByName();
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
     * @brief Handle encoder change event
     * 
     * @param rotation 
     */
    void encoderChange(Rotation rotation);
  private:
    /**
     * @brief Pointer to `SdFat` object
     */
    SdFat *_sd;
    /**
     * @brief JSON document to store button info
     */
    StaticJsonDocument<4096> _doc;
    /**
     * @brief Button JSON document
     */
    JsonObject _btnsDoc;
    /**
     * @brief Count of all the possible buttons
     */
    uint8_t _count = 0;
    /**
     * @brief Count of buttons currently used
     */
    uint8_t _btnCount = 0;
    /**
     * @brief Dynamic array of buttons
     */
    ValueButton **_btns;
    /**
     * @brief Pointer to `Paging` object, only used if needed
     */
    Paging *_paging = nullptr;
    /**
     * @brief Loco selected
     */
    Selected _selected;
    /**
     * @brief Destroy the loco buttons
     */
    void destroyButtons();
    /**
     * @brief Add loco to the button JSON document
     * 
     * @param loco 
     */
    void addLoco(FatFile loco);
    /**
     * @brief Get the loco address from the file name
     * 
     * @param loco 
     * @return uint16_t 
     */
    uint16_t getAddrFromFN(FatFile &loco);
    /**
     * @brief Draw paging and buttons
     */
    void drawPagingAndButtons();
    /**
     * @brief Load a loco group
     * 
     * @param locos 
     */
    void loadGroup(JsonArray locos);
    /**
     * @brief Draw loco buttons
     */
    void drawButtons();
};

#endif
