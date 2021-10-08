#ifndef DCC_EX_H
#define DCC_EX_H

#include <Arduino.h>

struct TracksEnum {
  enum Tracks : uint8_t {
    ALL,
    MAIN,
    PROG
  };
};
typedef TracksEnum::Tracks Track;

class DCCEx  {
  private:
    /**
     * @brief Pointer to the Serial used
     */
    HardwareSerial *_serial;
    /**
     * @brief Read the serial buffer until a newline character is found
     * 
     * @param buffer Pointer to char array
     * @param length Size of char array
     * @return uint16_t 
     */
    uint16_t readCSResponse(char *buffer, uint16_t length);
  public:
    /**
     * @brief Construct a new `DCCEx` object 
     * 
     * @param serial The serial interface to use
     * @param timeout Timeout for the serial interface
     */
    DCCEx(HardwareSerial *serial, uint16_t timeout = 2500);
    /**
     * @brief Some CS commands send a response we don't use so we need to clear the input buffer
     */
    void clearCSResponse();
    /**
     * @brief Power off the selected track
     * 
     * @param track A value from the `Track` enum
     */
    void powerOff(Track track);
    /**
     * @brief Power on the selected track
     * 
     * @param track A value from the `Track` enum
     */
    void powerOn(Track track);
    /**
     * @brief Join PROG and MAIN tracks
     */
    void powerJoin();
    /**
     * @brief Emergency stop all locos
     */
    void emergencyStopAll();
    /**
     * @brief Set the speed and direction of the loco at the address
     * 
     * @param address Loco address
     * @param speed Loco speed
     * @param direction Loco direction
     * @return true 
     * @return false 
     */
    bool setThrottle(uint16_t address, int8_t speed, uint8_t direction);
    /**
     * @brief Toggle the Fn state
     * 
     * @param address Loco address
     * @param fn Loco Fn #
     * @param state Fn state
     */
    void setFn(uint16_t address, uint16_t fn, bool state);
    /**
     * @brief Release the loco at address
     * 
     * @param address Loco address
     */
    void release(uint16_t address);
    /**
     * @brief Write address to the loco on the PROG track
     * 
     * @param address New loco address
     * @return true 
     * @return false 
     */
    bool writeAddress(uint16_t address);
    /**
     * @brief Read address from the loco on the PROG track
     * 
     * @return int16_t 
     */
    int16_t readAddress();
    /**
     * @brief Write a CV byte value to the loco on the PROG track
     * 
     * @param cv CV #
     * @param value CV value
     * @return true 
     * @return false 
     */
    bool writeCVByte(uint16_t cv, uint8_t value);
    /**
     * @brief Read a CV byte value from the loco on the PROG track
     * 
     * @param cv CV #
     * @return int16_t 
     */
    int16_t readCVByte(uint16_t cv);
    /**
     * @brief Write a CV bit value to the loco on the PROG track
     * 
     * @param cv CV #
     * @param bit CV bit #
     * @param value CV bit value
     * @return true 
     * @return false 
     */
    bool writeCVBit(uint16_t cv, uint8_t bit, bool value);
};

#endif
