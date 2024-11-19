#pragma once

#include <sqlite3.h>
#include <string>

#define _DBMAN_DBCREATED 0
#define _DBMAN_DBCREATE_ERROR 1
#define _DBMAN_DB_ALREADYEXIST 2

class DBMAN{
    private:
        static sqlite3* DB;
    public:
        static int setupDB(char**);
        static int open(char**);
        static void close();
        DBMAN();
        ~DBMAN();
};
