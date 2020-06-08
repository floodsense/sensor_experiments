/*
    Calculating the water depth using Throw in type Pressure sensor
    Programmed using C++
    Tested on Arduino Uno - ATmega328
    Sensor: Throw in type Pressure Sensor
    Connections
      Throw in type sensor:
        Red   -> Negative of Current to Voltage Converter
        Black -> Negative of 12-36V Power Supply
      Current to Voltage Converter:
        Positive -> Positive of 12-36V power Supply
        Negative -> Negative end of Sensor (Black)
        Black    -> GND
        Red      -> 5V VCC
        Blue     -> Analog Pin A1 of Arduino
 */


 void setup() {
   Serial.begin(9600);
 }

void loop() {

//Throw in type Pressure Sensor Snippet
     float thr_aread = analogRead(A0);
     Serial.print("Analog Reading of Throw in type:");
     Serial.println(thr_aread);
     float dataVoltage = 5.0 *thr_aread / 1023;
     //Serial.print("Voltage of throw in type:");
     //Serial.println(dataVoltage);
     //dataCurrent = dataVoltage / 120.0; //Sense Resistor:120ohm
     //Serial.print("Current:");
     //Serial.println(dataCurrent);
     //depth = (dataCurrent - CURRENT_INIT) * (RANGE/ DENSITY_WATER / 16.0); //Calculate depth from current readings

     float depth = 2604.16667* dataVoltage - 1250;
     if (depth < 0) depth = 0.0;

     //Serial print results
     Serial.print("depth:");
     Serial.print(depth);
     Serial.println("mm");
   delay(250);
}
