#ifndef PAGING_H
#define PAGING_H

#include <UI.h>
#include <Arduino.h>
#include <Adafruit_SPITFT.h>
#include <TouchButton.h>

class Paging : public UI {
  private:
    /**
     * @brief `TouchButton` to go back a page
     */
    TouchButton *_prev;
    /**
     * @brief `TouchButton` to go to next page
     */
    TouchButton *_next;
    /**
     * @brief Total pages
     */
    uint8_t _pages;
    /**
     * @brief Current page
     */
    uint8_t _page = 1;
    /**
     * @brief Change to next page, if greater than total we reset to 1
     */
    void nextPage();
    /**
     * @brief Change to prev page, if 0 we reset to end (total)
     */
    void prevPage();
    /**
     * @brief Update the current page label
     */
    void update();
  public:
    /**
     * @brief Construct a new `Paging` UI object
     * 
     * @param tft 
     * @param pages 
     */
    Paging(Adafruit_SPITFT *tft, uint8_t pages);
    /**
     * @brief Destroy the `Paging` UI object
     */
    ~Paging();
    /**
     * @brief Handle a screen touch
     * 
     * @param x 
     * @param y 
     * @param touched 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touched);
    /**
     * @brief Handle encoder rotation
     * 
     * @param rotation 
     */
    void encoderChange(Rotation rotation);
    /**
     * @brief Get the current page #
     * 
     * @return uint8_t 
     */
    uint8_t getPage();
};

#endif
