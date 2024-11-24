#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

class RobotInformation {

    public:
        /* [min(0)/max(1)][servoId] = PWM signal (71-455) */
        std::vector<std::vector<uint16_t>> servos_MIN_MAX;
        /* [servoId] = Servo position (0º-180º) */
        std::vector<uint8_t> servoPositions;
        /* [servoId] = Target Servo position (0º-180º) */
        std::vector<uint8_t> targetPositions;
        /* Number of servos */
        uint8_t servoCount = 0;
        /* "Name" of the mcu (Identifier) */
        std::string mcuName;
        /* Whether the MCU works with a position or PWM signal */
        bool smartMCU = false;
        /* Flag for pending servo updates */
        uint32_t updateFlag = 0;

        RobotInformation();

        RobotInformation(const std::string& mcuName,uint8_t servoCount,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,uint32_t updateFlag,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<uint8_t>& currentPosition,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<uint8_t>& currentPosition,uint32_t updateFlag,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<uint8_t>& currentPosition,uint32_t updateFlag,const std::vector<uint8_t>& tPosition,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,const std::vector<uint8_t>& currentPosition,uint32_t updateFlag,bool smartMCU);
        RobotInformation(const std::string& mcuName,uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,const std::vector<uint8_t>& currentPosition,uint32_t updateFlag,const std::vector<uint8_t>& tPosition,bool smartMCU);
        RobotInformation(const std::string& mcuName, uint8_t servoCount,const std::vector<std::vector<uint16_t>>& srvMinMax,const std::vector<uint8_t>& currentPosition,const std::vector<uint8_t>& targetPosition,uint32_t updateFlag,bool smartMCU);
        
        ~RobotInformation() = default;
        
        RobotInformation(const RobotInformation&);
        RobotInformation& operator=(const RobotInformation&);
        RobotInformation(RobotInformation&&) noexcept;
        RobotInformation& operator=(RobotInformation&&) noexcept;

};
