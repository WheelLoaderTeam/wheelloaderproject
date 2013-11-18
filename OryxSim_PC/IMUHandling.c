#include <sys/time.h>
#include <unistd.h>
#include "Socket.h"


// argv[1] is the frequency parameter to send packet to simulator
// 1 is normal =100Hz
// 2 is low = 50Hz


int main(int argc, char *argv[]){
    struct sockaddr_in insock, outsock ;
    struct timeval tv;
    int retval, mode;
    float buf[LEN_BUF_SENSOR];
    printf ("choose your mode(1-2):" );
    scanf("%d",&mode);

    switch (mode){
        case '1' :
        {//100Hz
        tv.tv_sec=0;
        tv.tv_usec=10000;
        break;
        }
        case '2' :
        {//50Hz
        tv.tv_sec=0;
        tv.tv_usec=20000;
        break;
        }
        default:
        break;

    }

    if (initSocket(IMU_PORT,MOV_PORT,SIM_IP,insock,outsock)==1){
        printf ("init success\n");

        while(1){
            printf("waiting data\n");
            recvfrom()
            if(mode == 1 ){
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");

                else
                    printf("No data within 10 mseconds.\n");

            }

            else if(mode == 2 ){
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else if (retval)
                    printf("Data is available now.\n");
                /* FD_ISSET(0, &rfds) will be true. */
                else
                printf("No data within 20 mseconds.\n");

            }

    }
}
return 1;
}
