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


class PCF8574 {
  public:
    /**
     * @brief Construct a new PCF8574::PCF8574 object
     * 
     * @param deviceAddress optional address of I2C device; default = 0x20;
     * @param wire optional address of Wire instance; default = \&Wire;
     */
    explicit PCF8574(const uint8_t deviceAddress = 0x20, TwoWire *wire = &Wire);

    /**
     * @brief init PCF8574 instance
     * 
     * @param value  optional init value to write; default = 0xFF;
     * @return true 
     * @return false 
     */
    bool begin(uint8_t value = PCF8574_INITIAL_VALUE);
  
    /**
     * @brief check if device is connected
     * 
     * @return true 
     * @return false 
     */
    bool isConnected();

    /**
     * @brief set address of PCF8574 device
     * note: setting the address corrupt internal buffer values.
     * A read8() / write8() call updates them.
     * @param deviceAddress I2C address; possible values 0x20...0x27
     * @return true 
     * @return false 
     */
    bool setAddress(const uint8_t deviceAddress);

    /**
    * @brief getter for I2C address of pcf7485 device
    *  should be in the range of 0x20...0x27
    * 
    * @return uint8_t address
    */
    uint8_t getAddress() const { return _address; }

    /**
     * @brief read all 8 bit at once from PCF8574
     * 
     * @return uint8_t value read from device
     */
    uint8_t read8();

    /**
     * @brief read one bit from PCF8574 device
     * 
     * @param pin pin to read (0...7)
     * @return uint8_t value
     */
    uint8_t read(const uint8_t pin);

    /**
     * @brief returns the last read inputs again,
     * as this information is buffered in the class this is faster than re-read the pins.
     * 
     * @return uint8_t internal member _dataIn
     */
    uint8_t value() const { return _dataIn; };

    /**
     * @brief write all 8 bit at once to PCF8574 device
     * 
     * @param value to write
     */
    void write8(const uint8_t value);

    /**
     * @brief writes a single pin; pin = 0..7.
     * 
     * @param pin pin number for write (0...7)
     * @param value value to write, HIGH(1) or LOW (0)
     */
    void write(const uint8_t pin, const uint8_t value);

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

    /**
     * @brief read Button mask. 
     * 
     * @param mask use the mask set by setButtonMask to select specific input pins.
     * @return * uint8_t 
     */
    uint8_t readButton8(const uint8_t mask);

    /**
     * @brief read button read a single input pin.
     * 
     * @param pin pin number to be read
     * @return uint8_t value read from pin
     */
    uint8_t readButton(const uint8_t pin);
  
    /**
     * @brief sets the bit mask which lines are input.
     * 
     * @param mask bit mask of inputs
     */
    void setButtonMask(const uint8_t mask) { _buttonMask = mask; };

    /**
     * @brief returns the internal button mask member
     * 
     * @return uint8_t the mask for input lines
     */
    uint8_t getButtonMask() { return _buttonMask; };

    /**
     * @brief toggle one bit on PCF8574 device
     * 
     * @param pin pin number for write (0...7)
     */
    void toggle(const uint8_t pin);

    /**
     * @brief toggles a selection of pins, if you want to invert all pins use 0xFF (default value).
     * 
     * @param mask 
     */
    void toggleMask(const uint8_t mask = 0xFF);

    /**
     * @brief shifts output channels n pins (default 1) pins right (e.g. LEDs ).
     * Fills the higher lines with zero's.
     * 
     * @param n number of shifts
     */
    void shiftRight(const uint8_t n = 1);

    /**
     * @brief shifts output channels n pins (default 1) pins left (e.g. LEDs ).
     * Fills the lower lines with zero's
     * 
     * @param n number of shifts
     */
    void shiftLeft(const uint8_t n = 1);

    /**
     * @brief rotates output channels to right,
     * moving lowest line to highest line.
     * 
     * @param n number of moves
     */
    void rotateRight(const uint8_t n = 1);

    /**
     * @brief rotates output channels to left,
     * moving highest line to lowest line.
     * 
     * @param n number of moves
     */
    void rotateLeft(const uint8_t n = 1);

    /**
     * @brief reverse the "bit pattern" of the lines,
     * swapping pin 7 with 0, 6 with 1, 5 with 2 etc.
     * 
     */
    void reverse();

    /**
     * @brief select sets the specified pin pin to High,
     * all others set to Low. If pin > 7 all pins are set to LOW.
     * 
     * @param pin pin which should be set to 1
     */
    void select(const uint8_t pin);

    /**
     * @brief select sets the pins from 0...pin to High,
     * all others set to Low. If pin > 7 all pins are set to LOW.
     * 
     * @param pin pin up to which should be set to 1
     */
    void selectN(const uint8_t pin);

    /**
     * @brief sets all pins to LOW.
     * 
     */
    void selectNone() { write8(0x00); };

    /**
     * @brief sets all pins to HIGH.
     * 
     */
    void selectAll() { write8(0xFF); };

    /**
     * @brief returns the last error from the lib (see .h file).
     * 
     * @return int error value to return
     */
    int lastError();

  private:

    // internal member for error
    int _error {PCF8574_OK};
    // internal buffer device address
    uint8_t _address;
    // internal buffer for data read from device
    uint8_t _dataIn {0};
    // internal buffer for data write to device
    uint8_t _dataOut {0xFF};
    // internal buffer for button (input) lines
    uint8_t _buttonMask {0xFF};

    TwoWire*  _wire;
  };


//  -- END OF FILE --

