#include "dbman.h"
#include <iostream>

sqlite3* DBMAN::DB;
sqlite3_stmt* DBMAN::pstmt;

int DBMAN::setupDB(char** errMsg){
    std::string str;
    str="CREATE TABLE mcu";
        str+="(id INTEGER PRIMARY KEY AUTOINCREMENT,";
        str+="name TEXT NOT NULL UNIQUE,";
        str+="servoCount INTEGER,"; /* If set to NULL -> no servodata for the given MCU */
        str+="smart BOOLEAN NOT NULL)";
    // std::string str="INSERT INTO mcu (name, servoCount, smart) VALUES (?,?,?)";
    if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){
        std::string b("table mcu already exists");
        if(!b.compare(*errMsg)){return _DBMAN_DB_ALREADYEXIST;}
        goto DB_CREATION_ERROR;
    }
    str.clear();
    str="CREATE TABLE servodata";
        str+="(mcu_id INTEGER NOT NULL,";
        str+="servoId INTEGER NOT NULL,";
        str+="servoAngle INTEGER,";
        str+="pwm_MIN INTEGER,";
        str+="pwm_MAX INTEGER,";
        str+="PRIMARY KEY (mcu_id,servoId),";
        str+="FOREIGN KEY (mcu_id) REFERENCES mcu(id) ON DELETE CASCADE)";
    // std::string str="INSERT INTO servodata (mcu_id, servoId, servoAngle, pwm_MIN, pwm_MAX) VALUES (?,?,?,?,?)";
    if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){goto DB_CREATION_ERROR;}
    return _DBMAN_DBCREATED;
DB_CREATION_ERROR:
    str.clear();
    return _DBMAN_DBCREATE_ERROR;
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

/* Data insertion */

    //Try to insert new mcu -> if name duped -> update servodata with new info

int DBMAN::registerMCU(RobotInformation mcu){
    std::string str="INSERT INTO mcu (name, servoCount, smart) VALUES (?,?,?)";
    if(pstmt){sqlite3_finalize(pstmt);pstmt=nullptr;}
    sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
        sqlite3_bind_text(pstmt,1,mcu.mcuName.c_str(),mcu.mcuName.length(),SQLITE_STATIC);
        sqlite3_bind_int(pstmt,2,mcu.servoCount);
        sqlite3_bind_int(pstmt,3,mcu.smartMCU); // A boolean is an int :)
    int c = sqlite3_step(pstmt);
        sqlite3_finalize(pstmt);
        pstmt=nullptr;
    if(c==SQLITE_CONSTRAINT||c==SQLITE_CONSTRAINT_UNIQUE){
        str="UPDATE mcu SET servoCount = ? WHERE name = ?";
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            sqlite3_bind_int(pstmt,1,mcu.servoCount);
            sqlite3_bind_text(pstmt,2,mcu.mcuName.c_str(),mcu.mcuName.length(),SQLITE_STATIC);
        c=sqlite3_step(pstmt);
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
    }
    if(c==SQLITE_DONE){
        if((!mcu.smartMCU)||(mcu.servoPositions.empty())){if(str.at(0)=='U'){return _DBMAN_OLD_D_MCU;}return _DBMAN_NEW_D_MCU;}
        str="SELECT id FROM mcu WHERE name = ?";
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            sqlite3_bind_text(pstmt,1,mcu.mcuName.c_str(),mcu.mcuName.length(),SQLITE_STATIC);
        c=sqlite3_step(pstmt);
        int id = sqlite3_column_int(pstmt,0);
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
        for (auto i=0; i<mcu.servoCount;i++){
            str="INSERT INTO servodata (mcu_id, servoId, servoAngle, pwm_MIN, pwm_MAX) VALUES (?,?,?,NULL,NULL)";
            sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
                sqlite3_bind_int(pstmt,1,id);
                sqlite3_bind_int(pstmt,2,i);
                sqlite3_bind_int(pstmt,3,mcu.servoPositions[i]);
            c=sqlite3_step(pstmt);
                sqlite3_finalize(pstmt);
                pstmt=nullptr;
            if(c==SQLITE_CONSTRAINT||c==SQLITE_CONSTRAINT_UNIQUE){
                goto UPDATESERVODATA;
            }
        }
        return _DBMAN_NEW_S_MCU;
        UPDATESERVODATA:
        for (auto i=0; i<mcu.servoCount;i++){
            str="UPDATE servodata SET servoAngle = ? WHERE mcu_id = ? AND servoId = ?";
            sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
                sqlite3_bind_int(pstmt,1,mcu.servoPositions[i]);
                sqlite3_bind_int(pstmt,2,id);
                sqlite3_bind_int(pstmt,3,i);
            c=sqlite3_step(pstmt);
                sqlite3_finalize(pstmt);
                pstmt=nullptr;
            if(c!=SQLITE_DONE){
                return _DBMAN_ERROR_REGSMCU;
            }
        }
        return _DBMAN_OLD_S_MCU;
    }
    return _DBMAN_ERROR_REGMCU;
}

    //Client uploads info -> check if theres info 

/* Data retrival */

DBMAN::DBMAN(){}
DBMAN::~DBMAN(){}




/*             str="UPDATE servodata SET (servoAngle = ?, pwm_MIN = ?, pwm_MAX = ?) WHERE mcu_id = AND servoId = ?";
            sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
                sqlite3_bind_int(pstmt,1,mcu.servoPositions[i]);
                sqlite3_bind_null(pstmt,2);
                sqlite3_bind_null(pstmt,3);
                sqlite3_bind_int(pstmt,4,id);
                sqlite3_bind_int(pstmt,5,i);
            c=sqlite3_step(pstmt);
                sqlite3_finalize(pstmt); */