/*
 * AquaPlumb: https://www.vegetronix.com/Products/AquaPlumb/
 * Connections:
 *    VCC: 3.5V to 20V DC : Red Wire
 *    GND: Shield
 *    Output: Across Black and Shield
 */

int analogPin = A0;
float reading = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  reading = analogRead(analogPin);
  reading = reading* (5.0 / 1023.0);
  Serial.print("Reading is: "); Serial.println(reading);
  delay(2000);
}
