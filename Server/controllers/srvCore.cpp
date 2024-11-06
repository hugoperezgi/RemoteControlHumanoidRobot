#include "srvCore.h"
#include <fstream>
#include <thread>

// FD_SET srvCore::allSCK;
std::mutex srvCore::mtx;
bool srvCore::srvUp;

srvCore::srvCore(char* ipAddress, int port){
    srvCore::srvUp=true;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=NO_ERROR){WSACleanup();std::cerr <<"WSA_Error";return;}
    sckListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sckListen==INVALID_SOCKET){WSACleanup();std::cerr <<"Socket_Error";return;}
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(ipAddress);
    srvAddr.sin_port = htons(port);
    if(bind(sckListen,(SOCKADDR *) &srvAddr,sizeof srvAddr) == SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();std::cout <<"Bind_Error";return;}
    if(listen(sckListen,5)==SOCKET_ERROR){closesocket(sckListen);sckListen = INVALID_SOCKET;WSACleanup();std::cerr <<"ListenError";return;}
        // Use backlog == SOMAXCONN for >>> client connections, else keep low (dont waste resources)
    // FD_ZERO(&allSCK);
    // FD_SET(sckListen,&allSCK);
}

srvCore::~srvCore(){
    // closeSockets();
    WSACleanup();
    Sleep(250);
    std::cerr<<"Server Out!";
}

// void srvCore::getNewClient(){
//     SOCKET s = accept(sckListen,NULL,NULL);
//     if(s==INVALID_SOCKET){std::cerr << "AcceptError";return;}
//     FD_SET(s,&allSCK);
// }

/*this->rmvSock(sck); //Remove the socket if client dc'd*/
// void srvCore::rmvSock(SOCKET s){
//     mtx.lock();
//     FD_CLR(s,&allSCK);
//     closesocket(s);
//     mtx.unlock();
// }
// void srvCore::rmvSockCheckMCU(SOCKET s){
    
//     serverLogic::checkNodeMCUdcd(s);
//     srvCore::rmvSock(s);
    
// }

void srvCore::mcuHandler(char* info, SOCKET s){
    new ClientSocket(s,"Name");
    // Add to socket list -> check resize if full

    // RobotCalibration class -> Stores the robot info Name+Servo(min/max/current) -> Stored on db
}

void srvCore::userHandler(SOCKET sck){
    //TODO the whole "dedicated thread" for client 

    /* check server shutdown variable -> deal with it if needed */
}

void srvCore::cliHandler(SOCKET s){

    int i;
    char rcvB[RCV_BF_LEN];
    std::string str = "";
    memset(rcvB, 0, sizeof rcvB);


    std::ofstream file;
    file.open("logger",std::ofstream::out|std::ofstream::app);

    file << "\n[" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "] ";

    do{
        i=recv(s,rcvB,RCV_BF_LEN,0);
        file << rcvB;
        str+=rcvB;
        memset(rcvB, 0, sizeof rcvB);
        if(i==0){std::cerr <<"ConnectionClosed";closesocket(s);return;}
        /* https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2 */
        if(i==SOCKET_ERROR){std::cout << WSAGetLastError();closesocket(s);return;} 
    } while (i==RCV_BF_LEN && WSAGetLastError()==WSAEMSGSIZE);

    file.close();

    //Shutdown Query /* TODO maybe add an admin password? */
    if(str=="!ShutDownServer"){
        srvCore::srvUp=false;
    }
    // else{
    //     std::cerr << str << "\n";
    // }

    // new std::thread(serverLogic::handleQuery,str.data(),s);

    // Check format + Check if MCU/Users

    //TODO create a serverLogic::parseQueryInfo() -> abuse auto info type so it can be reused for all queries?
    switch(serverLogic::checkLogInQuery(str.data())){
        case MCUHELLOQUERY:
            /* TODO 
                check if current positions are given
                check mcu name [db check]  
                ???
                log the mcu into MCU sock Array
            */
            //mcuHandler();
            break;
        case USRHELLOQUERY:
            /* HUGE - TODO */
            //userHandler();
            break;
        case BADQUERY: //intended chain
        default:
            /* TODO */
            break;
    }

}

void srvCore::runServer(){
    srvCore::srvUp=true;
    while (srvCore::srvUp){

        // Accept a new client connection
        SOCKET s = accept(sckListen,NULL,NULL);
        if(s==INVALID_SOCKET){std::cerr << "AcceptError";return;}

        // Handle the client (UserController / MCU)
        new std::thread(cliHandler,s);

        // //Fill the sock list that select will modify
        // FD_ZERO(&rSCK);
        // memcpy(&rSCK,&allSCK,sizeof allSCK);
        
        // //Give the sck list to select
        // switch (select(0,&rSCK,NULL,NULL,NULL)){
        //     case -1:
        //         /* Select Error - handle it? */
        //         break;
        //     case 0:
        //         /* Timeout - Shoould never happen as timeout is set to NULL */
        //         break;
        //     default:
        //         /* At least 1 sck is ready to accept/read */

        //         for (int i=0; i<rSCK.fd_count; i++){
        //             SOCKET sck = rSCK.fd_array[i];
        //             //Iterate through all the sockets ready to accept/read

        //             FD_CLR(sck,&rSCK);//Remove the sck being handled from the list 

        //             if(sck==sckListen){
        //                 //Main socket - Accept new connection
        //                 this->getNewClient();
        //             }else{
        //                 //Client socket - Handle it
        //                 cliHandler(sck);
        //             }
                    
        //             if(rSCK.fd_count==0)break;
        //                 //Check whether all rdy sockets were handled
        //         }

        //         break;
        // }

    }
    

}


// void srvCore::closeSockets(){
//     int j=0;
//     for (auto &&i : allSCK.fd_array){
        
//         shutdown(i, SD_RECEIVE);
//         send(i,"ServerShutdown",(int)strlen("ServerShutdown"), 0);
//         closesocket(i);

//         j++;
//         if(j==allSCK.fd_count)break;
//     }
// }
