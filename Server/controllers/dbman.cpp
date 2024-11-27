#include "dbman.h"
#include <iostream>


sqlite3* DBMAN::DB;
sqlite3_stmt* DBMAN::pstmt;
std::mutex DBMAN::mtxDB;

int DBMAN::setupDB(char** errMsg){
        std::lock_guard<std::mutex> lck(mtxDB);

    std::string str;
    str="CREATE TABLE mcu";
        str+="(id INTEGER PRIMARY KEY AUTOINCREMENT,";
        str+="name TEXT NOT NULL UNIQUE,";
        str+="servoCount INTEGER,"; 
        str+="updateFlag INTEGER,"; 
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
        str+="target INTEGER,";
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
        std::lock_guard<std::mutex> lck(mtxDB);

    if(sqlite3_open("data.db",&DBMAN::DB)){return 0;}
    std::string str="PRAGMA foreign_keys=ON";
    if(sqlite3_exec(DB, str.c_str(), NULL, 0, errMsg)!=SQLITE_OK){
        return 0;
    }
    return 1;
}
void DBMAN::close(){
        std::lock_guard<std::mutex> lck(mtxDB);
    sqlite3_close(DBMAN::DB);
}

/* Data insertion */

    //Try to insert new mcu -> if name duped -> update servodata with new info

