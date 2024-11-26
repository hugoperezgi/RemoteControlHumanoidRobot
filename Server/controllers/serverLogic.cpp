#include "serverLogic.h"
#include <cstring>

inline bool checkHeader(std::string query){
    return (query.compare(0,HEADERLEN,HEADERTXT)==0 && query.compare(query.length()-TAILLEN,TAILLEN,TAILTXT)==0);
}

std::string serverLogic::dispatchEMOD(uint8_t eMOD,ControllerInfo* c){
    if(eMOD==_eMOD_Delayed){(*c).updateOnRealTime=false;return QueryGenerator().ack(_eMOD_Delayed);}
    if(eMOD==_eMOD_RealTime){(*c).updateOnRealTime=true;return QueryGenerator().ack(_eMOD_RealTime);}
    return QueryGenerator().nack(_NACK_InvalidParameter); 
}

std::string serverLogic::dispatchSRVP(char* bquery,ControllerInfo* c){

    if((*c).mcuInfo.mcuName.empty()){return QueryGenerator().nack(_NACK_NoActiveMCU);}
    if(!srvCore::isMCUOnline((*c).mcuInfo.mcuName.c_str())){return QueryGenerator().nack(_NACK_MCUOffline);}

    std::string query = "";
    query.clear();
    query.append(bquery);

    /* [!s]-[SRVP]-[number of servos to update]-[servoid:servopos~servoid:servopos]-[e!] */
    /* [!s-]0-2 (3) Header [xxxx]3-6 (4) Type of Query [x]8(1) Number of servos [servoInfo]10-x((4*NumOfServos)-1) */
    /* Servoid and servoposition are sent with a +1 offset as a zero would mean end of stream */

    uint8_t numServ=query.at(8);
    uint8_t tmpID=0,tPos=0;
    query=query.substr(10,(4*numServ)-1);
    if((*c).mcuInfo.servoCount<numServ){return QueryGenerator().nack(_NACK_ServoCountMissmatch);}
    if((*c).mcuInfo.servos_MIN_MAX.empty() && (!(*c).mcuInfo.smartMCU)){return QueryGenerator().nack(_NACK_NoMCUInfo);}
    for (size_t i = 0; i < numServ; i++){
        tmpID=query.at(i*4)-1;
        tPos=query.at(2+numServ*4);
        if(tmpID>(*c).mcuInfo.servoCount || tPos>181){return QueryGenerator().nack(_NACK_InvalidParameter);}
        (*c).mcuInfo.updateFlag|=(0b1<<tmpID); 
        (*c).mcuInfo.targetPositions[tmpID]=tPos;
    }
    
    std::string rq="ACK";

    if((*c).updateOnRealTime){
        
        /* RealTime mode */
        if((*c).mcuInfo.smartMCU){
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions));
        }else{
            if((*c).mcuInfo.servos_MIN_MAX.empty()){return QueryGenerator().nack(_NACK_NoMCUInfo);}
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().dmb_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions,(*c).mcuInfo.servos_MIN_MAX));
        }
        (*c).mcuInfo.updateFlag=0;

    }else{
        /* Non-RT/Delayed mode */

        if((*c).mcuInfo.smartMCU){
            rq=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_updtServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions));
            (*c).mcuInfo.updateFlag=0;
        }    
    }
    if(strcmp(&rq[0],"ACK")){return QueryGenerator().nack(_NACK_ErrorContactingMCU);}
    return QueryGenerator().ack(_ACK_Generic);/* ACK query back to client*/
}

std::string serverLogic::dispatchMALL(ControllerInfo* c){

    if((*c).mcuInfo.mcuName.empty()){return QueryGenerator().nack(_NACK_NoActiveMCU);}

    std::string query=nullptr;

    if((*c).updateOnRealTime){return QueryGenerator().nack(_NACK_OnRTMode);}

    if((*c).mcuInfo.smartMCU){
        /* Smart mode */
        query=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().smrt_mvAll());
    }else{
        /* Dumb mode */
        if((*c).mcuInfo.servos_MIN_MAX.empty()){return QueryGenerator().nack(_NACK_NoMCUInfo);}
        if((*c).mcuInfo.updateFlag==0){return QueryGenerator().nack(_NACK_NoMCUInfo);}
        query=srvCore::contactMCU((*c).mcuInfo.mcuName.c_str(),QueryGenerator().dmb_mvServo((*c).mcuInfo.updateFlag,(*c).mcuInfo.targetPositions,(*c).mcuInfo.servos_MIN_MAX));
        (*c).mcuInfo.updateFlag=0;
    }
    if((!strcmp(&query[0],"E404"))||(!strcmp(&query[0],"DCd_MCU"))){return QueryGenerator().nack(_NACK_NoActiveMCU);}
    return QueryGenerator().ack(_ACK_Generic);
}

