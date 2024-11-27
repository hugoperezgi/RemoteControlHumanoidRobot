#include "CliSock.h"

int CliSock::i=0;

CliSock::CliSock(char* ipAddress, int port){
    s=new char[1]{'\0'};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();std::cout <<"WSA_Error";return;}
    sck = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sck==INVALID_SOCKET){WSACleanup();std::cout <<"Socket_Error";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sck);sck = INVALID_SOCKET;WSACleanup();std::cout <<"Connection_Error";return;}
}   

CliSock::CliSock(const char* ipAddress, int port){
    s=new char[1]{'\0'};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();std::cout <<"WSA_Error";return;}
    sck = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sck==INVALID_SOCKET){WSACleanup();std::cout <<"Socket_Error";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(connect(sck,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sck);sck = INVALID_SOCKET;WSACleanup();std::cout <<"Connection_Error";return;}
}   

CliSock::~CliSock(){
    shutdown(sck,SD_BOTH);
    closesocket(sck);
    WSACleanup();
    delete[] s;
}

void CliSock::snd(char* Message){
    if(send(sck,Message,(int)strlen(Message), 0)==SOCKET_ERROR){std::cout <<"SendError";}
}

char* CliSock::rcv(){
    memset(rcvB, 0, RCV_BF_LEN);
    if(s){delete[] s;}
    s=new char[1]{'\0'};
    do{
        i=recv(sck,rcvB,RCV_BF_LEN,0);
            if(i==0){std::cout <<"ReceiveError";}
            /* https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2 */
            if(i==SOCKET_ERROR){return "WSAERR";} 
        char* temp = new char[strlen(s) + i + 1];
        strcpy(temp, s); 
        strncat(temp, rcvB, i); 
        delete[] s;
        s = temp; 
    } while (i==RCV_BF_LEN);
    return s;
}

void CliSock::clearString(){
    if(s){delete[] s;}
    s=nullptr;
}