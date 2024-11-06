#pragma once
#include "queryGenerator.h"
#include "srvCore.h"
#include <stdint.h>
#include <iostream>


#define BADQUERY 0
#define MCUHELLOQUERY 1
#define USRHELLOQUERY 2

class serverLogic{
    private:
        static short mcuSmartMode, updateModeRT;
        static uint8_t currentServoPositions[15], targetServoPositions[15];
        static double servoStep[15];

        static SOCKET nodeMCU;
        static bool stateNodeMCU;

        static void handleNodeMCU(SOCKET);
        static void handleClientLogin(SOCKET);

        static void dispatchMCUM(SOCKET,uint8_t);
        static void dispatchEMOD(SOCKET,uint8_t);
        static void dispatchSRVP(SOCKET,uint8_t,uint8_t);
        static void dispatchMALL(SOCKET);

    public:
        serverLogic() = delete;
        ~serverLogic();

        static int checkLogInQuery(char*);
        static void checkNodeMCUdcd(SOCKET);
        static short handleQuery(char*, SOCKET);
};

