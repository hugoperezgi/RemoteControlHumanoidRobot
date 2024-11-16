#include "queryGenerator.h"
#include <cstring>

int main(int argc, char const *argv[]){
    QueryGenerator q;
    std::string qry="";

    // /* Test - servoPos */

    // qry.clear();
    // qry = "!s-SRVP:";qry+=(char)5;qry+="-";qry+=(char)139;qry+="-e!";
    // if(strcmp(q.servoPos(5,139),qry.data()) != 0){return 1;}

    /* Test - ACK */

    qry.clear();
    qry = "!s-_ACK:";qry+=(char)12;qry+="-e!";
    if(strcmp(q.ack(12),qry.data()) != 0){return 4;}

    return 0;
}

