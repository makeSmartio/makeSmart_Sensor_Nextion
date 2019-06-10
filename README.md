# makeSmart Sensor

Arudunio code for the multi-purpose sensor sold on Tindie and eBay. The sensor is designed to log data and alert for: water presence, too hot, too cold, motion, washing mashing machine/dyer finished cycle, and soil moisture.

https://makeSmart.io

## Getting Started

You will need to download and install the Arduino IDE add the board manager lines.
<br>IDE: https://www.arduino.cc/en/Main/Software
<br>How to install board managers: https://arduino-esp8266.readthedocs.io/en/latest/installing.html
<br>ESP8266: https://arduino.esp8266.com/stable/package_esp8266com_index.json
<br>ESP32: https://dl.espressif.com/dl/package_esp32_index.json

Then use the Arduino Library Manager to install the following libraries:
Time - by Michal Margolis
WifiManager - by tzapu
ArduinoJson - version 5 for now
OneWire
BME280 - by Tyler Glen
Adafruit_Si7021 - I prefer the BME280 chip, but some old sensors have the also good Si7021 temperature sensor
DallasTemperature - for the external temperature probes
MPU6050 - Gyrometer/Accelerometer

## Acknowledgments

* Huge thanks to tablatronix and everyone at the https://github.com/tzapu/WiFiManager library. I wouldn't have done this without this library
* All of the YouTubers and folks who have written how to's, and help guides on the web
