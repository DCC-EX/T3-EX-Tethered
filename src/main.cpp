#include <Arduino.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Encoder.h>
#include <ArduinoJson.h>

#include <TouchRegion.h>
#include <TouchButton.h>
#include <DCCEx.h>
#include <UI.h>
#include <Menu.h>
#include <LocoByAddress.h>
#include <LocoByName.h>
#include <Loco.h>
#include <Program.h>

// #define THROTTLE_DEBUG

// Pins
const uint8_t TFT_CS = 10;
const uint8_t TFT_DC = 9;
const uint8_t SD_CS = 4;

const uint8_t ENCODER_A = 18;
const uint8_t ENCODER_B = 19;
const uint8_t ENCODER_BTN = A12;

SdFat sd; // SD FileSystem
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); // TFT
Adafruit_FT6206 ts = Adafruit_FT6206(); // Touch Screen
TS_Point tp;

Encoder encoder(ENCODER_A, ENCODER_B); // Encoder
struct EncoderButtonStateEnum {
  enum State : uint8_t {
    IDLE,
    PRESSED,
    RELEASED
  };
};
typedef EncoderButtonStateEnum::State EncoderButtonState;
uint32_t encoderPressMillis = 0;
uint8_t encoderBtnState = EncoderButtonState::IDLE;
uint8_t currentEncoderPinState, lastEncoderPinState;

const uint8_t MAX_LOCOS = 50;
LocoState locos[MAX_LOCOS]; // Max 50, same as DCC++Ex
DCCEx dcc(&Serial2); // DCC++Ex Interface

bool rotated = false;
TouchRegion menu(208, 0, 32, 22); // Menu
UI *activeUI = nullptr;
int8_t activeLoco = -1;
bool isMenuUI = false;
void clearAndDrawMenuUI();
void setMenuUI();

#ifdef THROTTLE_DEBUG
// https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory#sram-370031-5
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
#endif

/**
 * @brief Get the index of the LocoState object
 * If the address isn't found in the `locos` array it'll add it to the first empty one
 * 
 * @param address 
 * @return int8_t 
 */
int8_t getLoco(uint16_t address) {
  int8_t firstEmpty = -1;
  for (uint8_t i = 0; i < MAX_LOCOS; i++) {
    if (firstEmpty == -1 && locos[i].address == 0) {
      firstEmpty = i;
    } else if (locos[i].address == address) {
      return i;
    }
  }

  if (firstEmpty != -1) {
    locos[firstEmpty].address = address;
  }
  return firstEmpty;
}

/**
 * @brief Free the `LocoState` for the address specified
 * 
 * @param address 
 */
void freeLoco(uint16_t address) {
  for (uint8_t i = 0; i < MAX_LOCOS; i++) {
    if (locos[i].address == address) {
      locos[i] = LocoState();
      return;
    }
  }
}

/**
 * @brief Draw the menu burger icon
 */
void drawMenuIcon() {
  tft.fillRect(208, 0, 32, 4, ILI9341_WHITE);
  tft.fillRect(208, 9, 32, 4, ILI9341_WHITE);
  tft.fillRect(208, 18, 32, 4, ILI9341_WHITE);
}

/**
 * @brief Change the current UI. This will clear the screen, free the current `UI` then set the new active
 * 
 * @tparam T 
 * @param ui 
 */
template<typename T>
void setUI(T&& ui) {
  tft.fillRect(0, 0, 207, 22, ILI9341_BLACK);
  tft.fillRect(0, 22, 240, 298, ILI9341_BLACK);

  delete activeUI;
  activeUI = ui();
}

/**
 * @brief Set the active UI to `Loco`
 */
void setLocoUI() {
  setUI([]() {
    return new Loco(&tft, &sd, &dcc, &locos[activeLoco]);
  });
}

/**
 * @brief Set the active UI to `LocoByAddress`
 * If an address is provided it'll be set as the active loco
 * If the KeyPad is cancelled we go back to the menu UI
 */
void setLocoByAddressUI() {
  setUI([]() {
    return new LocoByAddress(&tft, [](uint16_t value) { // Loco selected callback
      if (value != 0) {
        activeLoco = getLoco(value);
        setLocoUI();
      } else {
        setMenuUI();
      }
    });
  });
}

/**
 * @brief Set the active UI to `LocoByName`
 * If an address is provided it'll be set as the active loco
 */
void setLocoByNameUI(bool groups) {
  setUI([groups]() {
    return new LocoByName(&tft, &sd, groups, [](uint16_t value) { // Loco selected callback
      if (value != 0) {
        activeLoco = getLoco(value);
      }
      setLocoUI();
    });
  });
}

/**
 * @brief Set the active UI as `Program`
 */
void setProgramUI() {
  setUI([]() {
    return new Program(&tft, &dcc);
  });
}

/**
 * @brief Set the active UI as `Menu`
 * The `Menu` UI has a callback for when a menu button is pressed
 */
