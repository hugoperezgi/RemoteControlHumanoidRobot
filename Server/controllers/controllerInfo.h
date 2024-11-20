#pragma once

#include <windows.h>
#include <winsock2.h>
#include "robotInformation.h"

class ControllerInfo{
    private:
        /* data */
    public:
        ControllerInfo();
        // ControllerInfo(SOCKET,bool);
        ControllerInfo(SOCKET,RobotInformation,bool);
        ~ControllerInfo();
        bool updateOnRealTime;
        SOCKET controllerSCK;
        RobotInformation mcuInfo;
};

