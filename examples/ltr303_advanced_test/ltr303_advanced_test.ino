/***************************************************
  This is an example for the LTR303 light sensor that reads both channels
  and demonstrates how to set gain, interrupts and check data validity

  Designed specifically to work with the LTR-303 light sensor from Adafruit
  ----> https://www.adafruit.com/product/5610

  These sensors use I2C to communicate, 2 pins are required to
  interface
 ****************************************************/

#include "Adafruit_LTR329_LTR303.h"

Adafruit_LTR303 ltr = Adafruit_LTR303();

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit LTR-303 advanced test");

  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setGain(LTR3XX_GAIN_96);
  Serial.print("Gain : ");
  switch (ltr.getGain()) {
    case LTR3XX_GAIN_1: Serial.println(1); break;
    case LTR3XX_GAIN_2: Serial.println(2); break;
    case LTR3XX_GAIN_4: Serial.println(4); break;
    case LTR3XX_GAIN_8: Serial.println(8); break;
    case LTR3XX_GAIN_48: Serial.println(48); break;
    case LTR3XX_GAIN_96: Serial.println(96); break;
  }

  ltr.setIntegrationTime(LTR3XX_INTEGTIME_100);
  Serial.print("Integration Time (ms): ");
  switch (ltr.getIntegrationTime()) {
    case LTR3XX_INTEGTIME_50: Serial.println(50); break;
    case LTR3XX_INTEGTIME_100: Serial.println(100); break;
    case LTR3XX_INTEGTIME_150: Serial.println(150); break;
    case LTR3XX_INTEGTIME_200: Serial.println(200); break;
    case LTR3XX_INTEGTIME_250: Serial.println(250); break;
    case LTR3XX_INTEGTIME_300: Serial.println(300); break;
    case LTR3XX_INTEGTIME_350: Serial.println(350); break;
    case LTR3XX_INTEGTIME_400: Serial.println(400); break;
  }

  ltr.setMeasurementRate(LTR3XX_MEASRATE_200);
  Serial.print("Measurement Rate (ms): ");
  switch (ltr.getMeasurementRate()) {
    case LTR3XX_MEASRATE_50: Serial.println(50); break;
    case LTR3XX_MEASRATE_100: Serial.println(100); break;
    case LTR3XX_MEASRATE_200: Serial.println(200); break;
    case LTR3XX_MEASRATE_500: Serial.println(500); break;
    case LTR3XX_MEASRATE_1000: Serial.println(1000); break;
    case LTR3XX_MEASRATE_2000: Serial.println(2000); break;
  }

  // The LTR-303 has interrupt output support, we can enable the pin output!
  ltr.enableInterrupt(true);
  // The INT pin also has a polarity setting. For active LOW set to 'false',
  // for active HIGH set to 'true'
  ltr.setInterruptPolarity(false);

  // Then set the low threshold (values BELOW this trigger an interrupt)
  ltr.setLowThreshold(2000);
  // and set the high threshold (values ABOVE this trigger an interrupt)
  ltr.setHighThreshold(30000);
  Serial.print("Thresholds: ");  Serial.print(ltr.getLowThreshold());
  Serial.print(" & ");  Serial.println(ltr.getHighThreshold());

  // Finally, default is an interrupt on every value that is under/over the
  // threshold ranges. However, you're more likely to get spurious IRQs, so
  // we can set it to require "N counts in a row" before an IRQ. 1 count is
  // IRQ for each reading, 2 count means we need two outside readings in a row, etc
  // up to 16.
  ltr.setIntPersistance(4);
  Serial.print("Consecutive counts for IRQ: ");  Serial.println(ltr.getIntPersistance());
}

void loop() {
  bool valid;
  uint16_t visible_plus_ir, infrared;

  if (ltr.newDataAvailable()) {
    valid = ltr.readBothChannels(visible_plus_ir, infrared);
    if (valid) {
      Serial.print("CH0 Visible + IR: ");
      Serial.print(visible_plus_ir);
      Serial.print("\t\tCH1 Infrared: ");
      Serial.println(infrared);
    }
  }

  delay(100);
}
