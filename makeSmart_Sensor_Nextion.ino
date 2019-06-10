int ThisRockVersion = 89;
String AlphaOrBeta = "Alpha";
int NotifyEverySeconds = 6000;
int pirCount = 0;
const int myInterval = 100;
const char* host = "makeSmart.io";
const int httpPort = 80;
String SensorName;
String sensorMode = "Blank";
String mac_addr;

int relay1Pin = 14;
int relay2Pin = 27;
int relay3Pin = 26;
int relay4Pin = 25;

bool relay1State;
bool relay2State;
bool relay3State;
bool relay4State;

const char* apPassword = "asdfasdf";
long rssi;

bool keepDataPoint = 0;
bool sendMotionAlerts = 0;
bool trackMotion = 1;
int secsSinceLastSend = 100;
int sendFrequency = 300;
int secsSinceLastAlert;
#include <EEPROM.h>
#include <Time.h>
#include <TimeLib.h>
time_t lastGyroError = now() - 10000;
time_t lastTooColdAlert = now() - 10000;
time_t lastTooHotAlert = now() - 10000;
time_t lastWaterAlert = now() - 10000;
time_t lastSend = now() - 10000;
time_t lastAlertTime = now() - 10000;
time_t lastMotionAlert = now() - 1000;
time_t lastSoilAlert = now() - 1000;
time_t lastAirPolutionAlert = now() - 1000;

time_t dryerRunningSince;
int dryerSensitivity = 1000;
int dryerStarting = 0;
bool dryerRunning = false;
int dryerStopping = 0;
int minDryerCycleTime = 600; //10 minutes
time_t washerRunningSince;
int washerSensitivity = 1500;
int washerStarting = 0;
bool washerRunning = false;
int washerStopping = 0;
int minCycleTime = 300;

float warnAboveDHTTemp = 100;
float warnBelowDHTTemp = -100;
float warnAboveProbe1 = 100;
float warnBelowProbe1 = -100;
float warnAboveTemp2 = 100;
float warnBelowTemp2 = -100;
String webSite, javaScript, XML;

#include <ArduinoJson.h>

#include <OneWire.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdate;
#include <ESP8266httpUpdate.h>
#include <ESP8266mDNS.h>
//#include <ESP8266HTTPClient.h>
#define OLED_RESET   D4//  4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define ONE_WIRE_BUS_1 D6 //D6 on NodeMCU
int drySoilValue = 400;

#else
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#include <WebServer.h>
WebServer httpServer(80);
#include <ESP32httpUpdate.h>
#include <Update.h>
#include <ESPmDNS.h>
#define OLED_RESET   -1 //2 // Reset pin # (or -1 if sharing Arduino reset pin)
int D7 = 13;
int D5 = 5;
int A1 = 33;
int D2 = 2;
int D4 = 4;
#define ONE_WIRE_BUS_1 15
int drySoilValue = 3000;

#endif

#include <BME280I2C.h>
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

#include "Adafruit_Si7021.h" //legacy sensors. the BME chip is better
Adafruit_Si7021 si7021 = Adafruit_Si7021();
bool si7021onBoard;
float siTemp(NAN), siHumi(NAN);

#include <DallasTemperature.h>
OneWire oneWire_in(ONE_WIRE_BUS_1);
DallasTemperature DS18B20_Sensor(&oneWire_in);

//#include <SPI.h>

#include <DNSServer.h>

#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


#include <WiFiClient.h>
WiFiClient client;
String ipAddr;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//#include "I2Cdev.h"
#include "MPU6050.h"

bool metric = false;
double tcTemp;

int NoDhtData = 0;

int ledPin = 2; //D4

//I started this project using the popular DHT sensor. I moved away from it, but the nameing remians for now. Sorry!
time_t lastDHTread;
float Dht22Humi, Dht22Temp = -196;  // Values read from sensor
float bmeTemp(NAN), bmeHumi(NAN), bmePres(NAN);
float oldTemp;
time_t oldTempTime = now();
//ADC_MODE(ADC_VCC); //vcc read
//#define ESP_INIT_DATA_ENABLE_READADC

String apName;
MPU6050 accelgyroIC1(0x68);

int i = 0;
int d = 0;
bool first100LoopCompleted = false;
int16_t accelArray[100];
int16_t gyroArray[100];
float accelArrayAvg, gyroArrayAvg;
int16_t ax, ay, az, gx, gy, gz, accel1, gyro1;
int16_t axb, ayb, azb, gxb, gyb, gzb;
int16_t axa, aya, aza, ax1, ay1, az1;
int16_t gxa, gya, gza, gx1, gy1, gz1;
float AccelTemp, Probe1, Probe2;
String chipId;
char apNameCharBuf[100];

long vdd = 0;
int16_t analogVal;
bool bme280onBoard;
String webPage;
String line1;//, line2, line3, line4, line5;


#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;  // Declare an instance of the ADS1115

int16_t rawADCvalue;  // The is where we store the value we receive from the ADS1115
float scalefactor = 0.1875F; // This is the scale factor for the default +/- 6.144 Volt Range we will use
float volts; // The result of applying the scale factor to the raw value
float vout = 0.0;
float vin = 0.0;
float R1 = 1100000.0; //30000  
float R2 = 145000.0; //7500 
//float vRef = 5;
#define NUM_SAMPLES 100 // number of analog samples to take per reading
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number


//Nextion
#include "Nextion.h"
#include <ESPNexUpload.h>

void bt0PopCallback(void *ptr);

//Declare object [page id:0,component id:3, component name: "b1"].

NexText t0 = NexText(0, 1, "t0");

NexButton b0 = NexButton(0, 2, "b0");
NexButton b1 = NexButton(0, 3, "b1");

NexDSButton r1 = NexDSButton(0, 4, "r1");
NexDSButton r2 = NexDSButton(0, 5, "r2");
NexDSButton r3 = NexDSButton(0, 6, "r3");
NexDSButton r4 = NexDSButton(0, 7, "r4");
NexDSButton bt0 = NexDSButton(0, 8, "bt0");

char buffer[100] = {0};

NexTouch *nex_listen_list[] =
{
  &t0,
  &b0,
  &b1,
  &r1,
  &r2,
  &r3,
  &r4,
  &bt0,
  NULL
};
