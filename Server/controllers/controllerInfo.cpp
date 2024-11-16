#include "controllerInfo.h"

ControllerInfo::ControllerInfo(SOCKET s,RobotInformation i,bool rt){
    this->controllerSCK=s;
    this->mcuInfo=i;
    this->updateOnRealTime=rt;
}
// ControllerInfo::ControllerInfo(SOCKET s,bool rt){
//     this->controllerSCK=s;
//     this->mcuInfo=nullptr;
//     this->updateOnRealTime=rt;

// }
// ControllerInfo::ControllerInfo(SOCKET s){
//     this->controllerSCK=s;
//     this->mcuInfo=nullptr;
//     this->updateOnRealTime=NULL;
// }

ControllerInfo::~ControllerInfo(){
}
