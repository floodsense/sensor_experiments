/*************************************************************************************
  Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
  This sketch uses the RTClib to set the RTC date and time.
  The date and time is that of the instant when arduino's sketch upload has begun.
  Custom timestamp is of the format -> [ year-month-day_hours:minutes:senconds ]
  Usually there is a small delay caused by the verify and upload time of the arduino ide
  and to get the exact real clock time the overhead can be added.
  
 ************************************************************************************/
#include "RTClib.h"

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

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
}

int calibration_const_sec = 5;
void loop () {
  
    DateTime now = rtc.now();

    Serial.print(" Custom timestamp: ");
    String custom_timestamp = String(now.year(), DEC) + String('-') + String(now.month(), DEC)+ String('-') + String(now.day(), DEC)+ String('_') + String(now.hour(), DEC)+ String(':') + String(now.minute(), DEC)+ String(':')+ String(now.second(), DEC);
    Serial.println(custom_timestamp);
    
    Serial.print(" Custom timestamp(with calibration): ");
    DateTime calib_time (now + TimeSpan(0,0,0,calibration_const_sec));
    String custom_timestamp_calibrated = String(calib_time.year(), DEC) + String('-') + String(calib_time.month(), DEC)+ String('-') + String(calib_time.day(), DEC)+ String('_') + String(calib_time.hour(), DEC)+ String(':') + String(calib_time.minute(), DEC)+ String(':')+ String(calib_time.second(),DEC);
    Serial.println(custom_timestamp_calibrated);

    Serial.println();
    
    delay(3000);
}
