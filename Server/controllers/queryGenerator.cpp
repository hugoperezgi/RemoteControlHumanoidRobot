#include "queryGenerator.h"

QueryGenerator::QueryGenerator(/* args */){
}

QueryGenerator::~QueryGenerator(){
}

/* Client fucntions Query[Server <-> Client]*/

    /* !s-<query body>-e! */

        /* <QueryType>:<param1>-<param2> */

        /* Query Type */
        //  NodeMCU_here - hello from nodeMCU board, no extra params
        //  Client_here - hello from client, no extra params
        //  _ACK - ok msg, param1 expected
        //  MCUM - mcuMode(Smart/Dumb), param1 expected (0-Dumb, 1-Smart)
        //  eMOD - updateMode(RT/delayed), param1 expected (0-Delayed, 1-RealTime)
        //  SRVP - changeServoPosition, param1 (servoID), param2 (servoPosition)

        /* param1 */
        //  param1=ACK code for ACK query
        //  param1=0/1 for mcuMode and updateMode queries
        //  param1=servoID for changeServoPosition query


char* QueryGenerator::ack(uint8_t code){
    std::string q = "!s-_ACK:";q+=code;q+="-e!";
    return q.data();
}

/*Either smart mode (mode=1) or dumb mode (mode=0)*/
char* QueryGenerator::mcuMode(char mode){
    std::string q = "!s-MCUM:";q+=mode;q+="-e!";
    return q.data();
}
/*Either RT mode(mode=1) or non-RT mode (mode=0)*/
char* QueryGenerator::updateMode(char mode){
    std::string q = "!s-eMOD:";q+=mode;q+="-e!";
    return q.data();
}
char* QueryGenerator::servoPos(uint8_t id,uint8_t pos){
    std::string q = "!s-SRVP:";q+=id;q+="-";q+=pos;q+="-e!";
    return q.data();
}

/* MCU functions Query[Server -> MCU] */

/* -- Query format --

    -<m/u/e/c>-s:<id>-p:<0-180>-!

        m: move servo (Update a desired position and move the servo)
        u: update servo positon (Update a desired position, dont move)
        e: move all servos to desired positions (No update)
        c: Set current servo positions (get the current positions from srv)
        -s:<id> : Servo to be moved/updated
        -p:<0-180> : Position the servo should be updated/moved to

*/

/* Support function for Dumb functions */

char* QueryGenerator::divideIntoBytes(uint16_t u){
    std::string hl = "";
    hl+=(uint8_t)(u>>8);
    hl+=(uint8_t)(u&0x00ff);
    return hl.data();
}

/* Dumb functions - MCU*/

char* QueryGenerator::dmb_mvServo(uint8_t id, uint16_t pos){
    std::string q = "-m-s:";q+=id;q+="-p:";q+=divideIntoBytes(pos);q+="-!";
    return q.data();
}

/* Smart functions - MCU */

char* QueryGenerator::smrt_mvServo(uint8_t id, uint8_t pos){
    std::string q = "-m-s:";q+=id;q+="-p:";q+=pos;q+="-!";
    return q.data();
}
char* QueryGenerator::smrt_mvAll(){
    return "-e-!";
}
char* QueryGenerator::smrt_updtServo(uint8_t id, uint8_t pos){
    std::string q = "-u-s:";q+=id;q+="-p:";q+=pos;q+="-!";
    return q.data();
}
char* QueryGenerator::smrt_currPos(uint8_t id, uint8_t pos){
    std::string q = "-c-s:";q+=id;q+="-p:";q+=pos;q+="-!";
    return q.data();
}