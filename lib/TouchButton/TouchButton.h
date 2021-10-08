#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#include <TouchRegion.h>
#include <Arduino.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_ImageReader.h>

class TouchButton : public TouchRegion {
  public:
    /**
     * @brief Button style
     */
    struct Style {
      Style(uint16_t outline, uint16_t fill, uint16_t text, const char *icon = nullptr)
          : outline(outline), fill(fill), text(text), icon(icon) { }
      /**
       * @brief Button outline colour
       */
      uint16_t outline;
      /**
       * @brief Button fill colour
       */
      uint16_t fill;
      /**
       * @brief Button label text colour
       */
      uint16_t text;
      const char *icon;
    };

    TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label, Style idle, Style pressed, bool drawOnConstruct = true);
    TouchButton(Adafruit_SPITFT *tft,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label, Style idle, Style pressed, bool drawOnConstruct = true);

    TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label, bool drawOnConstruct = true);
    TouchButton(Adafruit_SPITFT *tft,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label, bool drawOnConstruct = true);

    TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const __FlashStringHelper *label, Style idle, Style pressed, bool drawOnConstruct = true);
    TouchButton(Adafruit_SPITFT *tft,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const __FlashStringHelper *label, Style idle, Style pressed, bool drawOnConstruct = true);

    TouchButton(Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const __FlashStringHelper *label, bool drawOnConstruct = true);
    TouchButton(Adafruit_SPITFT *tft,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const __FlashStringHelper *label, bool drawOnConstruct = true);
    /**
     * @brief Draw button
     * 
     * @param pressed Draw idle or pressed?
     */
    void draw(bool pressed = false);
  private:
    /**
     * @brief Pointer to TFT instance
     */
    Adafruit_SPITFT *_tft;
    /**
     * @brief Pointer to `Adafruit_ImageReader` instance, used for icons
     */
    Adafruit_ImageReader *_reader;
    /**
     * @brief Pointer to char array for button label
     */
    const char *_label;
    /**
     * @brief Is the char array in PROGMEM?
     */
    bool _pgm;
    /**
     * @brief Idle button style
     */
    Style _idle;
    /**
     * @brief Pressed button style
     */
    Style _pressed;
    /**
     * @brief Construct a new `TouchButton` object
     * 
     * @param drawOnConstruct 
     * @param tft 
     * @param reader 
     * @param x 
     * @param y 
     * @param w 
     * @param h 
     * @param label 
     * @param pgm 
     * @param idle 
     * @param pressed 
     */
    TouchButton(bool drawOnConstruct,
                Adafruit_SPITFT *tft, Adafruit_ImageReader *reader,
                int16_t x, int16_t y, uint16_t w, uint16_t h,
                const char *label = nullptr, bool pgm = false,
                Style idle = {
                  ILI9341_WHITE,
                  ILI9341_BLACK,
                  ILI9341_WHITE,
                },
                Style pressed = {
                  ILI9341_WHITE,
                  ILI9341_WHITE,
                  ILI9341_BLACK,
                });
};

#endif
