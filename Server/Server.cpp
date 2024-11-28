#include "srvCore.h"
#include "secrets.h"

int main(int argc, char const *argv[]){

    srvCore s(SERVER_IP,SERVER_PORT);
    s.runServer();
    
    return 0;
}

