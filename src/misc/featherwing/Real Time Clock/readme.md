In order to have timestamps for the data to be logged, it is necessary to have a real time clock.

Most of the microcontrollers have a built-in timekeepers usually timed via timers that are built into the chip that can track longer periods of time like minutes or days but only since when the MCU was last powered. Therefore we need a seperate timekeeper independent of the MCU power status and which can last much longer.

Therefore we are testing Featherwing which has an RTC: PCF8523 with a battery backup, the coin cell which can keep the time for few years.
