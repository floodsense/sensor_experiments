/*
    Calculating the water depth using Ultrasonic Sensor - Model A02YYUW
    Programmed using C++
    Tested on Arduino Uno - ATmega328
    Sensor: A02YYUW
    Connections:
    VCC(Red)   -> 3.3 Volts DC
    GND(Black) -> GND
    RX(Green)  -> 2
    TX(REd)    -> 3
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial2(2,3); // RX, TX
unsigned char data[4]={};
float distance;

void setup()
{
 Serial.begin(9600);
 mySerial2.begin(9600);
}

void loop()
{
    do{
     for(int i=0;i<4;i++)
     {
       data[i]=mySerial2.read();
     }
  }while(mySerial2.read()==0xff);

  mySerial2.flush();

  if(data[0]==0xff)
    {
      int sum;
      sum=(data[0]+data[1]+data[2])&0x00FF;
      if(sum==data[3])
      {
        distance=(data[1]<<8)+data[2];
        if(distance>30)
          {
           Serial.print("distance=");
           Serial.print(distance/10);
           Serial.println("cm");
          }else
             {
               Serial.println("Below the lower limit");
             }
      }else Serial.println("ERROR");
     }
     delay(100);
}
