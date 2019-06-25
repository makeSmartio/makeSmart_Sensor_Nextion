//void notFound(AsyncWebServerRequest *request) {
//    request->send(404, "text/plain", "Not found");
//}

void buildWebsite() {

  buildJavascript();
  webSite = "<!DOCTYPE html>\n";
  webSite += "<title>" + SensorName + "</title>";
  webSite += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  webSite += "<link rel='shortcut icon' type='image/png' href='http://makeSmart.io/favicon.ico'/>";
  webSite += "<style>\n";
  webSite += ".button {\n";
  webSite += "  background-color: grey;\n";
  webSite += "  border: none;\n";
  webSite += "  color: white;\n";
  webSite += "  padding: 10px 14px;\n";
  webSite += "  text-align: center;\n";
  webSite += "  text-decoration: none;\n";
  webSite += "  display: inline-block;\n";
  webSite += "  font-size: 12px;\n";
  webSite += "  margin: 4px 2px;\n";
  webSite += "  cursor: pointer;}\n";
  webSite += "body {font-family: Sans-Serif;}";
  webSite += "</style>\n";
  webSite += javaScript;
  webSite += "<body onload='process()'>\n";
  webSite += "<h1>" + SensorName + "</h1> <h3>"+ipAddr+" on "+WiFi.SSID()+" RSSI: <a id='rssi'></a></h3>";
  webSite += "<p><button class='button' onclick=getURL(this) id=ledOn>Led On</button>";
  webSite += " <button class='button'onclick=getURL(this) id=ledOn>Led Off</button>";
  webSite += " <a href=\"sendAlert\"><button class='button'>Send Alert Test</button></a>";
  webSite += " <a href=\"update\"><button class='button'>Update Firmware</button></a>";
  webSite += " <a href=\"updateNextion\"><button class='button'>Update Nextion</button></a>";
  webSite += " <a href=\"resetWifi\"><button class='button'>Erase WIFI settings and reboot</button></a>";
  webSite += " <a href=\"reboot\"><button class='button'>Reboot</button></a></p>";
  
  webSite += " <p><button onclick=relayBtn(this) class='button' id=relay1>Relay1</button>";
  webSite += " <button onclick=relayBtn(this) class='button' id=relay2>Relay2</button>";
  webSite += " <button onclick=relayBtn(this) class='button' id=relay3>Relay3</button>";
  webSite += " <button onclick=relayBtn(this) class='button' id=relay4>Relay4</button>";

  webSite += "<p>Runtime = <a id='upTime'></a>\n";
  webSite += "Accel1 = <a id='accel1'></a>\n";
  webSite += "Gyro1 = <a id='gyro1'></a>\n";
  webSite += "A0 = <a id='A0'></a>\n";
   if (si7021onBoard)
  {
    webSite += "<br>si Temp = <a id='siTemp'></a>\n";
    webSite += "si Humi = <a id='siHumi'></a>\n";
  }
  if (bme280onBoard)
  {
    webSite += "bme Temp = <a id='bmeTemp'></a>\n";
    webSite += "bme Humi = <a id='bmeHumi'></a>\n";
    webSite += "bme Pressure = <a id='bmePres'></a>\n";
  }
  if (sensorMode=="Washer")
  {
    webSite += "washerRunning = <a id='washerRunning'></a>\n";
    webSite += "washerStarting = <a id='washerStarting'></a>\n";
    webSite += "washerStopping = <a id='washerStopping'></a>\n";
  }
  if (sensorMode=="Dryer")
  {
    webSite += "dryerRunning = <a id='dryerRunning'></a>\n";
    webSite += "dryerStarting = <a id='dryerStarting'></a>\n";
    webSite += "dryerStopping = <a id='dryerStopping'></a>\n";
  }
  if (Probe1 > -196) {
    webSite += "Probe1 = <a id='Probe1'></a>\n";
  }
  if (Probe2 > -196) {
    webSite += "Probe2 = <a id='Probe2'></a>\n";
  }
  webSite += "Access Point MAC = <a id='apMac'></a>\n";
  webSite += "<p>View data online: <a href=https://makesmart.io/data.php?pageType=Detail&RockID=" + chipId + ">https://makesmart.io/data.php?pageType=Detail&RockID=" + chipId + "</a>";
  webSite += "<iframe width='100%' height='500' frameborder='1' src='https://makeSmart.io/data.php?pageType=Detail&RockID=" + chipId + "'></iframe>\n";

  webSite += "</body>\n";
  webSite += "</html>\n";
}

