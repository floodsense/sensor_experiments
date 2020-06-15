/*
Calculating the water depth using Ultrasonic Sensor - Model ULS (with Temperature correction and Checksum)
Programmed using C++
Tested on Arduino Uno - ATmega328
Sensor: ULS - Waterproof Ultrasonic Sensor
Connections:
Pin 1: VCC(Red)    -> 3.3 Volts DC
Pin 2: Yellow      -> 13 (SCK)
Pin 3: Blue        -> 10 (RX)
Pin 4: Green       -> 11 (TX)
Pin 5: GND(Black)  -> GND

 */

/*
Note: This model has some Nozzle Correction and can be checked by comparing
readings with ruler measurements
Example:
When the base (the side containing the LED) is installed at 46cm, the sensor reads 44cm.
When the Nozzle is installed at 40 cm, the sensor reads 44cm.
*/

#include <SoftwareSerial.h>                     //  Allows serial communication on other digital pins of the Arduino
SoftwareSerial mySerial(10, 11);                //  RX, TX

char col;
unsigned char buffer_RTT[6] = {};               // Buffer to store the sensor output frame
float Distance = 0;
float Temp = 0;
int Tflag = 0;

void setup() {
        Serial.begin(9600);                     // Enable the serial port and set the band rate to 9600 bps
        mySerial.begin(9600);
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(4);
  digitalWrite(LED_BUILTIN, HIGH);
  do{
    for (int j = 0; j <= 5; j++)
      {
      col = mySerial.read();
      buffer_RTT[j] = (char)col;
      }
        } while(mySerial.read() == 0xff);
   mySerial.flush();
   if(buffer_RTT[0]==0xff){
      int cor;
      cor=(buffer_RTT[0]+buffer_RTT[1]+buffer_RTT[2]+buffer_RTT[3]+buffer_RTT[4])&0x00FF;  //Checksum
            if(buffer_RTT[5]==cor)
                  {
                  Distance = (buffer_RTT[1] << 8) + buffer_RTT[2];
                  Tflag=  buffer_RTT[3]&0x80;
                  if(Tflag==0x80){
                  buffer_RTT[3]=buffer_RTT[3]^0x80;
                  }
                  Temp = (buffer_RTT[3] << 8) + buffer_RTT[4];
                  Temp = Temp/10;
                  }
                  else{
                    Distance = 0;
                    Temp = 0;
                     }
                 }
                Distance = Distance/10;

                Serial.print("Distance is: ");
                Serial.print(Distance);//Output distance unit cm, resolution 1mm
                Serial.println("cm");

      delay(100);

  }
