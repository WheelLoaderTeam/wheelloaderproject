#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "../net/socket.h"



// argv[1] is the frequency parameter to send packet to simulator
// 1 is normal =100Hz
// 2 is low = 50Hz
long sendAndWait (fd_set rfds, int s_out, float* buf, int buflen, int flags, struct sockaddr_in outsock, int slen,
                        struct timeval tv, int second, int usecond  );


int main(int argc, char *argv[]){

    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(outsock) ;
    struct timeval tv;
    int mode;
    float buf [LEN_BUF_SENSOR];
    //file descriptor for the input socket
    fd_set rfds;
    //some vqriqble to calcul time
    int time_to_wait;
    //  return value of the select fonction
    int retval;
     // here I set the variable to see in how many time a new packet is receive
        long ms;
        time_t s;
        struct timespec spec;

    //float* last_values[LEN_BUF_SENSOR];rent time in milliseconds linux
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
            printf ("wrong mode. chose 1 2 3 4 \n");
            return 0;
            break;

    }

    if ( initServerSocket(8888, &s_onBoard, &insock)==1) //&&
         //initClientSocket(8888, &s_sim, "127.0.0.1",&outsock)==1 )
        {
            printf ("init success\n");
        }
        FD_ZERO(&rfds);
        FD_SET(s_onBoard, &rfds);

        while(1){

           //is it a problem when the packets are received at 50Hz?

            if(mode == 1 ){
                // date the start of the recvfrom function
                clock_gettime(CLOCK_REALTIME, &spec);

                s  = spec.tv_sec;
                ms = round(spec.tv_nsec / 1.0e6);

                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }
                // date the end of the recvfrom function qnd calcul how many time it took to receive the packet
                clock_gettime(CLOCK_REALTIME, &spec);

                s  = spec.tv_sec;
                ms = round(spec.tv_nsec / 1.0e6)-ms;
                time_to_wait = 10000 - ms;


                printf("i'm in mode 1\n");
                ms = sendAndWait(rfds,s_onBoard, buf, LEN_BUF_SENSOR , 0 , outsock, slen, tv, 0, time_to_wait);
                if (ms!=-1){
                    if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                    }

                    printf("buf is : ");
                    int i;
                    for (i =0 ; i<8 ; i++){printf ("\t %f", buf[i] );}


                    tv.tv_sec = 0;
                    tv.tv_usec = ms*1000;
                    retval= select(1,NULL,NULL,NULL, &tv);

                    if (retval == -1)
                        perror("select()");
                    else
                    {
                    printf("Early packet discarded \n");
                    }

                }
                else printf ("i wait too long ");




            }
/* TO IMPLEMENT
            else if(mode == 2 ){
                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }


               waitAndSend(s_sim, buf, LEN_BUF_SENSOR , 0 , outsock, slen, tv, 1, 20000);

            }
            else if(mode == 3 ){
                if ((recv_len = recvfrom(s_onBoard, buf, LEN_BUF_SENSOR, 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                    die("recvfrom()");
                }

                waitAndSend(s_sim, buf, LEN_BUF_SENSOR , 0 , outsock, slen, tv, 1, 10000);
                waitAndSend(s_sim, buf, LEN_BUF_SENSOR , 0 , outsock, slen, tv, 1, 10000);
                }

            else printf("my mode doesn't exist");*/

    }

return 1;
}


long sendAndWait (fd_set rfds, int s_out, float* buf, int buflen, int flags, struct sockaddr_in outsock, int slen,
                        struct timeval tv, int second, int usecond  ){
        int retval;
        //need to reset timer at every loop
        tv.tv_sec=second;//TODO put the real value
        tv.tv_usec=usecond;
        // here I set the variable to see in how many time a new packet is receive
        long ms;
        time_t s;
        struct timespec spec;
         // Convert nanoseconds to milliseconds

        if (sendto(s_out, buf, (buflen*sizeof(float)) , 0 , (struct sockaddr *) &outsock, slen)==-1)
            {
                die("sendto()");
            }

        clock_gettime(CLOCK_REALTIME, &spec);
        s  = spec.tv_sec;
        ms = round(spec.tv_nsec / 1.0e6);
        retval= select(2,&rfds,NULL,NULL, &tv);

        if (retval == -1)
            perror("select()");
        else if (retval){
            clock_gettime(CLOCK_REALTIME, &spec);
            s  = spec.tv_sec;
            ms = round(spec.tv_nsec / 1.0e6)-ms;
            printf ("packet received");
            return ms;
            }
        else
        {
            printf("I have waited %d sec %d mseconds.\n",second, usecond);
            return -1;


        }

}

