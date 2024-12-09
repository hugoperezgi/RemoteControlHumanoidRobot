#include "secrets.h"
#include "servoCalibration.h"
#include <stdint.h>
#include <ArduinoJson.h>
#include <Firebase.h>
  Firebase fb(REFERENCE_URL, AUTH_TOKEN);

#include <Adafruit_PWMServoDriver.h>
  Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
  Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
//   Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

uint8_t currentPostition[27],targetPosition[27];
uint8_t i,j;
int cc;
unsigned long t0=0,t1=0;

JsonDocument jsonDB;

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

  // pwm1.begin();
  // pwm1.setOscillatorFrequency(27000000);
  // pwm1.setPWMFreq(SERVO_FREQ); 

//   pwm2.begin();
//   pwm2.setOscillatorFrequency(27000000);
//   pwm2.setPWMFreq(SERVO_FREQ); 
}

float med=0; int count=0;

void loop() {
  t0=millis();
  deserializeJson(jsonDB,fb.getJson(""));
  t1=millis();
  Serial.println(String((uint8_t)jsonDB["targetPosition"][0]));
  count++;
  med += ((t1-t0)-med)/count ;
  Serial.print("Running average: "+ String(med));
  Serial.print("ms | Count: "+String(count)+"\n");
  /** TODO - Implement min/max delay AND Jitter */

    // if(fb.getBool("FlagNewInfo")){ 

		// // cc=fb.getInt("ServoUpdateFlag"); 

		// // for (i=0; i < 27; i++){
		// // 	if((cc & (0b1<<i))==(0b1<<i)){
		// // 		targetPosition[i]=(uint8_t)fb.getInt("targetPosition/"+String(i));
		// // 	}
    //     // }
    //     // Serial.println(fb.getString("Test"));

		// fb.setBool("FlagNewInfo",false);
  
    // }
    // if(cc!=0){
    //     if(fb.getBool("FlagMoveServo")){ 

    //         for (i=0; i < 27; i++){ 
    //             if((cc & (0b1<<i))==(0b1<<i)){
    //                 if(i<6){/* LeftHand Servo Controls */
    //                     pwm0.setPWM(i,0,minServoPos[i]+targetPosition[i]*(maxServoPos[i]-minServoPos[i])/180);delay(10);
    //                 }else if(i<12){ /* LeftHand Servo Controls */
    //                     j=i-6;
    //                     //TODO - RightHand servo control (pwm board 2) - Servos 6-11 (6)
    //                     // pwm2.setPWM(j,0,minServoPos[i]+targetPosition[i]*(maxServoPos[i]-minServoPos[i])/180);delay(10);
    //                 }else{ /* Body/Head Servo Controls */
    //                     j=i-12;
    //                     //TODO - Body servo control (pwm board 1) - Servos 12-26 (15)
    //                     // pwm1.setPWM(j,0,minServoPos[i]+targetPosition[i]*(maxServoPos[i]-minServoPos[i])/180);delay(10);
    //                 }
    //             }
    //         }

    //         fb.setBool("FlagMoveServo",false);
    //         fb.setInt("ServoUpdateFlag",0);
    //         cc=0;
    //     }
	// }
    // delay(50);

}
