#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring> //for memset
#include <iostream>

#define RCV_BF_LEN 1492

class CliSock{
    private:
        int i=0;
        int sck;
        sockaddr_in srvAddr;
        char rcvB[RCV_BF_LEN];
        char* s;
        bool initialized;
    public:
        CliSock(char* ipAddress, int port);
        CliSock(const char* ipAddress, int port);
        ~CliSock();
        void snd(char* Message);
        char* rcv();
        void clearString();
};
