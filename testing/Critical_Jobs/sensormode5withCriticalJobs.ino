/*

  Sensor Mode 5: Relaxed LoRa setting + median{5 sensor readings}
  SD card logging with timestamped events

*/
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>
#include "RTClib.h"

#define VBATPIN A7
#define cardSelect 10
#define readPin 11
#define triggerPin 12

unsigned char mydata[5];
uint16_t distance = 0;
uint16_t duration = 0;
float measuredvbat;
uint16_t median_arr[5];
uint16_t batlevel;
byte lowbyte, highbyte, lowbat, highbat;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int calibration_const_sec = 5;
char filename[15];
int sensorMode = 5;
const unsigned TX_INTERVAL = 240;
bool TX_COMPLETED = false;

RTC_PCF8523 rtc;
File logfile;

// This EUI must be in little-endian format
static const u1_t PROGMEM APPEUI[8] = {  };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}
// This should also be in little endian format
static const u1_t PROGMEM DEVEUI[8] =  {  } ;
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}
// This key should be in big endian format
static const u1_t PROGMEM APPKEY[16] = {  };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

static osjob_t sendjob;

// Pin mapping
//
// Adafruit BSPs are not consistent -- m0 express defs ARDUINO_SAMD_FEATHER_M0,
// m0 defs ADAFRUIT_FEATHER_M0
//
#if defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
// Pin mapping for Adafruit Feather M0 LoRa, etc.
const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {3, 6, LMIC_UNUSED_PIN},
  .rxtx_rx_active = 0,
  .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
  .spi_freq = 8000000,
};
#elif defined(ARDUINO_AVR_FEATHER32U4)
// Pin mapping for Adafruit Feather 32u4 LoRa, etc.
// Just like Feather M0 LoRa, but uses SPI at 1MHz; and that's only
// because MCCI doesn't have a test board; probably higher frequencies
// will work.
const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {7, 6, LMIC_UNUSED_PIN},
  .rxtx_rx_active = 0,
  .rssi_cal = 8,              // LBT cal for the Adafruit Feather 32U4 LoRa, in dB
  .spi_freq = 1000000,
};
#elif defined(ARDUINO_CATENA_4551)
// Pin mapping for Murata module / Catena 4551
const lmic_pinmap lmic_pins = {
  .nss = 7,
  .rxtx = 29,
  .rst = 8,
  .dio = {
    25,    // DIO0 (IRQ) is D25
    26,    // DIO1 is D26
    27,    // DIO2 is D27
  },
  .rxtx_rx_active = 1,
  .rssi_cal = 10,
  .spi_freq = 8000000     // 8MHz
};
#else
# error "Unknown target"
#endif

void writeToSDCard(String string_tobe_written) {

  // The CS pin on Feather m0 LoRa board is shared by the radio module and should be pulled HIGH
  // before using and set back to LOW for the radio to work.
  digitalWrite(8, HIGH);
  digitalWrite(13, HIGH);
  //Serial.print("Initializing SD card...");
  if (!SD.begin(cardSelect)) {
    Serial.println("initialization failed!");
  } else {
    //Serial.println("initialization done.");
    logfile = SD.open(filename, FILE_WRITE);
    if ( ! logfile ) {
      Serial.print("Error opening ");
      Serial.println(filename);
    } else {
      DateTime now = rtc.now();
      DateTime calib_time (now + TimeSpan(0, 0, 0, calibration_const_sec));
      String custom_timestamp_calibrated = String(calib_time.year(), DEC) + String('-') + String(calib_time.month(), DEC) + String('-') + String(calib_time.day(), DEC) + String('_') + String(calib_time.hour(), DEC) + String(':') + String(calib_time.minute(), DEC) + String(':') + String(calib_time.second(), DEC);
      string_tobe_written = String(custom_timestamp_calibrated) + String(',') + String(string_tobe_written);
      //Serial.println(custom_timestamp_calibrated);
      logfile.println(string_tobe_written);
      //Serial.println("Done writing to SD Card");
      //flush after write but to note it takes extra power
      logfile.flush();
      logfile.close();
      SD.end();
    }
  }
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
}

void printHex2(unsigned v) {
  v &= 0xff;
  if (v < 16)
    Serial.print('0');
  Serial.print(v, HEX);
}

