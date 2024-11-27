#include "srvCore.h"

FD_SET srvCore::allSCK;
std::mutex srvCore::mtxSCK;
std::mutex srvCore::mtxLOG;
std::ofstream srvCore::logFile;
std::thread srvCore::threadLog;
std::queue<std::string> srvCore::queueLOG;
std::condition_variable srvCore::condLOG;
bool srvCore::srvUp;
bool srvCore::loggerUp;
std::vector<MCUSocket> srvCore::MCUSCK;
std::vector<ControllerInfo> srvCore::ActiveControllers;

void srvCore::writeToLog(char* s){
#ifdef LOGGER 
    char timeBuffer[20]; 
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

    std::string e = "\n["+std::string(timeBuffer)+"."+std::to_string(milliseconds.count())+"] "+s;

    {std::lock_guard<std::mutex> lck(mtxLOG);
        queueLOG.push(e);}
    condLOG.notify_one();
#endif
}
void srvCore::writeToLog(char* s,char* s2){
#ifdef LOGGER 
    char timeBuffer[20]; 
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

    std::string e = "\n["+std::string(timeBuffer)+"."+std::to_string(milliseconds.count())+"] "+s+" "+s2;
    {std::lock_guard<std::mutex> lck(mtxLOG);
        queueLOG.push(e);}
    condLOG.notify_one();
#endif
}
void srvCore::writeQueryToLog(char* s){
#ifdef LOGGER 
    char timeBuffer[20]; 
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

    std::string s1(s);std::string s2="";
    for(char c : s1){if(std::isprint((uint8_t)c)){s2+=c;} else {s2+=std::to_string((uint8_t)c);}}

    std::string e = "\n["+std::string(timeBuffer)+"."+std::to_string(milliseconds.count())+"] New query -> "+s2;

    {std::lock_guard<std::mutex> lck(mtxLOG);
        queueLOG.push(e);}
    condLOG.notify_one();
#endif
}
void srvCore::writeMCUMSGToLog(std::string s){
#ifdef LOGGER 
    char timeBuffer[20]; 
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

    std::string s2="";
    for(char c : s){if(std::isprint((uint8_t)c)){s2+=c;} else {s2+=std::to_string((uint8_t)c);}}

    std::string e = "\n["+std::string(timeBuffer)+"."+std::to_string(milliseconds.count())+"] Message to MCU -> "+s2;

    {std::lock_guard<std::mutex> lck(mtxLOG);
        queueLOG.push(e);}
    condLOG.notify_one();
#endif
}
void srvCore::writeDBERRToLog(char* s){
#ifdef LOGGER 
    char timeBuffer[20]; 
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", std::localtime(&t));

    std::string e = "\n["+std::string(timeBuffer)+"."+std::to_string(milliseconds.count())+"] DB LOG: "+s;
    {std::lock_guard<std::mutex> lck(mtxLOG);
        queueLOG.push(e);}
    condLOG.notify_one();
#endif
}

void srvCore::logfn(){
#ifdef LOGGER 
    while(srvCore::loggerUp || !queueLOG.empty()){
        std::unique_lock<std::mutex> lck(mtxLOG);
        condLOG.wait(lck,[](){return (!srvCore::loggerUp || !queueLOG.empty());});
        while(!queueLOG.empty()){
            logFile<<queueLOG.front();
            queueLOG.pop();
        }
        logFile.flush();
    }
    logFile.close();
#endif
}

void srvCore::setupLogger(){
#ifdef LOGGER 
    logFile.open("ServerLog.log",std::ofstream::out|std::ofstream::app);
    srvCore::loggerUp=true;
    threadLog = std::thread(&srvCore::logfn);
#endif
}

void srvCore::setupDB(){
    char* a;
    if(!DBMAN::open(&a)){writeToLog("DB Connection Failed");srvCore::srvUp=false;return;}else{writeToLog("DB Connection Success");}
    switch(DBMAN::setupDB(&a)){
        case _DBMAN_DB_ALREADYEXIST: break;
        case _DBMAN_DBCREATED: writeToLog("DB Created");break;
        case _DBMAN_DBCREATE_ERROR: writeToLog("DB Creation Error:",a);srvCore::srvUp=false;break;
    }
    return;
}

