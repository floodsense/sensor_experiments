/*  
*   This example code demonstrates how to log the read sensor values onto SD Card.
*   The function "writeToSDCard" writes to SD card and takes in a single string as an 
*   argument for simplicity.
*   More on string operations: 
*   https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/
*/

#include <SPI.h>
#include <SD.h>

// Pin definitions
#define cardSelect 10
#define readPin 11
#define triggerPin 12

long distance = 0;
long duration = 0;
char filename[15];

File logfile;

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  digitalWrite(triggerPin, LOW);
  digitalWrite(8, HIGH);
  Serial.print("Initializing SD card...");
  if (!SD.begin(cardSelect)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");
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
    } else {
      logfile.flush();
      logfile.close();
      Serial.print("Successfully created log file: "); Serial.println(filename);
    }
  }
  digitalWrite(8, LOW);
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


void writeToSDCard(String string_tobe_written) {

  // The CS pin on Feather m0 LoRa board is shared by the radio module and should be pulled HIGH
  // before using and set back to LOW for the radio to work.
  digitalWrite(8, HIGH);
  digitalWrite(13,HIGH);
  Serial.print("Initializing SD card...");
  if (!SD.begin(cardSelect)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");
    logfile = SD.open(filename, FILE_WRITE);
    if ( ! logfile ) {
      Serial.print("Error opening ");
      Serial.println(filename);
    } else {
      logfile.println(string_tobe_written);
      Serial.println("Done writing to SD Card");
      //flush after write but to note it takes extra power
      logfile.flush();
      logfile.close();
    }
  }
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
}

void loop() {

  // Read the sensor data
  read_sensor();
  print_data();
  
  //The String to be written must be a single string and can be concatenated using different arduino string declerations
  String string_tobe_written = String(distance);
  string_tobe_written = String ("The distance is: " + string_tobe_written);

  //Write to SD Card
  writeToSDCard(string_tobe_written);

  delay(1000);
}
