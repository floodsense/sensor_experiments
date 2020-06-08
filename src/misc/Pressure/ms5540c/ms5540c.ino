/*
  Programmed using C++
  Tested on Arduino Uno - ATmega328
  Sensor: ms5540c
  Connections:
    VCC ----------------> 3.3v
    GND ----------------> GND
    DIN (MOSI) ---------> pin (11)
    DOUT (MISO) --------> pin (12)
    SCLK ---------------> pin (13)
    MCLK ---------------> pin (10)
 */


#include "ms5540c.h"

void setup() {
  Serial.begin(9600);

}
void loop() {
  // put your main code here, to run repeatedly:
  static MS5540C snr;

  snr.calibration();
  snr.measure();

  Serial.print("Real pressure with compensation (mbar)");
  Serial.println(snr.pcompreal);
  //Serial.println(snr.pcomphg);
  delay(1000);

}
