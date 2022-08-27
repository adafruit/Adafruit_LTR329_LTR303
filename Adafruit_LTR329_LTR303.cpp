/*!
 *  @file Adafruit_LTR329_LTR303.cpp
 *
 *  @mainpage Adafruit LTR329 and LTR303 I2C Light Sensorso_sec Introduction
 *
 * 	I2C Driver for the LTR329 and LTR303 I2C Light Sensors
 *
 * 	This is a library for the Adafruit LTR329 & LTR303 breakouts:
 * 	https://www.adafruit.com/product/5591
 * 	https://www.adafruit.com/product/5610
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_LTR329_LTR303.h"

/*!
 *    @brief  Instantiates a new LTR329 class
 */
Adafruit_LTR329::Adafruit_LTR329(void) {}

/*!
 *    @brief  Setups the hardware for talking to the LTR329
 *    @param  theWire An optional pointer to an I2C interface
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_LTR329::begin(TwoWire *theWire) {
  delete i2c_dev;

  i2c_dev = new Adafruit_I2CDevice(LTR329_I2CADDR_DEFAULT, theWire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // check part ID!        

  Adafruit_I2CRegister idreg = Adafruit_I2CRegister(i2c_dev, LTR329_PART_ID);
  if (idreg.read() != 0xA0) {
    return false;
  }


  Adafruit_I2CRegister manreg = Adafruit_I2CRegister(i2c_dev, LTR329_MANU_ID);
  if (manreg.read() != 0x05) {
    return false;
  }

  // OK now we can do a soft reset
  if (!reset()) {
    return false;
  }

  // main screen turn on
  enable(true);
  if (!enabled()) {
    return false;
  }

  StatusReg = new Adafruit_I2CRegister(i2c_dev, LTR329_STATUS);
  DataReadyBit = new Adafruit_I2CRegisterBits(StatusReg, 1, 2);
  DataInvalidBit = new Adafruit_I2CRegisterBits(StatusReg, 1, 7);

  return true;
}

/*!
 *  @brief  Perform a soft reset with 10ms delay.
 *  @returns True on success
 */
bool Adafruit_LTR329::reset(void) {
  Adafruit_I2CRegister ctrlreg =
      Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits softreset =
      Adafruit_I2CRegisterBits(&ctrlreg, 1, 1); // # bits, bit_shift

  if (!softreset.write(1)) 
    return false;
  delay(10);

  return true;
}

/*!
 *  @brief  Enable or disable the light sensor
 *  @param  en True to enable, False to disable
 */
void Adafruit_LTR329::enable(bool en) {
  Adafruit_I2CRegister alsctrl =
      Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits active_mode =
      Adafruit_I2CRegisterBits(&alsctrl, 1, 0); // # bits, bit_shift

  active_mode.write(en);
}

/*!
 *  @brief  Read the enabled-bit from the sensor
 *  @returns True if enabled
 */
bool Adafruit_LTR329::enabled(void) {
  Adafruit_I2CRegister alsctrl =
      Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits active_mode =
      Adafruit_I2CRegisterBits(&alsctrl, 1, 0); // # bits, bit_shift

  return active_mode.read();
}


/*!
 *  @brief  Set the sensor gain
 *  @param  gain The desired gain: LTR3XX_GAIN_1, LTR3XX_GAIN_2, LTR3XX_GAIN_4
 *  LTR3XX_GAIN_8, LTR3XX_GAIN_48 or LTR3XX_GAIN_96
 */
void Adafruit_LTR329::setGain(ltr329_gain_t gain) {
  Adafruit_I2CRegister ctrlreg =
      Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits gainbits =
      Adafruit_I2CRegisterBits(&ctrlreg, 3, 2); // # bits, bit_shift

  gainbits.write(gain);
}

/*!
 *  @brief  Get the sensor's gain
 *  @returns gain The current gain: LTR3XX_GAIN_1, LTR3XX_GAIN_2, LTR3XX_GAIN_4
 *  LTR3XX_GAIN_8, LTR3XX_GAIN_48 or LTR3XX_GAIN_96
 */
ltr329_gain_t Adafruit_LTR329::getGain(void) {
  Adafruit_I2CRegister ctrlreg =
      Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits gainbits =
      Adafruit_I2CRegisterBits(&ctrlreg, 3, 2); // # bits, bit_shift

  return (ltr329_gain_t)gainbits.read();
}


