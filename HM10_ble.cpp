#include <Arduino.h>
#include "SoftwareSerial.h"
#include "HM10_ble.h"
#define BUFFER_SIZE   40
#include "db.h"

SoftwareSerial bluetooth(D3, D4); // RX, TX

// constants won't change. Used here to set a pin number :
uint8_t ledState;
const int ledPin =  13; 

bool flag_mac=0;

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 1000;  


String getbattlevel(){
  String response="";
  int level;
  bluetooth.print("AT+BATT?"); // here comes the magic
  delay(1000);
  response=HM10answer();
  response+="\0";
  if(response.startsWith("OK+Get:")){
    
      return response.substring(7,10);
    
    }else{
      
      return "error";
      }   
  }


void blink_led(){

  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}
  
  

String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}




String HM10discoverdevices(){

String response="";
char a;
response.reserve(BUFFER_SIZE);
bluetooth.print("AT+DISC?"); // here comes the magic
delay(100);

int thelastime=millis();
while((millis()-thelastime)<3500){
    while(bluetooth.available()>0){
    
  a=bluetooth.read();
   //Serial.print(a);
  response+=a;
    }
}
  response+='\0';
   Serial.print(response); 
   //delay(1000); 
  
  return response;
  }



String HM10answer(){

String response="";
char a;
response.reserve(BUFFER_SIZE);
//String response;

    while(bluetooth.available()>0){
    
  a=bluetooth.read();
   //Serial.print(a);
  response+=a;
 
  }
  response+='\0';
  delay(20);
//     Serial.print(response);  
  
  return response;
  }



uint8_t BLE_init(){


 String response="";
 String ok="OK";
 
 response.reserve(BUFFER_SIZE);


  bluetooth.begin(57600);
  bluetooth.print("AT");
  Serial.print(HM10answer());
  bluetooth.print("AT+RESET");
  Serial.print(HM10answer());
  bluetooth.print("AT+BAUD0");
  Serial.print(HM10answer());
  
  bluetooth.begin(9600);
 
// un REM this to set up a Master and connect to a Slave

//Version HM -10 
  bluetooth.print("AT");
  Serial.print(HM10answer());
  bluetooth.print("AT+RESET");
  Serial.print(HM10answer());
  bluetooth.print("AT+ROLE1"); // select master = central
  Serial.print(HM10answer());
     bluetooth.print("AT+RESET"); // actually more a restart than a reset .. needed after ROLE
  Serial.print(HM10answer());
//   bluetooth.print("AT+SLEEP"); // actually more a restart than a reset .. needed after ROLE
//  Serial.print(HM10answer());
  bluetooth.print("AT+CONNL"); // Try to connect to last succeded device
  response=HM10answer();
  response+="\0";
  Serial.print(response);
  delay(1500);
  bluetooth.print("AT+IMME1"); // "work immediately", not sure what this does
  Serial.print(HM10answer());
  
   return 1;

//Version HM-10 fake 

//  Serial.println("BLE CC41A Bluetooth");
//  Serial.println("----------------------------------");
//  Serial.println("");
//  Serial.println("Trying to connect to Slave Bluetooth");
//  delay(1000);
//  bluetooth.println("AT"); // just a check
//  Serial.print(HM10answer());
// // response=HM10answer();
////  Serial.print(response);
////  if(response.compareTo("OK")){
////    Serial.println("BLE detected");
////    }else{
////      Serial.println("BLE no detected");
////      return 1;
////      }
//  delay(2000);
//  bluetooth.println("AT+ROLE1"); // st up as Master
//  Serial.println(HM10answer());
//  delay(2000);
//  bluetooth.println("AT+INQ"); // look for nearby Slave
//  Serial.println(HM10answer());
//  delay(5000);
//  bluetooth.println("AT+CONN1"); // connect to it
//  Serial.println(HM10answer());
 // pinMode(wakePin, INPUT);
  //attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW

    
}


