#include "secrets.h"
#include "servoCalibration.h"
#include <stdint.h>
#include <Firebase.h>
  Firebase fb(REFERENCE_URL, AUTH_TOKEN);

#include <Adafruit_PWMServoDriver.h>
  Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
  Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
//   Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

uint8_t[] currentPostition[27],targetPosition[27];
float[] servoSteps[27];
int[] servoMin[27];
uint8_t i,j;

void setup() {
  Serial.begin(9600);
  // WiFi.disconnect();
  // delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);
  // WiFi.setSleep(false);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
	Serial.print("Stuck at wifi connection: ");
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

//   pwm2.begin();
//   pwm2.setOscillatorFrequency(27000000);
//   pwm2.setPWMFreq(SERVO_FREQ); 
}

void loop() {

    if(fb.getBool("NEWINFO")){ 
        int cc=fb.getInt("SERVOCONTROL"); 
        for (i=0; i < 27; i++){ 
            if((cc & (0b1<<i))==(0b1<<i)){
				targetPosition[i]=(uint8_t)fb.getInt("targetPosition/"+String(i));
            }
        }
        for (i=0; i < 27; i++){ /* Move servos */
            if((cc & (0b1<<i))==(0b1<<i)){
				if(i<6){
					//TODO - array con min/step positions!!
					pwm0.setPWM(i,0,servoMin[i] + servoSteps[i]*targetPosition[i]);delay(10);
				}else if(i<12){
					j=i-6;
					//TODO - RightHand servo control (pwm board 2) - Servos 6-11 (6)
					// pwm2.setPWM(j,0,targetPosition[i]);delay(10);
				}else{
					j=i-12;
					//TODO - Body servo control (pwm board 1) - Servos 12-26 (15)
					// pwm1.setPWM(j,0,targetPosition[i]);delay(10);
				}
            }
        }
        
    }
	// if(fb.getBool(/* TODO UPDATE control code */)){ 
	// 	//Move the UpdatePositions here Maybe-idk?
	// }
    delay(1000);

}