void swap(uint16_t  *p, uint16_t  *q) {
  int t;

  t = *p;
  *p = *q;
  *q = t;
}

void sort(uint16_t a[]) {
  int i, j, temp, n = 5;

  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - i - 1; j++) {
      if (a[j] > a[j + 1])
        swap(&a[j], &a[j + 1]);
    }
  }
}

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  String event_ev = String(os_getTime()) + String (": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      event_ev = String(event_ev + "EV_SCAN_TIMEOUT");
      writeToSDCard(event_ev);
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      event_ev = String(event_ev + "EV_BEACON_FOUND");
      writeToSDCard(event_ev);
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      event_ev = String(event_ev + "EV_BEACON_MISSED");
      writeToSDCard(event_ev);
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      event_ev = String(event_ev + "EV_BEACON_TRACKED" );
      writeToSDCard(event_ev);
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      event_ev = String(event_ev + "EV_JOINING");
      writeToSDCard(event_ev);
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      event_ev = String(event_ev + "EV_JOINED" );
      writeToSDCard(event_ev);
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
        Serial.print("netid: ");
        Serial.println(netid, DEC);
        Serial.print("devaddr: ");
        Serial.println(devaddr, HEX);
        Serial.print("AppSKey: ");
        for (size_t i = 0; i < sizeof(artKey); ++i) {
          if (i != 0)
            Serial.print("-");
          printHex2(artKey[i]);
        }
        Serial.println("");
        Serial.print("NwkSKey: ");
        for (size_t i = 0; i < sizeof(nwkKey); ++i) {
          if (i != 0)
            Serial.print("-");
          printHex2(nwkKey[i]);
        }
        Serial.println();
      }
      // Disable link check validation (automatically enabled
      // during join, but because slow data rates change max TX
      // size, we don't use it in this example.
      LMIC_setLinkCheckMode(0);
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
    */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      event_ev = String(event_ev + "EV_JOIN_FAILED");
      writeToSDCard(event_ev);
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      event_ev = String(event_ev + "EV_REJOIN_FAILED" );
      writeToSDCard(event_ev);
      break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      Serial.println("");
      Serial.println("");
      event_ev = String(event_ev + "EV_TXCOMPLETE (includes waiting for RX windows)");
      writeToSDCard(event_ev);
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      event_ev = String("Received ack");
      writeToSDCard(event_ev);
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
        event_ev = String("Received ") + String(LMIC.dataLen) + String(" bytes of payload");
        writeToSDCard(event_ev);
      }
      TX_COMPLETED = true;
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      event_ev = String(event_ev + "EV_LOST_TSYNC" );
      writeToSDCard(event_ev);
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      event_ev = String(event_ev + "EV_RESET");
      writeToSDCard(event_ev);
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      event_ev = String(event_ev + "EV_RXCOMPLETE");
      writeToSDCard(event_ev);
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      event_ev = String(event_ev + "EV_LINK_DEAD");
      writeToSDCard(event_ev);
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      event_ev = String(event_ev + "EV_LINK_ALIVE");
      writeToSDCard(event_ev);
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      event_ev = String(event_ev + "EV_TXSTART");
      writeToSDCard(event_ev);
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      event_ev = String(event_ev + "EV_TXCANCELED");
      writeToSDCard(event_ev);
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      event_ev = String(event_ev + "EV_JOIN_TXCOMPLETE: no JoinAccept" );
      writeToSDCard(event_ev);
      break;
    default:
      Serial.print(F("Unknown event: "));
      event_ev = String(event_ev + "Unknown event");
      Serial.println((unsigned) ev);
      writeToSDCard(event_ev);
      break;
  }
}


