#include <stdint.h>
#include <iostream>
#include <vector>

#define _NACK_InvalidQuery 255
#define _NACK_NoActiveMCU 254
#define _NACK_OnRTMode 253
#define _NACK_InvalidParameter 252
#define _NACK_ServoCountMissmatch 251
#define _NACK_NoMCUInfo 250
#define _NACK_MCUOffline 249
#define _NACK_ErrorContactingMCU 248
#define _NACK_ErrorLoadingMINMAX 247

#define _ACK_Generic 255

#define _eMOD_RealTime 101
#define _eMOD_Delayed 100

class QueryGenerator{
    private:
    public:
        std::string divideIntoBytes(uint16_t);
        inline uint16_t restore16int(uint8_t,uint8_t);
        QueryGenerator();
        ~QueryGenerator();

        /* Server -> MCU */

        /* Sends to the MCU the actual PWM signal for the desired step */
        std::string dmb_mvServo(uint32_t flag,std::vector<uint8_t> servoPositions,std::vector<std::vector<uint16_t>> minmax);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal, MCU executes movement */
        std::string smrt_mvServo(uint32_t flag,std::vector<uint8_t> servoPositions);
        /* Sends to the MCU the step angle (0-180), the MCU calculates de PWM signal */
        std::string smrt_updtServo(uint32_t flag,std::vector<uint8_t> servoPositions);
        /* Executes the movement "loaded" with the updtServo query */
        inline std::string smrt_mvAll();
        /* Asking the MCU the current positions */
        std::string smrt_currPos();


        /* Server -> Cli */

        /* ACK query */
        std::string ack(uint8_t);
        /* ACK query */
        std::string nack(uint8_t);
        /* Tell the client the current positions of the MCU */
        std::string servoPos(uint32_t flag,std::vector<uint8_t> servoPositions); 
        // /* Tell the client the current update (RealTime/Delayed) mode of the MCU */
        // std::string updateMode(char);
};

inline uint16_t QueryGenerator::restore16int(uint8_t c0,uint8_t c1){return (c0<<8)+c1;}
inline std::string QueryGenerator::smrt_mvAll(){return "-e-!";}