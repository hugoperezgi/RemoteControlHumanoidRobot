
#include "serverLogic.h"
#include <cstring>

#define HEADERTXT "!s-"
#define HEADERLEN 3
#define TAILTXT "-e!"
#define TAILLEN 3


#define SMART_MCU_MODE 1
#define PARAM1_POSITION 8
#define PARAM2_POSITION 10

SOCKET serverLogic::nodeMCU=INVALID_SOCKET;
bool serverLogic::stateNodeMCU=false;

short serverLogic::mcuSmartMode=SMART_MCU_MODE;
short serverLogic::updateModeRT=0;
uint8_t serverLogic::currentServoPositions[15];
uint8_t serverLogic::targetServoPositions[15];
double serverLogic::servoStep[15];
// uint16_t serverLogic::srvPulseLen[15][181];


inline bool checkHeader(std::string query){
    return (query.compare(0,HEADERLEN,HEADERTXT)==0 && query.compare(query.length()-TAILLEN,TAILLEN,TAILTXT)==0);
}

void serverLogic::handleNodeMCU(SOCKET sck){
    serverLogic::nodeMCU = sck;
    QueryGenerator qGen; char* query;
    stateNodeMCU=true;
    if(mcuSmartMode){

        for(int i=0;i<16;i++){
            query=qGen.smrt_currPos(i,currentServoPositions[i]);
            send(sck,query,(int)strlen(query),0);
        }

    }
}

void serverLogic::checkNodeMCUdcd(SOCKET sck){
    if(sck==serverLogic::nodeMCU){
        serverLogic::nodeMCU=INVALID_SOCKET;
        stateNodeMCU=false;
    }
    stateNodeMCU=true;
}

void serverLogic::handleClientLogin(SOCKET sck){

    QueryGenerator qGen; uint8_t id; char* query; std::string str="";
    query=qGen.ack(0);
    send(sck,query,(int)strlen(query),0);

    for(uint8_t i = 0; i < 15; i++){
        id=i; if(id>=10)id++;
        sendAgain:
        query = qGen.servoPos(id,currentServoPositions[i]);
        send(sck, query, (int)strlen(query),0);

        char rcvB[RCV_BF_LEN]; int j; str.clear(); 
        do{
            j=recv(sck,rcvB,RCV_BF_LEN,0);
            str+=rcvB;
            memset(rcvB, 0, sizeof rcvB);
            if(j==0){std::cerr <<"ConnectionClosed";srvCore::rmvSock(sck);return/* Thread dies */;}
            if(j==SOCKET_ERROR){std::cout << WSAGetLastError();srvCore::rmvSock(sck);return/* Thread dies */;} 
        } while (j==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);
        if(!(checkHeader(str)&&!(str.compare(HEADERLEN,4,"_ACK"))&&str.at(PARAM1_POSITION)==currentServoPositions[id])){
            /* !s-ACK:<servoPos>-e! expected */
            goto sendAgain;
        }

    }
    
    return/* Thread dies */;
}

void serverLogic::dispatchMCUM(SOCKET sck,uint8_t mCUM){
    
    if(mCUM>1){std::cout<<"Not a valid mode";return;}
    QueryGenerator q;
    mcuSmartMode=mCUM;
    char* query=q.ack(mcuSmartMode);
    send(sck,query,strlen(query),0);
    return /* Thread dies */;

}

void serverLogic::dispatchEMOD(SOCKET sck, uint8_t eMOD){
    
    if(eMOD>1){std::cout<<"Not a valid mode";return;}
    QueryGenerator q;
    updateModeRT=eMOD;
    char* query = q.ack(updateModeRT);
    send(sck,query,strlen(query),0);
    return /* Thread dies */;
    
}

