/**
 *    @file PCF8574.cpp
 *  @author Rob Tillaart
 *    @date 02-febr-2013
 * @version 0.4.1
 *   @brief Arduino library for PCF8574 - 8 channel I2C IO expander
 *
 *     @see https://github.com/RobTillaart/PCF8574
 *     @see http://forum.arduino.cc/index.php?topic=184800
*/

#include "PCF8574.h"

/**
 * @brief Construct a new PCF8574::PCF8574 object
 * 
 * @param deviceAddress optional address of I2C device; default = 0x20;
 * @param wire optional address of Wire instance; default = \&Wire;
 */
PCF8574::PCF8574(const uint8_t deviceAddress, TwoWire *wire)
: _address {deviceAddress}, _wire {wire}
{}

/**
 * @brief init PCF8574 instance
 * 
 * @param value  optional init value to write; default = 0xFF;
 * @return true 
 * @return false 
 */
bool PCF8574::begin(uint8_t value)
{
  if (! isConnected()) return false;
  PCF8574::write8(value);
  return true;
}

/**
 * @brief check if device is connected
 * 
 * @return true 
 * @return false 
 */
bool PCF8574::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}

/**
 * @brief set address of PCF8574 device
 * 
 * @param deviceAddress I2C address; possible values 0x20...0x27
 * @return true 
 * @return false 
 */
bool PCF8574::setAddress(const uint8_t deviceAddress)
{
  _address = deviceAddress;
  return isConnected();
}

/**
 * @brief getter for I2C address of pcf7485 device
 *  should be in the range of 0x20...0x27
 * 
 * @return uint8_t address
 */
uint8_t PCF8574::getAddress()
{
  return _address;
}

/**
 * @brief read all 8 bit at once from PCF8574
 * 
 * @return uint8_t value read from device
 */
uint8_t PCF8574::read8()
{
  if (_wire->requestFrom(_address, (uint8_t)1) != 1)
  {
    _error = PCF8574_I2C_ERROR;
    return _dataIn;  //  last value
  }
  _dataIn = _wire->read();
  return _dataIn;
}

/**
 * @brief write all 8 bit at once to PCF8574 device
 * 
 * @param value to write
 */
void PCF8574::write8(const uint8_t value)
{
  _dataOut = value;
  _wire->beginTransmission(_address);
  _wire->write(_dataOut);
  _error = _wire->endTransmission();
}

/**
 * @brief read one bit from PCF8574 device
 * 
 * @param pin pin to read (0...7)
 * @return uint8_t value
 */
uint8_t PCF8574::read(const uint8_t pin)
{
  if (pin > 7)
  {
    _error = PCF8574_PIN_ERROR;
    return 0;
  }
  PCF8574::read8();
  return (_dataIn & (1 << pin)) > 0;
}

/**
 * @brief writes a single pin; pin = 0..7.
 * 
 * @param pin pin number for write (0...7)
 * @param value value to write, HIGH(1) or LOW (0)
 */
void PCF8574::write(const uint8_t pin, const uint8_t value)
{
  if (pin > 7)
  {
    _error = PCF8574_PIN_ERROR;
    return;
  }
  if (value == LOW)
  {
    _dataOut &= ~(1 << pin);
  }
  else
  {
    _dataOut |= (1 << pin);
  }
  write8(_dataOut);
}

/**
 * @brief toggle one bit on PCF8574 device
 * 
 * @param pin pin number for write (0...7)
 */
void PCF8574::toggle(const uint8_t pin)
{
  if (pin > 7)
  {
    _error = PCF8574_PIN_ERROR;
    return;
  }
  toggleMask(1 << pin);
}

/**
 * @brief toggles a selection of pins, if you want to invert all pins use 0xFF (default value).
 * 
 * @param mask 
 */
void PCF8574::toggleMask(const uint8_t mask)
{
  _dataOut ^= mask;
  PCF8574::write8(_dataOut);
}

/**
 * @brief shifts output channels n pins (default 1) pins right (e.g. LEDs ).
 * Fills the higher lines with zero's.
 * 
 * @param n number of shifts
 */