int DBMAN::registerMCU(RobotInformation mcu){
        std::lock_guard<std::mutex> lck(mtxDB);

    std::string str="INSERT INTO mcu (name, servoCount, updateFlag, smart) VALUES (?,?,0,?)";
    if(pstmt){sqlite3_finalize(pstmt);pstmt=nullptr;}
    sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
        sqlite3_bind_text(pstmt,1,mcu.mcuName.c_str(),mcu.mcuName.length(),SQLITE_STATIC);
        sqlite3_bind_int(pstmt,2,mcu.servoCount);
        sqlite3_bind_int(pstmt,3,mcu.smartMCU); // A boolean is an int :)
    int c = sqlite3_step(pstmt);
        sqlite3_finalize(pstmt);
        pstmt=nullptr;
    if(c==SQLITE_CONSTRAINT||c==SQLITE_CONSTRAINT_UNIQUE){
        str="SELECT servoCount FROM mcu WHERE name = ?";
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            sqlite3_bind_text(pstmt,1,mcu.mcuName.c_str(),mcu.mcuName.length(),SQLITE_STATIC);
        c = sqlite3_step(pstmt);
        if(c!=SQLITE_ROW){
            sqlite3_finalize(pstmt);pstmt=nullptr;  
            return _DBMAN_ERROR_REGSMCU;      
        }
        int cnt = sqlite3_column_int(pstmt,0);
            c = sqlite3_step(pstmt);
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
        if(cnt!=mcu.servoCount){return _DBMAN_ERROR_INFMISSMATCH;}
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
            str="INSERT INTO servodata (mcu_id, servoId, servoAngle, pwm_MIN, pwm_MAX, target) VALUES (?,?,?,NULL,NULL,NULL)";
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

RobotInformation DBMAN::getMCUInfo(char* name){
        std::lock_guard<std::mutex> lck(mtxDB);

    std::string str;
    str="SELECT * FROM mcu WHERE name = ?";
    if(pstmt){sqlite3_finalize(pstmt);pstmt=nullptr;}
    sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
        sqlite3_bind_text(pstmt,1,name,strlen(name),SQLITE_STATIC);
    int c=sqlite3_step(pstmt);
    if(c!=SQLITE_ROW){
        sqlite3_finalize(pstmt);
        pstmt=nullptr;
        return RobotInformation();
    }
        int id = sqlite3_column_int(pstmt,0);
        const unsigned char* n=sqlite3_column_text(pstmt,1);
        std::string mnam=n ? reinterpret_cast<const char*>(n):"";
        int count = sqlite3_column_int(pstmt,2);
        int uFlag = sqlite3_column_int(pstmt,3);
        bool smrt = sqlite3_column_int(pstmt,4);
    sqlite3_finalize(pstmt);
        pstmt=nullptr;
    std::vector<uint8_t> cV,tV;
        cV.clear();tV.clear();
    std::vector<uint16_t> minV,maxV;
        minV.clear();maxV.clear();
    for(int i=0;i<count;i++){
        str="SELECT servoAngle, pwm_MIN, pwm_MAX, target FROM servodata WHERE mcu_id = ? AND servoId = ?";
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            sqlite3_bind_int(pstmt,1,id);
            sqlite3_bind_int(pstmt,2,i);
        c=sqlite3_step(pstmt);
            if(c==SQLITE_DONE){
                sqlite3_finalize(pstmt);
                pstmt=nullptr;
                return RobotInformation(mnam,count,uFlag,smrt);
            }
            cV.emplace_back(sqlite3_column_int(pstmt,0));
            minV.emplace_back(sqlite3_column_int(pstmt,1));
            maxV.emplace_back(sqlite3_column_int(pstmt,2));
            tV.emplace_back(sqlite3_column_int(pstmt,3));
        sqlite3_finalize(pstmt);
            pstmt=nullptr;
    }
    if(minV.at(0)==NULL || maxV.at(0)==NULL){
        if(tV.at(0)==NULL){return RobotInformation(mnam,count,cV,uFlag,smrt);}else{return RobotInformation(mnam,count,cV,uFlag,tV,smrt);}
    } 
    std::vector<std::vector<uint16_t>> iV;
    iV.emplace_back(minV);iV.emplace_back(maxV);
    if(tV.at(0)==NULL){return RobotInformation(mnam,count,iV,cV,uFlag,smrt);}else{return RobotInformation(mnam,count,iV,cV,tV,uFlag,smrt);}
}   

/** TODO */
void DBMAN::updateMCUInfo(RobotInformation r){
        std::lock_guard<std::mutex> lck(mtxDB);

    std::string str;int c;
    str="SELECT id FROM mcu WHERE name = ?";
    sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
        sqlite3_bind_text(pstmt,1,r.mcuName.c_str(),r.mcuName.length(),SQLITE_STATIC);
    c=sqlite3_step(pstmt);
    int id = sqlite3_column_int(pstmt,0);
        sqlite3_finalize(pstmt);
        pstmt=nullptr;
    if(c!=SQLITE_ROW){
        srvCore::writeDBERRToLog("Could not save MCU data - MCU Not Found");
        return;
    }

    /* Insert information to servodata table */
    return;

}

void DBMAN::saveMCUInfo(RobotInformation r){
        std::lock_guard<std::mutex> lck(mtxDB);

    std::string str;int c=0;
    str="SELECT id FROM mcu WHERE name = ?";
    sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
        sqlite3_bind_text(pstmt,1,r.mcuName.c_str(),r.mcuName.length(),SQLITE_STATIC);
    c=sqlite3_step(pstmt);
    int id = sqlite3_column_int(pstmt,0);
        if(c!=SQLITE_ROW){
            srvCore::writeDBERRToLog("Could not save MCU data - MCU Not Found");
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
            return;
        }
        c=sqlite3_step(pstmt);
        sqlite3_finalize(pstmt);
        pstmt=nullptr;

    sqlite3_exec(DB,"BEGIN TRANSACTION",nullptr,nullptr,nullptr);
    str="UPDATE mcu SET updateFlag = ? WHERE name = ?";
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            sqlite3_bind_int(pstmt,1,r.updateFlag);
            sqlite3_bind_text(pstmt,1,r.mcuName.c_str(),r.mcuName.length(),SQLITE_STATIC);
        c=sqlite3_step(pstmt);
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
        if(c!=SQLITE_DONE){
            sqlite3_exec(DB,"ROLLBACK",nullptr, nullptr,nullptr);
            srvCore::writeDBERRToLog("Could not save MCU data - Error updating UpdateFlag");
            return;
        }

    str="UPDATE servodata SET servoAngle = ?, target = ? WHERE mcu_id = ? AND servoId = ?";
    for (size_t i = 0; i < r.servoCount; i++){
        sqlite3_prepare_v2(DB,str.c_str(),str.length(),&pstmt,nullptr);
            if(r.servoPositions[i]==NULL){sqlite3_bind_null(pstmt,1);}else{sqlite3_bind_int(pstmt,1,r.servoPositions[i]);}
            if(r.targetPositions[i]==NULL){sqlite3_bind_int(pstmt,2,0);}else{sqlite3_bind_int(pstmt,2,r.targetPositions[i]);}
            sqlite3_bind_int(pstmt,3,id);
            sqlite3_bind_int(pstmt,4,i);
        c=sqlite3_step(pstmt);
            sqlite3_finalize(pstmt);
            pstmt=nullptr;
        if(c!=SQLITE_DONE){
            sqlite3_exec(DB,"ROLLBACK",nullptr, nullptr,nullptr);
            srvCore::writeDBERRToLog("Could not save MCU data - Error updating current/target position");
            return;
        }        
    }
    sqlite3_exec(DB,"COMMIT",nullptr,nullptr,nullptr);
    srvCore::writeDBERRToLog(std::string("Updated MCU data ("+r.mcuName+")").data());
    return;
}

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