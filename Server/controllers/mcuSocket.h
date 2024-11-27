#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


class MCUSocket{
    public:
        SOCKET sck;
        std::string name;
        MCUSocket(SOCKET sck,const std::string& name);
        MCUSocket();
        ~MCUSocket();

        MCUSocket(const MCUSocket& other);
        MCUSocket& operator=(const MCUSocket& other);
        MCUSocket(MCUSocket&& other) noexcept;
        MCUSocket& operator=(MCUSocket&& other) noexcept;
};