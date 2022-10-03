/*!
 *  @file Adafruit_LTR329_LTR303.h
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
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_LTR329_LTR303_H
#define _ADAFRUIT_LTR329_LTR303_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Wire.h>

#define LTR329_I2CADDR_DEFAULT 0x29 ///< I2C address
#define LTR329_PART_ID 0x86         ///< Part id/revision register
#define LTR329_MANU_ID 0x87         ///< Manufacturer ID register
#define LTR329_ALS_CTRL 0x80        ///< ALS control register
#define LTR329_STATUS 0x8C          ///< Status register
#define LTR329_CH1DATA 0x88         ///< Data for channel 1 (read all 4 bytes!)
#define LTR329_MEAS_RATE 0x85       ///< Integration time and data rate

// These registers on LTR-303 only!
#define LTR303_REG_INTERRUPT 0x8F ///< Register to enable/configure int output
#define LTR303_REG_THRESHHIGH_LSB 0x97 ///< ALS 'high' threshold limit
#define LTR303_REG_THRESHLOW_LSB 0x99  ///< ALS 'low' threshold limit
#define LTR303_REG_INTPERSIST 0x9E ///< Register for setting the IRQ persistance

/*!    @brief  Sensor gain for ALS  */
typedef enum {
  LTR3XX_GAIN_1 = 0,
  LTR3XX_GAIN_2 = 1,
  LTR3XX_GAIN_4 = 2,
  LTR3XX_GAIN_8 = 3,
  // 4 & 5 unused!
  LTR3XX_GAIN_48 = 6,
  LTR3XX_GAIN_96 = 7,
} ltr329_gain_t;

/*!    @brief Integration times, in milliseconds */
typedef enum {
  LTR3XX_INTEGTIME_100,
  LTR3XX_INTEGTIME_50,
  LTR3XX_INTEGTIME_200,
  LTR3XX_INTEGTIME_400,
  LTR3XX_INTEGTIME_150,
  LTR3XX_INTEGTIME_250,
  LTR3XX_INTEGTIME_300,
  LTR3XX_INTEGTIME_350,
} ltr329_integrationtime_t;

/*!    @brief Measurement rates, in milliseconds */
typedef enum {
  LTR3XX_MEASRATE_50,
  LTR3XX_MEASRATE_100,
  LTR3XX_MEASRATE_200,
  LTR3XX_MEASRATE_500,
  LTR3XX_MEASRATE_1000,
  LTR3XX_MEASRATE_2000,
} ltr329_measurerate_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            LTR329 Light Sensor
 */
class Adafruit_LTR329 {
public:
  Adafruit_LTR329();
  bool begin(TwoWire *theWire = &Wire);
  bool reset(void);

  void enable(bool en);
  bool enabled(void);

  void setGain(ltr329_gain_t gain);
  ltr329_gain_t getGain(void);

  void setIntegrationTime(ltr329_integrationtime_t inttime);
  ltr329_integrationtime_t getIntegrationTime(void);

  void setMeasurementRate(ltr329_measurerate_t rate);
  ltr329_measurerate_t getMeasurementRate(void);

  bool newDataAvailable(void);
  bool readBothChannels(uint16_t &ch0, uint16_t &ch1);

protected:
  Adafruit_I2CDevice *i2c_dev = NULL; ///< The underlying I2C interface

private:
  Adafruit_I2CRegister *StatusReg = NULL;
  Adafruit_I2CRegisterBits *DataReadyBit = NULL;
  Adafruit_I2CRegisterBits *DataInvalidBit = NULL;
};

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            LTR303 Light Sensor
 */
class Adafruit_LTR303 : public Adafruit_LTR329 {
public:
  Adafruit_LTR303();

  void enableInterrupt(bool en);
  void setInterruptPolarity(bool pol);

  void setLowThreshold(uint16_t value);
  uint16_t getLowThreshold(void);

  void setHighThreshold(uint16_t value);
  uint16_t getHighThreshold(void);

  void setIntPersistance(uint8_t counts);
  uint8_t getIntPersistance(void);
};

#endif
