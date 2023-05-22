void ADC0readVoltage()
{
  int16_t rawADC0value;  // The is where we store the value we receive from the ADS1115
  float volts; // The result of applying the scale factor to the raw value
  float vout = 0.0;
  int sum = 0;                    // sum of samples taken
  int sample_count = 0; // current sample number
  int NUM_SAMPLES = 50; // number of analog samples to take per reading
  
  float scalefactor = 0.1875F; // This is the scale factor for the default +/- 6.144 Volt Range we will use
  float R1 = 1000000.0; //30000  
  float R2 = 150000.0; //7500 


 rawADC0value = ads.readADC_SingleEnded(0); 
 while (sample_count < NUM_SAMPLES) {
      sum += (ads.readADC_SingleEnded(0) * scalefactor);
      sample_count++;
      delay(10);
  }
  vout = ((float)sum / (float)NUM_SAMPLES)/1000;
  sample_count = 0;
  sum = 0;

  voltage = vout / (R2/(R1+R2)); 

  volts = (rawADC0value * scalefactor)/1000.0;

  Serial.print("\tRaw ADC0 Value = "); 
  Serial.print(rawADC0value); 
  Serial.print("\tVoltage Measured = ");
  Serial.print(volts,2);

  Serial.print("  V= ");
  Serial.println(voltage);
  display.print("Voltage Sensor: "); display.println(voltage);
}
void checkSoil()
{
  {
    //delay(1000);
    int sum = 0;                    // sum of samples taken
    int sample_count = 0; // current sample number
    int NUM_SAMPLES = 1; // number of analog samples to take per reading

     analogVal = ads.readADC_SingleEnded(1); 
     while (sample_count < NUM_SAMPLES) {
      sum += (ads.readADC_SingleEnded(1));// * scalefactor);
      sample_count++;
      delay(10);
    }
    analogVal = ((float)sum / (float)NUM_SAMPLES);
    sample_count = 0;
    sum = 0;
    Serial.print("\t Soil value: ");
    Serial.print(analogVal);
    display.print("Soil Sensor: "); display.println(analogVal);
    if (analogVal > drySoilValue)
    {
      Serial.print("Dry soil detected!!!");
      if (now() - lastSoilAlert > NotifyEverySeconds)
      {
        Serial.println("Sending Dry Soil Alert");
        sendData("DrySoil", String(analogVal));
      }
    }
  }
}
void checkProbes()
{
  getDS18B20Temp();
  if (Probe1 > warnAboveProbe1  && (now() - lastTooHotAlert > NotifyEverySeconds) && Probe1 != 185)
  {
    sendData("TooHot", "Probe1: " + String(Probe1));
  }
  if (Probe1 < warnBelowProbe1 && Probe1 != -196.00  && (now() - lastTooColdAlert > NotifyEverySeconds))
  {
    sendData("TooCold", "Probe1: " + String(Probe1));
  }
  if (Probe2 > warnAboveTemp2  && (now() - lastTooHotAlert > NotifyEverySeconds) && Probe2 != 185)
  {
    sendData("TooHot", "Probe2: " + String(Probe2));
  }
  if (Probe2 < warnBelowTemp2 && Probe2  != -196.00  && (now() - lastTooColdAlert > NotifyEverySeconds))
  {
    sendData("TooCold", "Probe2: " + String(Probe2));
  }
}
void r1PopCallback(void *ptr)
{
    dbSerialPrintln("r1PopCallback");
    relayToggle(relay1Pin);
}
void r2PopCallback(void *ptr)
{
    dbSerialPrintln("r2PopCallback");
    relayToggle(relay2Pin);
}
void r3PopCallback(void *ptr)
{
    dbSerialPrintln("r3PopCallback");
    relayToggle(relay3Pin);
}
void r4PopCallback(void *ptr)
{
    dbSerialPrintln("r4PopCallback");
    relayToggle(relay4Pin);
}
void relayToggle(int relayNumber)
{
    if (digitalRead(relayNumber)==1)
    {
      Serial.println(relayNumber);
      digitalWrite(relayNumber, LOW);
    }
    else
    {
      Serial.println(relayNumber);
      digitalWrite(relayNumber, HIGH);
    }

}void writeEEPROM(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
  Serial.println(data);
}

