#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#ifndef RCV_BF_LEN
#define RCV_BF_LEN 1492
#endif
class CliSock{
    private:
        static int i;
        SOCKET sck;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        WSADATA wsaData;        
    public:
        CliSock(char* ipAddress, int port);
        CliSock(const char* ipAddress, int port);
        ~CliSock();
        void snd(char* Message);
        char* rcv();
        void clearString();
};