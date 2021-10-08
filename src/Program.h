#ifndef PROGRAM_H
#define PROGRAM_H

#include <UI.h>
#include <DCCEx.h>
#include <TouchButton.h>
#include <KeyPad.h>

/**
 * @brief `Program` buttons
 */
struct ProgramButtonsEnum {
  enum Buttons : uint8_t {
    WRITE_ADDRESS,
    WRITE_BYTE,
    WRITE_BIT,
    READ_ADDRESS,
    READ_BYTE,
    COUNT
  };
};
typedef ProgramButtonsEnum::Buttons ProgramButton;

/**
 * @brief `Program` steps
 */
struct ProgramStepsEnum {
  enum Steps : uint8_t {
    WRITE_ADDRESS_GET_ADDRESS,
    WRITE_CV_BYTE_GET_CV,
    WRITE_CV_BYTE_GET_VALUE,
    WRITE_CV_BIT_GET_CV,
    WRITE_CV_BIT_GET_BIT,
    WRITE_CV_BIT_GET_VALUE,
    READ_CV_BYTE_GET_CV
  };
};
typedef ProgramStepsEnum::Steps ProgramStep;

class Program : public UI {
  public:
    /**
     * @brief Construct a new `Program` UI object
     * 
     * @param tft 
     * @param dcc 
     */
    Program(Adafruit_SPITFT *tft, DCCEx *dcc);
    /**
     * @brief Destroy the `Program` UI object
     */
    ~Program();
    /**
     * @brief Handle a UI touch event
     * 
     * @param x 
     * @param y 
     * @param touched 
     * @return int8_t 
     */
    int8_t touch(uint16_t x, uint16_t y, Touched touched);
  private:
    /**
     * @brief Pointer to `DCCEx` object
     */
    DCCEx *_dcc;
    /**
     * @brief Current program step, only used by program options that require multiple values
     */
    uint8_t _step;
    /**
     * @brief Carried step data, used by program options that require multiple values
     */
    uint16_t _stepData[2];
    /**
     * @brief `Program` option buttons
     */
    TouchButton *_programBtns[ProgramButton::COUNT];
    /**
     * @brief The `Ok` button, used on write and read results
     */
    TouchButton *_ok = nullptr;
    /**
     * @brief `KeyPad` UI object
     */
    KeyPad *_keyPad = nullptr;
    /**
     * @brief Destroy the `KeyPad`
     */
    void destroyKeyPad();
    /**
     * @brief Clear the UI
     */
    void clearUI();
    /**
     * @brief Draw the program option buttons
     */
    void drawButtons();
    /**
     * @brief `Program` option button pressed
     * 
     * @param btn 
     */
    void programButtonPress(uint8_t btn);
    /**
     * @brief Next step in multi value program option
     * 
     * @param step 
     * @param title 
     * @param max 
     * @param min 
     */
    void newStep(uint8_t step, const __FlashStringHelper *title, uint16_t max, uint8_t min);
    /**
     * @brief Use value from `KeyPad`
     */
    void keyPadEnter();
    /**
     * @brief Was write successful
     * 
     * @param result 
     */
    void writeResult(bool result);
    /**
     * @brief Print result of read
     * 
     * @param result 
     */
    void readResult(int16_t result);
};

#endif
