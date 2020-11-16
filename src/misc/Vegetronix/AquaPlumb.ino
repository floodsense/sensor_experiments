/*
 * AquaPlumb: https://www.vegetronix.com/Products/AquaPlumb/
 * Connections:
 *    VCC: 3.5V to 20V DC : Red Wire
 *    GND: Shield
 *    Output: Across Black and Shield
 */

int analogPin = A0;
long reading = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(analogPin);
  Serial.print("Reading is: "); Serial.println(reading);
}
