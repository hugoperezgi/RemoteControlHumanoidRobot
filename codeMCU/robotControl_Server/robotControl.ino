#include "secrets.h"
#include "servoCalibration.h"

#define FORMAT_LEN 6
#define SERVOPAIR_LEN 4
#define SERVOCOUNT_OFFSET 3
#define SERVOID_OFFSET 5
#define SERVOPOS_OFFSET 7

#include <Arduino.h>
#include <WiFi.h>

#include <Adafruit_PWMServoDriver.h>

WiFiClient client;
String query = "";
uint8_t i = 0;
uint8_t cServoID = 0;
uint32_t flag = 0;

unsigned long t0=0;

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);

inline uint8_t getServoID(){return (query[SERVOID_OFFSET+(i*SERVOPAIR_LEN)] -1);}
inline uint8_t getServoPos(){return (query[SERVOPOS_OFFSET+(i*SERVOPAIR_LEN)] -1);}

void handleQuery(){

  Serial.println("Dispatch time (Pre-check):"+String((millis()-t0))+"ms");
  if(!(query.startsWith("-")&&query.endsWith("-!"))){
    client.print("NACK0"); /* Invalid query format */
    return;
  }
  Serial.println("Dispatch time (Post-check):"+String((millis()-t0))+"ms");
  switch(query.charAt(1)){
    case 'm': /* Move servos */
      // Serial.println("Move Servos");
      Serial.println("Dispatch time (Pre-ifs):"+String((millis()-t0))+"ms");
      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK2");break;} /* Servocounnt missmatch */
      Serial.println("Dispatch time (Post-ifs):"+String((millis()-t0))+"ms");
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){
        cServoID=getServoID();
        // pwm0.setPWM(cServoID,0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) );   

        if(cServoID<6){/* LeftHand Servo Controls (pwm board 0) - Servos 0-5 (6) */
          // pwm0.setPWM(cServoID,0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) ); 
          Serial.println("Servoid:"+String(cServoID)+"Pos:"+String(getServoPos()));
				}else if(cServoID<12){ /* RightHand Servo Controls (pwm board 2) - Servos 6-11 (6) */
          // pwm2.setPWM((cServoID-6),0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) );   
          Serial.println("Servoid:"+String(cServoID)+"Pos:"+String(getServoPos()));
				}else{ /* Body/Head Servo Controls (pwm board 1) - Servos 12-26 (15) */
          // pwm1.setPWM((cServoID-12),0, getServoPos() * ((maxServoPos[cServoID]-minServoPos[cServoID])/180) );   
          Serial.println("Servoid:"+String(cServoID)+"Pos:"+String(getServoPos()));
				}

        delayMicroseconds(50);
      }
          Serial.println("Dispatch time:"+String((millis()-t0))+"ms");
      client.print("ACK");
      break;
    case 'u': /* Update target positions (For delayed mode) */
      Serial.println("Update positions");
      if(query.length()<(FORMAT_LEN+SERVOPAIR_LEN*query[SERVOCOUNT_OFFSET])){client.print("NACK1");break;} /* Would be Out of bounds */
      if(query[SERVOCOUNT_OFFSET]>SERVOCOUNT){client.print("NACK2");break;} /* Servocounnt missmatch */
      for(i=0;i<query[SERVOCOUNT_OFFSET];i++){
        cServoID=getServoID();
        flag|=(0b1<<cServoID);
        targetServoPos[cServoID]=getServoPos();
        Serial.println("Servoid:"+String(cServoID)+"Pos:"+String(targetServoPos[cServoID]));
      }
      client.print("ACK");
      break;
    case 'e': /* Execute all movements (Delayed mode) */
      Serial.println("Execute movements");
      if(flag==0){client.print("NACK3");break;} /* No movements loaded - Should never happen but still */
      for(i=0;i<SERVOCOUNT;i++){
        if((flag&(0b1<<i))==((0b1<<i))){
          // pwm0.setPWM(i,0, targetServoPos[i] * ((maxServoPos[i]-minServoPos[i])/180) );   

          if(i<6){/* LeftHand Servo Controls (pwm board 0) - Servos 0-5 (6) */
            pwm0.setPWM(i,0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }else if(i<12){ /* RightHand Servo Controls (pwm board 2) - Servos 6-11 (6) */
            pwm2.setPWM((i-6),0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }else{ /* Body/Head Servo Controls (pwm board 1) - Servos 12-26 (15) */
            pwm1.setPWM((i-12),0, getServoPos() * ((maxServoPos[i]-minServoPos[i])/180) );   
          }
            
          delayMicroseconds(50);
        }
      }
      flag=0;
      client.print("ACK");
      break;
    default:
      client.print("NACK5"); /* Invalid query format */
      break;
  }

}

void setup() {
  Serial.begin(9600);

  // WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSSW);

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

  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(SERVO_FREQ); 
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
      for(i=0;i<SERVOCOUNT;i++){
        pwm0.setPWM(i,0,minServoPos[i]);
        if(i<6){/* LeftHand Servo Controls (pwm board 0) - Servos 0-5 (6) */
          pwm0.setPWM(i,0,minServoPos[i]);   
        }else if(i<12){ /* RightHand Servo Controls (pwm board 2) - Servos 6-11 (6) */
          pwm2.setPWM((i-6),0,minServoPos[i]);   
        }else{ /* Body/Head Servo Controls (pwm board 1) - Servos 12-26 (15) */
          pwm1.setPWM((i-12),0,minServoPos[i]);   
        }
      }
    }
  }
  /* Control loop */
  query="";
  if(client.available()){
    t0=millis();
    do{
      query+=(char)client.read();
    }while(client.available());
    Serial.println(query);
    Serial.println("Dispatch time (Pre-handle):"+String((millis()-t0))+"ms");
    handleQuery();

  }

}