void PCF8574::shiftRight(const uint8_t n)
{
  if ((n == 0) || (_dataOut == 0)) return;
  if (n > 7)         _dataOut = 0;     //  shift 8++ clears all, valid...
  if (_dataOut != 0) _dataOut >>= n;   //  only shift if there are bits set
  PCF8574::write8(_dataOut);
}

/**
 * @brief shifts output channels n pins (default 1) pins left (e.g. LEDs ).
 * Fills the lower lines with zero's
 * 
 * @param n number of shifts
 */
void PCF8574::shiftLeft(const uint8_t n)
{
  if ((n == 0) || (_dataOut == 0)) return;
  if (n > 7)         _dataOut = 0;    //  shift 8++ clears all, valid...
  if (_dataOut != 0) _dataOut <<= n;  //  only shift if there are bits set
  PCF8574::write8(_dataOut);
}

/**
 * @brief returns the last error from the lib (see .h file).
 * 
 * @return int error value to return
 */
int PCF8574::lastError()
{
  int e = _error;
  _error = PCF8574_OK;  //  reset error after read, is this wise?
  return e;
}

/**
 * @brief rotates output channels to right,
 * moving lowest line to highest line.
 * 
 * @param n number of moves
 */
void PCF8574::rotateRight(const uint8_t n)
{
  uint8_t r = n & 7;
  if (r == 0) return;
  _dataOut = (_dataOut >> r) | (_dataOut << (8 - r));
  PCF8574::write8(_dataOut);
}

/**
 * @brief rotates output channels to left,
 * moving highest line to lowest line.
 * 
 * @param n number of moves
 */
void PCF8574::rotateLeft(const uint8_t n)
{
  rotateRight(8 - (n & 7));
}

/**
 * @brief reverse the "bit pattern" of the lines,
 * swapping pin 7 with 0, 6 with 1, 5 with 2 etc.
 * 
 */
void PCF8574::reverse()  //  quite fast: 4 and, 14 shifts, 3 or, 3 assignment.
{
  uint8_t x = _dataOut;
  x = (((x & 0xAA) >> 1) | ((x & 0x55) << 1));
  x = (((x & 0xCC) >> 2) | ((x & 0x33) << 2));
  x =          ((x >> 4) | (x << 4));
  PCF8574::write8(x);
}

/**
 * @brief read Button mask. 
 * 
 * @param mask use the mask set by setButtonMask to select specific input pins.
 * @return * uint8_t 
 */
uint8_t PCF8574::readButton8(const uint8_t mask)
{
  uint8_t temp = _dataOut;
  PCF8574::write8(mask | _dataOut);  //  read only selected lines
  PCF8574::read8();
  PCF8574::write8(temp);             //  restore
  return _dataIn;
}

/**
 * @brief read button read a single input pin.
 * 
 * @param pin pin number to be read
 * @return uint8_t value read from pin
 */
uint8_t PCF8574::readButton(const uint8_t pin)
{
  if (pin > 7)
  {
    _error = PCF8574_PIN_ERROR;
    return 0;
  }

  uint8_t temp = _dataOut;
  PCF8574::write(pin, HIGH);
  uint8_t value = PCF8574::read(pin);
  PCF8574::write8(temp);
  return value;
}

/**
 * @brief select sets the specified pin pin to High,
 * all others set to Low. If pin > 7 all pins are set to LOW.
 * 
 * @param pin pin which should be set to 1
 */
void PCF8574::select(const uint8_t pin)
{
  uint8_t n = 0x00;
  if (pin < 8) n = 1 << pin;
  write8(n);
};

/**
 * @brief select sets the pins from 0...pin to High,
 * all others set to Low. If pin > 7 all pins are set to LOW.
 * 
 * @param pin pin up to which should be set to 1
 */
void PCF8574::selectN(const uint8_t pin)
{
  uint8_t n = 0xFF;
  if (pin < 8) n = (2 << pin) - 1;
  write8(n);
};


//  -- END OF FILE --

