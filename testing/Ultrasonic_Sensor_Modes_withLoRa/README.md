### Ultrasonic Sensor Modes:

**Ultrasonic sensor used:** MB7389 HRXL-MaxSonar-WRMT [(link)](https://www.maxbotix.com/Ultrasonic_Sensors/MB7389.htm) 

The HRXL-MaxSonar-WR use an internal filter to process range data. However, there are still particular reflections that can cause some [surprised clutter!](https://www.maxbotix.com/tutorials2/103-cube-corner-reflectors.htm) Since the sensor is supposed to go in the real world, we have tested this sensor in laboratory and experimental environments and the occurrence of clutter is likely to happen one or two readings out of a 100 (readings taken every two minutes). Therefore the following five modes that have been tested to reduce this clutter.

Mode 1: Single pulseIn(trigger) and single reading (regular reading)   
Mode 2: Three pulseIns(triggers) and read at the end of final pulseIn   
Mode 3: Median of 5 regular readings    

**Results:**      
![](https://github.com/floodsense/sensor_experiments/blob/master/img/mode1vs2_mode1vs3.png?raw=true)
<br />

Further two of these modes (Mode 1 and Mode 3) are kept separate from lmic timing and hence in a "relaxed" setting.

Mode 4: regular reading in a relaxed setting    
Mode 5: Median of 5 regular readings in relaxed Setting

**Results:** 
![](https://github.com/floodsense/sensor_experiments/blob/master/img/mode1vs4andmode1vs5.png?raw=true)
<br />

**Observation:**

The Mode 5, median of 5 readings has a smoother curve and lesser peaks when compared with the other modes.     

**Effect of Temperature on Ultrasonic Sensor readinds:**
There is a notable drift of the ultrasonic sensor readings over time due to the change in temperature. Even though this particular sensor model has internal temperature compensation, there is still drift in the sensor readings. This can be observed from the fact that when there is direct sunlight during the day the readings are increased because the direct sunlight is raising the temperature of the sensor housing which is erroneously inflating the internal temperature of sensor reading. The internal temperature compensation algorithm is then over compensating for the inflated temperature and returning an increased distance measure.     

The following is the ultrasonic sensor readings vs temperature over a period of 24 hours.     

![](https://github.com/floodsense/sensor_experiments/blob/master/img/UltrasonicvsTemp_aug22nd_.png)
<br />