/*!
 *  @brief  Set the sensor integration time. Longer times are more sensitive but
 *  take longer to read!
 *  @param  inttime The desired integration time (in millis): 
 *  LTR3XX_INTEGTIME_50, LTR3XX_INTEGTIME_100, LTR3XX_INTEGTIME_150, 
 *  LTR3XX_INTEGTIME_200,LTR3XX_INTEGTIME_250, LTR3XX_INTEGTIME_300,
 *  LTR3XX_INTEGTIME_350, LTR3XX_INTEGTIME_400, 
 */
void Adafruit_LTR329::setIntegrationTime(ltr329_integrationtime_t inttime) {
  Adafruit_I2CRegister ratereg =
      Adafruit_I2CRegister(i2c_dev, LTR329_MEAS_RATE);
  Adafruit_I2CRegisterBits intbits =
      Adafruit_I2CRegisterBits(&ratereg, 3, 3); // # bits, bit_shift

  intbits.write(inttime);
}

/*!
 *  @brief  Get the sensor's integration time for light sensing
 *  @returns The current integration time, in milliseconds.
 *  LTR3XX_INTEGTIME_50, LTR3XX_INTEGTIME_100, LTR3XX_INTEGTIME_150, 
 *  LTR3XX_INTEGTIME_200,LTR3XX_INTEGTIME_250, LTR3XX_INTEGTIME_300,
 *  LTR3XX_INTEGTIME_350, LTR3XX_INTEGTIME_400, 
 */
ltr329_integrationtime_t Adafruit_LTR329::getIntegrationTime(void) {
  Adafruit_I2CRegister ratereg =
      Adafruit_I2CRegister(i2c_dev, LTR329_MEAS_RATE);
  Adafruit_I2CRegisterBits intbits =
      Adafruit_I2CRegisterBits(&ratereg, 3, 3); // # bits, bit_shift

  return (ltr329_integrationtime_t)intbits.read();
}


/*!
 *  @brief  Set the sensor measurement rate. Longer times are needed when
 *  the integration time is longer OR if you want to have lower power usage
 *  @param  rate The desired measurement rate (in millis): 
 *  LTR3XX_MEASRATE_50, LTR3XX_MEASRATE_100, LTR3XX_MEASRATE_200,
 *  LTR3XX_MEASRATE_500, LTR3XX_MEASRATE_1000, or LTR3XX_MEASRATE_2000
 */
void Adafruit_LTR329::setMeasurementRate(ltr329_measurerate_t rate) {
  Adafruit_I2CRegister ratereg =
      Adafruit_I2CRegister(i2c_dev, LTR329_MEAS_RATE);
  Adafruit_I2CRegisterBits measbits =
      Adafruit_I2CRegisterBits(&ratereg, 3, 0); // # bits, bit_shift

  measbits.write(rate);
}

/*!
 *  @brief  Get the sensor's measurement rate.
 *  @returns The current measurement rate (in millis): 
 *  LTR3XX_MEASRATE_50, LTR3XX_MEASRATE_100, LTR3XX_MEASRATE_200,
 *  LTR3XX_MEASRATE_500, LTR3XX_MEASRATE_1000, or LTR3XX_MEASRATE_2000
 */
ltr329_measurerate_t Adafruit_LTR329::getMeasurementRate(void) {
  Adafruit_I2CRegister ratereg =
      Adafruit_I2CRegister(i2c_dev, LTR329_MEAS_RATE);
  Adafruit_I2CRegisterBits measbits =
      Adafruit_I2CRegisterBits(&ratereg, 3, 0); // # bits, bit_shift

  return (ltr329_measurerate_t)measbits.read();
}

/*!
 *  @brief  Checks if new data is available in data register
 *  @returns True on new data available
 */
bool Adafruit_LTR329::newDataAvailable(void) { return DataReadyBit->read(); }


/*!
 *  @brief  Read both 16-bit channels at once, and place data into argument pointers
 *  @returns True if data is valid (no over-run), false on bad data
 */
bool Adafruit_LTR329::readBothChannels(uint16_t &ch0, uint16_t &ch1) {
  Adafruit_I2CRegister datareg =
      Adafruit_I2CRegister(i2c_dev, LTR329_CH1DATA, 4, LSBFIRST);
  uint32_t both = datareg.read();
  ch0 = both >> 16;
  ch1 = both & 0xFFFF;

  return !DataInvalidBit->read();
}


/*!
 *  @brief  Read both 16-bit channels at once, and subract to get visible light
 *  @returns 16-bit visible data, or 0xFFFF if invalid data (overrun)
 */
uint16_t Adafruit_LTR329::readVisible(void) {
  uint16_t visible_plus_ir, infrared;
  
  if (! readBothChannels(visible_plus_ir, infrared)) {
    return 0xFFFF;
  }

  return visible_plus_ir - infrared;
}
