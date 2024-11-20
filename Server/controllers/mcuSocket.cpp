#include "mcuSocket.h"

MCUSocket::MCUSocket()
    : sck(INVALID_SOCKET),name(nullptr){}

MCUSocket::MCUSocket(SOCKET s,const char* n)
    : sck(sck){
    if (name) {
        this->name=new char[std::strlen(name+1)];
        std::strcpy(this->name,name);               
    } else {
        this->name=nullptr;
    }
}

MCUSocket::MCUSocket(const MCUSocket& other) 
    : sck(other.sck) {
    if (other.name) {
        name = new char[std::strlen(other.name) + 1]; 
        std::strcpy(name, other.name);               
    } else {
        name = nullptr;
    }
}

MCUSocket& MCUSocket::operator=(const MCUSocket& other) {
    if (this == &other) {return *this;}
    sck = other.sck;
    delete[] name;

    if (other.name) {
        name = new char[std::strlen(other.name) + 1];
        std::strcpy(name, other.name);
    } else {
        name = nullptr;
    }

    return *this;
}

MCUSocket::MCUSocket(MCUSocket&& other) noexcept 
    : sck(other.sck),name(other.name) {
    other.sck = INVALID_SOCKET;other.name = nullptr;       
}

MCUSocket& MCUSocket::operator=(MCUSocket&& other) noexcept {
    if (this == &other) {return *this;}
    delete[] name;

    sck = other.sck;name = other.name;
    other.sck = INVALID_SOCKET;other.name = nullptr;

    return *this;
}

MCUSocket::~MCUSocket(){
    delete[] name; 
}