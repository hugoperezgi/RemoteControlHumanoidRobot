#include "robotInformation.h"

RobotInformation::RobotInformation() = default;

RobotInformation::RobotInformation(const std::string& mcuName,uint8_t servoCount,bool smartMCU)
    : mcuName(mcuName),servoCount(servoCount),smartMCU(smartMCU) {}

RobotInformation::RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,bool smartMCU)
    : mcuName(mcuName),servoCount(servoCount),servos_MIN_MAX(srvMinMax),smartMCU(smartMCU) {}

RobotInformation::RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<uint8_t>& currentPosition,bool smartMCU)
    : mcuName(mcuName),servoCount(servoCount),servoPositions(currentPosition),smartMCU(smartMCU) {}

RobotInformation::RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,const std::vector<uint8_t>& currentPosition,bool smartMCU)
    : mcuName(mcuName),servoCount(servoCount),servos_MIN_MAX(srvMinMax),servoPositions(currentPosition),smartMCU(smartMCU) {}

RobotInformation::RobotInformation(const std::string& mcuName, uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,const std::vector<uint8_t>& currentPosition,const std::vector<uint8_t>& targetPosition, bool smartMCU)
    : mcuName(mcuName),servoCount(servoCount),servos_MIN_MAX(srvMinMax),servoPositions(currentPosition),targetPositions(targetPosition),smartMCU(smartMCU) {}


RobotInformation::RobotInformation(const RobotInformation&) = default;
RobotInformation& RobotInformation::operator=(const RobotInformation&) = default;
RobotInformation::RobotInformation(RobotInformation&&) noexcept = default;
RobotInformation& RobotInformation::operator=(RobotInformation&&) noexcept = default;
