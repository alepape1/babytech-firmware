#include <Arduino.h>
#include <EEPROM.h>

String eppromGet(String key) {
  int ssidMemoryRange[2] = { 0, 32};
  int passMemoryRange[2] = {32, 64};
  int mac1MemoryRange[2] = {64, 96};
  int mac2MemoryRange[2] = {96, 128};
  int range[2];
  String value;

  if (key == "ssid") {
    range[0] = ssidMemoryRange[0];
    range[1] = ssidMemoryRange[1];
  } else if (key == "pass") {
    range[0] = passMemoryRange[0];
    range[1] = passMemoryRange[1];
  } else if (key == "mac1") {
    range[0] = mac1MemoryRange[0];
    range[1] = mac1MemoryRange[1];
  }
   else if (key == "mac2") {
    range[0] = mac2MemoryRange[0];
    range[1] = mac2MemoryRange[1];
  }else {
    return value;
  }

  for (int i = range[0]; i < range[1]; i++) {
    value += char(EEPROM.read(i));
  }

  return value.c_str();
}

void eppromSet(String key, String value) {
  int ssidMemoryRange[2] = { 0, 32};
  int passMemoryRange[2] = {32, 64};
  int mac1MemoryRange[2] = {64, 96};
  int mac2MemoryRange[2] = {96, 128};
  int range[2];

  if (key == "ssid") {
    range[0] = ssidMemoryRange[0];
    range[1] = ssidMemoryRange[1];
  } else if (key == "pass") {
    range[0] = passMemoryRange[0];
    range[1] = passMemoryRange[1];
  } else if (key == "mac1") {
    range[0] = mac1MemoryRange[0];
    range[1] = mac1MemoryRange[1];
  }
   else if (key == "mac2") {
    range[0] = mac2MemoryRange[0];
    range[1] = mac2MemoryRange[1];
  } else {
    return;
  }

  for (int i = range[0]; i < range[1]; i++) {
    EEPROM.write(i, 0);

    if ((i - range[0]) < value.length()) {
      EEPROM.write(i, value[i - range[0]]);
    }
  }

  EEPROM.commit();
}


