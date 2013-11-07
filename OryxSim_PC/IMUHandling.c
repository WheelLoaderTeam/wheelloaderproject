#include "Socket.h"


int main (void){
    struct sockaddr_in insock, outsock ;
    if (initSocket(IMU_PORT,MOV_PORT,SIM_IP,insock,outsock)==1)
        printf ("init success");

    while(1){
        printf ("waiting movements data from WL_PC ");




    }
return 1;
}
