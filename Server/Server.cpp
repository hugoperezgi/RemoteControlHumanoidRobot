#include "srvCore.h"
#include "secrets.h"

#include <iostream>
#include <cstring>


int main(int argc, char const *argv[]){

    srvCore s(SERVER_IP,SERVER_PORT);
    s.runServer();
    
}
