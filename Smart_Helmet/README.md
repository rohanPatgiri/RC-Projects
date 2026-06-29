
Link to the blog: (will be updated)

---

Libraries to install — open Arduino IDE → Manage Libraries and install VirtualWire and LiquidCrystal I2C before compiling either sketch.
MQ-3 calibration — open the Serial Monitor after upload, wait 30 seconds for the sensor to warm up, note the baseline ADC reading in clean air vs. when exposed to alcohol fumes, then set ALCOHOL_THRESHOLD to a value between the two.
LCD address — most I2C backpacks use 0x27, but some use 0x3F. If the display stays blank, run an I2C scanner sketch and update line 14 of motorcycle_unit.ino.
RF baud rate — both sketches use vw_setup(2000). This must be identical on both boards or communication will fail.
