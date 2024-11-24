#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <cstring>

#include "robotInformation.h"

#define _DBMAN_DBCREATED 0
#define _DBMAN_DBCREATE_ERROR 1
#define _DBMAN_DB_ALREADYEXIST 2
#define _DBMAN_NEW_S_MCU 255
#define _DBMAN_NEW_D_MCU 254
#define _DBMAN_OLD_S_MCU 253
#define _DBMAN_OLD_D_MCU 252
#define _DBMAN_ERROR_REGMCU 251
#define _DBMAN_ERROR_REGSMCU 250


class DBMAN{
    private:
        static sqlite3* DB;
        static sqlite3_stmt* pstmt;
    public:
        static int setupDB(char**);
        static int open(char**);
        static void close();

        static int registerMCU(RobotInformation);
        static RobotInformation getMCUInfo(char* name);

        DBMAN();
        ~DBMAN();
};