uint8_t BLE_configuration(String& mac_ble){

 String response="";
 String ok="OK";
 //String head="[{\"; 
 response.reserve(BUFFER_SIZE);
 bluetooth.begin(9600);
// un REM this to set up a Master and connect to a Slave

//Version HM -10 
 bluetooth.print("AT"); // check if working, always returns OK
  bluetooth.print("AT+ROLE1"); // select master = central
  Serial.print(HM10answer());
     bluetooth.print("AT+RESET"); // actually more a restart than a reset .. needed after ROLE
  Serial.print(HM10answer());
//   bluetooth.print("AT+SLEEP"); // actually more a restart than a reset .. needed after ROLE
//  Serial.print(HM10answer());
  bluetooth.print("AT+SHOW1"); // include Bluetooth name in response
  Serial.print(HM10answer());
  bluetooth.print("AT+IMME1"); // "work immediately", not sure what this does
  Serial.print(HM10answer());
  bluetooth.print("AT+FILT0"); // show all BLE devices, not only HM ones
  delay(1000); // wait a bit, NECESSARY!!
  Serial.print(HM10answer());
  response=HM10discoverdevices();
  response+="\0";
  Serial.println("Devices availables:");
  Serial.println(response);
   int index=response.indexOf("DIS0:",1);
   
  if (response.startsWith("DIS0",index)) {
  
   // mac_ble+= String("{");
   // mac_ble+=String("\"devices\":[")+response.substring(index+5,index+17)+"\", \""+"name\":\""+ response.substring(index+25,index+37)+" \"";
    mac_ble+=String("\"devices\":[");
    mac_ble+="\""+response.substring(index+5,index+17)+"\"";
   // Serial.println("MAC1 EEPROM:");
   // Serial.println(mac_ble);
    eppromSet("mac1",response.substring(index+5,index+17));
    index=response.indexOf("DIS0:",index+5);
    if(response.startsWith("DIS0",index)){
    mac_ble+="\"" + response.substring(index+5,index+17)+ "\"";
    //\""+"name\":\"" + response.substring(index+25,index+37)+" \"";
      //Serial.println("MAC2 EEPROM:");
      eppromSet("mac2",response.substring(index+5,index+17));
      //Serial.println(mac_ble.substring(26));
      //Serial.println(mac_ble);
      mac_ble+=String("]");
      //mac_ble+=String("}");
      Serial.println("Two Devices were found");
      return 2;
      }else{
           Serial.println("One device was found");
            mac_ble+=String("]");
       //     mac_ble+=String("}");
           return 1;
        }
  }else{
    Serial.println("No devices found");
    return 0;
    }
 
   
}

uint8_t BLE_connection(String& mac,String sensor_data[],uint8_t devices_n){


String str_con;
String response;
String question="";
char c;

if(devices_n==2){
      if(flag_mac==0){
  
    Serial.println("Conectando MAC1");
    mac=eppromGet("mac1");
    flag_mac=1;

  }else if(flag_mac==1){
    
    Serial.println("Conectando MAC2");
    mac=eppromGet("mac2");
    flag_mac=0;
    
  }}else{
    
    Serial.println("Conectando MAC");
    mac=eppromGet("mac1");
    
    }
    bluetooth.print("AT");
    Serial.println(HM10answer());
    str_con="AT+CON"+mac;
    //Serial.println(mac);
    bluetooth.print(str_con);
    delay(350);                     //This delay of 350ms is strictly necesary
    //Serial.println(HM10answer());
    
    //Serial.println(HM10answer());
    
    readbabysensor(HM10answer(),sensor_data);
  
    
  return 1;
  }


void HM10sendstring(String string_sensors){
  
 
  string_sensors.reserve(BUFFER_SIZE);

  bluetooth.print(string_sensors);

  string_sensors="";
  return;
  }


//void readbabysensor(String *oxygen,String *beat,String *angle,String *temp_b){
void readbabysensor(String BLE_data,String sensor_data[]){
char buffer[BUFFER_SIZE];


char separator=',';
//uint8_t wet_baby;
char inChar;
int index,i;
String wet;
// buffer.reserve(BUFFER_SIZE); 
 

//Imprime la variable con los caracteres acumulados hasta la ","   
   index=0;
   while(inChar!='$' && index<BLE_data.length()){
     inChar = BLE_data[index];
     index++;
   }
   if(inChar =='$'){
    Serial.println("Encontrado");
    i=0;
   while (inChar!='&' && index<BLE_data.length()){
    inChar = BLE_data[index];
    buffer[i]=inChar;
    index++;
    i++;
    }
    
    if(buffer[i-1]=='&' && i<BUFFER_SIZE){
    
    buffer[i-1]='\0';
    //prueba=getValue(buffer,separator,1);
    //Serial.println(prueba);
    //Serial.print("Lectura: ");
    //Serial.println(buffer); 
   
    //humidy=getValue(buffer,separator,1);
     sensor_data[0]=getValue(buffer,separator,0);      //wet
     sensor_data[1]=getValue(buffer,separator,1);      //Oxygen
     sensor_data[2]=getValue(buffer,separator,2);      //beat
     sensor_data[3]=getValue(buffer,separator,3);      //angle
     sensor_data[4]=getValue(buffer,separator,4);      //Baby Temperature
     sensor_data[5]=getValue(buffer,separator,5);      //Skin contact flag 
//      wet_baby=wet.toInt();
//      Serial.print(wet);
//      Serial.print(",");
//      Serial.println(wet_baby);
//     *oxygen=getValue(buffer,separator,1);    //Oxygen
//     *beat=getValue(buffer,separator,2);      //beat
//     *angle=getValue(buffer,separator,3);     //angle
//     *temp_b=getValue(buffer,separator,4);    //Baby Temperature
      
    } 
   }else{
     
     // Serial.println(buffer);     
      Serial.println("No encontrado");
          //Borra la variable string para almacenar nuevos datos   
     for(i=0;i<BUFFER_SIZE;i++){
      buffer[i]='\0';
      }
   }
      
 }
