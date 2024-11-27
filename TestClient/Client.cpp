#include "Client.h"

Client::Client(const char* i)
    :sck(CliSock(i, 57573)){
        sck.snd("!s-Client_here-e!");
    }

Client::Client(const char* i, int p)
    :sck(CliSock(i, p)){
        sck.snd("!s-Client_here-e!");
    }

Client::~Client(){}

void Client::startUp(){
    
}

int Client::connectToMCU(const char* mcuName){
    std::string q = "!s-sMCU-";
    q+=mcuName; q+="-e!";
    this->sck.snd(q.data());
    const char* rq = this->sck.rcv();
    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    return _CLI_ok;
}

int Client::moveServos(std::vector<int> aV){
    if(aV.empty()){return _CLI_INVALIDPARAMS;}
    std::string q="!s-SRVP-0-"; q.replace(8,1,1,aV.size());
    uint8_t id = 1;
    for (auto &&i : aV){
        if(i>180 || i<0){return _CLI_OutOfRange;}
        q+=id;q+=":";q+=(uint8_t)(i+1);q+="-";
        id++;
    }
    q+="e!";

    this->sck.snd(q.data());
    const char* rq = this->sck.rcv();

    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    return _CLI_ok;
}

int Client::moveServos(uint32_t flag,std::vector<int> aV){
    if(aV.empty()){return _CLI_INVALIDPARAMS;}
    std::string q="!s-SRVP-0-"; 
    uint8_t count=0;
    for (size_t i = 0; count<aV.size(); i++){
        if(aV[count]>179 || aV[count]<0){return _CLI_OutOfRange;}
        if((flag&(0b1<<i))==(0b1<<i)){ 
            q+=(i+1);q+=":";q+=(uint8_t)(aV[count]+1);q+="-";
            count++;
        }
    }
    q.replace(8,1,1,count);
    q+="e!";

    this->sck.snd(q.data());
    const char* rq = this->sck.rcv();

    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    return _CLI_ok;
}

int Client::setDelayedMode(bool t){
    std::string q="!s-eMOD-0-e!";
    if(t){q.replace(8,1,1,_eMOD_Delayed);}else{q.replace(8,1,1,_eMOD_RealTime);}

    this->sck.snd(q.data());
    const char* rq = this->sck.rcv();

    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    if((t && rq[8] != _eMOD_Delayed)||(!t && rq[8] != _eMOD_RealTime)){return _CLI_CERR;}
    return _CLI_ok;
}

int Client::executeMovements(){
    this->sck.snd("!s-mALL-e!");
    const char* rq = this->sck.rcv();

    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    return _CLI_ok;
}

int Client::shutdownServer(){
    this->sck.snd("!s-sOFF-e!");
    const char* rq = this->sck.rcv();
    if(strncmp(&rq[3],"_ACK",4)){
        if(!strncmp(&rq[3],"NACK",4)){
            return rq[8];
        }
        return _CLI_NVALIDRESPONSE;
    }
    return _CLI_ok;
}