void buildJavascript() {
  javaScript = "<script src=https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js></script>\n";
  javaScript += "<SCRIPT>\n";
  

  javaScript += "   function getURL(element) {\n";
  javaScript += "        $.ajax({\n";
  javaScript += "          url: '/'+element.id,\n";
  javaScript += "          type: 'POST',\n";
  javaScript += "        success:function(results) {$('#Status').html(results);}\n";
  javaScript += "      });\n";
  javaScript += "  };\n";

  javaScript += "   function relayBtn(element) {\n";
  javaScript += "        $.ajax({\n";
  javaScript += "          url: '/'+element.id,\n";
  javaScript += "          type: 'POST',\n";
  javaScript += "          data:({\n";
  javaScript += "            relay: element.id,\n";
  javaScript += "              toggle: true\n";
  javaScript += "          }),\n";
  javaScript += "        success:function(results) {$('#Status').html(results);}\n";
  javaScript += "      });\n";
  javaScript += "  };\n";
  
  javaScript += "var xmlHttp=createXmlHttpObject();\n";

  javaScript += "function createXmlHttpObject(){\n";
  javaScript += " if(window.XMLHttpRequest){\n";
  javaScript += "    xmlHttp=new XMLHttpRequest();\n";
  javaScript += " }else{\n";
  javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript += " }\n";
  javaScript += " return xmlHttp;\n";
  javaScript += "}\n";

  javaScript += "function process(){\n";
  javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript += "   xmlHttp.open('PUT','xml',true);\n";
  javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript += "   xmlHttp.send(null);\n";
  javaScript += " }\n";
  javaScript += " setTimeout('process()',1000);\n";
  javaScript += "}\n";

  javaScript += "function handleServerResponse(){\n";
  javaScript += " if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript += "   xmlResponse=xmlHttp.responseXML;\n";
  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('response');\n";
  javaScript += "   //message = xmldoc[0].firstChild.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('upTime')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('upTime').innerHTML=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('A0')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('A0').innerHTML=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('accel1')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('accel1').innerHTML=y.nodeValue;\n";
  
  javaScript += "   x = xmldoc[0].getElementsByTagName('gyro1')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('gyro1').innerHTML=y.nodeValue;\n";

  if (si7021onBoard)
  {
    javaScript += "   x = xmldoc[0].getElementsByTagName('siTemp')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('siTemp').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('siHumi')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('siHumi').innerHTML=y.nodeValue;\n";
  }

  if (bme280onBoard)
  {
    javaScript += "   x = xmldoc[0].getElementsByTagName('bmeTemp')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('bmeTemp').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('bmeHumi')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('bmeHumi').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('bmePres')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('bmePres').innerHTML=y.nodeValue;\n";
}
  if (sensorMode=="Washer")
  {
    javaScript += "   x = xmldoc[0].getElementsByTagName('washerRunning')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('washerRunning').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('washerStarting')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('washerStarting').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('washerStopping')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('washerStopping').innerHTML=y.nodeValue;\n";
  }
  
  if (sensorMode=="Dryer")
  {
    javaScript += "   x = xmldoc[0].getElementsByTagName('dryerRunning')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('dryerRunning').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('dryerStarting')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('dryerStarting').innerHTML=y.nodeValue;\n";

    javaScript += "   x = xmldoc[0].getElementsByTagName('dryerStopping')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('dryerStopping').innerHTML=y.nodeValue;\n";
  }

  javaScript += "   x = xmldoc[0].getElementsByTagName('rssi')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('rssi').innerHTML=y.nodeValue;\n";

  if (Probe1 > -196) {
    javaScript += "   x = xmldoc[0].getElementsByTagName('Probe1')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('Probe1').innerHTML=y.nodeValue;\n";
  }
  if (Probe2 > -196) {
    javaScript += "   x = xmldoc[0].getElementsByTagName('Probe2')[0];\n";
    javaScript += "   y = x.childNodes[0];\n";
    javaScript += "   document.getElementById('Probe2').innerHTML=y.nodeValue;\n";
  }

  javaScript += "   x = xmldoc[0].getElementsByTagName('apMac')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('apMac').innerHTML=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('relay1State')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('relay1').style.backgroundColor=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('relay2State')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('relay2').style.backgroundColor=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('relay3State')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('relay3').style.backgroundColor=y.nodeValue;\n";

  javaScript += "   x = xmldoc[0].getElementsByTagName('relay4State')[0];\n";
  javaScript += "   y = x.childNodes[0];\n";
  javaScript += "   document.getElementById('relay4').style.backgroundColor=y.nodeValue;\n";

  javaScript += " }\n";
  javaScript += "}\n";
  javaScript += "</SCRIPT>\n";
}

