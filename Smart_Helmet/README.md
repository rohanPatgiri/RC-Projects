## Project: Build a Smart Helmet Using Arduino


### A brief overview of this project:

This project turns a standard motorcycle helmet into a smart safety system using two Arduino Uno boards communicating wirelessly via RF modules. The helmet unit monitors whether the rider is wearing the helmet and checks for alcohol consumption using an MQ-3 sensor. If all safety conditions pass, a signal is sent to the motorcycle unit, which enables the ignition relay. No helmet, no ride.


- Link to the detailed blog: (will be updated)

---

- Libraries to install — open Arduino IDE → Manage Libraries and install VirtualWire and LiquidCrystal I2C before compiling either sketch.
- MQ-3 calibration — open the Serial Monitor after upload, wait 30 seconds for the sensor to warm up, note the baseline ADC reading in clean air vs. when exposed to alcohol fumes, then set ALCOHOL_THRESHOLD to a value between the two.
- LCD address — most I2C backpacks use 0x27, but some use 0x3F. If the display stays blank, run an I2C scanner sketch and update line 14 of motorcycle_unit.ino.
- RF baud rate — both sketches use vw_setup(2000). This must be identical on both boards or communication will fail.
