// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------

#include <Wire.h>

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

const byte FIRST_ROW = 0x58;
const byte SECOND_ROW = 0x59;
const byte THRID_ROW = 0x5A;
const byte FOURTH_ROW = 0x5B;
const byte ALL_ROWS = 0x5C;

const byte DEVICE_ENABLE = 0x01;
const byte CHANNEL_ENABLE_LOW = 0x02;
const byte CHANNEL_ENABLE_HIGH = 0x03;

void reset_all() {
  byte payload = 0b10000000;
  
  Wire.beginTransmission(ALL_ROWS);
  Wire.write(DEVICE_ENABLE);
  Wire.write(payload);
  Wire.endTransmission();
}


void enable_channel(size_t channel, bool enable) {
  int shift = channel % 8;
  int payload = 0x01 << shift;

  byte target_register = channel >= 8 ? CHANNEL_ENABLE_HIGH : CHANNEL_ENABLE_LOW;
  byte other_register = channel < 8 ? CHANNEL_ENABLE_HIGH : CHANNEL_ENABLE_LOW;

  Wire.beginTransmission(FIRST_ROW);
  Wire.write(target_register);
  Wire.write(payload);
  Wire.endTransmission();

  Wire.beginTransmission(FIRST_ROW);
  Wire.write(other_register);
  Wire.write(0x00);
  Wire.endTransmission();
}

void setup() {
  WIRE.begin();

  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Scanner");

  ///reset_all();
}


void loop() {
  const byte device_1 = 0x58;
  const byte device_enable = 0x01;

  int channel = 0;
  //enable_channel(0, true);
  enable_channel(2, true);

  delay(1000);           // wait 5 seconds for next scan
}