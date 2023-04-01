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
     delay(100);
  Serial.println("\nI2C Scanner");

    Wire.beginTransmission(0x5C);
  Wire.write(0x01);
  Wire.write(0x01);
  Wire.endTransmission();

  // Wire.beginTransmission(0x5C);
  // Wire.write(0x01);
  // Wire.write(0x00);
  // Wire.endTransmission();

  Wire.beginTransmission(0x5C);
  Wire.write(0x02);
  Wire.write(0x00);
  // Wire.write(0x03);
  // Wire.write(15);
  Wire.endTransmission();

  Wire.beginTransmission(0x5C);
  Wire.write(0x03);
  Wire.write(0x00);
  // Wire.write(0x03);
  // Wire.write(15);
  Wire.endTransmission();
}


void loop() {
  const byte device_1 = 0x5C;
  const byte device_enable = 0x01;

  /// AFTER INIT


  
  Wire.beginTransmission(0x5C);
  Wire.write(0x02);
  Wire.write(1 + 8 + 64);
  // Wire.write(0x03);
  // Wire.write(15);
  Wire.endTransmission();


  Wire.beginTransmission(0x5C);
  Wire.write(0x03);
  Wire.write(2);
  // Wire.write(0x03);
  // Wire.write(15);
  Wire.endTransmission();

  // Wire.beginTransmission(0x5C);
  // Wire.write(0x02);
  // Wire.write(64);
  // // Wire.write(0x03);
  // // Wire.write(15);
  // Wire.endTransmission();

  //     Wire.beginTransmission(0x5C);
  // Wire.write(0x03);
  // Wire.write(0x01);
  // // Wire.write(0x03);
  // // Wire.write(15);
  // Wire.endTransmission();
  Wire.beginTransmission(0x5C);
  Wire.write(0x09);
  Wire.write(8);
  // Wire.write(0x03);
  // Wire.write(15);
  Wire.endTransmission();
  delay(1000);




  // Wire.beginTransmission(0x5C);
  // Wire.write(0x02);
  // Wire.write(0x02);
  // // Wire.write(0x03);
  // // Wire.write(15);
  // Wire.endTransmission();
  // delay(1000); 
  // Wire.beginTransmission(0x5C);
  // Wire.write(0x02);
  // Wire.write(0x04);
  // // Wire.write(0x03);
  // // Wire.write(15);
  // Wire.endTransmission();

  //   delay(1000); 
  // Wire.beginTransmission(0x5C);
  // Wire.write(0x02);
  // Wire.write(0x03);
  // // Wire.write(0x03);
  // // Wire.write(15);
  // Wire.endTransmission();

  // Wire.beginTransmission(0x5C);
  // Wire.write(0x01);
  // Wire.write(0x01);
  // Wire.endTransmission();

  // Wire.beginTransmission(0x59);
  // Wire.write(0x02);
  // Wire.write(0x02);
  // Wire.endTransmission();


  // delay(1000); 
  // Wire.beginTransmission(0x5A);
  // Wire.write(device_enable);
  // Wire.write(0x01);
  // Wire.endTransmission();


  // delay(1000); 
  // Wire.beginTransmission(0x5B);
  // Wire.write(device_enable);
  // Wire.write(0x01);
  // Wire.endTransmission();


  // delay(1000); 
  // Wire.beginTransmission(0x58);
  // Wire.write(device_enable);
  // Wire.write(0x00);
  // Wire.endTransmission();

  //   delay(1000); 
  // Wire.beginTransmission(0x59);
  // Wire.write(device_enable);
  // Wire.write(0x00);
  // Wire.endTransmission(); 

  //   delay(1000); 
  // Wire.beginTransmission(0x5A);
  // Wire.write(device_enable);
  // Wire.write(0x00);
  // Wire.endTransmission();

  //   delay(1000); 
  // Wire.beginTransmission(0x5B);
  // Wire.write(device_enable);
  // Wire.write(0x00);
  // Wire.endTransmission();

  delay(1000); 
}