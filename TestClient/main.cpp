
#include "Client.h"

#include <iostream>
#include <thread>
#include <unistd.h>


int main(int argc, char const *argv[]){

    // char ip[]= "10.60.104.44";
    char ip[]="192.168.1.164";

    /* Start connection */
    Client c(ip);
    c.startUp();
        usleep(500000);

    /* Change from RT(default) to delayed mode */
    // std::cout<<c.setDelayedMode(true) <<"\n";
    // std::cout<<c.setDelayedMode(false) <<"\n";

    /* Select the mcu to control */
    std::cout<<c.connectToMCU("smartMCU") <<"\n";

    /* Update all servos */
    // std::vector<int> pV(6,12);
    // std::cout<<c.moveServos(pV) <<"\n";

    /* Update some servos only */
    std::vector<int> pV(2,5);
    std::cout<<c.moveServos(6,pV) <<"\n";
    
    /* Execute movements - Delayed mode only */
    // std::cout<<c.executeMovements() <<"\n";
    
    /* Shutdown server */
    std::cout<<c.shutdownServer() <<"\n";

}
