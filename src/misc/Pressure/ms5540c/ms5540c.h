/*
 MS5540C Pressure Sensor
*/


// include library:
#include <SPI.h>


const int MCLK = 10; //CLOCK
class MS5540C{
  public:
    // Calibration words 1-4
    unsigned int cw1;
    unsigned int cw2;
    unsigned int cw3;
    unsigned int cw4;
    // Calibration factors 1-6
    long c1;
    long c2;
    long c3;
    long c4;
    long c5;
    long c6;
    // Pressure
    unsigned int rawPress;
    int Pressure;
    int secondOrderPressure;
    // Temperature
    unsigned int rawtemp;
    float temp;

    // Second order pressure
    float pcompreal;
    float pcomphg;
    long pcomp;
    long ptemp;

    void resetSensor();
    void spiCheck();
    void measure(int printOut, int raw);
    void factorsFromWords();
    void secondDegCompPressure();
    void calibration(int readOut);
    MS5540C(); //constructor
    //MS5540C();

};

// default constructor
/*MS5540C::MS5540C(){
  MS5540C(int MOSI = 11, int MISO = 12, int SCLK = 13, int MCLK = 9);
}*/
// Parameterized constructor
MS5540C::MS5540C() {
  Serial.begin(9600);
  SPI.begin(); //see SPI library details on arduino.cc for details
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32); //divide 16 MHz to communicate on 500 kHz
  pinMode(MCLK, OUTPUT);
  Serial.print("MS5540C initialized\n");
  delay(100);
}
void MS5540C::spiCheck(){
  SPI.begin(); //see SPI library details on arduino.cc for details
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32); //divide 16 MHz to communicate on 500 kHz
  pinMode(MCLK, OUTPUT);
  TCCR1B = (TCCR1B & 0xF8) | 1 ; //generates the MCKL signal
  analogWrite (MCLK, 128) ;
}
void MS5540C::resetSensor() //this function keeps the sketch a little shorter
{
  SPI.setDataMode(SPI_MODE0);
  SPI.transfer(0x15);
  SPI.transfer(0x55);
  SPI.transfer(0x40);
}

void MS5540C::calibration(int readOut = 0)
{
  TCCR1B = (TCCR1B & 0xF8) | 1 ; //generates the MCKL signal
  analogWrite (MCLK, 128) ;

  resetSensor(); //resets the sensor - caution: afterwards mode = SPI_MODE0!

  //Calibration word 1
  unsigned int result1 = 0;
  unsigned int inbyte1 = 0;
  SPI.transfer(0x1D); //send first byte of command to get calibration word 1
  SPI.transfer(0x50); //send second byte of command to get calibration word 1
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  result1 = SPI.transfer(0x00); //send dummy byte to read first byte of word
  result1 = result1 << 8; //shift returned byte
  inbyte1 = SPI.transfer(0x00); //send dummy byte to read second byte of word
  result1 = result1 | inbyte1; //combine first and second byte of word
  if (readOut){
    Serial.print("Calibration word 1 = ");
    Serial.print(result1,HEX);
    Serial.print(" ");
    Serial.println(result1);
  }

  cw1 = result1;

  resetSensor(); //resets the sensor

  //Calibration word 2; see comments on calibration word 1
  unsigned int result2 = 0;
  byte inbyte2 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0x60);
  SPI.setDataMode(SPI_MODE1);
  result2 = SPI.transfer(0x00);
  result2 = result2 <<8;
  inbyte2 = SPI.transfer(0x00);
  result2 = result2 | inbyte2;
  if (readOut){
    Serial.print("Calibration word 2 = ");
    Serial.print(result2,HEX);
    Serial.print(" ");
    Serial.println(result2);
  }

  cw2 = result2;

  resetSensor(); //resets the sensor

  //Calibration word 3; see comments on calibration word 1
  unsigned int result3 = 0;
  byte inbyte3 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0x90);
  SPI.setDataMode(SPI_MODE1);
  result3 = SPI.transfer(0x00);
  result3 = result3 <<8;
  inbyte3 = SPI.transfer(0x00);
  result3 = result3 | inbyte3;
  if (readOut){
    Serial.print("Calibration word 3 = ");
    Serial.print(result3,HEX);
    Serial.print(" ");
    Serial.println(result3);
  }

  cw3 = result3;
  resetSensor(); //resets the sensor

  //Calibration word 4; see comments on calibration word 1
  unsigned int result4 = 0;
  byte inbyte4 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0xA0);
  SPI.setDataMode(SPI_MODE1);
  result4 = SPI.transfer(0x00);
  result4 = result4 <<8;
  inbyte4 = SPI.transfer(0x00);
  result4 = result4 | inbyte4;
  if ( readOut){
    Serial.print("Calibration word 4 = ");
    Serial.print(result4,HEX);
    Serial.print(" ");
    Serial.println(result4);
  }

  cw4 = result4;

  //now we do some bitshifting to extract the calibration factors
  //out of the calibration words;
  c1 = cw1 >> 1;
  c2 = ((cw3 & 0x3F) << 6) | ((cw4 & 0x3F));
  c3 = (cw4 >> 6) ;
  c4 = (cw3 >> 6);
  c5 = (cw2 >> 6) | ((cw1 & 0x1) << 10);
  c6 = cw2 & 0x3F;
  if (readOut){
    Serial.print("c1 = ");
    Serial.println(c1);
    Serial.print("c2 = ");
    Serial.println(c2);
    Serial.print("c3 = ");
    Serial.println(c3);
    Serial.print("c4 = ");
    Serial.println(c4);
    Serial.print("c5 = ");
    Serial.println(c5);
    Serial.print("c6 = ");
    Serial.println(c6);
  }


  resetSensor(); //resets the sensor
}

