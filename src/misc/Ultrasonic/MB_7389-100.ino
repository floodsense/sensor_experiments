/*
 *   Calculating the water depth using Ultrasonic Sensor - Model A02YYUW
 *   Programmed using C++
 *   Tested on Arduino - ATmega328, Adafruit Feather M0
 *   Sensor: A02YYUW
 *   Connections:
 *      Pin 2  ->  Pin 10 MCU
 *      Pin 4  ->  Pin 11 MCU
 *      VCC    ->  VCC
 *      GND    ->  GND
 *   Sensor operating mode: Trigger
 *     Pin 4 of the sensor is te trigger pin and needed to be connected to LOW. When you 
 *     want to take a reading, you have to pull this pin HIGH for a minimum of 20 microseconds.
 *   Note: This sensor has some Nozzle Correction
 */

#define readPin 10
#define triggerPin 11

long distance = 0;
long duration = 0;

void setup() 
{
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
  digitalWrite(triggerPin, LOW);
  Serial.begin(9600);
  delay(3000);
  Serial.println("Sensor is ready!");
}

void read_sensor() 
{
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds. 
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  distance = duration;
  delay(150);
}

void print_data() 
{
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
}

void loop() 
{

    read_sensor();
    print_data();
    delay(100);

}
