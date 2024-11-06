#include "clientSocket.h"

ClientSocket::ClientSocket(SOCKET s, char* n){
    this->sck=s;
    this->name=n;
}