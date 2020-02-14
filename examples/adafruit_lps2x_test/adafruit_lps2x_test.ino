// Basic demo for pressure readings from Adafruit LPS25
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>

Adafruit_LPS2X lps;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit LPS2X test!");

  // Try to initialize!
  if (! lps.begin()) {
    Serial.println("Failed to find LPS2X chip");
    while (1) { delay(10); }
  }
  Serial.println("LPS2X Found!");

  //lps.setDataRate(MSA301_DATARATE_31_25_HZ);
  //Serial.print("Data rate set to: ");
  //switch (lps.getDataRate()) {
  //  case MSA301_DATARATE_1_HZ: Serial.println("1 Hz"); break;
  //  case MSA301_DATARATE_1_95_HZ: Serial.println("1.95 Hz"); break;
  //  case MSA301_DATARATE_3_9_HZ: Serial.println("3.9 Hz"); break;

  //}
}

void loop() {
  lps.read();      // get X Y and Z data at once

  delay(100);
}