String readEEPROM(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

void resetWifi() {
  //WiFiManager wifiManager;
  //DNSServer dns;
  //AsyncWiFiManager wifiManager(&httpServer,&dns);
  //wifiManager.resetSettings();
  //WiFi.disconnect(false,true);
  Serial.println("Reset Wifi done.");
  
  //WiFi.disconnect(true);   // still not erasing the ssid/pw. Will happily reconnect on next start
  WiFi.begin("0","0");
  #if defined(ESP8266)
    WiFi.disconnect(true);
  #else
    WiFi.disconnect(true,true);
  #endif
  delay(2000);
  
}
void thermoCouple()
{
   //Serial.print("Max Temp = ");
   //Serial.println(thermocouple.readInternal());

   //tcTemp = thermocouple.readFarenheit();
   if (isnan(tcTemp)) {
     //Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print(" tc: "); 
     Serial.print(tcTemp);
   }
}
void   si7021getReadings()
{
  //float hum, temp;
  siHumi = si7021.readHumidity();
  // Measure Temperature from the HTU21D or Si7021
  siTemp = si7021.readTemperature() * 1.8 + 32;
  Serial.print("\tsi7021temp: ");
  Serial.print(siTemp);
  Serial.print("\tHum: ");
  Serial.print(siHumi);

  if (siTemp > -51) //avoid erronios values
  {
    Dht22Temp = siTemp;
    Dht22Humi = siHumi;
  }

}

void checkWasher()
{
  if ((washerRunning == false) && (accel1 > washerSensitivity || gyro1 > washerSensitivity))
  {
    washerStarting++;
    Serial.println(); Serial.print(washerStarting); Serial.println(":washerStarting");
    if (washerStarting > 20) {
      Serial.println("Cycle Started");
      washerStarting = 0;
      washerRunning = true;
      washerRunningSince = now();
      sendData("Cycle Started", "");
    }
  }
  else if ((washerRunning == false) && (accel1 < washerSensitivity))
  { //decrement accidental increments
    washerStarting = washerStarting - 1;

    Serial.print(washerStarting); Serial.println(":decrement accidental increments");
    if (washerStarting < 1) {
      washerStarting = 0; //never started
    }
  }
  else if ((washerRunning == true) && (accel1 > washerSensitivity))
  { //still running, decrement accidental increments
    washerStopping = washerStopping - 1;
    if (washerStopping < 1)
    {
      Serial.println("Washer clear: 0");
      washerStopping = 0;
    }
  }
  else if (washerRunning == true)
  { //must be stopping
    washerStopping++;
    Serial.println("Wahser Stopping: "); Serial.println(washerStopping);
    if (washerStopping > 20)
    {
      washerRunning = false;
      sendData("Cycle Stopped", "");
    }
  }
}
void checkDryer()
{

  if ((dryerRunning == false) && (accel1 > dryerSensitivity || gyro1 > dryerSensitivity))
  {
    dryerStarting++;
    Serial.println(); Serial.print(dryerStarting); Serial.println(":dryerStarting");
    if (dryerStarting > 20) {
      Serial.println("Dryer Started");
      sendData("Cycle Started", "");
      dryerStarting = 0;
      dryerRunning = true;
      dryerRunningSince = now();
    }
  }
  else if (( dryerRunning == false) && (accel1 < dryerSensitivity && gyro1 < dryerSensitivity))
  { //decrement accidental increments
    dryerStarting = dryerStarting - 1;

    Serial.print(dryerStarting); Serial.println(":decrement accidental increments");
    if (dryerStarting < 1) {
      dryerStarting = 0; //never started
    }
  }
  else if (( dryerRunning == true) && (accel1 > dryerSensitivity || gyro1 > dryerSensitivity))
  { //still running, decrement accidental increments
    dryerStopping = dryerStopping - 1;
    if (dryerStopping < 1)
    {
      Serial.println("Dryer clear: 0");
      dryerStopping = 0;
    }
  }
  else if (dryerRunning == true)
  { //must be stopping
    dryerStopping++;
    Serial.println("Dryer Stopping: "); Serial.println(dryerStopping);
    if (dryerStopping > 20)
    {
      dryerRunning = false;
      dryerStopping = 0;
      if (now()-dryerRunningSince > minDryerCycleTime) //don't send an alert on short cycles
      {
        sendData("Cycle Stopped", "");
      }
    }
  }
}
int sendData(String alertType, String message)
{
  String sendAlert = "true";
  Serial.println("SendData:");
  Serial.print("\t AlertType: ");
  Serial.print(alertType);
  Serial.println("\t" + sensorMode);
  if (alertType == "Timer")
  {
    sendAlert = "false";
  }
  else if (alertType == "Motion" && sendMotionAlerts == 1 && (now() - lastMotionAlert > NotifyEverySeconds))
  {
    lastMotionAlert = now();
    sendAlert = "true";
  }
  else if (alertType == "Motion" && sendMotionAlerts == 0)
  {
    sendAlert = "false";
  }
  else if (alertType == "DrySoil")
  {
    lastSoilAlert = now();
    sendAlert = "true";
  }
  else if (alertType == "Startup" || alertType == "DryerStopped" || alertType == "DryerStarted")
  {
    sendAlert = "true";
  }
  else if (alertType == "Water")
  {
    sendAlert = "true";
    lastWaterAlert = now();
  }
  else if (alertType == "TooHot")
  {
    sendAlert = "true";
    lastTooHotAlert = now();
  }
  else if (alertType == "TooCold")
  {
    sendAlert = "true";
    lastTooColdAlert = now();
  }
  else if (alertType == "GyroError")
  {
    sendAlert = "true";
  }
  else
  {
    Serial.println("\t hit else statement in sendData function");
    sendAlert = "true";
  }

  String data;
  data = "RockID=" + String(chipId) + "&DhtTemp=" + String(Dht22Temp) + "&DhtHumi=" + String(Dht22Humi) ;
  data += "&Voltage=" + String(voltage) + "&WaterPresent=" + String(analogVal) + "&temp1=" + String(Probe1) + "&temp2=" ;
  data += String(Probe2) + "&accel1=" + String(accel1) + "&AccelTemp=" + String(AccelTemp);
  data += "&gyro1=" + String(gyro1) + "&accel2=" + String(accelArrayAvg) + "&gyro2=" + String(gyroArrayAvg) + "&DateTime=TIMESTAMP";
  data += "&SendAlert=" + sendAlert + "&AlertType=" + String(alertType) + "&SensorName=" + SensorName;
  data += "&Version=" + String(ThisRockVersion) + "&Note=" + message + "&sensorMode=" + sensorMode;
  data += "&RSSI=" + String(rssi);
  data += "&privateIp=" + String(ipAddr);
  data += "&pressure=" + String(bmePres);

  Serial.print("\t Data:");
  Serial.println(data);

  if (client.connect(host, httpPort))
  {
    client.println("POST /addSend2.php HTTP/1.1");
    client.println("Host: " + String(host)); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);

    delay(100);

    String streamData;
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      //Serial.print(line);
      streamData = streamData + line;
    }
    Serial.println("StreamData: " + streamData);
    Serial.println("Complete");
  }
  else 
  {
    Serial.println("Not Connected");
    display.clearDisplay();
    display.setCursor(0, 0);
    //display.print("Wifi lost, connect to\n" + apName);
    display.print("Rebooting");
    display.display();
    writeEEPROM(10, "Lost Wifi. Restart.");
    ESP.restart();
    /*WiFiManager wifiManager;
    wifiManager.setTimeout(120);
    wifiManager.setAPStaticIPConfig(IPAddress(10, 10, 10, 10), IPAddress(), IPAddress(255, 255, 255, 0));
    wifiManager.autoConnect(apNameCharBuf, apPassword);
    //wifiManager.autoConnect(apNameCharBuf);
*/
    //add beep code
  }

  if (client.connected()) {
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  lastSend = now();
  if (sendAlert == "true")
  {
    lastAlertTime = now();
  }
  GetParamsFromWeb();
  return 1;
}

