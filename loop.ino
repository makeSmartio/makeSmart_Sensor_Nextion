void loop() {

  if(shouldReboot){
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }

  
  Serial.print(SensorName);

  rssi = WiFi.RSSI();
  Serial.print("\t RSSI:");
  Serial.print(rssi);

  secsSinceLastSend = now() - lastSend;
  secsSinceLastAlert = now() - lastAlertTime;

  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setCursor(0, 0);

  if (SensorName.length() == 0)
    display.println("makeSmart():" + chipId + "." + ThisRockVersion);
  else
    display.println("makeSmart(" + SensorName.substring(0, 10) + ")");

  //if (Probe2 == -196.00)
  //  display.println(sensorMode + " ver:1." + ThisRockVersion);

  checkProbes();

  if (Probe1 > -195)
  {
    display.print("Probe1: "); display.print(Probe1); display.println(" Deg");
    Serial.print("\t Probe1: "); Serial.print(Probe1);
  }

  if (Probe2 > -195)
  {
    display.print("Probe2: "); display.print(Probe2); display.println(" Deg");
    Serial.print("\t Probe2: "); Serial.print(Probe2);
  }

  int d5 = digitalRead(D5); 
  Serial.print("\t D5: ");
  Serial.print(d5);
  if (d5 == 0)
  {
    d++;
    //if (d == 1) //reset after 1 run
    {
      writeEEPROM(10, "Reset button.");
      display.println("Resetting sensor, keep holding to erase wifi settings");
      display.display();
      delay(1000);
      ESP.restart();
    }
  }

  nexLoop(nex_listen_list);

  ADC0readVoltage();
  //delay(500);
 
  if (sensorMode == "Soil")
    checkSoil();
  else if (sensorMode == "Air")
  {
    analogVal = analogRead(A0);
    Serial.print("\t A0 value: ");
    Serial.print(analogVal);
    if (analogVal < 500)
    {
      display.print("Air polution detected!!!");
      if (now() - lastAirPolutionAlert > NotifyEverySeconds)
      {
        lastAirPolutionAlert = now();
        sendData("Air Polution detected", String(analogVal));
      }
    }
  }
  else
  {
    analogVal = analogRead(A0);
    Serial.print("\t A0 value: ");
    Serial.print(analogVal);
    if (analogVal > 400)
    {
      display.print("Water detected!!!");
      if (now() - lastWaterAlert > NotifyEverySeconds)
      {
        sendData("Water", String(analogVal));
      }
    }
  }

  thermoCouple();

  if (si7021onBoard)
  {
    si7021getReadings();
  }
  
  if (bme280onBoard)
  {
    BME280Data();
  }
  
  if (Dht22Temp == oldTemp) //if the temperature sensor stops updating, reboot after an hour
  {
    if (now() - oldTempTime > 12000)
    {
      sendData("Rebooting", "true");
      writeEEPROM(10, "Dht22Temp == oldTemp");
      ESP.restart();
    }
    else
    {
      oldTemp = Dht22Temp;
      oldTempTime = now();
    }
  }
  if (Dht22Temp > warnAboveDHTTemp && (now() - lastTooHotAlert > NotifyEverySeconds))
  {
    sendData("TooHot", String(Dht22Temp));
  }
  if (Dht22Temp < warnBelowDHTTemp && Dht22Temp != -196.00  && (now() - lastTooColdAlert > NotifyEverySeconds))
  {
    Serial.println(Dht22Temp);
    sendData("TooCold", String(Dht22Temp));
  }
  Serial.print("\ttemperatureSensor:\t"); Serial.print(Dht22Temp); Serial.print("\t"); Serial.print(Dht22Humi); //Serial.print("\t");

  //line1 = + "Temperature: " + String(Dht22Temp);

  if (Dht22Temp != 0)
  {
    display.print(Dht22Temp); display.print(" Deg "); display.print(Dht22Humi); display.println("%");
  }

  if (tcTemp > 0)
  {
    display.print("Grill: "); display.print(tcTemp); display.println(" Deg");
    Serial.print("\t Grill: "); Serial.print(tcTemp);
    Probe2=tcTemp;
  }
  
  if (trackMotion == 1)
  {
    getAccelGyroData();
    if (sensorMode == "Dryer")
      checkDryer();

    if (sensorMode == "Washer")
      checkWasher();

    if ((accel1 > 1000 || gyro1 > 1000 || vdd > 1000) && now() - lastMotionAlert > 60)
    { //GunSafe mode
      lastMotionAlert = now();
      display.println("Motion detected!!!");
      display.print("A/G:"); display.print(AccelTemp); display.print(" Deg "); display.print(accel1); display.print(":"); display.println(gyro1);
      display.display();
      sendData("Motion", "");
    }

    display.print("  A/G: "); display.print(accel1); display.print(":"); display.println(gyro1);
    display.print("x: "); display.print(ax); display.print(" y:"); display.print(ay); display.print(" z:"); display.println(gz);
    
  }//trackMotion

  if (secsSinceLastSend > sendFrequency) //only send data every x seconds
  {
    if (keepDataPoint == 1) //keep every other timer data point
    {
      keepDataPoint = 0;
    }
    else
    {
      keepDataPoint = 1;
    }
    sendData("Timer", String(keepDataPoint));
  }
  Serial.println("");
  
  //Nextion
  if (doUpdateNextion == true)
    nextionUpdate();

  nexLoop(nex_listen_list);

  int n = String(Probe1).length(); 
  char char_array[n + 1]; 
  strcpy(char_array, String(Probe1).c_str());       
  t0.setText(char_array);

  n = String(ipAddr).length(); 
  char_array[n + 1]; 
  strcpy(char_array, String(ipAddr).c_str());       
  t2.setText(char_array);

  n = String(voltage).length(); 
  char_array[n + 1]; 
  strcpy(char_array, String(voltage).c_str());       
  t4.setText(char_array);

//  if (Probe1 > relay1OnTemp)
//    digitalWrite(relay1Pin, LOW);
//
//  if (Probe1 < relay1OffTemp)
//    digitalWrite(relay1Pin, HIGH);
    
  if (digitalRead(relay1Pin)==true)
  {
    r1.setValue(false);
    relay1State=true;
    //Serial.println("Relay1 Off");
  }
  else
  {
    r1.setValue(true);
    relay1State=false;
    Serial.println("Relay1 On");
  }

  if (digitalRead(relay2Pin)==true)
  {
    r2.setValue(false);
    relay2State=true;
  }
  else
  {
    r2.setValue(true);
    relay2State=false;
  }

  if (digitalRead(relay3Pin)==true)
  {
    r3.setValue(false);
    relay3State=true;
  }
  else
  {
    r3.setValue(true);
    relay3State=false;
  }

  if (digitalRead(relay4Pin)==true)
  {
    r4.setValue(false);
    relay4State=true;
  }
  else
  {
    r4.setValue(true);
    relay4State=false;
  }

  display.display();

  Serial.print("\t Last Send: ");
  Serial.print(secsSinceLastSend);
  Serial.print("\t Last Alert: ");
  Serial.println(secsSinceLastAlert);

  delay(loopDelay);
}
