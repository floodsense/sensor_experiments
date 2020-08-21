/*
 *  This code tests different Sensor Modes on the same end-node using LoRa. The following are
 *  the different testing modes. Mode 1, 2 and 3 are different modes of sensing done after
 *  scheduling a next uplink where as 4 and 5 are in a relaxed setting i.e. sensing is
 *  kept seperate from the scheduling timing.
 *
 *  Testing sensor Modes:
 *    - Mode 1: default single reading.
 *    - Mode 2: 3 - pulse-ins and the last reading is used
 *    - Mode 3: median of 5 consecutive readings
 *    - Mode 4: reading in a relaxed setiing
 *    - Mode 5: median of 5 readings in a relaxed setting
 *
 */
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Adafruit_SleepyDog.h>

#define VBATPIN A7
unsigned char mydata[5];
uint16_t distance = 0;
uint16_t duration = 0;
float measuredvbat;
#define readPin 11
#define triggerPin 12
int sensorMode = 1;
uint16_t median_arr[5];
uint16_t batlevel;
byte lowbyte, highbyte, lowbat, highbat;

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.

static const u1_t PROGMEM APPEUI[8] = { < Enter your key > };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}
// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { < Enter your key > };

void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}
// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { < Enter your key > };

void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}
//static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;
// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1;
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

void printHex2(unsigned v) {
  v &= 0xff;
  if (v < 16)
    Serial.print('0');
  Serial.print(v, HEX);
}

//int sleep_loop_cnt = 2;
int sleep_loop_cnt = 15;

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
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      Serial.println("");
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }

      // Sleep now
      sleepnow();

      // Change testing mode
      changesensorMode();

      // Prepare a packet in a relaxed setting
      prepare_packet();         // only for modes 4 and 5 (does nothing for modes 1 2 and 3)
      delay(10);                // To prevent current spikes or power dips due to LoRa (does it really?)

      // Schedule next transmission right now and also prepares packet for modes 1 2 and 3
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);

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
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}


void prepare_packet(void) {

  switch (sensorMode) {
    case 4:
      // Dedicated sensing mode
      Serial.print("Sensor Mode: ");
      Serial.println(sensorMode);
      // Maxbotix
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      distance = duration;
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
      lowbat = lowByte(batlevel);
      highbat = highByte(batlevel);
      mydata[2] = (unsigned char)lowbat; //we're unsigned
      mydata[3] = (unsigned char)highbat;
      break;
    case 5:
      // dedicated sensing period + median of 5 readings
      Serial.print("Sensor Mode: ");
      Serial.println(sensorMode);
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      median_arr[0] = duration;
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      median_arr[1] = duration;
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      median_arr[2] = duration;
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      median_arr[3] = duration;
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(20);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(readPin, HIGH);
      median_arr[4] = duration;
      // Median of the 5 readings
      int n = 5;
      sort(median_arr);
      n = (n + 1) / 2 - 1;    // -1 as array indexing in C starts from 0
      distance = median_arr[n];
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
      lowbat = lowByte(batlevel);
      highbat = highByte(batlevel);
      mydata[2] = (unsigned char)lowbat; //we're unsigned
      mydata[3] = (unsigned char)highbat;
      break;
  }
}

void changesensorMode(void) {
  // Simple if-else loop to change testing-Modes
  if (sensorMode == 5) {
    sensorMode = 1;
    Serial.println("Mode has been changed from 5 to 1");
  }
  else {
    sensorMode = sensorMode + 1;
    Serial.print("Mode has been changed to ");
    Serial.println(sensorMode);
  }
}

void sleepnow(void) {
  // Sleep For a while (currently 6 mins) --> This controls duty cycle
  Serial.println("About to sleep");
  for (int i = 0; i < sleep_loop_cnt; i++) {
    Watchdog.sleep(8000);
    Serial.println("Im Awake!");
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.

    switch (sensorMode) {
      case 1:
        // Default mode
        Serial.print("Sensor Mode: ");
        Serial.println(sensorMode);
        // Maxbotix
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        distance = duration;
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
        lowbat = lowByte(batlevel);
        highbat = highByte(batlevel);
        mydata[2] = (unsigned char)lowbat; //we're unsigned
        mydata[3] = (unsigned char)highbat;
        break;
      case 2:
        // Maxbotix 3 times read (no processing; sends out the last reading)
        Serial.print("Sensor Mode: ");
        Serial.println(sensorMode);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        distance = duration;
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
        lowbat = lowByte(batlevel);
        highbat = highByte(batlevel);
        mydata[2] = (unsigned char)lowbat; //we're unsigned
        mydata[3] = (unsigned char)highbat;
        break;
      case 3:
        // median of 5 readings
        Serial.print("Sensor Mode: ");
        Serial.println(sensorMode);
        // Maxbotix
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        median_arr[0] = duration;
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        median_arr[1] = duration;
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        median_arr[2] = duration;
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        median_arr[3] = duration;
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(triggerPin, LOW);
        duration = pulseIn(readPin, HIGH);
        median_arr[4] = duration;
        // Median of the 5 readings
        int n = 5;
        sort(median_arr);
        n = (n + 1) / 2 - 1;    // -1 as array indexing in C starts from 0
        distance = median_arr[n];
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
        lowbat = lowByte(batlevel);
        highbat = highByte(batlevel);
        mydata[2] = (unsigned char)lowbat; //we're unsigned
        mydata[3] = (unsigned char)highbat;
        break;
    }
    mydata[4] = (unsigned char)sensorMode;
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  delay(5000);
  while (! Serial);
  Serial.begin(9600);
  Serial.println(F("Starting"));
#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Disable link check validation (automatically enabled)
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_selectSubBand(1);
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

}
void loop() {
  os_runloop_once();
}