void prepare_packet(void) {
  // dedicated sensing period + median of 5 readings
  String packet_string = String("Preparing a packet at: ") + String(os_getTime());
  writeToSDCard(packet_string);
  Serial.print("Sensor Mode: ");
  Serial.println(sensorMode);
  packet_string = "Sensor Mode: ";
  writeToSDCard(packet_string);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  median_arr[0] = duration;
  packet_string = String("Reading 1: ") + String(median_arr[0]);
  writeToSDCard(packet_string);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  median_arr[1] = duration;
  packet_string = String("Reading 2: ") + String(median_arr[1]);
  writeToSDCard(packet_string);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  median_arr[2] = duration;
  packet_string = String("Reading 3: ") + String(median_arr[2]);
  writeToSDCard(packet_string);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  median_arr[3] = duration;
  packet_string = String("Reading 4: ") + String(median_arr[3]);
  writeToSDCard(packet_string);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  median_arr[4] = duration;
  packet_string = String("Reading 5: ") + String(median_arr[4]);
  writeToSDCard(packet_string);
  // Median of the 5 readings
  int n = 5;
  sort(median_arr);
  n = (n + 1) / 2 - 1;    // -1 as array indexing in C starts from 0
  distance = median_arr[n];
  packet_string = String("Median of 5 readings is: ") + String(median_arr[n]);
  writeToSDCard(packet_string);
  lowbyte = lowByte(distance);
  highbyte = highByte(distance);
  mydata[0] = (unsigned char)lowbyte;
  mydata[1] = (unsigned char)highbyte;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
  // Battery
  measuredvbat = analogRead(VBATPIN); //Float
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  measuredvbat *= 1000; //make it milli volts to transmit
  Serial.print("VBat in mVolts: " ); Serial.println(measuredvbat);
  batlevel = measuredvbat; //Payload
  packet_string = String("battery level is: ") + String(batlevel);
  writeToSDCard(packet_string);
  lowbat = lowByte(batlevel);
  highbat = highByte(batlevel);
  mydata[2] = (unsigned char)lowbat; //we're unsigned
  mydata[3] = (unsigned char)highbat;
}

void gotosleepnow(void) {
  int sleep_cycles = TX_INTERVAL / 8;
  Serial.println("About to sleep");
  for (int i = 0; i < sleep_cycles ; i++) {
    Watchdog.sleep(8000);
  }
  Serial.println("Im Awake!");
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
    String do_sendstr1 = "OP_TXRXPEND, not sending";
    writeToSDCard(do_sendstr1);
  } else {
    // Prepare upstream data transmission at the next possible time.
    mydata[4] = (unsigned char)sensorMode;
    int lmic_tx_retVAL = LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    String do_sendstr;
    if (lmic_tx_retVAL == 0){
      Serial.println(F("Packet queued and lmic_tx_retVAL is 0."));
      do_sendstr = "Packet queued";
      writeToSDCard(do_sendstr);
    } else {
      do_sendstr = String ("Something is wrong ") + String("Error number: ") +String(lmic_tx_retVAL); 
      Serial.println(do_sendstr);
      writeToSDCard(do_sendstr);
    }
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  // Need to fix failure initializations.
  delay(5000);
  // while (! Serial);   // if battery is on this is blocking!!!
  Serial.begin(9600);
  Serial.println(F("Starting"));
#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

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
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  //SD Card init
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
  delay(1000);

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  //LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  // Disable link check validation (automatically enabled)
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_selectSubBand(1);

  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.println("Setup Ready!");
  String setupstr = "Setup Ready!";
  writeToSDCard(setupstr);

  // Start job (sending automatically starts OTAA too)
  Serial.println("Starting first job in setup");
  setupstr = "Starting first job in setup";
  writeToSDCard(setupstr);
  prepare_packet();
  do_send(&sendjob);

}

void loop() {
  os_runloop_once();
  if ( !os_queryTimeCriticalJobs(ms2osticksRound((TX_INTERVAL * 1000) - 1000 )) && TX_COMPLETED == true) {
    TX_COMPLETED = false;
    // This means the previous TX is complete and also no Critical Jobs pending in LMIC
    Serial.println("About to go to deep sleep and no critical jobs");
    String str_loop = "About to go to deep sleep and no critical jobs";
    writeToSDCard(str_loop);
    gotosleepnow();
    
    Serial.println("Im awake and TX_COMPLETED is set to false");
    str_loop = "Im awake from deep sleep";
    writeToSDCard(str_loop);

    // Prepare a packet in relaxed setiing
    prepare_packet();

    // Can we do os_setCallback?
    str_loop = "About to send a packet!";
    writeToSDCard(str_loop);
    os_setCallback(&sendjob, do_send);
  }
}
