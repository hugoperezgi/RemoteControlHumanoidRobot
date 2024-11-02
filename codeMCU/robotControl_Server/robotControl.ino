#include "secrets.h"
#include "servoCalibration.h"

#include <Arduino.h>
#include <stdint.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
// #include <WiFiClient.h>

#include <Adafruit_PWMServoDriver.h>

//Server ip:port
const uint16_t port = SERVER_PORT;
const char *host = SERVER_IP;
//Wifi data
const char *ssid = WIFI_SSID;  
const char *password = WIFI_PASSWORD;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
