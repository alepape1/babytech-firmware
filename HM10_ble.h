#ifndef HM10_ble_H
#define HM10_ble_H

#include "HM10_ble.h"


//BLE Hm10 init
uint8_t BLE_init();
uint8_t BLE_configuration(String& mac_ble);
uint8_t BLE_connection(String& mac,String sensor_data[],uint8_t devices_n);
String HM10answer();
String HM10discoverdevices();
String getbattlevel();
void blink_led();
void HM10sendstring(String string_sensors);
//void readbabysensor(String *oxygen,String *beat,String *angle,String *temp_b);
void readbabysensor(String BLE_data,String sensor_data[]);
String getValue(String data, char separator, int index);
#endif
