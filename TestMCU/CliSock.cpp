#include "CliSock.h"

CliSock::CliSock(char* ipAddress, int port){
    initialized = false;
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == -1){std::cout << "Socket_Error\n";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck, (sockaddr*) &srvAddr, sizeof srvAddr) == -1){close(sck);std::cout << "Connection_Error\n";return;}
    initialized = true;
}
CliSock::CliSock(const char* ipAddress, int port){
    initialized = false;
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == -1){std::cout << "Socket_Error\n";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck, (sockaddr*) &srvAddr, sizeof srvAddr) == -1){close(sck);std::cout << "Connection_Error\n";return;}
    initialized = true;
}


CliSock::~CliSock(){
    if(initialized){
        shutdown(sck, SHUT_RDWR);
        close(sck);
    }
    delete[] s;    
}

void CliSock::snd(char* Message){
    if(send(sck, Message, strlen(Message), 0) == -1){
        std::cout << "SendError\n";
    }
}

char* CliSock::rcv() {
    memset(rcvB, 0, sizeof rcvB);
    s=new char[1]{'\0'};
    do{
        i = recv(sck, rcvB, RCV_BF_LEN, 0);
        std::cout << "RCV: " << rcvB <<"\n";
        if(i == 0){std::cout << "ReceiveError\n";exit(0);}
        char* temp = new char[strlen(s) + i + 1];
        strcpy(temp, s); 
        strncat(temp, rcvB, i); 
        delete[] s;
        s = temp; 
    }while(i == RCV_BF_LEN);
    return s;
}

void CliSock::clearString(){
    if(s){delete[] s;}
    s=nullptr;
}
