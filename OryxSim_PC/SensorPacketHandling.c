#include <sys/time.h>
#include <unistd.h>
#include "Socket.h"


// argv[1] is the frequency parameter to send packet to simulator
// 1 is normal =100Hz
// 2 is low = 50Hz


int main(int argc, char *argv[]){
    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(outsock) ;
    struct timeval tv;
    int retval, mode;
    float buf [LEN_BUF_SENSOR];

    sscanf(argv[1],"%d",&mode);
    printf("le mode est %d \n", mode);
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

    if (initSocket(IMU_PORT,MOV_PORT,s_onBoard, s_sim, SIM_IP,insock,outsock)==1){
        printf ("init success\n");

        while(1){
        printf("mode value %d\n", mode);
             /*if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, &slen)) == -1)
        {
            die("recvfrom()");
        */

            if(mode == 1 ){
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else
                    printf("I have waited 10 mseconds.\n");

            }

            else if(mode == 2 ){
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else
                printf("I have waited 20 mseconds.\n");

            }

    }
}
return 1;
}
