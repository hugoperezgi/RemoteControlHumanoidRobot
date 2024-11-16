#include "serverLogic.h"
#include <cstring>

uint32_t flag;


inline bool checkHeader(std::string query){
    return (query.compare(0,HEADERLEN,HEADERTXT)==0 && query.compare(query.length()-TAILLEN,TAILLEN,TAILTXT)==0);
}

char* serverLogic::dispatchEMOD(uint8_t eMOD,ControllerInfo c){
    if(eMOD==_eMOD_Delayed){c.updateOnRealTime=false;return QueryGenerator().ack(_eMOD_Delayed);}
    if(eMOD==_eMOD_RealTime){c.updateOnRealTime=true;return QueryGenerator().ack(_eMOD_RealTime);}
    return QueryGenerator().nack(_NACK_InvalidParameter); 
}

char* serverLogic::dispatchSRVP(char* bquery,ControllerInfo c){

    if(c.mcuInfo.mcuName==nullptr){return QueryGenerator().nack(_NACK_NoActiveMCU);}

    std::string query = "";
    query.clear();
    query.append(bquery);

    /* [!s]-[SRVP]-[number of servos to update]-[servoid:servopos~servoid:servopos]-[e!] */
    /* [!s-]0-2 (3) Header [xxxx]3-6 (4) Type of Query [x]8(1) Number of servos [servoInfo]10-x((4*NumOfServos)-1) */
    uint8_t numServ=query.at(8);
    uint8_t tmpID=0;
    query=query.substr(10,(4*numServ)-1);
    for (size_t i = 0; i < numServ; i++){
        tmpID=query.at(i*4);
        flag|=(0b1<<tmpID); 
        c.mcuInfo.targetPositions[tmpID]=query.at(2+numServ*4);
    }
    
    QueryGenerator q;
    std::string rq=nullptr;

    if(c.updateOnRealTime){
        
        /* RealTime mode */
        if(c.mcuInfo.smartMCU){
            rq=srvCore::contactMCU(c.mcuInfo.mcuName,q.smrt_mvServo(flag,c.mcuInfo.targetPositions));
        }else{
            rq=srvCore::contactMCU(c.mcuInfo.mcuName,q.dmb_mvServo(flag,c.mcuInfo.targetPositions,c.mcuInfo.servos_MIN_MAX));
        }
        flag=0;

    }else{
        /* Non-RT/Delayed mode */

        if(c.mcuInfo.smartMCU){
            rq=srvCore::contactMCU(c.mcuInfo.mcuName,q.smrt_updtServo(flag,c.mcuInfo.targetPositions));
            flag=0;
        }    
    }
    if((!strcmp(&rq[0],"E404"))||(!strcmp(&rq[0],"DCd_MCU"))){return q.nack(_NACK_NoActiveMCU);}
    return q.ack(_ACK_Generic);/* ACK query back to client*/
}

char* serverLogic::dispatchMALL(ControllerInfo c){

    if(c.mcuInfo.mcuName==nullptr){return QueryGenerator().nack(_NACK_NoActiveMCU);}

    QueryGenerator q;
    std::string query=nullptr;

    if(c.updateOnRealTime){return QueryGenerator().nack(_NACK_OnRTMode);}

    if(c.mcuInfo.smartMCU){
        /* Smart mode */
        query=srvCore::contactMCU(c.mcuInfo.mcuName,q.smrt_mvAll());
    }else{
        /* Dumb mode */
        query=srvCore::contactMCU(c.mcuInfo.mcuName,q.dmb_mvServo(flag,c.mcuInfo.targetPositions,c.mcuInfo.servos_MIN_MAX));
        flag=0;
    }
    if((!strcmp(&query[0],"E404"))||(!strcmp(&query[0],"DCd_MCU"))){return q.nack(_NACK_NoActiveMCU);}
    return q.ack(_ACK_Generic);
}

int serverLogic::checkLogInQuery(std::string q){
    if(q.compare(HEADERLEN,12,"NodeMCU_here")==0){return MCUHELLOQUERY;}
    if(q.compare(HEADERLEN,11,"Client_here")==0){return USRHELLOQUERY;}
    return BADQUERY;
}

RobotInformation serverLogic::getQueryInformation(std::string q){
    /* !s-NodeMCU_here-[info]-e! */

    char* tmp=new char[strlen(q.data())-18];
    memcpy(tmp,q.data()+16,strlen(tmp));

    /* [info] mcuName-<servocount>-<pos0>-<pos1>...*/
    auto i=strcspn(tmp,"-");
    if(i==strlen(tmp)){return RobotInformation(tmp,false);}   
    char tmpName[i+1]; memcpy(tmpName,tmp,i); tmpName[i]=0;
    uint8_t* tmpPos = new uint8_t[tmp[i+1]];
    for (size_t j = 0; j < tmp[i+1]; j++){
        tmpPos[j]=tmp[i+3+(2*j)];
    }
    
    return RobotInformation(tmpName,tmpPos,true);
}

void serverLogic::handleQuery(std::string q, ControllerInfo c){

    /* !s-<query body>-e! */
        
        /* <QueryType>:<param1>-<param2> */

        /* Query Type */
        // sOFF - Server off

        //  NodeMCU_here - hello from nodeMCU board, +info
        //  Client_here - hello from client, no extra params

        //  _ACK - ok msg, param1 expected
        //  NACK - error msg, param1 expected

        //  eMOD - updateModeRT(RT/delayed), param1 expected (1-Delayed, 2-RealTime)
        //  SRVP - changeServoPosition, param1 (servoID), param2 (servoPosition)
        //  mALL - moveAll servos - AKA execute movement update


        /* param1 */
        //  param1=ACK code for ACK query
        //  param1=1/2 for updateModeRT queries
        //  param1=servoID for changeServoPosition query

    char* qr = QueryGenerator().nack(_NACK_InvalidQuery);
    if(checkHeader(q)){

        char* code = new char[5];
        memcpy(code,q.data()+3,4);
        code[4]=0;
        //Retrocompatibility with firebase client
        if(!strcmp(code,"SRVP")){delete code;qr=dispatchSRVP(q.data(),c);}
        /* !s-SRVP-<Number of servos to update>-<servoid>:<position>-e! -> [!s][SRVP][number of servos to update][servoid:servopos~servoid:servopos][e!] */
        if(!strcmp(code,"eMOD")){delete code;qr=dispatchEMOD(q.at(PARAM1_POSITION),c);}
        /* !s-eMOD-0/1-e! ~ Used to change from Realtime updates to delayed mode */
        if(!strcmp(code,"mALL")){delete code;qr=dispatchMALL(c);} 
        /* !s-mALL-e! ~ IF on delayed mode, execute target movements */
        if(!strcmp(code,"sOFF")){delete code;srvCore::srvUp=false;qr=QueryGenerator().ack(_ACK_Generic);}
        /* !s-mALL-e! ~ Shutdown server */

        //New functionality
        // if(!strcmp(code,"sMCU")){delete code;/* Swap active MCU */;return 0;}
        /* !s-sMCU-mcuName-e! ~ Swap active MCU */
        // if(!strcmp(code,"iMCU")){delete code;/* Get MCU info */;return 0;}
        /* !s-iMCU-e! ~ Get MCU info */
        // if(!strcmp(code,"uINF")){delete code;/* Upload MCU info */;return 0;}
        /* !s-uINF-[data]-e! ~ Upload MCU info */

    }
    send(c.controllerSCK,qr,strlen(qr), 0);
    return;
}