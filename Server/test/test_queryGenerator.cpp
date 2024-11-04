#include "queryGenerator.h"
#include <cstring>

int main(int argc, char const *argv[]){
    QueryGenerator q;
    std::string qry="";

    /* Test - servoPos */

    qry.clear();
    qry = "!s-SRVP:";qry+=(char)5;qry+="-";qry+=(char)139;qry+="-e!";
    if(strcmp(q.servoPos(5,139),qry.data()) != 0){return 1;}

    /* Test - updateMode */

    qry.clear();
    qry = "!s-eMOD:";qry+=(char)0;qry+="-e!";
    if(strcmp(q.updateMode(0),qry.data()) != 0){return 2;}

    /* Test - mcuMode */

    qry.clear();
    qry = "!s-MCUM:";qry+=(char)1;qry+="-e!";
    if(strcmp(q.mcuMode(1),qry.data()) != 0){return 3;}

    /* Test - ACK */

    qry.clear();
    qry = "!s-_ACK:";qry+=(char)12;qry+="-e!";
    if(strcmp(q.ack(12),qry.data()) != 0){return 4;}

    return 0;
}

