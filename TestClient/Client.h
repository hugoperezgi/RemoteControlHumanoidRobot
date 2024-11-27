#include <vector>
#include <stdint.h>
#include "CliSock.h"


/* SERVER ACK/NACK CODES */
#define _NACK_InvalidQuery 255
#define _NACK_NoActiveMCU 254
#define _NACK_OnRTMode 253
#define _NACK_InvalidParameter 252
#define _NACK_ServoCountMissmatch 251
#define _NACK_NoMCUInfo 250
#define _NACK_MCUOffline 249
#define _NACK_ErrorContactingMCU 248

#define _ACK_Generic 255

#define _eMOD_RealTime 101
#define _eMOD_Delayed 100

/* Generic OK */
#define _CLI_ok 0
/* Generic Error */
#define _CLI_ERROR 1
/* Invalid response query */
#define _CLI_NVALIDRESPONSE 2
/* Query was not received correctly by server */
#define _CLI_CERR 3
/* [SelectMCU] -> Error [MoveServos] -> No MCU selected */
#define _CLI_NoSelectedMCU 4
/* Invalid Parameters */
#define _CLI_INVALIDPARAMS 5
/* Parameters provided out of valid ranges */
#define _CLI_OutOfRange 6

class Client{
    private:
        CliSock sck;
    public:
        Client(const char*);
        Client(const char*,int);
        ~Client();

        void startUp();
        /* Returns a _CLI_ or _NACK_ constant */
        int connectToMCU(const char* mcuName);
        int moveServos(std::vector<int>);
        int moveServos(uint32_t,std::vector<int>);
        int setDelayedMode(bool);
        int executeMovements();
        int shutdownServer();
};

