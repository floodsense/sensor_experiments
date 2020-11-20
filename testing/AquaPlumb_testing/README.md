# AquaPlumb Sensor Experiments:

## 1. Static Water Level
The AquaPlumb sensors are mounted onto a metal rod, which is a similar scenario to a pole mount. The sensor cables have been immersed in a container filled with water of  depth 14 cm. The MCU logs the analog readings of the sensor outputs. Here below is the test setup:

<img src="https://github.com/floodsense/sensor_experiments/blob/master/img/aquaplumb_static.jpg?raw=true" width="560">

<br />
<br />
Due to evaporation, the final water level is 13.5 cm. 

Scaled down Aquaplumb ADC values and scaled to mm. Noise looks minimal and resolution seems good at the ~1mm level. The downward trend shows water evaporation over 30 hours resulting in a ~5mm drop in water depth. The calibration of each length of cable was slightly off which explains the slightly different gradients of the short (1m) and long (3m).

![image](https://user-images.githubusercontent.com/5649897/99604098-bfa40300-29d2-11eb-91b2-13618b62586e.png)
