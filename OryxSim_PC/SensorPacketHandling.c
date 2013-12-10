#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>
#include "SensorPacketHandling.h"



// TODO verify if next value to be send is close enough to the last value sent and if not modify the value
// TODO one key on the keyboard that quit the program nicely
int running =1;
void INThandler ();

int main(int argc, char *argv[]){
    //Setup Signal handler and atexit functions
	signal(SIGINT, INThandler);



    //declaration of variables to set the sockets
    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(struct sockaddr_in) ;
    // some boolean to check if it's the first packet received or if a packet is pending
    bool packetPending = false;
    bool firstPacket = true;
    int packetID = 2001;
    // retval => result of the select fonction
    // freq the frequency thqt we set in argv[1]
    int freq, retval;
    // init of the packets so it doesn't point to nowhere at the beginning of the program
    SensorData data;
    data.id = packetID;
    data.psize = 32;
    int i;

    sscanf(argv[1],"%d",&freq);
    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        data.values[i]=0;
    }

    // oldData is the variable that remember the last two datas. We use it for extrapolation in case of TIMEOUTs
    SensorData oldData[2];
    oldData[0].id = packetID;
    oldData[0].psize = 32;

    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        oldData[0].values[i]=0;
    }
    oldData[1].id = packetID;
    oldData[1].psize = 32;

    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        oldData[1].values[i]=0;
    }
    SensorData toBeSend;
    toBeSend.id = packetID;
    toBeSend.psize = 32;

    for (i=0; i<LEN_BUF_SENSOR-2; i++)
    {
        toBeSend.values[i]=0;
    };

    //file descriptor for the input socket
    fd_set rfds;

    // variables to handle time tv is the value put in the select fonction, spec is the structure
    // where the program put the current time
    struct timeval tv;
    struct timespec spec;
    long timeOfLastPacketSend;
    long sec=0;
    long usec = (long)(1.0 / freq * 1000000.0);
    long currentTime;


       // init the sockets
    if ( initServerSocket(IMU_PORT, &s_onBoard, &insock)==1 && initClientSocket(MOV_PORT, &s_sim, SIM_IP,&outsock)==1 )
    {
        printf ("init success\n");
    }
   // printf("port: %d", ntohs(outsock.sin_port));


    while(running){



        // set the file descriptor of the input socket so it can be used in the select fonction
            FD_ZERO(&rfds);
            FD_SET(s_onBoard, &rfds);
            //if (FD_ISSET(s_onBoard, &rfds))
                //printf("rfds set, beginning the loop\n");

        // set the value of the timer and start the timer
        // the timer stop either if timeout is reach or a nez packet is received
            tv.tv_sec   = sec;
            tv.tv_usec  = usec;
            //printf("invoking select() with timeout: %ld ms\n",sec*1000+usec/1000);
            retval=select(sizeof(rfds)*8,&rfds,NULL,NULL,&tv);

            if(retval ==-1)
            {
                perror("select()");
            }
            else if(retval)//new packet is received
            {
                if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &insock, (socklen_t*) &slen)) == -1)
                {
                    die("recvfrom()");
                }
