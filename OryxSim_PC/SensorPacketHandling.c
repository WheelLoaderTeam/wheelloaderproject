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
    //float* last_values[LEN_BUF_SENSOR];
    sscanf(argv[1],"%d",&mode);
    switch (mode){
        case 1 :
        {//100Hz
            printf("mode 1: receive 100Hz send 100Hz\n");
            break;
        }
        case 2 :
        {//50Hz
            printf("mode 2: receive 100Hz send 50Hz\n");
            break;
        }
        case 3:
        {//50Hz
            printf("mode 3: receive 50Hz send 100Hz\n");
            break;
        }
        case 4:
        {//50Hz
            printf("mode 4: receive 50Hz send 50Hz\n");
            break;
        }
        default:
            printf ("wrong mode chosen 1 or 2\n");
            break;

    }

    if (initSocket(IMU_PORT,MOV_PORT,s_onBoard, s_sim, "127.0.0.1",insock,outsock)==1){
        printf ("init success\n");

        while(1){

           //is it a problem when the packets are received at 50Hz?

            if(mode == 1 ){
                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }
                printf("i'm in mode 1\n");
                //need to reset timer at every loop
                tv.tv_sec=1; //TODO put the real value
                tv.tv_usec=10000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else
                    printf("I have waited 10 mseconds.\n");
                    //send the message
                    if (sendto(s_sim, buf, (LEN_BUF_SENSOR*sizeof(float)) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                    {
                        die("sendto()");
                    }

            }

            else if(mode == 2 ){
                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }


                //need to reset timer at every loop
                tv.tv_sec=2;//TODO put the real value
                tv.tv_usec=20000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else {

                    printf("I have waited 20 mseconds.\n");
                    if (sendto(s_sim, buf, (LEN_BUF_SENSOR*sizeof(float)) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                    {
                        die("sendto()");
                    }

                }

            }
            else if(mode == 3 ){
                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }


                //need to reset timer at every loop
                tv.tv_sec=2;//TODO put the real value
                tv.tv_usec=10000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else {

                    printf("I have waited 10 mseconds.\n");
                    if (sendto(s_sim, buf, (LEN_BUF_SENSOR*sizeof(float)) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                    {
                        die("sendto()");
                    }

                }
                 //need to reset timer at every loop
                tv.tv_sec=2;//TODO put the real value
                tv.tv_usec=10000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else {

                    printf("I have waited 10 mseconds.\n");
                    if (sendto(s_sim, buf, (LEN_BUF_SENSOR*sizeof(float)) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                    {
                        die("sendto()");
                    }

                }

            }
            else printf("my mode doesn't exist");

    }
}
return 1;
}
