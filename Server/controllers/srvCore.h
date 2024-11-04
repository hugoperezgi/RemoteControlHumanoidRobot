#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <mutex>

#include "serverLogic.h"

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
        static void cliHandler(SOCKET);
        static void closeSockets();
    public:
        static bool srvUp;
        srvCore(char*, int);
        ~srvCore();
        void runServer();
        static void rmvSock(SOCKET);
};