#include <Arduino.h>
#include <LSM6DSOSensor.h>

HardwareTimer timer(TIM1);
bool ledOn = false;

#define DEV_I2C Wire
#define SerialPort Serial
LSM6DSOSensor AccGyr(&DEV_I2C);

int trial = 0;

void PoolSensorValues() {
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);

    // Read accelerometer and gyroscope.
    int32_t accelerometer[3];
    int32_t gyroscope[3];
    AccGyr.Get_X_Axes(accelerometer);
    AccGyr.Get_G_Axes(gyroscope);


    //SerialPort.print(" | Acc[mg]: ");
    //SerialPort.print(accelerometer[0]);
    //SerialPort.print(" ");
    //SerialPort.print(accelerometer[1]);
    //SerialPort.print(" ");
    //SerialPort.print(accelerometer[2]);


    double var = sqrt(gyroscope[0] * gyroscope[0] + gyroscope[1] * gyroscope[1] + gyroscope[2] * gyroscope[2]);
    if(var > 30000){
      trial++;
    }
    if(trial > 5) {
      SerialPort.println("SHAKE");
      trial = 0;
      }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    DEV_I2C.begin();
    
    SerialPort.begin(9600);
    
    AccGyr.begin();
    AccGyr.Enable_X();
    AccGyr.Enable_G();
    
    // Configure timer
    timer.setPrescaleFactor(2564); // Set prescaler to 2564 => timer frequency = 168MHz/2564 = 65522 Hz (from prediv'd by 1 clocksource of 168 MHz)
    //timer.setOverflow(32761); // Set overflow to 32761 => timer frequency = 65522 Hz / 32761 = 2 Hz
    timer.setOverflow(5000);
    timer.attachInterrupt(PoolSensorValues);
    timer.refresh(); // Make register changes take effect
    timer.resume(); // Start


}



void loop() {
}