void setMenuUI() {
  isMenuUI = true;
  setUI([]() {
    return new Menu(&tft, [](uint8_t btn) { // Menu option callback
      switch (btn) {
        case MenuButton::ROTATE: {
          rotated = !rotated;
          tft.setRotation(rotated ? 2 : 0);

          EEPROM.update(0, !rotated);
          
          clearAndDrawMenuUI();
        } break;
        case MenuButton::LOCO_LOAD_BY_ADDRESS: {
          isMenuUI = false; 
          setLocoByAddressUI();
        } break;
        case MenuButton::LOCO_LOAD_BY_NAME:
        case MenuButton::LOCO_LOAD_BY_GROUP: {
          isMenuUI = false; 
          setLocoByNameUI(btn == MenuButton::LOCO_LOAD_BY_GROUP);
        } break;
        case MenuButton::LOCO_RELEASE: {
          dcc.release(locos[activeLoco].address);
          locos[activeLoco] = { };
          activeLoco = -1;
        } break;
        case MenuButton::LOCO_PROGRAM: {
          isMenuUI = false; 
          setProgramUI();
        } break;
        case MenuButton::POWER_OFF_ALL: {
          dcc.powerOff(Track::ALL);
        } break;
        case MenuButton::POWER_ON_ALL: {
          dcc.powerOn(Track::ALL);
        } break;
        case MenuButton::POWER_OFF_MAIN: {
          dcc.powerOff(Track::MAIN);
        } break;
        case MenuButton::POWER_ON_MAIN: {
          dcc.powerOn(Track::MAIN);
        } break;
        case MenuButton::POWER_OFF_PROG: {
          dcc.powerOff(Track::PROG);
        } break;
        case MenuButton::POWER_ON_PROG: {
          dcc.powerOn(Track::PROG);
        } break;
        case MenuButton::POWER_JOIN: {
          dcc.powerJoin();
        } break;
      };
    });
  });
}

/**
 * @brief Clear the screen and redraw the menu icon and menu UI
 */
void clearAndDrawMenuUI() {
  // Clear screen
  tft.fillScreen(ILI9341_BLACK);
  // Menu burger icon
  drawMenuIcon();
  // Start with the menu
  setMenuUI();
}

void setup() {
  #ifdef THROTTLE_DEBUG
  Serial.begin(115200);
  #endif

  // Setup interrupt for the encoder button
  pinMode(ENCODER_BTN, INPUT_PULLUP);

  if (!sd.begin(SD_CS)) {
    // TODO, print error to tft?
  }

  // Setup the screen
  tft.begin();
  tft.setFont(&FreeSans9pt7b);
  ts.begin();

  // Should we rotate?
  rotated = !EEPROM.read(0);
  if (rotated) {
    tft.setRotation(2);
  }

  clearAndDrawMenuUI();
}

void loop() {
  // Encoder press
  currentEncoderPinState = digitalRead(ENCODER_BTN);
  if (currentEncoderPinState != lastEncoderPinState) {
    if (millis() - encoderPressMillis > 50) { // Debounce
      if (currentEncoderPinState == LOW && encoderBtnState == EncoderButtonState::IDLE) { // Press
        encoderBtnState = EncoderButtonState::PRESSED;
      } else if (currentEncoderPinState == HIGH && encoderBtnState == EncoderButtonState::PRESSED) { // Release
        encoderBtnState = EncoderButtonState::RELEASED;
      }
    }
    encoderPressMillis = millis();
  }
  lastEncoderPinState = currentEncoderPinState;

  if (ts.touched()) {
    #ifdef THROTTLE_DEBUG
    Serial.println(freeMemory());
    #endif
    // Remap the touch point
    tp = ts.getPoint(); 
    tp.x = map(tp.x, 0, 240, 240, 0);
    tp.y = map(tp.y, 0, 320, 320, 0);

    if (rotated) { // Invert touch points if rotated
      tp.x = 240 - tp.x;
      tp.y = 320 - tp.y;
    }

    if (menu.contains(tp)) { // Menu press?
      while (ts.touched()) {
        delay(50);
      }
      if (isMenuUI) { // If the menu is the current UI and we have an active loco we switch to the `Loco` UI
        if (activeLoco != -1) {
          isMenuUI = false;
          setLocoUI();
        }
      } else { // If current UI isn't `Menu` then switch to that
        setMenuUI();
      }
    } else { // Send the touch to the active UI
      activeUI->touch(tp.x, tp.y, []() { return ts.touched(); });
    }
  } else if (encoder.read() != 0) { // Encoder change, send to active UI
    int32_t read = encoder.read();
    if (read >= 2) {
      activeUI->encoderChange(CW);
      encoder.write(0);
    } else if (read <= -2) {
      activeUI->encoderChange(CCW);
      encoder.write(0);
    }
  } else if (encoderBtnState == EncoderButtonState::PRESSED && millis() - encoderPressMillis > 2000) { // Encoder pressed and held for more than 2 seconds
    encoderBtnState = EncoderButtonState::IDLE;
    dcc.emergencyStopAll(); // Stop all locos
    for (uint8_t i = 0; i < MAX_LOCOS; i++) { // Reset all loco speeds to zero
      locos[i].speed = 0;
    }
    activeUI->encoderPress(true);
  } else if (encoderBtnState == EncoderButtonState::RELEASED && millis() - encoderPressMillis < 1000) { // Encoder pressed for less than 1 second
    encoderBtnState = EncoderButtonState::IDLE;
    activeUI->encoderPress();
  }
  dcc.clearCSResponse();
}