bool srvCore::isMCUOnline(const char* n){
    for (auto it=MCUSCK.begin();it<MCUSCK.end();it++){
        if(!(*it).name.compare(n)){return true;}
    } 
    return false;
}

srvCore::srvCore(char* ipAddress, int port){
    srvCore::srvUp=true;
    setupLogger();
    this->MCUSCK.reserve(2);this->ActiveControllers.reserve(2);
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();writeToLog("WSA_Error");srvCore::srvUp=false;return;}
    sckListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sckListen==INVALID_SOCKET){WSACleanup();writeToLog("SocketError");srvCore::srvUp=false;return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(bind(sckListen,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;writeToLog("BindError:",std::to_string(WSAGetLastError()).data());WSACleanup();srvCore::srvUp=false;return;}
    if(listen(sckListen,5)==SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;writeToLog("ListenError:",std::to_string(WSAGetLastError()).data());WSACleanup();srvCore::srvUp=false;return;}
        // Use backlog == SOMAXCONN for >>> client connections, else keep low (dont waste resources)
    FD_ZERO(&allSCK);
    FD_SET(sckListen,&allSCK);
    writeToLog("Server Up");
    setupDB();
}

srvCore::~srvCore(){
    DBMAN::close();
    writeToLog("DB Connection Closed");
    MCUSCK.clear();
    ActiveControllers.clear();
    WSACleanup();
    writeToLog("Server Shutdown");
#ifdef LOGGER 
    srvCore::loggerUp=false;
    condLOG.notify_all();
    if(threadLog.joinable()){threadLog.join();}
#endif
    Sleep(250);
}

void srvCore::rmvSock(SOCKET s){
    std::lock_guard<std::mutex> lck(mtxSCK);
    FD_CLR(s,&allSCK);
    rmvSockfromVectors(s);
    closesocket(s);
}
void srvCore::rmvSockfromVectors(SOCKET s){
    for (auto it=MCUSCK.begin();it<MCUSCK.end();it++){
        if((*it).sck==s){MCUSCK.erase(it); return;}
    }    
    for (auto it=ActiveControllers.begin();it<ActiveControllers.end();it++){
        if((*it).controllerSCK==s){
            if(!(*it).mcuInfo.mcuName.empty()){DBMAN::saveMCUInfo((*it).mcuInfo);}
            ActiveControllers.erase(it); 
            return;
        }
    }        
}

void srvCore::mcuLogIn(RobotInformation info, SOCKET s){
    MCUSCK.push_back(MCUSocket(s,info.mcuName));
    switch(DBMAN::registerMCU(info)){
        case _DBMAN_NEW_S_MCU:writeToLog("New smartMCU registered:",info.mcuName.data()); break;
        case _DBMAN_OLD_S_MCU:writeToLog("New smartMCU connected:",info.mcuName.data()); break;
        case _DBMAN_NEW_D_MCU:writeToLog("New dumbMCU registered:",info.mcuName.data()); break;
        case _DBMAN_OLD_D_MCU:writeToLog("New dumbMCU connected:",info.mcuName.data()); break;
        case _DBMAN_ERROR_INFMISSMATCH:
            writeToLog("MCU Connection Error: Information Missmatch - MCU:",info.mcuName.data()); 
            rmvSock(s);
            writeToLog("Removed errored mcu connection - MCU:",info.mcuName.data()); 
            break;
        case _DBMAN_ERROR_REGMCU:
            writeToLog("Unknown error during mcu login - MCU:",info.mcuName.data()); 
            rmvSock(s);
            writeToLog("Removed errored mcu connection - MCU:",info.mcuName.data()); 
            break;
        case _DBMAN_ERROR_REGSMCU:
            writeToLog("Error while updating mcu info at login - MCU:",info.mcuName.data()); 
            rmvSock(s);
            writeToLog("Removed errored mcu connection - MCU:",info.mcuName.data()); 
            break;
    }
}

void srvCore::usrLogIn(SOCKET s){
    FD_SET(s,&allSCK);
    ActiveControllers.push_back(ControllerInfo(s,RobotInformation(),true));
}

std::string srvCore::readSocket(SOCKET s){
    int i;
    char rcvB[RCV_BF_LEN];
    std::string str = "";
    memset(rcvB, 0, sizeof rcvB);

    do{
        i=recv(s,rcvB,RCV_BF_LEN,0);
            if(i==0){writeToLog("Connection Closed");rmvSock(s);return "DC";}
            /* https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2 */
            if(i==SOCKET_ERROR){writeToLog("WSA Error:",std::to_string(WSAGetLastError()).data());rmvSock(s);return "WSAERR";} 
        str.append(rcvB, i);
    } while (i==RCV_BF_LEN);

    writeQueryToLog(str.data());
    return str;
}

/* (N)ACK from MCU or -> "DC" if the Socket closes the connection or "E404" if mcuName cannot be found */
std::string srvCore::contactMCU(const char* mcuName, std::string query){
    for(auto it=MCUSCK.begin();it<MCUSCK.end();it++){
        if((*it).name==mcuName){
            
            u_long m=1;char tmp[1];
            ioctlsocket((*it).sck, FIONBIO, &m);
            int i = recv((*it).sck,tmp,sizeof(tmp),MSG_PEEK);
            if(i==0){writeToLog("MCU Disconnected:", (*it).name.data());rmvSock((*it).sck);return "DC";}
            m=0;
            ioctlsocket((*it).sck, FIONBIO, &m);

            writeMCUMSGToLog(query);
            if(send((*it).sck, query.c_str(), query.size(),0)==SOCKET_ERROR){writeToLog("Connection Error [MCU]",std::to_string(WSAGetLastError()).data());rmvSock((*it).sck);return "DC";}
            return readSocket((*it).sck);
        }
    }
    return "E404";
}

void srvCore::userHandler(SOCKET s){

    std::string str = readSocket(s);
    for(auto i=0;i<ActiveControllers.size();i++){
        if(ActiveControllers[i].controllerSCK==s){
            std::thread(serverLogic::handleQuery,str,&ActiveControllers[i]).detach();
        }
    }

}

void srvCore::cliHandler(SOCKET s){

    std::string str;
    str = readSocket(s); 
    switch(serverLogic::checkLogInQuery(str)){
        case USRHELLOQUERY:
            usrLogIn(s);
            break;
        case MCUHELLOQUERY:
            try{
                mcuLogIn(serverLogic::getQueryInformation(str),s);
                break;
            }catch(const std::invalid_argument& e){
                writeToLog("MCU Log-in error: Malformed query string.");
            }
        case BADQUERY: //intended chain
        default:
            std::string qr = QueryGenerator().nack(_NACK_InvalidQuery);
            send(s,qr.c_str(),qr.size(), 0);
            rmvSock(s);
            break;
    }

}

void srvCore::runServer(){

    while (srvCore::srvUp){

        //Fill the sock list that select will modify
        FD_ZERO(&rSCK);
        memcpy(&rSCK,&allSCK,sizeof allSCK);
        
        timeval timeout; timeout.tv_sec = 10; timeout.tv_usec = 0;
        //Give the sck list to select
        switch (select(0,&rSCK,NULL,NULL,&timeout)){
            case -1:
                /* Select Error - handle it? */
                break;
            case 0:
                /* Timeout - Shoould happen every 10s, used to check if shutdown was issued */
                break;
            default:
                /* At least 1 sck is ready to accept/read */

                // for(auto &&sck : rSCK.fd_array){}
                
                for (int i=0; i<rSCK.fd_count; i++){
                    SOCKET sck = rSCK.fd_array[i];
                    //Iterate through all the sockets ready to accept/read

                    FD_CLR(sck,&rSCK);//Remove the sck being handled from the list 

                    if(sck==sckListen){
                        //Main socket - Accept new connection
                        SOCKET s = accept(sckListen,NULL,NULL);
                        if(s==INVALID_SOCKET){writeToLog("Accept Error");return;}
                        cliHandler(s);
                    }else{
                        //Client Controller (AKA User) socket - Handle it
                        userHandler(sck);
                    }
                    
                    if(rSCK.fd_count==0)break;
                        //Check whether all rdy sockets were handled
                }

                break;
        }

    }

}

