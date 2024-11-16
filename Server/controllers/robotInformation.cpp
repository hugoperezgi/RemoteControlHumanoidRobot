#include "robotInformation.h"

RobotInformation::RobotInformation(){
    this->mcuName=nullptr;
    this->smartMCU=NULL;
    this->servos_MIN_MAX=nullptr;
    this->servoPositions=nullptr;    
    this->targetPositions=nullptr;
}
RobotInformation::RobotInformation(char* Identifier,bool smart){
    this->mcuName=Identifier;
    this->smartMCU=smart;
    this->servos_MIN_MAX=nullptr;
    this->servoPositions=nullptr;    
    this->targetPositions=nullptr;
}
RobotInformation::RobotInformation(char* Identifier,uint16_t** srvMinMax,bool smart){
    this->mcuName=Identifier;
    this->smartMCU=smart;
    this->servos_MIN_MAX=srvMinMax;
    this->servoPositions=nullptr;    
    this->targetPositions=nullptr;
}
RobotInformation::RobotInformation(char* Identifier,uint8_t* currentPosition,bool smart){
    this->mcuName=Identifier;
    this->smartMCU=smart;
    this->servos_MIN_MAX=nullptr;
    this->servoPositions=currentPosition;    
    this->targetPositions=nullptr;
}
RobotInformation::RobotInformation(char* Identifier,uint16_t** srvMinMax,uint8_t* currentPosition,bool smart){
    this->mcuName=Identifier;
    this->smartMCU=smart;
    this->servos_MIN_MAX=srvMinMax;
    this->servoPositions=currentPosition;    
    this->targetPositions=nullptr;
}
RobotInformation::RobotInformation(char* Identifier,uint16_t** srvMinMax,uint8_t* currentPosition,uint8_t* targetPosition,bool smart){
    this->mcuName=Identifier;
    this->smartMCU=smart;
    this->servos_MIN_MAX=srvMinMax;
    this->servoPositions=currentPosition;    
    this->targetPositions=targetPosition;
}
RobotInformation::~RobotInformation(){
    delete[] this->mcuName;
    delete[] this->servos_MIN_MAX;
    delete[] this->servoPositions;
    delete[] this->targetPositions;
}