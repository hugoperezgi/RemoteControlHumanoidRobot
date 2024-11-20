#include "controllerInfo.h"

ControllerInfo::ControllerInfo(SOCKET s,RobotInformation i,bool rt)
    : controllerSCK(s),mcuInfo(i),updateOnRealTime(rt) {}

ControllerInfo::ControllerInfo()
    : controllerSCK(INVALID_SOCKET),mcuInfo(),updateOnRealTime(NULL) {}

ControllerInfo::~ControllerInfo(){
}
