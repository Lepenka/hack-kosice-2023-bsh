// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------

#include <Wire.h>

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

void setup() {
  WIRE.begin();

  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Scanner");
}


void loop() {
  const byte device_1 = 0x5C;
  const byte device_enable = 0x01;

  Wire.beginTransmission(device_1);
  Wire.write(device_enable);
  Wire.write(0x01);
  Wire.endTransmission();

  delay(1000);           // wait 5 seconds for next scan
}