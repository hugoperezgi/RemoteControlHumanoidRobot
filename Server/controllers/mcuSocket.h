#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


class MCUSocket{
    public:
        SOCKET sck;
        char* name;
        MCUSocket(SOCKET,char*);
        MCUSocket();
};