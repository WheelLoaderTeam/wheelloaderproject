#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "sensorData.h"

int main(int argc, char *argv[]){

    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(struct sockaddr_in) ;
    struct timeval tv;
    bool packetPending = false;
    bool firstPacket = true;
    int freq, retval;
    // init of the packets so it doesn't point to nowhere at the beginning of the program
    SensorData data;
    data.id = 0;
    data.psize = LEN_BUF_SENSOR*4;
    int i;

    sscanf(argv[1],"%d",&freq);
    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        data.values[i]=0;
    }

    // oldData is the variable that remember the last two datas. We use it for extrapolation in case of TIMEOUTs
    SensorData oldData[2];
    oldData[0].id = 0;
    oldData[0].psize = LEN_BUF_SENSOR;

    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        oldData[0].values[i]=0;
    }
    oldData[1].id = 0;
    oldData[1].psize = LEN_BUF_SENSOR;

    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        oldData[1].values[i]=0;
    }

    //file descriptor for the input socket
    fd_set rfds;
    long sec=0;
    long  usec = (long)(1.0/freq * 1000000.0);


       // init the sockets
    if ( initServerSocket(8888, &s_onBoard, &insock)==1 && initClientSocket(6666, &s_sim, "127.0.0.1",&outsock)==1 )
    {
        printf ("init success\n");
    }
    struct timespec spec;
    long timeOfLastPacketSend;

    while(1){



        // set the file descriptor of the input socket so it can be used in the select fonction
            FD_ZERO(&rfds);
            FD_SET(s_onBoard, &rfds);
            if (FD_ISSET(s_onBoard, &rfds))
                printf("rfds set, beginning the loop\n");

            tv.tv_sec   = sec;
            tv.tv_usec  = usec;
            printf("invoking select() with timeout: %ld ms\n",sec*1000+usec/1000);
            retval=select(sizeof(rfds)*8,&rfds,NULL,NULL,&tv);

            if(retval ==-1)
            {
                perror("select()");
            }
            else if(retval)//new packet
            {
                if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1)
                {
                    die("recvfrom()");
                }
                printf("packet : %d\t%d\t",data.id,data.psize);
                int i;
                for (i=0; i<LEN_BUF_SENSOR-2; i++)
                {
                    if (i<LEN_BUF_SENSOR-3)
                    {
                        printf("%f\t",data.values[i]);
                    }
                    else printf("%f\n",data.values[i]);
                }
                packetPending = true;


                if(data.id<oldData[1].id)
                {//discard the data
                printf("packet discarded\n");


                }
                else if(data.id<oldData[0].id)
                {
                    //put data in dataOld[1]
                    oldData[1] = data;
                    printf("data copied in oldData[1]\n");
                }
                else
                {
                //put dataOld[0] in dtaOld[1] qnd data in dataOld[1]
                    // save the packet into old buf
                    oldData[1] = oldData[0];
                    oldData[0] = data;
                    printf("data in order\n");


                }
                //calcul of remaining time
                clock_gettime(CLOCK_REALTIME, &spec);
                usec = spec.tv_sec*1000000 + spec.tv_nsec / 1000 -timeOfLastPacketSend;//relative to time when the last packet was sent!!!
                printf("waiting until sending for %ld ms\n",usec);
                // if TIMEOUT is close send directly the packet and reset the TIMEOUT
                if (firstPacket)
                {
                    firstPacket = false;
                    if (sendto(s_sim, &oldData[0], sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                        {
                            die("sendto()");
                        }
                    timeOfLastPacketSend = usec;
                    usec = (long)(1.0 / freq * 1000000);
                    continue;
                }
                if (usec < 100)
                {
                    if (sendto(s_sim, &oldData[0], sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                        {
                            die("sendto()");
                        }
                    printf("packet send before timeout");
                    // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
                    printf("current time: %ld us since epoch\n",timeOfLastPacketSend);
                    sec = 0;
                    usec = (long)(1.0 / freq * 1000000);

                }



            }
            else if (retval== 0 && packetPending==true)
            {
                if (sendto(s_sim, &oldData[0], sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                        {
                            die("sendto()");
                        }
                    printf("packet send");
                    // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
                    printf("current time: %ld us since epoch\n",timeOfLastPacketSend);
                    sec = 0;
                    usec = (long)(1.0 / freq * 1000000);

            }
            else if (retval==0 && packetPending==false)
            {
                printf("TIMEOUT\n");
                // if TIMEOUT is reached we extrapolate with linear approximation from the last two values
                data.values[0] = (oldData[0].values[0])+1; // increment the id
                int i;
                for ( i=0; i<6; i++)
                {
                    data.values[i] = oldData[0].values[i] + oldData[0].values[i] - oldData[1].values[i]; // calcul & assign the extrapolated value to the data we send
                }
               if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                        {
                            die("sendto()");
                        }
                    printf("packet send");
                    // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
                    printf("current time: %ld us since epoch\n",timeOfLastPacketSend);
                    sec = 0;
                    usec = (long)(1.0 / freq * 1000000);
            }


    }

}
