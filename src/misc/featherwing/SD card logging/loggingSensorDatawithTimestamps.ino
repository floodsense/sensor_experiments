#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

// Pin definitions
#define cardSelect 10
#define readPin 11
#define triggerPin 12

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
File logfile;
long distance = 0;
long duration = 0;
int calibration_const_sec = 5;

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

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

    logfile = SD.open(filename, FILE_WRITE);

    if ( ! logfile ) {
      Serial.print("error opening ");
      Serial.println(filename);
    }

    DateTime now = rtc.now();
    DateTime calib_time (now + TimeSpan(0, 0, 0, calibration_const_sec));
    String custom_timestamp_calibrated = String(calib_time.year(), DEC) + String('-') + String(calib_time.month(), DEC) + String('-') + String(calib_time.day(), DEC) + String('_') + String(calib_time.hour(), DEC) + String(':') + String(calib_time.minute(), DEC) + String(':') + String(calib_time.second(), DEC);
    logfile.print(custom_timestamp_calibrated); logfile.print(" Dist = "); logfile.println(distance);
    Serial.println("Writing to SD Card");
    Serial.print(custom_timestamp_calibrated); Serial.print(" Dist = "); Serial.println(distance);
    //flush after write but to note it takes extra power
    logfile.flush();
    logfile.close();
    digitalWrite(8, LOW);
  }
}

void loop() {

  // Read the sensor data
  read_sensor();

  //Write to SD Card with timestamps
  writeToSDCard();

  delay(1000);
}
