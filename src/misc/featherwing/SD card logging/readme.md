Please note only FAT16 and FAT32 SD cards are supported. 

Notes for SD card logging using Feather m0 LoRa RFM9x module:
  - The cardSelect is on pin 10
  - The LoRa module's CS pin is connected to the pin 8 and is pulled low by default. In order to talk with other SPI devices, pin 8 must be pulled high before talking to them,     then pull back to low after done so the microcontroller can talk to the LoRa module again.
  - If you really want to make sure you save every data point, put a logfile.flush() right after the logfile.print's however this will cause the adalogger to draw a log more power, maybe about 3x as much on average (30mA avg rather than about 10mA).
  
