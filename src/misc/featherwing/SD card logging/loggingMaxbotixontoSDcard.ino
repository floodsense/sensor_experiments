#include <SPI.h>
#include <SD.h>

// Pin definitions
#define cardSelect 10
#define readPin 11
#define triggerPin 12

long distance = 0;
long duration = 0;
File logfile;

// blink out an error code
void error(uint8_t errno) {
  while (1) {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++) {
      delay(200);
    }
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

   digitalWrite(8, HIGH);
  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

   char filename[15];
  strcpy(filename, "/SENSOR00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i / 10;
    filename[8] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  logfile = SD.open(filename, FILE_WRITE);
  if ( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to ");
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  digitalWrite(triggerPin, LOW);

  delay(3000);
  Serial.println("Setup Ready!");
  digitalWrite(8, LOW);
}
void read_sensor() {

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds.
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  distance = duration;
  delay(150);
}

void print_data() {

  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
}

void loop() {

  read_sensor();
  logfile.print("Dist = "); logfile.println(distance);
  Serial.print("Dist = "); Serial.println(distance);

  delay(1000);
}

