#include <Adafruit_ADXL345_U.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
//#include <TimerOne.h>
#include "BTSdk.h"
#include "db.h"
#include <Wire.h>
#include <BH1750.h>
#include "ClosedCube_HDC1080.h"
#include "HM10_ble.h"
#include "String.h"
#define time_sensor     200
#define REPORTING_PERIOD_MS     3000


ESP8266WebServer server(80);
BH1750 lightMeter;
ClosedCube_HDC1080 hdc1080;

const String ssid = "iPhone de Alejandro";
const String pass = "alepape159";
//const String uuid = "00:A3:G2:65:T5:55";
//const String uuid = "test";
const String hash = "MDA6QTM6RzI6NjU6VDU6NTU=";

uint32_t tsLastReport = 0;
uint32_t lastime=0;

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
//unsigned long tiempo;
uint16_t lux;
float temp;
float humidy;
//int wet_baby;
//String oxy;
//String temp_baby;
//String rate;
//String angle;
 String mac_ble;
 String mac_connected;
 String sensor_data[7];
 String response;
 int noise;
 int noise_detect;
 uint8_t devices_n;


String get_MAC_device(){

  uint8_t MAC_array[6];
  char MAC_char[18];

        WiFi.macAddress(MAC_array);
    for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
    }
    //String MACdevice(MAC_char);
      Serial.println(MAC_char);
      return MAC_char;
  
  }

void setup() {
  
 
  mac_connected.reserve(15);
  mac_ble.reserve(70);
  response.reserve(60);
  ESP.wdtDisable();
  Serial.begin(115200);                   //Serial port init
  EEPROM.begin(4096);                     //EEPROM init 4KB
  Wire.begin(D2,D1);
  do{
   WiFi.begin(ssid.c_str(), pass.c_str());   //Init internet connection SSID and PASS from EEPROM
   delay(50);
   } while(WiFi.status()!= WL_CONNECTED && (millis() % 3000 == 0));
  //Serial.println(eppromGet("mac1"));        //Last MAC BLE connected
  //WiFi.begin(ssid.c_str(), pass.c_str());   //Init internet connection SSID and PASS from EEPROM
 // delay(1000); 
  BLE_init();
  //delay(1000);
  devices_n=BLE_configuration(mac_ble);
 // delay(1000);
  Serial.println("Devices founded:");
  Serial.print(devices_n);
  Serial.println("Valor de mac_ble:");
  Serial.println(mac_ble);
  Serial.println(eppromGet("ssid"));
  Serial.println(eppromGet("pass"));
  lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);                //Init ligth sensor station
  hdc1080.begin(0x40);                                              //Init humidity and temperature sensor in station
  
  //delay(5000);
  
//  MAC_char.reserve(18);

 // delay(1000);

 
   //WiFi.begin(eppromGet("ssid"), eppromGet("pass")));  
   WiFi.softAP("Babytech Station");
 

   delay(2000);
  
 

 // Timer1.initialize(500000);
 // Timer1.attachInterrupt(send_data_server());
  //Initialize sensors


 //wet_baby.reserve(8);
//  temp_baby.reserve(8);
//  oxy.reserve(8);
//  rate.reserve(8);
//  angle.reserve(8);

  


//
//  server.on("/networks", []() {
//    String responseBody;
//    int networksCount = WiFi.scanNetworks();                        //Scannig networks availables
//
//    for (int i = 0; i < networksCount; i++) {
//      responseBody += (i != 0) ? "," : "";
//      responseBody += "\"" + String(WiFi.SSID(i)) + "\"";          //Server response with the SSID found
//    }
//
//    return server.send(200, "application/json", "[" + responseBody + "]");
//  });

//  server.on("/status", []() {
//    String responseBody;
//    bool connected = (WiFi.status() == WL_CONNECTED);
//
//    responseBody += String("\"connected\":" + String(connected ? "true" : "false"));
//    responseBody += String(",\"mac_address\":\"test\",\"password\":\"test\"");
//
//    return server.send(200, "application/json", "{" + responseBody + "}");
//  });

 server.on("/status", []() {
    String responseBody;
    int networksCount = WiFi.scanNetworks();
    bool connected = (WiFi.status() == WL_CONNECTED);

    responseBody.reserve(300);
    
    devices_n=BLE_configuration(mac_ble);
    
    responseBody += String("{");
    responseBody += String("\"connected\":" + String(connected ? "true" : "false") + ",");
    //responseBody += String("\"devices\":[\"00:A3:G2:65:T5:55\", \"AA:00:00:00:00:00\", \"BB:00:00:00:00:00\"],"); // This is a mock, you should scan all the BTOOTH devices
    responseBody += mac_ble;
    responseBody += String("\"networks\":[");
    for (int i = 0; i < networksCount; i++) {
      responseBody += (i != 0) ? "," : "";
      responseBody += "\"" + String(WiFi.SSID(i)) + "\"";
    }
    responseBody += String("]");
    responseBody += String("}");
    
    return server.send(200, "application/json", responseBody);
  }); 




/**
   * Receives a mac_address from the user app
   * and stores it to the eeprom
   */
//  server.on("/configure-device", []() {
//    String responseBody;
//    String mac1 = server.arg("mac1");
//    String mac2 = server.arg("mac2");
//    
//    if (!mac1.length() || !mac2.length()) {
//      responseBody += "{\"error\":\"A mac is required\"}";
//      return server.send(400, "application/json", responseBody);
//    }
//
//    eppromSet("mac1", mac1);
//    eppromSet("mac2", mac2);
//
//    responseBody += "{\"success\": true}";
//    server.send(201, "application/json", responseBody);
//  });

  /**
   * Retrieves a list of all the available
   * devices
   */
 // server.on("/devices", []() {
 //   String responseBody;
 //   uint8_t devices_n;
    // Get all ble devices and send them to
    // the app as a JSON String

