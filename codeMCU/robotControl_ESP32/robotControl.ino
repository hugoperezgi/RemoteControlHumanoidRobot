#include "secrets.h"
#include "servoCalibration.h"
// #include <WiFi.h>
#include <Firebase.h>
  Firebase fb(REFERENCE_URL, AUTH_TOKEN);

#include <Adafruit_PWMServoDriver.h>
  Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
  Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);


void setup() {
  Serial.begin(9600);
  // WiFi.disconnect();
  // delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);
  // WiFi.setSleep(false);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(WiFi.waitForConnectResult());
    // WiFi.reconnect();
    delay(1000);
  }

  pwm0.begin();
  pwm0.setOscillatorFrequency(27000000);
  pwm0.setPWMFreq(SERVO_FREQ); 

  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(SERVO_FREQ); 
  // fb.setInt("currentPosition/servo0",29);
}

void loop() {

    if(fb.getBool(/* TODO whatever control code*/)){
        int cc=fb.getInt(/* TODO whatever control code*/);
        for (int i=0; i < 27; i++){
            if((cc & (0b1<<i))==(0b1<<i)){
                /* Whatever code - Store positions */
            }
        }
        for (int i=0; i < 27; i++){
            if((cc & (0b1<<i))==(0b1<<i)){
                /* Whatever code - Move servos */
            }
        }
        
    }
    delay(1000);

}
