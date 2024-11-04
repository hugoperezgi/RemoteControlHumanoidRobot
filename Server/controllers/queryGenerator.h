#include <stdint.h>
#include <iostream>

class QueryGenerator{
    private:
        char* divideIntoBytes(uint16_t );
    public:
        QueryGenerator();
        ~QueryGenerator();
        /* Server -> MCU */
        char* dmb_mvServo(uint8_t,uint16_t);
        char* smrt_mvServo(uint8_t,uint8_t);
        char* smrt_updtServo(uint8_t,uint8_t);
        char* smrt_mvAll();
        char* smrt_currPos(uint8_t,uint8_t);
        /* Server -> Cli */
        char* ack(uint8_t);
        char* servoPos(uint8_t,uint8_t); 
        char* mcuMode(char);
        char* updateMode(char);
};