//    if (BLE_configuration(mac_ble)==0) {
//      responseBody += "{\"error\":\"No device found\"}";
//      return server.send(400, "application/json", responseBody);
//    }else{
     //   responseBody += "{\"mac\":\"001122334455\"}";
//      responseBody +=mac_ble;
  //  server.send(200, "application/json", responseBody);
  ///  }
//  });



// server.on("/settings", []() {
//    String responseBody;
//    String ssid = server.arg("ssid");
//    String pass = server.arg("pass");
//    String mac1 = server.arg("mac1");
//    String mac2 = server.arg("mac2");
//
//   
//    
//    if (!ssid.length() || !pass.length()) {
//      responseBody += "{\"error\":\"SSID and PASS are required\"}";
//      return server.send(400, "application/json", responseBody);
//    }
//
//    if (!mac1.length() && !mac2.length()) {
//      responseBody += "{\"error\":\"At least a mac is required\"}";
//      return server.send(400, "application/json", responseBody);
//    }
//
//    if (mac1.length()) {
//      eppromSet("mac1", mac1);
//    }
//
//    if (mac2.length()) {
//      eppromSet("mac2", mac2);
//    }
//
//    eppromSet("ssid", ssid);
//    eppromSet("pass", pass);
//    responseBody += "{\"ssid\":\"" + eppromGet("ssid") + "\",\"pass\":\"" + eppromGet("pass") + "\"}";
//    server.send(201, "application/json", responseBody);
//    WiFi.disconnect();
//    WiFi.begin(eppromGet("ssid").c_str(), eppromGet("pass").c_str());
//  });

  server.begin();                                                     //Server AP init
}

void loop() {


//readbabysensor();



//  
  

//        noise = analogRead(analogInPin);
//        //Serial.println("Noise:");
//        //Serial.print(noise);
//        //Serial.print(",");
//        if(noise_detect<noise){
//          noise_detect=noise;
//          }

       //Reading light sensor BH170
//      lux = lightMeter.readLightLevel();
//      delay(30);
// 
//      temp=hdc1080.readTemperature();
//   
//   
//      delay(time_sensor);
//      humidy=hdc1080.readHumidity();
//      delay(time_sensor);

      

            if ( (WiFi.status() == WL_CONNECTED) && (millis()-tsLastReport > REPORTING_PERIOD_MS) ){
     
      BLE_connection(mac_connected,sensor_data,devices_n);

 
    


      
       
          String body[10][2] = {
   //  {"battery_status", String("\"")+String("98")+String("\"")},
      {"temperature", String(temp, 1)},
      {"humidity",String( humidy, 1)},
      {"light", String(lux,DEC)},
//      {"nappy_wet", "2"},
//      {"oxygen_saturation", "98"},
//      {"beats_per_minute", "75"},
//      {"position", "20.5"},
//      {"temp_baby", "34.5"},
      {"nappy_wet", sensor_data[0]},
      {"oxygen_saturation", sensor_data[1]},
      {"beats_per_minute", sensor_data[2]},
      {"position", sensor_data[3]},
      {"temp_baby", sensor_data[4]},
      {"skin_contact", sensor_data[5]},
      {"noise", String(noise_detect,DEC)}
      //{"timestamp", String(123455)}
    };

      Serial.print("Light:"); 
      Serial.print(lux);
      Serial.print("Temperatura habitacion:");
      Serial.print(temp);
      Serial.print("Humedad:");
      Serial.print(humidy);
      Serial.print(" Noise: ");
      Serial.println(noise_detect);
      Serial.print("Nappy: ");
      Serial.print(sensor_data[0]);
      Serial.print("Oxigeno: ");
      Serial.print(sensor_data[1]);
      Serial.print(" Heart rate: ");
      Serial.print(sensor_data[2]);
      Serial.print(" Angle: ");   
      Serial.print(sensor_data[3]);
      Serial.print(" Temperature Baby: ");   
      Serial.println(sensor_data[4]);
      Serial.print(" Skin Contact: ");   
      Serial.println(sensor_data[5]);
  


      

      //uint32_t contador=millis();
      // La MacAddress aquí
      // Por cada vez que quieras escribir por dispositivo
      Serial.println("Enviando a...");
      Serial.print(mac_connected);
      response = postDeviceReadings(mac_connected.c_str(), body);
      Serial.println("[LOG]: Wrote " + String(response.startsWith("HTTP/1.1 201") ? "true" : "false"));
      noise_detect=0;
    //  Serial.println(millis()-contador);
       
       tsLastReport = millis();
  
    } else if (WiFi.status() != WL_CONNECTED && (millis() % 1000 == 0 )) {

     // delay(1000);
      Serial.println(".");    
  }
  

  server.handleClient();
 // delay(200);
}

/**
ESTE EJEMPLO COMPILA, 
FÍJATE QUE AHORA HAY QUE PASAR COMO PRIMER PARAMETRO
LA MAC DEL BABYDEVICE Y COMO SEGUNDO LAS LECTURAS QUE SE QUIERAN 
ENVIAR AL SERVIDOR

#include "BTSdk.h"

void setup() {
  String body[9][2] = {
    {"temperature", String("30")},
    {"humidity",String("30")},
    {"light", String("30")},
    {"nappy_wet", String("30")},
    {"oxygen_saturation", String("30")},
    {"beats_per_minute", String("30")},
    {"position", String("30")},
    {"temp_baby", String("30")},
    {"noise", String("30")}
  };
  
  String response = postDeviceReadings("00:A3:G2:65:T5:55", body);
}

void loop() {
  //
}
*/ 


