#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <mutex>
#include <cstring>

#include "srvCore.h"

#include "robotInformation.h"

#define _DBMAN_DBCREATED 0
#define _DBMAN_OK 100
#define _DBMAN_DBCREATE_ERROR 1
#define _DBMAN_DB_ALREADYEXIST 2
#define _DBMAN_NEW_S_MCU 255
#define _DBMAN_NEW_D_MCU 254
#define _DBMAN_OLD_S_MCU 253
#define _DBMAN_OLD_D_MCU 252
#define _DBMAN_ERROR_REGMCU 251
#define _DBMAN_ERROR_REGSMCU 250
#define _DBMAN_ERROR_INFMISSMATCH 249


class DBMAN{
    private:
        static sqlite3* DB;
        static sqlite3_stmt* pstmt;
        static std::mutex mtxDB;
    public:
        static int setupDB(char**);
        static int open(char**);
        static void close();

        /* Used to register a new MCU into DB */
        static int registerMCU(RobotInformation);
        /* Used to save the current state of the MCU when controller goes offline [updateFlag, targetPositions] */
        static void saveMCUInfo(RobotInformation);
        /* Used to upload MCU information through the controller client [servoMin-Max] */
        static void updateMCUInfo(RobotInformation);
        /* Load all MCU information to controller */
        static RobotInformation getMCUInfo(char* name);

        DBMAN();
        ~DBMAN();
};