void serverLogic::dispatchSRVP(SOCKET sck,uint8_t id,uint8_t pos){

    QueryGenerator q; char* query;
    
    if(updateModeRT){
        /* Update on Real time (1) */

        //update both current&target servoPos arrays
        currentServoPositions[id]=pos;
        targetServoPositions[id]=pos;
        
        if(mcuSmartMode){
            /* Smart mode (1) */
            if(id>=10)id++;
            query = q.smrt_mvServo(id,pos);
            send(nodeMCU, query, strlen(query),0);

        }else{
            /* Dumb mode (0) */

            uint16_t mask = 0; uint8_t idq=0;

            while(mask!=32767){
                for(int id=0;id<15;id++){
                    if(currentServoPositions[id]==targetServoPositions[id]){
                        (mask |= (1<<id));
                        continue;
                    }else{
                        currentServoPositions[id]<targetServoPositions[id] ? currentServoPositions[id]<(targetServoPositions[id]-1) ? currentServoPositions[id]+=2 : currentServoPositions[id]++ : currentServoPositions[id]>(targetServoPositions[id]+1) ? currentServoPositions[id]-=2 : currentServoPositions[id]--;                                             
                        idq=id;
                        if(idq>=10)idq++;
                        query = q.dmb_mvServo(id,currentServoPositions[id]);
                        send(sck,query,strlen(query),0);
                    }
                }
            }
        }    
        

    }else{
        /* Non-RT/Delayed mode */

        //update target servoPos arrays
        targetServoPositions[id]=pos;

        if(mcuSmartMode){
            /* Smart mode (1) */

            if(id>=10)id++;
            query=q.smrt_updtServo(id,pos);
            send(sck, query, strlen(query),0);
        }    
    }

    //ACK back to client with pos
    query=q.ack(pos);
    send(sck,query,strlen(query),0);

    return /* Thread dies */;
}

void serverLogic::dispatchMALL(SOCKET sck){

    QueryGenerator q;
    char* query;

    if(mcuSmartMode){
        /* Smart mode */
        query=q.smrt_mvAll();
        send(nodeMCU,query,strlen(query),0);
    }else{
        /* Dumb mode */
        
        uint16_t mask = 0; uint8_t idq=0;

        while(mask!=0b111111111111111){
            for(int id=0;id<15;id++){
                if(currentServoPositions[id]==targetServoPositions[id]){
                    (mask |= (1<<id));
                    continue;
                }else{
                    currentServoPositions[id]<targetServoPositions[id] ? currentServoPositions[id]<(targetServoPositions[id]-1) ? currentServoPositions[id]+=2 : currentServoPositions[id]++ : currentServoPositions[id]>(targetServoPositions[id]+1) ? currentServoPositions[id]-=2 : currentServoPositions[id]--;                                             
                    idq=id;
                    if(idq>=10)idq++;
                    // query = q.dmb_mvServo(idq,srvPulseLen[id][currentServoPositions[id]]);
                    send(sck,query,strlen(query),0);
                }
            }
        }
    }

    query=q.ack(0);
    send(sck,query,strlen(query),0);

    return /* Thread dies */;
}

short serverLogic::handleQuery(char* q, SOCKET sck){

    std::string query = "";
    query.clear();
    query.append(q);
    // std::cout<<query<<"\n";

    /* !s-<query body>-e! */
        
        /* <QueryType>:<param1>-<param2> */

        /* Query Type */
        //  NodeMCU_here - hello from nodeMCU board, no extra params
        //  Client_here - hello from client, no extra params
        //  _ACK - ok msg, param1 expected
        //  MCUM - mcuSmartMode(Smart/Dumb), param1 expected (0-Dumb, 1-Smart)
        //  eMOD - updateModeRT(RT/delayed), param1 expected (0-Delayed, 1-RealTime)
        //  SRVP - changeServoPosition, param1 (servoID), param2 (servoPosition)
        //  mALL - moveAll servos - AKA execute movement update

        /* param1 */
        //  param1=ACK code for ACK query
        //  param1=0/1 for mcuSmartMode and updateModeRT queries
        //  param1=servoID for changeServoPosition query

    if(checkHeader(query)){
        /* Query is correct format */
        if(query.compare(HEADERLEN,12,"NodeMCU_here")==0){
            /* NodeMCU connection -> Register the socket and tell it the current position of servos */

            handleNodeMCU(sck);
            return 0;

        }else if(query.compare(HEADERLEN,11,"Client_here")==0){
            /* Client connection -> Send current servo position data */
            
            handleClientLogin(sck);
            return 0;
            
        }else{
            /* ACK/MCUM/eMOD/SRVP Queries - from client*/

            char code[5];
            code[5]=0;
            memcpy(code,query.data()+3,4);

            if(!strcmp(code,"MCUM")){dispatchMCUM(sck,query.at(PARAM1_POSITION));return 0;}
            if(!strcmp(code,"eMOD")){dispatchEMOD(sck,query.at(PARAM1_POSITION));return 0;}
            if(!strcmp(code,"SRVP")){dispatchSRVP(sck,query.at(PARAM1_POSITION),query.at(PARAM2_POSITION));return 0;}
            if(!strcmp(code,"mALL")){dispatchMALL(sck);return 0;} 
            return 1;

        }


    }else{
        /* Shiet happens */
        return 1;
    }

}