void buildXML() {
  XML = "<?xml version='1.0'?>";
  XML += "<response>";
  XML += "<upTime>";
  XML += millis2time();
  XML += "</upTime>";
//if (si7021onBoard)
    {
    XML += "<siTemp>";
    XML += siTemp;
    XML += "</siTemp>";
    XML += "<siHumi>";
    XML += siHumi;
    XML += "</siHumi>";
  }

  if (bme280onBoard)
  {
    XML += "<bmeTemp>";
    XML += bmeTemp;
    XML += "</bmeTemp>";
    XML += "<bmeHumi>";
    XML += bmeHumi;
    XML += "</bmeHumi>";
    XML += "<bmePres>";
    XML += bmePres;
    XML += "</bmePres>";
    XML += "<accel1>";
    XML += accel1;
    XML += "</accel1>";
    XML += "<gyro1>";
    XML += gyro1;
    XML += "</gyro1>";
   if (sensorMode=="Washer")
    {
    XML += "<washerRunning>";
    XML += washerRunning;
    XML += "</washerRunning>";
    XML += "<washerStarting>";
    XML += washerStarting;
    XML += "</washerStarting>";
    XML += "<washerStopping>";
    XML += washerStopping;
    XML += "</washerStopping>";
  }
if (sensorMode=="Dryer")
    {
    XML += "<dryerRunning>";
    XML += dryerRunning;
    XML += "</dryerRunning>";
    XML += "<dryerStarting>";
    XML += dryerStarting;
    XML += "</dryerStarting>";
    XML += "<dryerStopping>";
    XML += dryerStopping;
    XML += "</dryerStopping>";
  }
  }
  XML += "<rssi>";
  XML += rssi;
  XML += "</rssi>";
  XML += "<A0>";
  XML += analogVal;
  XML += "</A0>";
  XML += "<Probe1>";
  XML += Probe1;
  XML += "</Probe1>";
  XML += "<Probe2>";
  XML += Probe2;
  XML += "</Probe2>";
  
  XML += "<accel1>";
  XML += accel1;
  XML += "</accel1>";
  XML += "<gyro1>";
  XML += gyro1;
  XML += "</gyro1>";
  
  XML += "<washerRunning>";
  XML += washerRunning;
  XML += "</washerRunning>";
  XML += "<washerStarting>";
  XML += washerStarting;
  XML += "</washerStarting>";
  XML += "<washerStopping>";
  XML += washerStopping;
  XML += "</washerStopping>";

  XML += "<dryerRunning>";
  XML += dryerRunning;
  XML += "</dryerRunning>";
  XML += "<dryerStarting>";
  XML += dryerStarting;
  XML += "</dryerStarting>";
  XML += "<dryerStopping>";
  XML += dryerStopping;
  XML += "</dryerStopping>";

  XML += "<relay1State>";
  if (relay1State==true)
    XML += "grey";
  else
    XML += "green";
  XML += "</relay1State>";

  XML += "<relay2State>";
  if (relay2State==true)
    XML += "grey";
  else
    XML += "green";
  XML += "</relay2State>";

  XML += "<relay3State>";
  if (relay3State==true)
    XML += "grey";
  else
    XML += "green";
  XML += "</relay3State>";

  XML += "<relay4State>";
  if (relay4State==true)
    XML += "grey";
  else
    XML += "green";
  XML += "</relay4State>";

  XML += "<apMac>";
  XML += WiFi.BSSIDstr();
  XML += "</apMac>";
  XML += "</response>";
}
String millis2time() {
  String Time = "";
  unsigned long ss;
  byte mm, hh;
  ss = millis() / 1000;
  hh = ss / 3600;
  mm = (ss - hh * 3600) / 60;
  ss = (ss - hh * 3600) - mm * 60;
  if (hh < 10)Time += "0";
  Time += (String)hh + ":";
  if (mm < 10)Time += "0";
  Time += (String)mm + ":";
  if (ss < 10)Time += "0";
  Time += (String)ss;
  return Time;
}
