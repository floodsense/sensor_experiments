#include <Adafruit_SleepyDog.h>
#include <SdFat.h>
#include "RTClib.h"
String get_timestamp(void);
#define triggerPin 12
#define cardSelect 10
#define chipSelect 10
// File name
char name[] = "SERIAL.TXT";
int SD_ERROR = 0;
// file system object
SdFat sd;
SdFile file;
// create Serial stream
ArduinoOutStream cout(Serial);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int calibration_const_sec = 5;
RTC_PCF8523 rtc;
void setup_featherWing(void) {
  //RTC init
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
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}
String get_timestamp(void) {
  DateTime now = rtc.now();
  String custom_timestamp_calibrated = String(now.year(), DEC) + String('-') + String(now.month(), DEC) + String('-') + String(now.day(), DEC) + String(' ') + String(now.hour(), DEC) + String(':') + String(now.minute(), DEC) + String(':') + String(now.second(), DEC);
  return custom_timestamp_calibrated;
}
void writeToSDCard(String StringtobeWritten) {
  //add time stamp to every sd card write
  String timestamp = get_timestamp();
  StringtobeWritten = timestamp + String(',') + StringtobeWritten ;
  char buffchar[StringtobeWritten.length() + 1];
  StringtobeWritten.toCharArray(buffchar, StringtobeWritten.length() + 1);
  digitalWrite(8, HIGH);
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    // set the SD_ERROR flag high;
    SD_ERROR = 1;
    Serial.println("SD Initialization Failed.");
  }
  else
  {
    ofstream sdout(name, ios::out | ios::app);
    if (!sdout)
    {
      Serial.println("SD Card Open Failed.");
      SD_ERROR = 1;
    }
    else {
      sdout << buffchar << endl;
      // close the stream
      sdout.close();
      SD_ERROR = 0;
    }
  }
  digitalWrite(8, LOW);
}
void setup()
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  while (!Serial) {   /// Blocking......
  }
  Serial1.begin(9600);                  //TX and RX on feather (Serial1 is already setup no need to use SoftwareSerial)
  setup_featherWing();
  //read_sensor();  //1st read in serial mode is 12208(wrong)
  Serial.println("Setup Ready");
  Serial.println("******************************************");
  writeToSDCard("Setup Ready");
  digitalWrite(13, LOW);
  delay(3000);
}
uint16_t distance;
void read_sensor()
{
  char buffer_RTT[6] = {};
  digitalWrite(13, HIGH);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds.
  digitalWrite(triggerPin, LOW);
  delay(150);
  if (Serial1.available() > 0) {
    do {
      if (Serial1.read() == 'R') {
        for (int i = 0; i < 5; i++) {
          buffer_RTT[i] = Serial1.read();
        }
      }
    } while (buffer_RTT == NULL);
    distance = (buffer_RTT[0] - '0') * 1000 + (buffer_RTT[1] - '0') * 100 + (buffer_RTT[2] - '0') * 10 + (buffer_RTT[3] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
  digitalWrite(13, LOW);
}
void gotodeepsleepnow(unsigned int sleep_time) {
  int sleep_cycles = sleep_time / 8;
  Serial.println("About to sleep");
  for (int i = 0; i < sleep_cycles ; i++) {
    Watchdog.sleep(8000);
  }
  Serial.println("Im Awake!");
}
void loop()
{
  Serial.println("reading sensor....");
  read_sensor();
  writeToSDCard(String("distance: ") + String(distance) + String(" mm"));
  /////////////////////
  //gotodeepsleepnow(8);
  //Serial.begin(9600);
}
