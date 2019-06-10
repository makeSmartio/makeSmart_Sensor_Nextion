/*
  This sketch is based on the default WifiClient example that comes with the Arduino ESP core.
  And was adapted for use with the ESPNexUpload library.

  You need to change wifi cred, host(server) and url (file name)

  Created on: 19 Dec 2018
  by Onno Dirkzwager
*/


/*
  ESP8266 uses Software serial RX:5, TX:4 Wemos D1 mini RX:D1, TX:D2 
  ESP32 uses Hardware serial RX:16, TX:17
  Serial pins are defined in the ESPNexUpload.cpp file
*/

const char* fileName       = "/makeSmart.tft";
bool updated          = false;



void nextionUpdate(){

  if(!updated){
    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)){
      Serial.println("connection failed");
      return;
    }
  
    Serial.print("Requesting URL: ");
    Serial.println(fileName);
    
  
    // This will send the (get) request to the server
    client.print(String("GET ") + fileName + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // Scan reply header for succes (code 200 OK) and the content lenght
    int contentLength;
    int code;
    while(client.available()){
  
      String line = client.readStringUntil('\n');
      //Serial.println(line); // Read all the lines of the reply from server and print them to Serial
      
      if(line.startsWith("HTTP/1.1 ")){
        line.remove(0, 9);
        code = line.substring(0, 3).toInt();
  
        if(code != 200){
          line.remove(0, 4);
          Serial.println(line);
          break;
        }
  
      }else if(line.startsWith("Content-Length: ")){
        line.remove(0, 16);
        contentLength = line.toInt();
  
      }else if(line == "\r"){
        line.trim();
        break;
      } 
    }

    // Update the nextion display
    if(code == 200){
      Serial.println("File received. Update Nextion...");

      bool result;

      // initialize ESPNexUpload
      ESPNexUpload nextion(115200);

      // set callback: What to do / show during upload.... Optional!
      nextion.setUpdateProgressCallback([](){
        Serial.print(".");
      });
      
      // prepare upload: setup serial connection, send update command and send the expected update size
      result = nextion.prepareUpload(contentLength);
      
      if(!result){
          Serial.println("Error: " + nextion.statusMessage);
      }else{
          Serial.print(F("Start upload. File size is: "));
          Serial.print(contentLength);
          Serial.println(F(" bytes"));
          
          // Upload the received byte Stream to the nextion
          result = nextion.upload(client);
          
          if(result){
            updated = true;
            Serial.println("\nSuccesfully updated Nextion!");
          }else{
            Serial.println("\nError updating Nextion: " + nextion.statusMessage);
          }

          // end: wait(delay) for the nextion to finish the update process, send nextion reset command and end the serial connection to the nextion
          nextion.end();
      }
    }
  
    Serial.println("Closing connection\n");
  }
}
