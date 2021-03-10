#define triggerPin 11

char c;
char buffer_RTT[5] = {};
int distance;

void setup()
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  Serial.begin(9600);
  while (!Serial) {
    //wait till Serial is setup
  }
  Serial1.begin(9600);                  //TX and RX on feather (Serial1 is already setup no need to use SoftwareSerial)
  Serial.println("Sensor is ready!");
  read_sensor();  //1st read is 0
}

void read_sensor()
{
  int i = 0;
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds.
  digitalWrite(triggerPin, LOW);
  while (Serial1.available()){
    c = Serial1.read();
    if (c != 'R' && c != '\0') {               // Serial output is of the format R*** (The serial data format is 9600 baud, 8 data bits, no parity, with one stop bit (9600-8-N-1))
      buffer_RTT[i - 1] = c;
    }
    i++;
  } 
  Serial1.flush();
  distance = atoi(buffer_RTT);
  delay(150);
}

void loop()
{
  read_sensor();
  Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  delay(100);
}
