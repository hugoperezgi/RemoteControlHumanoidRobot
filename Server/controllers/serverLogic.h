#pragma once
#include "queryGenerator.h"
#include "srvCore.h"
#include <stdint.h>
#include <iostream>


#define BADQUERY 0
#define MCUHELLOQUERY 1
#define USRHELLOQUERY 2

//Std Max-Min of servos
#define SERVOMIN  71 // This is the 'minimum' pulse length count (out of 4096) AKA 0
#define SERVOMAX  455 // This is the 'maximum' pulse length count (out of 4096) AKA 180º


#define HEADERTXT "!s-"
#define HEADERLEN 3
#define TAILTXT "-e!"
#define TAILLEN 3

#define PARAM1_POSITION 8
#define PARAM2_POSITION 10

class serverLogic{
    private:

        static std::string dispatchEMOD(uint8_t,ControllerInfo*);
        static std::string dispatchSRVP(char*,ControllerInfo*);
        static std::string dispatchMALL(ControllerInfo*);

        static std::string dispatchSMCU(char*,ControllerInfo*);
        static std::string dispatchUINF(char*,ControllerInfo*);

    public:
        serverLogic() = delete;
        ~serverLogic();

        static int checkLogInQuery(std::string);
        static RobotInformation getQueryInformation(std::string);
        static void handleQuery(std::string,ControllerInfo*);
};