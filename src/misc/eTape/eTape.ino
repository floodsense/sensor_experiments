/*
    Calculating the water depth using eTape after Calibration
    Programmed using C++
    Tested on Arduino Uno - ATmega328
    Sensor: eTape 24"
    Connections
    eTape:
    VCC(Red) -> 6 Volts DC
    GND(Black) -> GND
    Vout(White) -> Analog pin 1.
 */

//Calibration constants of the eTape
const float offset = 491
const float m = 12.5

 void setup() {
   Serial.begin(9600);
 }

void loop() {

 //Reading the analog readings of the eTape at Analog pin 1
 float etape_reading = analogRead(A1);
 Serial.print("eTape_arduino Analog reading:");
 Serial.println(etape_reading);

 //Convert the value to Voltage
 float reading = 5 * etape_reading/1024;
 Serial.print("eTape Voltage:");
 Serial.print(reading);
 Serial.println(" V");

 //Converting the Voltage reading to depth based on the linearity of the eTape
 float depth = (etape_reading - offset)/m;
 Serial.print("Depth calculated using eTape:");
 Serial.print(depth);
 Serial.println(" inches");

}
