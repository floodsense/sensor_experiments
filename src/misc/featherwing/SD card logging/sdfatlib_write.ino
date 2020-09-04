#include <SdFat.h>

// SD chip select pin
const uint8_t chipSelect = 10;
// file system object
SdFat sd;
// create Serial stream
ArduinoOutStream cout(Serial);

// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

// File name
char name[] = "TESTFILE.TXT";

void writeToSDCard(String StringtobeWritten){
  char buffchar[StringtobeWritten.length() + 1];
  StringtobeWritten.toCharArray(buffchar, StringtobeWritten.length() + 1);
  digitalWrite(8,HIGH);
  delay(400);  // Catch Due reset problem
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  cout << pstr("Writing to: ") << name;
  ofstream sdout(name, ios::out | ios::app);
  if (!sdout) error("open failed");
  sdout << buffchar << endl;
  // close the stream
  sdout.close();
  if (!sdout) error("writing failed");
  cout << endl << "Done" << endl;
  digitalWrite(8,LOW);
}

void setup() {
  // File name
  Serial.begin(9600);
  while (!Serial) {} 
  
}

int count = 0;

void loop() {
  // put your main code here, to run repeatedly:
 
  //char StringtobeWritten[] = "I am a dummy 2";
  String StringtobeWritten = "I am a dummy String";
  if (count<10){
    writeToSDCard(StringtobeWritten);
    count = count +1 ;
  }

}
