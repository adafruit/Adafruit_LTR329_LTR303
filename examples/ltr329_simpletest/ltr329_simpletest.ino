/*************************************************** 
  This is an example for the LTR329 light sensor that 
  gets you started fast by setting the default gain/rates
  and only displays visible light

  Designed specifically to work with the LTR-329 light sensor from Adafruit
  ----> https://www.adafruit.com/product/5591

  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/
 
#include "Adafruit_LTR329_LTR303.h"

Adafruit_LTR329 ltr = Adafruit_LTR329();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // wait for serial port to open
  }
  Serial.println("Adafruit LTR-329 simple test");

  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  // Set gain of 1 (see advanced demo for all options!
  ltr.setGain(LTR3XX_GAIN_1);
  // Set integration time of 50ms (see advanced demo for all options!
  ltr.setIntegrationTime(LTR3XX_INTEGTIME_50);
  // Set measurement rate of 50ms (see advanced demo for all options!
  ltr.setMeasurementRate(LTR3XX_MEASRATE_50);
}

void loop() {
  uint16_t light = ltr.readVisible();
  Serial.print("Light: ");
  Serial.println(light);

  // wait 50ms since new data will be ready then!
  delay(50);
}
