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
 *    @brief  Instantiates a new LTR303 class
 */
Adafruit_LTR303::Adafruit_LTR303(void) {}

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
  Adafruit_I2CRegister ctrlreg = Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
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
  Adafruit_I2CRegister alsctrl = Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
  Adafruit_I2CRegisterBits active_mode =
      Adafruit_I2CRegisterBits(&alsctrl, 1, 0); // # bits, bit_shift

  active_mode.write(en);
}

/*!
 *  @brief  Read the enabled-bit from the sensor
 *  @returns True if enabled
 */
bool Adafruit_LTR329::enabled(void) {
  Adafruit_I2CRegister alsctrl = Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
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
  Adafruit_I2CRegister ctrlreg = Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
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
  Adafruit_I2CRegister ctrlreg = Adafruit_I2CRegister(i2c_dev, LTR329_ALS_CTRL);
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
 *  @brief  Read both 16-bit channels at once, and place data into argument
 * pointers
 *  @param  ch0 Reference to uint16_t where visible+IR data will be stored
 *  @param  ch1 Reference to uint16_t where IR-only data will be stored
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
 *  @brief  Enables the interrupt output pin
 *  @param en Pass in 'true' to enable the output pin, false to tri-state
 */
void Adafruit_LTR303::enableInterrupt(bool en) {
  // we must be in non-active mode to change this register!
  bool curr_mode = enabled();
  enable(false);
  // now change the bit...

  Adafruit_I2CRegister intreg =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_INTERRUPT);
  Adafruit_I2CRegisterBits enbit =
      Adafruit_I2CRegisterBits(&intreg, 1, 1); // # bits, bit_shift

  enbit.write(en);

  // and reset the mode
  enable(curr_mode);
}

/*!
 *  @brief  The INT pin  has a polarity setting for active low or high!
 *  @param pol For active LOW set to 'false', for active HIGH set to 'true'
 */
void Adafruit_LTR303::setInterruptPolarity(bool pol) {
  // we must be in non-active mode to change this register!
  bool curr_mode = enabled();
  enable(false);
  // now change the bit...

  Adafruit_I2CRegister intreg =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_INTERRUPT);
  Adafruit_I2CRegisterBits polbit =
      Adafruit_I2CRegisterBits(&intreg, 1, 2); // # bits, bit_shift

  polbit.write(pol);

  // and reset the mode
  enable(curr_mode);
}

/*!
 *  @brief  The low threshold for ALS reading interrupts : values BELOW this
 * trigger an interrupt
 *  @param value This number is compared against the 'visible + IR' data
 * register
 */
void Adafruit_LTR303::setLowThreshold(uint16_t value) {
  Adafruit_I2CRegister thresh =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_THRESHLOW_LSB, 2, LSBFIRST);
  thresh.write(value);
}

/*!
 *  @brief  The low threshold for ALS reading interrupt getter
 *  @returns The value that ALS compares for low-value interrupts
 */
uint16_t Adafruit_LTR303::getLowThreshold(void) {
  Adafruit_I2CRegister thresh =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_THRESHLOW_LSB, 2, LSBFIRST);
  return thresh.read();
}

/*!
 *  @brief  The high threshold for ALS reading interrupts : values ABOVE this
 * trigger an interrupt
 *  @param value This number is compared against the 'visible + IR' data
 * register
 */
void Adafruit_LTR303::setHighThreshold(uint16_t value) {
  Adafruit_I2CRegister thresh =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_THRESHHIGH_LSB, 2, LSBFIRST);
  thresh.write(value);
}

/*!
 *  @brief  The high threshold for ALS reading interrupt getter
 *  @returns The value that ALS compares for high-value interrupts
 */
uint16_t Adafruit_LTR303::getHighThreshold(void) {
  Adafruit_I2CRegister thresh =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_THRESHHIGH_LSB, 2, LSBFIRST);
  return thresh.read();
}

/*!
 *  @brief  The default behavior is an interrupt on every value that is
 * under/over the threshold ranges. However, you're more likely to get spurious
 * IRQs, so we can set it to require "N counts in a row" before an IRQ.
 *  @param counts 1 count is IRQ for each reading, 2 counts means we need two
 * outside readings in a row, etc up to 16.
 */
void Adafruit_LTR303::setIntPersistance(uint8_t counts) {
  Adafruit_I2CRegister persistreg =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_INTPERSIST);
  Adafruit_I2CRegisterBits persist =
      Adafruit_I2CRegisterBits(&persistreg, 4, 0); // # bits, bit_shift

  persist.write(counts - 1);
}

/*!
 *  @brief Getter for the number of counts required to trigger IRQ
 *  @returns 1 count is IRQ for each reading, 2 counts means we need two outside
 * readings in a row, etc up to 16.
 */
uint8_t Adafruit_LTR303::getIntPersistance(void) {
  Adafruit_I2CRegister persistreg =
      Adafruit_I2CRegister(i2c_dev, LTR303_REG_INTPERSIST);
  Adafruit_I2CRegisterBits persist =
      Adafruit_I2CRegisterBits(&persistreg, 4, 0); // # bits, bit_shift
  return persist.read() + 1;
}