void GetParamsFromWeb()
{
  Serial.print("connecting to ");
  Serial.println(String(host) + ":" + String(httpPort));

  if (!client.connect(host, httpPort)) {
    Serial.println("GetParamsFromWeb connection failed");
    if (NotifyEverySeconds < 60)
    {
      NotifyEverySeconds = 6000;
    }
    SensorName = "GetParamsFromWebConnectionFailed";
    return;
  }

  // We now create a URI for the request
  String url = "/SensorConfigerJSON.php?";
  url += "RockID=";
  url += chipId;
  url += "&version=";
  url += ThisRockVersion;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    String streamData;
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        //Serial.print(line);
        streamData = streamData + line;
    }

  Serial.println("StreamData: " + streamData);

  int descLocation = streamData.indexOf("Desc") - 2;
  Serial.println(descLocation);
  streamData = streamData.substring(descLocation);

  StaticJsonDocument<800> doc;
  DeserializationError error = deserializeJson(doc, streamData);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    SensorName = "parseObjectFailed";
    return;
  }
 
  const char* Desc = doc["Desc"];
  Serial.print("SensorName: "); Serial.println(Desc);
  const char* Version = doc["Version"];
  Serial.print("Version: "); Serial.println(Version);
  NotifyEverySeconds = doc["NotifyEvery"];
  NotifyEverySeconds = NotifyEverySeconds * 60; //convert to seconds
  Serial.print("NotifyEverySeconds: "); Serial.println(NotifyEverySeconds);
  const char* Mode = doc["Mode"];
  Serial.print("Mode: "); Serial.println(Mode);
  const char* AlphaOrBetaTemp = doc["AlphaOrBeta"];
  AlphaOrBeta = String(AlphaOrBetaTemp);
  Serial.print("AlphaOrBeta: "); Serial.println(AlphaOrBeta);

  const char* warnAboveDHTTempCast = doc["warnAboveDHTTemp"];
  warnAboveDHTTemp = String(warnAboveDHTTempCast).toInt();
  Serial.print("warnAboveDHTTemp: "); Serial.print(warnAboveDHTTemp);

  const char* warnBelowDHTTempCast = doc["warnBelowDHTTemp"];
  warnBelowDHTTemp = String(warnBelowDHTTempCast).toInt();
  Serial.print("\twarnBelowDHTTemp: "); Serial.println(warnBelowDHTTemp);

  const char* warnAboveProbe1Cast = doc["warnAboveTemp1"];
  warnAboveProbe1 = String(warnAboveProbe1Cast).toInt();
  Serial.print("warnAboveProbe1: "); Serial.print(warnAboveProbe1);

  const char* warnBelowProbe1Cast = doc["warnBelowTemp1"];
  warnBelowProbe1 = String(warnBelowProbe1Cast).toInt();
  Serial.print("\t warnBelowProbe1: "); Serial.println(warnBelowProbe1);

  const char* warnAboveTemp2Cast = doc["warnAboveTemp2"];
  warnAboveTemp2 = String(warnAboveTemp2Cast).toInt();
  Serial.print("warnAboveTemp2: "); Serial.print(warnAboveTemp2);

  const char* warnBelowTemp2Cast = doc["warnBelowTemp2"];
  warnBelowTemp2 = String(warnBelowTemp2Cast).toInt();
  Serial.print("\t warnBelowTemp2: "); Serial.println(warnBelowTemp2);

  const char* relay1OnTempCast = doc["relay1OnTemp"];
  relay1OnTemp = String(relay1OnTempCast).toInt();
  Serial.print("\t relay1OnTemp: "); Serial.println(relay1OnTemp);

  const char* relay1OffTempCast = doc["relay1OffTemp"];
  relay1OffTemp = String(relay1OffTempCast).toInt();
  Serial.print("\t relay1OffTemp: "); Serial.println(relay1OffTemp);

  const char* trackMotionCast = doc["trackMotion"];
  if (String(trackMotionCast)=="true" || String(trackMotionCast)=="1")
    {trackMotion = 1;}
  else
    {trackMotion = 0;}
  Serial.print("\t trackMotion: "); Serial.println(trackMotion);

  const char* sendMotionAlertsCast = doc["sendMotionAlerts"];
  Serial.println(sendMotionAlertsCast);
  if (String(sendMotionAlertsCast) == "true" || String(sendMotionAlertsCast) == "1")
    {sendMotionAlerts = 1;}
  else
    {sendMotionAlerts = 0;  }
  //sendMotionAlerts = bool(sendMotionAlertsCast);
  Serial.print("\t sendMotionAlerts: "); Serial.println(sendMotionAlerts);

  const char* sendFrequencyCast = doc["sendFrequency"];
  sendFrequency = String(sendFrequencyCast).toInt();
  Serial.print("\t sendFrequency: "); Serial.println(sendFrequency);
  if (sendFrequency<10)
    sendFrequency=300;

  SensorName = String(Desc);

  if (SensorName.length() < 2)
  {
    SensorName = "Connected to Wifi, but not able to connect to the web. Are you blocked?";
    Serial.println(SensorName);
  }
  else if (Desc == "NewSensor")
  {
    SensorName = "Register your device: makeSmart.io with your RockID: " + chipId;
    Serial.println(SensorName);
    sendData("Not registered: " + chipId, "");
  }
  else
  {
    sensorMode = Mode;
  }

  float NewVersion = String(Version).toFloat();
  Serial.print("NewVersion: ");
  Serial.println(NewVersion);
  Serial.print("RunningVersion: ");
  Serial.println(ThisRockVersion);
  if (NewVersion > ThisRockVersion)
  {
    Serial.println("Update needed.");
    //sendData("Updating", Version);
    Serial.println("Updating...");
    delay(10000);
    updateFirmware();
  }

  Serial.println("Finshed");
  return;
}

