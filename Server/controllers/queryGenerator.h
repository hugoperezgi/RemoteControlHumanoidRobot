#include <stdint.h>
#include <iostream>

#define _NACK_InvalidQuery 255
#define _NACK_NoActiveMCU 254
#define _NACK_OnRTMode 253
#define _NACK_InvalidParameter 252

#define _ACK_Generic 255

#define _eMOD_RealTime 101
#define _eMOD_Delayed 100

class QueryGenerator{
    private:
    public:
        char* divideIntoBytes(uint16_t);
        QueryGenerator();
        ~QueryGenerator();

        /* Server -> MCU */

        /* Sends to the MCU the actual PWM signal for the desired step */
        char* dmb_mvServo(uint32_t,uint8_t*,uint16_t**);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal, MCU executes movement */
        char* smrt_mvServo(uint32_t,uint8_t*);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal */
        char* smrt_updtServo(uint32_t,uint8_t*);
        /* Executes the movement "loaded" with the updtServo query */
        char* smrt_mvAll();
        /* Asking the MCU the current positions */
        char* smrt_currPos(uint8_t,uint8_t);


        /* Server -> Cli */

        /* ACK query */
        char* ack(uint8_t);
        /* ACK query */
        char* nack(uint8_t);
        /* Tell the client the current positions of the MCU */
        char* servoPos(uint32_t, uint8_t*); 
        // /* Tell the client the current update (RealTime/Delayed) mode of the MCU */
        // char* updateMode(char);
};

