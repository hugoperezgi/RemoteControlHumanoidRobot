#include "dbman.h"

sqlite3* DBMAN::DB;

int DBMAN::setupDB(char** errMsg){
    std::string str="CREATE TABLE robotinfo";
                str+="(id INTEGER PRIMARY KEY AUTOINCREMENT,";
                str+="name TEXT NOT NULL UNIQUE,";
                str+="smart BOOLEAN NOT NULL)";
    if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){
        std::string b("table robotinfo already exists");
        if(!b.compare(*errMsg)){return _DBMAN_DB_ALREADYEXIST;}
        return _DBMAN_DBCREATE_ERROR;
    }
    // str="CREATE TABLE robotinfo";
    // str+="(id INTEGER PRIMARY KEY AUTOINCREMENT,";
    // str+="name TEXT NOT NULL UNIQUE,";
    // str+="smart BOOLEAN NOT NULL)";
    // if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){return _DBMAN_DBCREATE_ERROR;}
    return _DBMAN_DBCREATED;
}

int DBMAN::open(char** errMsg){
    if(sqlite3_open("data.db",&DBMAN::DB)){return 0;}
    std::string str="PRAGMA foreign_keys=ON";
    if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){
        return 0;
    }
    return 1;
}
void DBMAN::close(){sqlite3_close(DBMAN::DB);}


DBMAN::DBMAN(){}
DBMAN::~DBMAN(){}
