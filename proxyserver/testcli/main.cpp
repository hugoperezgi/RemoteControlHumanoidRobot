#include "CliSock.cpp"

#include <iostream>
#include <thread>
#include <unistd.h>


int main(int argc, char const *argv[]){

    char ip[]="10.60.122.206";
    // // char ip[]="192.168.1.164";
    CliSock c(ip,57573);

    std::cout << "Set 180's\n";
    c.snd("!s-SRVP-27-0:180~1:180~2:180~3:180~4:180~5:180~6:180~7:180~8:180~9:180~10:180~11:180~12:180~13:180~14:180~15:180~16:180~17:180~18:180~19:180~20:180~21:180~22:180~23:180~24:180~25:180~26:180-e!"); 
    usleep(500000);

    // std::cout << "Exectute Movement!\n";
    // c.snd("!s-mALL-e!\n");
    // usleep(20000000);//20sec

    // std::cout << "Set zeros\n";
    // c.snd("!s-SRVP-5-0:0~1:0~2:0~3:0~4:0-e!\n"); 
    // usleep(500000);

    // std::cout << "Exectute Movement!\n";
    // c.snd("!s-mALL-e!");
    // usleep(500000);


}
