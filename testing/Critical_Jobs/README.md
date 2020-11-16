# Testing for Critical Jobs before Deep Sleep

This code uses a feather m0 LoRa module. The Adafruit's Feather mcu has the ability to store all the system variables during deepsleep when slept using a watchdog timer. Critical Jobs before deepsleep has been tested for low power and stability.

## More on Lmic timing and Critical Jobs:
Using lmic library for lora with Adafruits Sleepy_dog, the MCU can be put to deepsleep when ever not in use. But however the lmic timing is pretty tight. More detailed structure and working of lmic can be found at [mcci-catena's](https://github.com/mcci-catena/arduino-lmic/blob/master/doc/LMIC-FSM.pdf) repository. A packet is transmission via uplink is followed by two RX windows R1 and R2 consecutively followed by a TX Complete, which sets the EV_TXCOMPLETE flag high.


The LoRa and LMIC might be still sending some traffic. Trying to sleep right after EV_TXCOMPLETE causes instability and errors. To solve this problem, any Critical Jobs needed to be checked before deepsleep. 