std::string serverLogic::dispatchSMCU(char* bquery,ControllerInfo* c){
    
    std::string query = "";
    query.clear();
    query.append(bquery);

    /* !s-sMCU-[mcuName]-e! */
    query=query.substr(8,query.size()-11);

    (*c).mcuInfo=DBMAN::getMCUInfo(query.data());
    if(!(*c).mcuInfo.mcuName.compare(query)){
        return QueryGenerator().ack(_ACK_Generic);
    }
    return QueryGenerator().nack(_NACK_NoActiveMCU);
}

int serverLogic::checkLogInQuery(std::string q){
    if(q.compare(HEADERLEN,12,"NodeMCU_here")==0){return MCUHELLOQUERY;}
    if(q.compare(HEADERLEN,11,"Client_here")==0){return USRHELLOQUERY;}
    return BADQUERY;
}

RobotInformation serverLogic::getQueryInformation(std::string q){

    /* !s-NodeMCU_here-[info]-e! */
    std::string tmp=q.substr(16,q.size()-19);

    /* [info] mcuName-<servocount>-<pos0>-<pos1>...*/
        // dumbMCU-<servocount>-<255> {!s-NodeMCU_here-dumbMCU-<servocount>-<255>-e!}
    auto i=tmp.find('-');
    if (i == std::string::npos){
        throw std::invalid_argument("Malformed query string.");
    }
    std::string mcuName=tmp.substr(0,i);    

    if(tmp.at(i+3)==(char)187){return RobotInformation(mcuName,tmp.at(i+1),false);}

    std::vector<uint8_t> positions;
    positions.reserve(tmp.at(i+1));
    for (size_t j = 0; j < tmp.at(i+1); j++){
        positions.emplace_back(tmp.at(i+3+(2*j)));
    }

    return RobotInformation(mcuName,tmp.at(i+1),positions,true);
}

void serverLogic::handleQuery(std::string q, ControllerInfo* c){

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

    std::string qr = QueryGenerator().nack(_NACK_InvalidQuery);
    if(checkHeader(q)){

        char* code = new char[5];
        memcpy(code,q.data()+3,4);
        code[4]=0;
        //Retrocompatibility with firebase client
        if(!strcmp(code,"SRVP")){delete code;qr=dispatchSRVP(q.data(),c);}
            /* !s-SRVP-<Number of servos to update>-<servoid>:<position>-e! -> [!s][SRVP][number of servos to update][servoid:servopos~servoid:servopos][e!] */
        if(!strcmp(code,"eMOD")){delete code;qr=dispatchEMOD(q.at(PARAM1_POSITION),c);}
            /* !s-eMOD-[_eMOD_Delayed/_eMOD_RealTime]-e! ~ Used to change from Realtime updates to delayed mode */
        if(!strcmp(code,"mALL")){delete code;qr=dispatchMALL(c);} 
            /* !s-mALL-e! ~ IF on delayed mode, execute target movements */

        //New functionality
        if(!strcmp(code,"sMCU")){delete code;qr=dispatchSMCU(q.data(),c);}
            /* !s-sMCU-mcuName-e! ~ Swap active MCU */
        // if(!strcmp(code,"iMCU")){delete code;/* Get MCU info */;return 0;}
            /* !s-iMCU-e! ~ Get MCU info */
        // if(!strcmp(code,"uINF")){delete code;/* Upload MCU info */;return 0;}
            /* !s-uINF-[data]-e! ~ Upload MCU info */

        if(!strcmp(code,"sOFF")){delete code;srvCore::srvUp=false;qr=QueryGenerator().ack(_ACK_Generic);}
        /* !s-mALL-e! ~ Shutdown server */
    }
    
    send((*c).controllerSCK,qr.c_str(),qr.size(), 0);
    return;
}