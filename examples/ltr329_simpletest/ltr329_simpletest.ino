/***************************************************
  This is an example for the LTR329 light sensor that reads both channels
  and demonstrates how to set gain and check data validity

  Designed specifically to work with the LTR-329 light sensor from Adafruit
  ----> https://www.adafruit.com/product/5591

  These sensors use I2C to communicate, 2 pins are required to
  interface
 ****************************************************/

#include "Adafruit_LTR329_LTR303.h"

Adafruit_LTR329 ltr = Adafruit_LTR329();

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit LTR-329 advanced test");

  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setGain(LTR3XX_GAIN_2);
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
