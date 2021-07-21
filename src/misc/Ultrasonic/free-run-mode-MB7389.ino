#include "softSerial.h"

softSerial softwareSerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);
#define triggerPin GPIO3

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, HIGH);
}

char endMarker = '\n\n';

void loop() {
  int distance;
  char serialbuffer[4];
  int index = 0;
  char rc;
  softwareSerial.flush();
  boolean newData = false;
  while (newData == false) {
    if (softwareSerial.available())
    {
      char rc = softwareSerial.read();
      if (rc == 'R')
      {
        while (index < 3)
        {
          if (softwareSerial.available())
          {
            serialbuffer[index] = softwareSerial.read();
            index++;
          }
        }
      }
      newData = true;                        
    }
  }
  if (newData){
    distance = (serialbuffer[0] - '0') * 1000 + (serialbuffer[1] - '0') * 100 + (serialbuffer[2] - '0') * 10 + (serialbuffer[3] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
}
