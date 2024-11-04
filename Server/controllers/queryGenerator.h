#include <stdint.h>
#include <iostream>

class QueryGenerator{
    private:
        char* divideIntoBytes(uint16_t);
    public:
        QueryGenerator();
        ~QueryGenerator();

        /* Server -> MCU */

        /* Sends to the MCU the actual PWM signal for the desired step */
        char* dmb_mvServo(uint8_t,uint16_t);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal, MCU executes movement */
        char* smrt_mvServo(uint8_t,uint8_t);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal */
        char* smrt_updtServo(uint8_t,uint8_t);
        /* Executes the movement "loaded" with the updtServo query */
        char* smrt_mvAll();
        /* Asking the MCU the current positions */
        char* smrt_currPos(uint8_t,uint8_t);


        /* Server -> Cli */

        /* ACK query */
        char* ack(uint8_t);
        /* Tell the client the current positions of the MCU */
        char* servoPos(uint8_t,uint8_t); 
        /* Tell the client the current "intelligence" mode of the MCU */
        char* mcuMode(char);
        /* Tell the client the current update (RealTime/Delayed) mode of the MCU */
        char* updateMode(char);
};

