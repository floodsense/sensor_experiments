# Technical documentation of sensor design, including instructions on building, deploying the sensors, with schematics for technology transfer

## Executive Summary:

This document specifies the sensor design, building instructions, initial deployment and schematics for technology transfer of the sensor development for the Floodsense project at 370 Jay Street, 13th Floor.

## Sensor Design and building instructions:

The sensor is of the dimensions 5.1" x 3.1" x 2.8". Using metal straight brace brackets the sensor can be mounted onto poles, walls and additional mounting hardware depending on the scenario.

<img src="img\sensor-v1-front.jpg" width="480">

<br />

The sensor uses a high-end ultrasonic range sensor, the MB7389 from Maxbotix, which provides range detection from 30-500 cm with an accuracy of ±3 mm. The sensor can be pole or wall mounted. The sensor is battery powered with solar energy harvesting for extended operation. Connectivity is provided via a LoRaWAN system described in the following sections. The solar panels are installed at an angle for optimal solar power using metal brackets. The sensor will typically upload data every 30 mins with faster update rates when flood water is detected. A complete list of BOM with suppliers is mentioned below.


| Item                                                        | Cost|
| ----------------------------------------------------------  |------|
| [Feather M0](https://www.adafruit.com/product/3178)        | $34.95|
| [2200mAh battery](https://www.adafruit.com/product/1781)   | $9.95|
| [Connector board](https://www.adafruit.com/product/2926)   | $14.95|
| [RTC battery](https://www.adafruit.com/product/380)        | $0.95|
| [SD feather wing](https://www.adafruit.com/product/2922)   | $8.95|
| [Duponts](https://www.amazon.com/Generic-Breakaway-Headers-Length-Centered/dp/B015KA0RRU/ref=sr_1_2?dchild=1&keywords=long+pin+dupont&qid=1593041308&sr=8-2)|$7.95    |
| [Voltaic 1.2W solar panel](https://voltaicsystems.com/1-watt-6-volt-solar-panel-etfe/) |$14.00|
| [Solar board](https://www.amazon.com/Adafruit-Lithium-Polymer-Charger-ADA390/dp/B00OKCSK88/ref=sr_1_24?dchild=1&keywords=ADA390&qid=1599062478&s=electronics&sr=1-24)|$22.90|
| [Transistors](https://www.amazon.com/WINGONEER-15Values-Transistor-Assortment-2N2222-S9018/dp/B06XSFNYFP/ref=sr_1_3?dchild=1&keywords=2n2222&qid=1593044183&s=electronics&sr=1-3)|$14.50|
| [Ultrasonic sensor](https://www.robotshop.com/en/maxbotix-maxsonar-weather-resistant-ultrasonic-range-finder.html) | $99.95|
| [Mounting headers](https://www.amazon.com/gp/product/B07KM5B3PT/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1) | $9.19|
| [Panel mount](https://www.amazon.com/uxcell-Aluminum-Brackets-Profile-Connectors/dp/B07SGDXJWN/ref=pd_sim_60_1/142-2831092-9952011?_encoding=UTF8&pd_rd_i=B07SGDXJWN&pd_rd_r=a68c7b36-c091-406e-a9ee-7e69af613cbe&pd_rd_w=3Tq0h&pd_rd_wg=EmJXn&pf_rd_p=4e0e1c4f-27a6-4ddd-82e5-121637280059&pf_rd_r=R1FTFNTPHYW9YGYM2JPC&psc=1&refRID=R1FTFNTPHYW9YGYM2JPC)| $14.09|
| [Housing (5.1"x3.1"x2.8")](https://www.amazon.com/dp/B07FFM3T1P/ref=dp_iou_view_product?ie=UTF8&psc=1)| $9.99 |
| [16GB SD card](https://www.amazon.com/SanDisk-Ultra-SDSQUNS-016G-GN3MN-UHS-I-microSDHC/dp/B074B4P7KD/ref=sr_1_4?dchild=1&keywords=sd+card&qid=1593044968&refinements=p_89%3ASamsung%7CSanDisk%2Cp_36%3A1253503011%2Cp_72%3A1248879011&rnid=1248877011&s=pc&sr=1-4)| $6.00|
| [Mounting plate](https://www.amazon.com/dp/B07G4XYQXX/ref=sspa_dk_detail_2?pd_rd_i=B07G4XYQXX&pd_rd_w=88b7Q&pf_rd_p=a64be657-55f3-4b6a-91aa-17a31a8febb4&pd_rd_wg=WHW9D&pf_rd_r=P2VY3R2D6VFS2DFV2BD2&pd_rd_r=10b85dcb-8399-480c-be22-23432f9786aa&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFTNE43U09LQkRIRkEmZW5jcnlwdGVkSWQ9QTAzMzEyNTcyRzlLVlFNRjBPRzAwJmVuY3J5cHRlZEFkSWQ9QTA4NTczMjJTSzE3NlBDT1RJSCZ3aWRnZXROYW1lPXNwX2RldGFpbF90aGVtYXRpYyZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU&th=1) | $11.99|
| [Lock nut](https://www.amazon.com/Lock-nut-female-75inch-locknut-Spacer/dp/B07QM7S33K/ref=sr_1_2?dchild=1&keywords=3%2F4inch+lock+nut&qid=1593126413&sr=8-2)| $5.99|
| Grand Total | $293.90|

### Components
All components are readily available from internet suppliers, primarily NYC based. Figure below labels the core components within the prototype sensor.

<img src="img\components.png" width="480" >
<br />

### Building Instructions
The following are the building instructions listed as steps:

#### Step 1: Microcontroller Assembly

The Feather m0 LoRa module does not come with a built-in antenna for LoRa. A simple single core standard wire can be used as an antenna when cut down to right lengths according to the desired frequencies
  - 433 MHz - 6.5 inches, or 16.5 cm
  - 868 MHz - 3.25 inches or 8.2 cm
  - 915 MHz - 3 inches or 7.8 cm

Using single core wire as antenna:

<img src="img\single-core-wireas-antenna.jpg" width="480" >
<br />

And the other components used are SD feather wing, male long headers and a coin cell battery.

<img src="img\mcu-components-2.jpg" width="480" >
<br />

Solder the antenna onto the ANT pad on the very right hand edge of the sensor. Using the male headers solder the SD feather wing onto the feather m0 module and mount this onto the connector board and fix it onto the mounting board of the housing.

<img src="img\assembled-mcu.jpg" width="480" >
<br />

Finally the coin cell and SD card goes into the SD feather wing.

#### Step 2: Drilling holes for Sensor and Solar Panel

Two holes need to be drilled into the housing. The first hole is to mount the Ultrasonic sensor and this needs to be at the center of the mounting side for constant and reliable readings. The second hole is next to the first one to accommodate the wire connecting the solar panel.

<img src="img\drilling.jpg" width="480" >
<br />

#### Step 3: Mounting MCU into the housing
Mount the complete MCU setup from step 1 into the housing using screws.

#### Step 4: Mounting the Ultrasonic sensor

Connect the ultrasonic sensor to the MCU and screw it using a ¾” lock nut.

<img src="img\mounted-mcu-us.jpg" width="480" >
<br />

#### Step 5: Mounting the Solar Panel

Since the solar panel sits outside, it is always a good measure to use heat sinks and potting for waterproofing the contacts on the back.

<img src="img\solar-panel-waterproofed.jpg" width="480" >
<br />

Now run the wire back from the solar panel through the smaller hole and use a PG-7 cable gland to waterproof the opening.

<img src="img\gland-bottom.jpg" width="480" >
<br />

Further, solder the solar panel onto the solar battery charger and stick the solar battery charger onto the side wall to make space for the battery.

<img src="img\solarboard-ontheside.jpg" width="480" >
<br />


#### Step 6: Final assembly of the sensor

Using an aluminum corner bracket, the solar panel is mounted at an angle for optimal amount of solar radiation.

<img src="img\panel-mounted.jpg" width="480" >
<br />


<img src="img\final-withoutbatt.jpg" width="480" >
<br />

Now we are ready for battery plug in!
