#include "srvCore.h"
#include <fstream>
#include <thread>

FD_SET srvCore::allSCK;
std::mutex srvCore::mtx;
bool srvCore::srvUp;
std::vector<MCUSocket> srvCore::MCUSCK;
std::vector<ControllerInfo> srvCore::ActiveControllers;

void srvCore::writeToLog(char* s){
#ifdef LOGGER 
    std::ofstream file;
    file.open("ServerLog.log",std::ofstream::out|std::ofstream::app);
    char timeBuffer[20]; 
    std::time_t t = std::time(nullptr);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M", std::localtime(&t));
    file << "\n[" << timeBuffer << "] "<<s;
    if(!strcmp(s,"WSA Error")){file << "Error code: " << WSAGetLastError();}
    file.close();
#endif
}
void srvCore::writeToLog(char* s,char* s2 ){
#ifdef LOGGER 
    std::ofstream file;
    file.open("ServerLog.log",std::ofstream::out|std::ofstream::app);
    char timeBuffer[20]; 
    std::time_t t = std::time(nullptr);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M", std::localtime(&t));
    file << "\n[" << timeBuffer << "] "<<s<<" "<<s2;
    if(!strcmp(s,"WSA Error")){file << "Error code: " << WSAGetLastError();}
    file.close();
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

srvCore::srvCore(char* ipAddress, int port){
    srvCore::srvUp=true;
    this->MCUSCK.reserve(2);this->ActiveControllers.reserve(2);
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();writeToLog("WSA_Error");srvCore::srvUp=false;return;}
    sckListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sckListen==INVALID_SOCKET){WSACleanup();writeToLog("SocketError");srvCore::srvUp=false;return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(bind(sckListen,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();writeToLog("BindError");srvCore::srvUp=false;return;}
    if(listen(sckListen,5)==SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();writeToLog("ListenError");srvCore::srvUp=false;return;}
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
    Sleep(250);
}

void srvCore::rmvSock(SOCKET s){
    mtx.lock();
    FD_CLR(s,&allSCK);
    rmvSockfromVectors(s);
    closesocket(s);
    mtx.unlock();
}
void srvCore::rmvSockfromVectors(SOCKET s){
    for (auto it=MCUSCK.begin();it<MCUSCK.end();it++){
        if((*it).sck==s){MCUSCK.erase(it); return;}
    }    
    for (auto it=ActiveControllers.begin();it<ActiveControllers.end();it++){
        if((*it).controllerSCK==s){ActiveControllers.erase(it); return;}
    }        
}

void srvCore::mcuLogIn(RobotInformation info, SOCKET s){
    MCUSCK.push_back(MCUSocket(s,info.mcuName));
    // TODO Stores the robot info Name+Servo(current) -> on db
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

    std::ofstream file;
    file.open("ServerLog.log",std::ofstream::out|std::ofstream::app);

    char timeBuffer[20]; 
    std::time_t t = std::time(nullptr);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M", std::localtime(&t));

    do{
        i=recv(s,rcvB,RCV_BF_LEN,0);
            if(i==0){writeToLog("Connection Closed");rmvSock(s);return "DC";}
        str+=rcvB;
        memset(rcvB, 0, sizeof rcvB);
        /* https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2 */
        if(i==SOCKET_ERROR){writeToLog("WSA Error");rmvSock(s);return "WSAERR";} 
    } while (i==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);

    file << "\n[" << timeBuffer << "] New query -> " << str;
    file.close();
    return str;
}

/* \return (char*) "DCd_MCU" if the Socket closes the connection or "E404" if mcuName cannot be found */
std::string srvCore::contactMCU(char* mcuName, char* query){
    for(auto s:MCUSCK){
        if(s.name==mcuName){
            if(send(s.sck, query, strlen(query),0)==SOCKET_ERROR){writeToLog("Connection Closed - MCU");rmvSock(s.sck);return "DCd_MCU";}
            return readSocket(s.sck);
        }
    }
    return "E404";
}

void srvCore::userHandler(SOCKET s){

    std::string str = readSocket(s);
    for(auto c:ActiveControllers){
        if(c.controllerSCK==s){
            new std::thread(serverLogic::handleQuery,str,c);
        }
    }

}

void srvCore::cliHandler(SOCKET s){

    std::string str;
    str = readSocket(s); 
    switch(serverLogic::checkLogInQuery(str)){
        case MCUHELLOQUERY:
            mcuLogIn(serverLogic::getQueryInformation(str),s);
            break;
        case USRHELLOQUERY:
            usrLogIn(s);
            break;
        case BADQUERY: //intended chain
        default:
            /* TODO */
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

