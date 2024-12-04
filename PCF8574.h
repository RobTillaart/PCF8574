#pragma once
/**
 *    @file PCF8574.cpp
 *  @author Rob Tillaart
 *    @date 02-febr-2013
 * @version 0.4.2
 *   @brief Arduino library for PCF8574 - 8 channel I2C IO expander
 *
 *     @see https://github.com/RobTillaart/PCF8574
 *     @see http://forum.arduino.cc/index.php?topic=184800
*/


#include "Arduino.h"
#include "Wire.h"


#define PCF8574_LIB_VERSION         (F("0.4.1"))

#ifndef PCF8574_INITIAL_VALUE
#define PCF8574_INITIAL_VALUE       0xFF
#endif

#define PCF8574_OK                  0x00
#define PCF8574_PIN_ERROR           0x81
#define PCF8574_I2C_ERROR           0x82


class PCF8574
{
public:
  explicit PCF8574(const uint8_t deviceAddress = 0x20, TwoWire *wire = &Wire);

  bool    begin(uint8_t value = PCF8574_INITIAL_VALUE);
  bool    isConnected();

  //  note: setting the address corrupt internal buffer values
  //  a read8() / write8() call updates them.
  bool    setAddress(const uint8_t deviceAddress);

  /**
  * @brief getter for I2C address of pcf7485 device
  *  should be in the range of 0x20...0x27
  * 
  * @return uint8_t address
  */
  uint8_t getAddress() const { return _address; }


  uint8_t read8();
  uint8_t read(const uint8_t pin);
  
  /**
   * @brief returns the last read inputs again,
   * as this information is buffered in the class this is faster than re-read the pins.
   * 
   * @return uint8_t internal member _dataIn
   */
  uint8_t value() const { return _dataIn; };

  void    write8(const uint8_t value);
  void    write(const uint8_t pin, const uint8_t value);
  
  /**
   * @brief returns the last written data.
   * 
   * @return uint8_t internal member _dataOut
   */
  uint8_t valueOut() const { return _dataOut; }

  /**
   * @brief returns the set buttonMask.
   * 
   * @return uint8_t the mask for input lines
   */
  uint8_t readButton8() { return PCF8574::readButton8(_buttonMask); }
  
  uint8_t readButton8(const uint8_t mask);
  uint8_t readButton(const uint8_t pin);
  
  /**
   * @brief sets the bit mask which lines are input.
   * 
   * @param mask bit mask of inputs
   */
  void    setButtonMask(const uint8_t mask) { _buttonMask = mask; };

  
  /**
   * @brief returns the internal button mask member
   * 
   * @return uint8_t the mask for input lines
   */
  uint8_t getButtonMask() { return _buttonMask; };


  //  rotate, shift, toggle, reverse expect all lines are output
  void    toggle(const uint8_t pin);
  //      default 0xFF ==> invertAll()
  void    toggleMask(const uint8_t mask = 0xFF);
  void    shiftRight(const uint8_t n = 1);
  void    shiftLeft(const uint8_t n = 1);
  void    rotateRight(const uint8_t n = 1);
  void    rotateLeft(const uint8_t n = 1);
  void    reverse();

  void    select(const uint8_t pin);
  void    selectN(const uint8_t pin);

  /**
   * @brief sets all pins to LOW.
   * 
   */
  void    selectNone() { write8(0x00); };

  /**
   * @brief sets all pins to HIGH.
   * 
   */
  void    selectAll()  { write8(0xFF); };

  int     lastError();

private:
  int     _error {PCF8574_OK};
  uint8_t _address;
  uint8_t _dataIn {0};
  uint8_t _dataOut {0xFF};
  uint8_t _buttonMask {0xFF};
  

  TwoWire*  _wire;
};


//  -- END OF FILE --

