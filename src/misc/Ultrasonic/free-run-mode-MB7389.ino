#include "softSerial.h"

softSerial softwareSerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);
#define triggerPin GPIO3

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, HIGH);
}

char endMarker = '\n';

void loop() {
  char serialbuffer[6] = {0};
  int indx = 0;
  char rc;
  boolean newData = false;
  while (softwareSerial.available() > 0 && newData == false) {
    rc = softwareSerial.read();
    if (rc != endMarker) {
      serialbuffer[indx] = rc;
      Serial.println(rc);
      indx++;
    }
    else {
      newData = true;
    }
  }
  //Serial.println(newData);
  if (newData){
    int distance = (serialbuffer[1] - '0') * 1000 + (serialbuffer[2] - '0') * 100 + (serialbuffer[3] - '0') * 10 + (serialbuffer[4] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
  newData = false;
}
