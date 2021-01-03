//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2020-12-03
// PURPOSE: unit tests for the PCF8574 I2C port expander
//          https://github.com/RobTillaart/PCF8574
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)

#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "PCF8574.h"

PCF8574 PCF(0x38);


unittest_setup()
{
}

unittest_teardown()
{
}

unittest(test_begin)
{
  fprintf(stderr, "VERSION: %s\n", PCF8574_LIB_VERSION);

  PCF8574 PCF(0x38);

  PCF.begin();

  int readValue = PCF.read8();
  assertEqual(0, readValue);

  int expect = PCF8574_I2C_ERROR;
  assertEqual(expect, PCF.lastError());
}


unittest(test_read)
{
  PCF8574 PCF(0x38);
  int readValue;

  PCF.begin();
  for (int i = 0; i < 8; i++)
  {
    fprintf(stderr, "line %d\t", i);
    readValue = PCF.read(i);
    assertEqual(0, readValue);

    int expect = PCF8574_I2C_ERROR;
    assertEqual(expect, PCF.lastError());
  }
  printf("test");
  readValue = PCF.read(8);
  assertEqual(0, readValue);
  int expect = PCF8574_PIN_ERROR;
  assertEqual(expect, PCF.lastError());
}


unittest_main()

// --------
