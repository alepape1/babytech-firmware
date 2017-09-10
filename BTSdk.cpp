#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <base64.h>
#include "BTSdk.h"

/**
 * Credentials setter
 * @param string
 * @param array 9,2
 * @return String
 */
String postDeviceReadings(String uid, String body[READINGS_LENGTH][2]) {
  WiFiClientSecure client;
  base64 b64;
  int port = 443;
  char *host = "babytech.herokuapp.com";
  //String path = "/devices/" + b64.encode("/test/") + "/readings";
  String path = "/devices/" + b64.encode(uid) + "/readings";
  String endPoint = "https://babytech.herokuapp.com";
  char *fingerprint = "F5 69 8C CA 29 68 5E 47 26 38 C5 1A 18 F1 8A 6A EA 60 56 4D";
  String formattedBody;
  String response;

  for (int i = 0; i < READINGS_LENGTH; i++) {
    formattedBody += "\"" + body[i][0] + "\":";
    formattedBody += (body[i][0] == "position") ? ("\"" + body[i][1]  + "\"") : body[i][1];
    formattedBody += (i == READINGS_LENGTH - 1) ? "" : ",";
  }

  formattedBody = "{" + formattedBody + "}";

  if (!client.connect(host, port)) {
    return String("Error: Connection failed");
  }

  if (!client.verify(fingerprint, host)) {
    return String("Error: Certificate invalid");
  }

  client.print(
    String("POST " + String(path) + " HTTP/1.1\r\n") +
    String("Host: " + String(host) + "\r\n") +
    String("Content-Type: application/json\r\n") +
    String("Connection: close\r\n") +
    String("Content-Length: " + String(formattedBody.length()) + "\r\n\r\n") +
    String(formattedBody + "\n")
  );

  while(client.connected()){
    response += client.readStringUntil('\r');
  }

  return response;
}
