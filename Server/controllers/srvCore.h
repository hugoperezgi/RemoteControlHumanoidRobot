#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <mutex>
#include <vector>

#include <ctime> 

#include "robotInformation.h"
#include "controllerInfo.h"
#include "serverLogic.h"
#include "mcuSocket.h"

#ifndef RCV_BF_LEN
#define RCV_BF_LEN 1492
#endif


class srvCore{
    private:
        static std::mutex mtx;
        SOCKET sckListen;
        sockaddr_in srvAddr;
        char* s;
        WSADATA wsaData;        
        FD_SET rSCK;
        static FD_SET allSCK;
        /* Vector containing all mcu's connected to the server */
        static std::vector<MCUSocket> MCUSCK;
        /* Vector containing all controller client connected to the server */
        static std::vector<ControllerInfo> ActiveControllers;
        static void cliHandler(SOCKET);
        static void mcuLogIn(RobotInformation,SOCKET);
        static void usrLogIn(SOCKET);
        static void userHandler(SOCKET);
        static std::string readSocket(SOCKET);
        static void rmvSockfromVectors(SOCKET);
        static void writeToLog(char*);
    public:
        static bool srvUp;
        srvCore(char*, int);
        ~srvCore();
        void runServer();
        static void rmvSock(SOCKET);
        static std::string contactMCU(char*,char*);

};