#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LOGGER /* Comment this line to disable logging */

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <fstream>
#include <thread>

#include <ctime> 

#include "robotInformation.h"
#include "controllerInfo.h"
#include "dbman.h"
#include "serverLogic.h"
#include "mcuSocket.h"

#ifndef RCV_BF_LEN
#define RCV_BF_LEN 1492
#endif


class srvCore{
    private:
        static std::ofstream logFile;
        static std::mutex mtxLOG;
        static std::queue<std::string> queueLOG;
        static std::condition_variable condLOG;

        static std::mutex mtxSCK;
        static std::thread threadLog;
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
        static void writeToLog(char*,char*);
        static void setupDB();
        static void setupLogger();
        static void logfn();
    public:
        static bool srvUp;
        srvCore(char*, int);
        ~srvCore();
        void runServer();
        static bool isMCUOnline(const char*);
        static void rmvSock(SOCKET);
        static std::string contactMCU(const char*,std::string query);

};