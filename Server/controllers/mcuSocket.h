#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>


class MCUSocket{
    public:
        SOCKET sck;
        char* name;
        MCUSocket(SOCKET sck,const char* name);
        MCUSocket();
        ~MCUSocket();

        MCUSocket(const MCUSocket& other);
        MCUSocket& operator=(const MCUSocket& other);
        MCUSocket(MCUSocket&& other) noexcept;
        MCUSocket& operator=(MCUSocket&& other) noexcept;
};