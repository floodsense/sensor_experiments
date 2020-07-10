/*******************************************************************************
   This code sends the Maxbotix Sensor reading and Battery level using OTAA 
   (Over the air activation), where where a DevEUI and application key is 
   configured, which are used in an over-the-air activation procedure where 
   a DevAddr and session keys are assigned/generated for use with all 
   further communication. 
   To use this sketch, first register your application and device with
   the things network, to set or generate an AppEUI, DevEUI and AppKey.
   Multiple devices can use the same AppEUI, but each device has its own
   DevEUI and AppKey.
   Do not forget to define the radio type correctly in config.h.
*******************************************************************************/
#include <lmic.h>
#include "LowPower.h"
#include <hal/hal.h>
#include <SPI.h>
#include <avr/dtostrf.h>
#include <Adafruit_SleepyDog.h>
//#include <stdlib.h>
//Feather M0
#define VBATPIN A7
//Arduino MKRWAN 1310
//#define VBATPIN A0
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.

//Feather M0
static const u1_t PROGMEM APPEUI[8] = { <Enter your key here> };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

// This should also be in little endian format, see above.
//Feather M0
static const u1_t PROGMEM DEVEUI[8] = { <Enter your key here> };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
//Feather M0
static const u1_t PROGMEM APPKEY[16] = { <Enter your key here> };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

int sleep_loop_cnt = 3;

char mydata[16];
static osjob_t sendjob;
// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1;
// Pin mapping
// Pin mapping for Adafruit Feather M0 LoRa
const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {3, 6, LMIC_UNUSED_PIN},
  .rxtx_rx_active = 0,
  .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
  .spi_freq = 8000000,
};
void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE: 
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      pinMode(13, OUTPUT);
      digitalWrite(13, LOW);
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}

uint16_t distance = 0;
uint16_t duration = 0;
float measuredvbat;
#define readPin 10
#define triggerPin 11


void readValues(unsigned char vals[4]) {


  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  distance = duration;


  byte lowbyte = lowByte(distance);
  byte highbyte = highByte(distance);
  vals[0] = (unsigned char)lowbyte;
  vals[1] = (unsigned char)highbyte;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");

  //Lipoly batteries are 'maxed out' at 4.2V and stick around 3.7V for much of the battery life,
  //then slowly sink down to 3.2V or so before the protection circuitry cuts it off.
  //By measuring the voltage you can quickly tell when you're heading below 3.7V

  measuredvbat = analogRead(VBATPIN); //Float
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  measuredvbat *= 1000; //make it milli volts to transmit
  Serial.print("VBat in mVolts: " ); Serial.println(measuredvbat);
  uint16_t batlevel = measuredvbat; //Payload
  byte lowbat = lowByte(batlevel);
  byte highbat = highByte(batlevel);
  vals[2] = (unsigned char)lowbat; //we're unsigned
  vals[3] = (unsigned char)highbat;
}


void do_send(osjob_t* j) {
    for (int i = 0; i <= sleep_loop_cnt; i++) {
      Watchdog.sleep(1000);
    }

//     Check if there is not a current TX/RX job running
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);


  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    unsigned char payload[4];

    readValues(payload);


    LMIC_setTxData2(1, payload, sizeof(payload), 0);

    Serial.println(delta);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}


void setup() {
  delay(3000);
  Serial.begin(9600);
  Serial.println("Setting up!");
  Serial.println(F("Starting"));
  //   LMIC init
  os_init();
  //Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  //Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
 
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  digitalWrite(triggerPin, LOW);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}


void loop() {
  os_runloop_once();
}
