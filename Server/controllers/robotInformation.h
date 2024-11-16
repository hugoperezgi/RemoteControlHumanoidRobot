#pragma once
#include <stdint.h>


class RobotInformation{

    public:

        /* [min(0)/max(1)][servoId] = PWM signal (71-455) */
        uint16_t** servos_MIN_MAX;
        /* [servoId] = Servo position (0º-180º) */
        uint8_t* servoPositions;
        /* [servoId] = Target Servo position (0º-180º) */
        uint8_t* targetPositions;
        /* "Name" of the mcu (Identifier) */
        char* mcuName;
        /* Whether the MCU works with a position or PWM signal */
        bool smartMCU;

        RobotInformation();
        RobotInformation(char*,bool);
        RobotInformation(char*,uint8_t*,bool);
        RobotInformation(char*,uint16_t**,bool);
        RobotInformation(char*,uint16_t**,uint8_t*,bool);
        RobotInformation(char*,uint16_t**,uint8_t*,uint8_t*,bool);
        ~RobotInformation();

};

