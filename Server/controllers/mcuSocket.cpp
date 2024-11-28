#include "mcuSocket.h"

MCUSocket::MCUSocket()
    : sck(INVALID_SOCKET),name(""){}

MCUSocket::MCUSocket(SOCKET s,const std::string& n)
    : sck(s),name(n){}

MCUSocket::MCUSocket(const MCUSocket& other) = default;
MCUSocket& MCUSocket::operator=(const MCUSocket& other) = default;
MCUSocket::MCUSocket(MCUSocket&& other) noexcept = default;
MCUSocket& MCUSocket::operator=(MCUSocket&& other) noexcept = default;

MCUSocket::~MCUSocket(){}