#include "secrets.h"
// #include "servoCalibration.h"

#include <Arduino.h>
#include <stdint.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
// #include <WiFiClient.h>

// #include <Adafruit_PWMServoDriver.h>

WiFiClient client;
String query = "";

void wrongFormat(){
  client.print("NACK");
}

void handleQuery(){
  switch(query.charAt(1)){
    case 'm': /* Move servos */
      Serial.println("Move Servos");
      client.print("ACK");
      break;
    case 'u': /* Update target positions (For delayed mode) */
      Serial.println("Update positions");
      client.print("ACK");
      break;
    case 'e': /* Execute all movements (Delayed mode) */
      Serial.println("Execute movements");
      client.print("ACK");
      break;
    default:
      wrongFormat();
      break;
  }

}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("Stuck at wifi connection: ");
    Serial.println(WiFi.waitForConnectResult());
    // WiFi.reconnect();
    delay(1000);
  }

  // pwm0.begin();
  // pwm0.setOscillatorFrequency(27000000);
  // pwm0.setPWMFreq(SERVO_FREQ); 

  // pwm1.begin();
  // pwm1.setOscillatorFrequency(27000000);
  // pwm1.setPWMFreq(SERVO_FREQ); 

  // pwm2.begin();
  // pwm2.setOscillatorFrequency(27000000);
  // pwm2.setPWMFreq(SERVO_FREQ); 
}

void loop() {
  /* Check server connection - reconnect if necessary */
  if(!client.connected()){
    if (!client.connect(SERVER_IP, SERVER_PORT)){
      Serial.println("Connection to server failed");
      delay(1000);
      return;
    }else{
      /* Say hello to server and stop reseting the connection */
      client.print("!s-NodeMCU_here-smartMCU-\6-\1-\1-\1-\1-\1-\1-e!");
    }
  }
  /* Control loop */
  query="";
  if(client.available()){
    query = client.readStringUntil('\n');
    Serial.println(query);

    (query.startsWith("-")&&query.endsWith("-!")) ? handleQuery() : wrongFormat();
  }

}
