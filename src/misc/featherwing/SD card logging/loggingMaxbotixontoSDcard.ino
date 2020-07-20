#include <SPI.h>
#include <SD.h>

// Pin definitions
#define cardSelect 10
#define readPin 11
#define triggerPin 12

long distance = 0;
long duration = 0;

File logfile;

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(13, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  digitalWrite(triggerPin, LOW);

  delay(3000);
  Serial.println("Setup Ready!");
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


void writeToSDCard() {

  // The CS pin on Feather m0 LoRa board is shared by the radio module and should be pulled HIGH
  // before using and set back to LOW for the radio to work.
  digitalWrite(8, HIGH);
  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");
    char filename[15];
    strcpy(filename, "/SENSOR00.TXT");

    //  for (uint8_t i = 0; i < 100; i++) {
    //    filename[7] = '0' + i / 10;
    //    filename[8] = '0' + i % 10;
    //    // create if does not exist, do not open existing, write, sync after write
    //    if (! SD.exists(filename)) {
    //      break;
    //    }
    //  }

    logfile = SD.open(filename, FILE_WRITE);

    if ( ! logfile ) {
      Serial.print("error opening ");
      Serial.println(filename);
    }

    logfile.print("Dist = "); logfile.println(distance);
    Serial.println("Writing to SD Card");
    Serial.print("Dist = "); Serial.println(distance);
    //flush after write but to note it takes extra power
    logfile.flush();
    logfile.close();
    digitalWrite(8, LOW);
  }
}

void loop() {

  // Read the sensor data
  read_sensor();

  //Write to SD Card
  writeToSDCard();

  delay(1000);
}