void updateFirmware()
{
  #if defined(ESP8266)
    String fileName = "/" + AlphaOrBeta + "_8266.bin";
  #else
    String fileName = "/" + AlphaOrBeta + "_32.bin";
  #endif
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Updating Sensor");
  display.display();
  Serial.print("Starting web update to:");
  Serial.println(String(host) + ":" + String(httpPort) + String(fileName));
  delay(1000);

  //t_httpUpdate_return ret = ESPhttpUpdate.update("10.0.0.2", "8000", "/sketch_6050_wifi_web_update.ino.bin", "optional current version string here");
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(host, httpPort, fileName, "optional current version string here");
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      //Serial.println("[update] Update failed.");
      Serial.println("HTTP_UPDATE_FAILD Error (%d): %s" + ESPhttpUpdate.getLastError() + String(ESPhttpUpdate.getLastErrorString().c_str()));
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[update] Update no Update.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("[update] Update ok."); // may not called we reboot the ESP
      break;
    }
    
 }

void getDS18B20Temp()
{
  //DS18B20 Temp Sensor
  DS18B20_Sensor.requestTemperatures();
  Probe1 = DS18B20_Sensor.getTempCByIndex(0) * 1.8 + 32.0;
  if (Probe1 < -195)
    Probe1 = -196;

  Probe2 = DS18B20_Sensor.getTempCByIndex(1) * 1.8 + 32.0;
  if (Probe2 < -195)
    Probe2 = -196;    
}
void getAccelGyroData()
{
  accelgyroIC1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  AccelTemp = (accelgyroIC1.getTemperature() / 340.00 + 36.53) * 1.80 + 32.0;

  //Find the difference in value from last read
  ax1 = axb - ax; ay1 = ayb - ay; az1 = azb - az;
  axa = abs(ax1); aya = abs(ay1); aza = abs(az1);
  accel1 = axa + aya + aza;

  gx1 = gxb - gx; gy1 = gyb - gy; gz1 = gzb - gz;
  gxa = abs(gx1); gya = abs(gy1); gza = abs(gz1);
  gyro1 = gxa + gya + gza;

  //Serial.print("\t accelTemp: ");
  //Serial.print(AccelTemp);
  Serial.print("\t accel1: ");
  Serial.print(accel1);
  Serial.print("\t gyro1 ");
  Serial.print(gyro1);
  if (accel1 < 0)
  {
    accel1 = accel1 * -1;
  }
  if (gyro1 < 0)
  {
    gyro1 = gyro1 * -1;
  }

  if (String(AccelTemp) == String(97.75) && trackMotion == 1 && sensorMode=="Security") { //lost connection with MPU6050
    accelgyroIC1.initialize(); //try reconnecting
    Serial.print("GYRO ERROR");
    display.clearDisplay();
    display.print("Gyro Error");
    display.display();
    if (now() - lastGyroError > 6000)
    {
      lastGyroError = now();
      sendData("GyroError", ""); 
    }
    delay(1000);
    //ESP.restart();
  }

  axb = ax; ayb = ay; azb = az;
  gxb = gx; gyb = gy; gzb = gz;
}

void BME280Data()
{
  //float bmeTemp(NAN), bmeHumi(NAN), bmePres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Fahrenheit);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(bmePres, bmeTemp, bmeHumi, tempUnit, presUnit);

  Serial.print("\tbme280Temp: ");
  Serial.print(bmeTemp);
  Serial.print("\tHumidity: ");
  Serial.print(bmeHumi);
  Serial.print("% RH");
  Serial.print("\tPressure: ");
  Serial.print(bmePres);
  Serial.print(" Pa");

  if (bmeTemp > -51) //avoild erronios values
  {
    Dht22Temp = bmeTemp;
    Dht22Humi = bmeHumi;
  }
}