//                printf("packet : %d\t%d\t",data.id,data.psize);
//                int i;
//                for (i=0; i<LEN_BUF_SENSOR-2; i++)
//                {
//                    if (i<LEN_BUF_SENSOR-3)
//                    {
//                        printf("%f\t",data.values[i]);
//                    }
//                    else printf("%f\n",data.values[i]);
//                }

                packetPending = true;

                // check ID
                if(data.id<oldData[1].id)
                {//discard the data
                //printf("packet discarded\n");
                }
                else if(data.id<oldData[0].id)
                {
                    //put data in dataOld[1]
                    data = smoothMotion(toBeSend,data);
                    oldData[1] = data;
                  //  printf("data copied in oldData[1]\n");
                }
                else
                {

                    //put dataOld[0] in dataOld[1] and data in dataOld[0]
                    // save the packet into old buf
                    data = smoothMotion(toBeSend,data);
                    oldData[1] = oldData[0];
                    oldData[0] = data;
                    toBeSend = oldData[0];
//                    printf("data in order\n");
//                    printf("oldData[0] : %d\t%d\t",oldData[0].id,oldData[0].psize);
//                    int i;
//                    for (i=0; i<LEN_BUF_SENSOR-2; i++)
//                    {
//                        if (i<LEN_BUF_SENSOR-3)
//                        {
//                            printf("%f\t",oldData[0].values[i]);
//                        }
//                        else
//                            printf("%f\n",oldData[0].values[i]);
//                    }


                }
                //calcul of remaining time
                clock_gettime(CLOCK_REALTIME, &spec);
                currentTime = spec.tv_sec*1000000 + spec.tv_nsec / 1000;
                //printf("current time is %ld ms\n",sec*1000+currentTime/1000);
                usec =  timeOfLastPacketSend + (long)(1.0 / freq * 1000000) - currentTime;
                //printf("usec = %ld\n",usec);

                // if first packet then set the first timeOfLastPacketSend and send directly the ppacket received and reset the timer
                if (firstPacket == true)
                {
                    firstPacket = false;
                    int bitsSent;
                    //printf("port: %d", ntohs(outsock.sin_port));
                    toBeSend.id = packetID;
                    toBeSend.psize =32;
                    if ((bitsSent = sendto(s_sim, &toBeSend, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen) )==-1)
                        {
                            die("sendto()");
                        }
                    printf("packet : %d\t%d\t",toBeSend.id,toBeSend.psize);
                    int i;
                    for (i=0; i<LEN_BUF_SENSOR-2; i++)
                    {
                        if (i<LEN_BUF_SENSOR-3)
                        {
                            printf("%f\t",toBeSend.values[i]);
                        }
                        else printf("%f\n",toBeSend.values[i]);
                    }

                    //printf("firstPacket false and first packet send : return=> %d\n", bitsSent);
                    timeOfLastPacketSend = currentTime;
                    usec = (long)(1.0 / freq * 1000000);
                    printf("usec = %ld",usec);
                    continue;
                }

                 // if TIMEOUT is close send directly the packet and reset the TIMEOUT
                if (usec < 100)
                {   //printf("port: %d", ntohs(outsock.sin_port));
                    toBeSend.id = packetID;
                    toBeSend.psize =32;
                    if (sendto(s_sim, &toBeSend, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                    {
                        die("sendto()");
                    }
                    printf("packet : %d\t%d\t",toBeSend.id,toBeSend.psize);
                    int i;
                    for (i=0; i<LEN_BUF_SENSOR-2; i++)
                    {
                        if (i<LEN_BUF_SENSOR-3)
                        {
                            printf("%f\t",toBeSend.values[i]);
                        }
                        else printf("%f\n",toBeSend.values[i]);
                    }

                    //printf("packet send before timeout");
                    // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
                   // printf("time of last pascket sent :  %ld us since epoch\n",timeOfLastPacketSend);
                    sec = 0;
                    usec = (long)(1.0 / freq * 1000000);
                    continue;

                }



            }
            else if (retval== 0 && packetPending==true)
            {
                int bitsSent;
                //printf("port: %d", ntohs(outsock.sin_port));
                toBeSend.id = packetID;
                toBeSend.psize =32;
                if ((bitsSent = sendto(s_sim, &toBeSend, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen))==-1)
                {
                    die("sendto()");
                }
                printf("packet : %d\t%d\t",toBeSend.id,toBeSend.psize);
                int i;
                for (i=0; i<LEN_BUF_SENSOR-2; i++)
                {
                    if (i<LEN_BUF_SENSOR-3)
                    {
                        printf("%f\t",toBeSend.values[i]);
                    }
                    else printf("%f\n",toBeSend.values[i]);
                }
               // printf("packet send %d\n",bitsSent);
                // date the start of the recvfrom function
                clock_gettime(CLOCK_REALTIME, &spec);
                timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
                //printf("current time: %ld us since epoch\n",timeOfLastPacketSend);
                sec = 0;
                usec = (long)(1.0 / freq * 1000000);

            }
            else if (retval==0 && packetPending==false)
            {
                printf("TIMEOUT\n");
                // if TIMEOUT is reached we extrapolate with linear approximation from the last two values
                toBeSend.values[0] = (oldData[0].values[0])+1; // increment the id
                int i;
                for ( i=0; i<6; i++)
                {
                    toBeSend.values[i] = oldData[0].values[i] + oldData[0].values[i] - oldData[1].values[i]; // calcul & assign the extrapolated value to the data we send
                }
                //printf("port: %d", outsock.sin_port);
                toBeSend.id = packetID;
                toBeSend.psize =32;
                if (sendto(s_sim, &toBeSend, sizeof(SensorData) , 0, (struct sockaddr *) &outsock, slen)==-1)
                {
                    die("sendto()");
                }

                printf("packet : %d\t%d\t",toBeSend.id,toBeSend.psize);
                for (i=0; i<LEN_BUF_SENSOR-2; i++)
                {
                    if (i<LEN_BUF_SENSOR-3)
                    {
                        printf("%f\t",toBeSend.values[i]);
                    }
                    else printf("%f\n",toBeSend.values[i]);
                }
                printf("packet send\n");
                // date the start of the recvfrom function
                clock_gettime(CLOCK_REALTIME, &spec);
                timeOfLastPacketSend = spec.tv_sec*1000000+spec.tv_nsec/1000;
               // printf("current time: %ld us since epoch\n",timeOfLastPacketSend);
                sec = 0;
                usec = (long)(1.0 / freq * 1000000);
            }


    }
        // executed only if the interuption is triggered: It's triggered if we press CTRL+C
        float n = fabs(toBeSend.values[0]) + fabs(toBeSend.values[1]) + fabs(toBeSend.values[2])
                + fabs(toBeSend.values[3]) + fabs(toBeSend.values[4]) + fabs(toBeSend.values[5]);
        SensorData zero;
        zero.id = packetID;
        zero.psize = 32;

        for (i=0; i<LEN_BUF_SENSOR-2; i++)
        {
            zero.values[i]=0;
        }

        while(n!=0){
            toBeSend=smoothMotion(toBeSend,zero);
            toBeSend.id = packetID;
            toBeSend.psize =32;
            if (sendto(s_sim, &toBeSend, sizeof(SensorData) , 0, (struct sockaddr *) &outsock, slen)==-1)
                {
                    die("sendto()");
                }
            n = fabs(toBeSend.values[0]) + fabs(toBeSend.values[1]) + fabs(toBeSend.values[2])
            + fabs(toBeSend.values[3]) + fabs(toBeSend.values[4]) + fabs(toBeSend.values[5]);

            usleep(10000);

        }
 return 0;


}





SensorData smoothMotion(SensorData previous, SensorData next ){

    SensorData temp;

    temp.id = next.id;
    temp.psize = 32;
    temp.values[0] = verifMaxMovement(previous.values[0],next.values[0],MAX_TRANS_MOV);
    temp.values[1] = verifMaxMovement(previous.values[1],next.values[1],MAX_TRANS_MOV);
    temp.values[2] = verifMaxMovement(previous.values[2],next.values[2],MAX_TRANS_MOV);
    temp.values[3] = verifMaxMovement(previous.values[3],next.values[3],MAX_ROT_MOV);
    temp.values[4] = verifMaxMovement(previous.values[4],next.values[4],MAX_ROT_MOV);
    temp.values[5] = verifMaxMovement(previous.values[5],next.values[5],MAX_ROT_MOV);
    return temp;

}

float verifMaxMovement(float p, float n, float maxi){
    if (fabs(p-n) > maxi)
    {
        if ( p-n < 0 )
            return p+maxi;
        else if ( p-n > 0)
            return p-maxi;
        else return n;
    }
    else return n;
}

void INThandler (){
    running=0;
}
