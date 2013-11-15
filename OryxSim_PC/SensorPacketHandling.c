#include <sys/time.h>
#include <unistd.h>
#include "Socket.h"
#include "log"


// argv[1] is the frequency parameter to send packet to simulator
// 1 is normal =100Hz
// 2 is low = 50Hz


int main(int argc, char *argv[]){

    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(outsock) ;
    struct timeval tv;
    int retval, mode;
    float buf [LEN_BUF_SENSOR];
    /*TODO il faut remettre les valeurs du timer á chaque tour de boucle sinon ca marche pas */
    sscanf(argv[1],"%d",&mode);
    switch (mode){
        case 1 :
        {//100Hz
            printf("mode 1\n");
            break;
        }
        case 2 :
        {//50Hz
            printf("mode 1\n");
            break;
        }
        default:
            printf ("wrong mode chosen 1 or 2\n");
            break;

    }

    if (initSocket(IMU_PORT,MOV_PORT,s_onBoard, s_sim, SIM_IP,insock,outsock)==1){
        printf ("init success\n");

        while(1){

           if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                die("recvfrom()");
            }

            if(mode == 1 ){
                printf("i'm in mode 1\n");
                tv.tv_sec=1;
                tv.tv_usec=10000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else
                    printf("I have waited 10 mseconds.\n");
                    //send the message
                    if (sendto(s_sim, buf, strlen(buf) , 0 , (struct sockaddr *) &outsock, (socklen_t*) &slen)==-1)
                    {
                        die("sendto()");
                    }

            }

            else if(mode == 2 ){
                printf("i'm in mode 2\n");
                tv.tv_sec=2;
                tv.tv_usec=20000;
                retval= select(1,NULL,NULL,NULL, &tv);
                if (retval == -1)
                    perror("select()");
                else
                printf("I have waited 20 mseconds.\n");

            }
            else printf("my mode doesn't exist");

    }
}
return 1;
}
