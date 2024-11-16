#include "mcuSocket.h"

MCUSocket::MCUSocket(SOCKET s, char* n){
    this->sck=s;
    this->name=n;
}
MCUSocket::MCUSocket(){
}