// get appropriate factors from calibration words
void MS5540C::factorsFromWords(){
  c1 = cw1 >> 1;
  c2 = ((cw3 & 0x3F) << 6) | ((cw4 & 0x3F));
  c3 = (cw4 >> 6) ;
  c4 = (cw3 >> 6);
  c5 = (cw2 >> 6) | ((cw1 & 0x1) << 10);
  c6 = cw2 & 0x3F;
  resetSensor();
}

void MS5540C::measure(int printOut = 0, int raw = 0){
    spiCheck();
    unsigned int presMSB = 0; //first byte of value
    unsigned int presLSB = 0; //last byte of value
    unsigned int D1 = 0;
    SPI.transfer(0x0F); //send first byte of command to get pressure value
    SPI.transfer(0x40); //send second byte of command to get pressure value
    delay(35); //wait for conversion end
    SPI.setDataMode(SPI_MODE1); //change mode in order to listen
    presMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
    presMSB = presMSB << 8; //shift first byte
    presLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
    D1 = presMSB | presLSB; //combine first and second byte of value
    if (raw){
      Serial.print("D1 - Pressure raw = ");
      Serial.println(D1);
    }

    rawPress = D1;

    resetSensor(); //resets the sensor

  unsigned int tempMSB = 0; //first byte of value
  unsigned int tempLSB = 0; //last byte of value
  unsigned int D2 = 0;
  SPI.transfer(0x0F); //send first byte of command to get temperature value
  SPI.transfer(0x20); //send second byte of command to get temperature value
  delay(35); //wait for conversion end
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  tempMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
  tempMSB = tempMSB << 8; //shift first byte
  tempLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
  D2 = tempMSB | tempLSB; //combine first and second byte of value
  if (raw){
    Serial.print("D2 - Temperature raw = ");
    Serial.println(D2);
  }

  rawtemp = D2;

  //calculation of the real values by means of the calibration factors and the maths
  //in the datasheet. const MUST be long

  const long UT1 = (c5 << 3) + 20224;
  const long dT = D2 - UT1;
  const long TEMP = 200 + ((dT * (c6 + 50)) >> 10);
  const long OFF  = (c2 * 4) + (((c4 - 512) * dT) >> 12);
  const long SENS = c1 + ((c3 * dT) >> 10) + 24576;
  const long X = (SENS * (D1 - 7168) >> 14) - OFF;
  long PCOMP = ((X * 10) >> 5) + 2500;
  float TEMPREAL = TEMP/10;
  float PCOMPREAL = PCOMP / 10;
  float PCOMPHG = PCOMP * 750.06 / 10000; // mbar*10 -> mmHg === ((mbar/10)/1000)*750/06
  temp = TEMPREAL;
  pcompreal = PCOMPREAL;
  pcomphg = PCOMPHG;
  pcomp = PCOMP;
  ptemp = TEMP;
  /*Serial.print("===========\n");
  Serial.print(PCOMP);
  Serial.print("===========\n");*/
  if (raw){
    Serial.print("c1 = ");
    Serial.println(c1);
    Serial.print("c2 = ");
    Serial.println(c2);
    Serial.print("c3 = ");
    Serial.println(c3);
    Serial.print("c4 = ");
    Serial.println(c4);
    Serial.print("c5 = ");
    Serial.println(c5);
    Serial.print("c6 = ");
    Serial.println(c6);

    Serial.print("UT1 = ");
    Serial.println(UT1);
    Serial.print("dT = ");
    Serial.println(dT);
    Serial.print("TEMP = ");
    Serial.println(TEMP);
    Serial.print("OFFP = ");
    Serial.println(OFF);
    Serial.print("SENS = ");
    Serial.println(SENS);
    Serial.print("X = ");
    Serial.println(X);
  }

  if(printOut){
    Serial.print("Real Temperature in C = ");
    Serial.println(TEMPREAL);
    Serial.print("Compensated pressure in mbar = ");
    Serial.println(PCOMPREAL);
    Serial.print("Compensated pressure in mmHg = ");
    Serial.println(PCOMPHG);
  }

}
void MS5540C::secondDegCompPressure(){
  //2-nd order compensation only for T < 20Â°C or T > 45Â°C

  long T2 = 0;
  float P2 = 0;

  if (ptemp < 200)
    {
      T2 = (11 * (c6 + 24) * (200 - ptemp) * (200 - ptemp) ) >> 20;
      P2 = (3 * T2 * (pcomp - 3500) ) >> 14;
    }
  else if (ptemp > 450)
    {
      T2 = (3 * (c6 + 24) * (450 - ptemp) * (450 - ptemp) ) >> 20;
      P2 = (T2 * (pcomp - 10000) ) >> 13;
    }

  if ((ptemp < 200) || (ptemp > 450))
  {
    const float TEMP2 = ptemp - T2;
    const float PCOMP2 = pcomp - P2;

    float TEMPREAL2 = TEMP2/10;
    float PCOMPHG2 = PCOMP2 * 750.06 / 10000; // mbar*10 -> mmHg === ((mbar/10)/1000)*750/06

    Serial.print("2-nd Real Temperature in C = ");
    Serial.println(TEMPREAL2);

    Serial.print("2-nd Compensated pressure in mbar = ");
    Serial.println(PCOMP2);
    Serial.print("2-nd Compensated pressure in mmHg = ");
    Serial.println(PCOMPHG2);
  }
}
