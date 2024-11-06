#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


class ClientSocket{
    public:
        SOCKET sck;
        char* name;
        ClientSocket(SOCKET,char*);
};