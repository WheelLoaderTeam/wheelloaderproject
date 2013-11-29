#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "sensorData.h"

/** \SensorPacketHandling
    \Brief class handling packets
* this class handles packets comming from the onboard computer and send them at a right freauency to the simulator
*/


// argv[1] is the frequency parameter to send packet to simulator
// 1 is normal =100Hz
// 2 is low = 50Hz
/*! \fn long sendAndWait (fd_set rfds, int s_out, struct SensorData buf, int buflen, struct sockaddr_in outsock, int slen,
                        struct timeval tv, int second, int msecond )
 *  \brief function that send the SensorData buf to the socket outsock and wait the time second and usecond. while waiting it listens
    to the file descriptor rfds of the inputsocket. if a new packet comes while waiting it returns the time remaining to wait
 *  \param fd_set rfds file descriptor of the input socket
 *  \param int s_out int describing the  output socket
 *  \param SensorData buf the data that we want to send
 *  \param int buflen the length of the buf parameter
 *  \param sockaddr_in ousock the socket on which to send the data
 *  \param int slen length of the socket
 *  \param timeval tv struct containing the time to wait
 *  \param int second the number of second to wait. will fill the tv_sec attribute of tv
 *  \param int msecond the number of msecond to wait. will fill the tv_usec attribute of tv
 *
 *  \return long return the number of ms to wait if a early packet arrive. -1 otherwise.
 */
long sendAndWait (fd_set rfds, int s_out, struct SensorData buf, int buflen, struct sockaddr_in outsock, int slen,
                        struct timeval tv, int second, int msecond ) ;


int main(int argc, char *argv[]){

    struct sockaddr_in insock, outsock ;
    int s_onBoard, s_sim, recv_len, slen = sizeof(struct sockaddr_in) ;
    struct timeval tv;
    int mode;
    // init of the packets so it doesn't point to nowhere at the beginning of the program
    SensorData data;
    data.id = 0;
    data.psize = LEN_BUF_SENSOR*4;
    int i;
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
    //some vqriqble to calcul time
    long time_to_wait;
    //  return value of the select fonction
    int retval;
     // here I set the variable to see in how many time a new packet is receive
        long s;
        long ms;
        struct timespec spec;

    //float* last_values[LEN_BUF_SENSOR];rent time in milliseconds linux
    sscanf(argv[1],"%d",&mode);
    switch (mode){
        case 1 :
        {//100Hz
            printf("mode 1: send 100Hz\n");
            break;
        }
        case 2 :
        {//50Hz
            printf("mode 2: send 50Hz\n");
            break;
        }
        default:
            printf ("wrong mode. choose 1 for sending at 100Hz, 2 for sending 50Hz\n");
            return 0;
            break;

    }
    // init the sockets
    if ( initServerSocket(8888, &s_onBoard, &insock)==1 && initClientSocket(6666, &s_sim, "127.0.0.1",&outsock)==1 )
        {
            printf ("init success\n");
        }

        while(1){

            if(mode == 1 ){
            // set the file descriptor of the input socket so it can be used in the select fonction
                FD_ZERO(&rfds);
                FD_SET(s_onBoard, &rfds);
                if (FD_ISSET(s_onBoard, &rfds)) printf("rfds set\n");
                // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    s           = spec.tv_sec;
                    ms          = s*1000+round(spec.tv_nsec / 1.0e6);
                    tv.tv_sec   = 0;
                    tv.tv_usec  = 10000;
//                  printf("msbefore %ld ...........",ms);
                    retval= select(sizeof(rfds)*8,&rfds,NULL,NULL, &tv);

                        if (retval == -1)
                        {
                            perror("select()");
                        }
                        else if (retval)
                        {
                            if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                                die("recvfrom()");
                            }
                            printf("packet : %d\t%d\t",data.id,data.psize);
                            int i;
                            for (i=0; i<LEN_BUF_SENSOR-2; i++)
                            {
                                printf("%f\t",data.values[i]);
                            }
                             // save the packet into old buf
                            oldData[1]= oldData[0];
                            oldData[0]= data;
                            // date the endof the recvfrom function and calcul how many time it took to receive the packet
                            clock_gettime(CLOCK_REALTIME, &spec);
                            s= spec.tv_sec;
                            ms = s*1000+round(spec.tv_nsec / 1.0e6)-ms;
                            // calcul the time to wait before sending another packet
                            time_to_wait = 10 - ms;
//                          printf ("--------\n%ld",ms);
                            printf ("--------\n%ld",time_to_wait);
                            //send the received packet and wait for another packet to come
                            // ms is the among of time remaining before sending a new packet. if no time remain ms =-1
                            ms = sendAndWait(rfds,s_onBoard, data, sizeof(SensorData) , outsock, slen, tv, 0, time_to_wait);
                            printf("ms after waiting  %ld\n", ms );
                            // if a new packet is received do....
                            if (ms!=-1)
                            {   printf("j'y suis");
                                if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                                    die("recvfrom()");
                                }
                        // save the packet into oldData
                                oldData[1] = oldData[0];
                                oldData[0] = data;

                                printf("packet is : %d %d", data.id, data.psize);
                                int i;
                                for (i =0; i<6; i++){printf ("\t %f", data.values[i] );}
                        // reset the time value to wait the reöaining time
                                tv.tv_sec   = 0;
                                tv.tv_usec  = ms*1000;
                        // get time so if a new packet arrive i can know how many time i have to wait before sending it
                                clock_gettime(CLOCK_REALTIME, &spec);
                                s   = spec.tv_sec;
                                ms  = s*1000+ round(spec.tv_nsec / 1.0e6);

                                if (FD_ISSET(s_onBoard, &rfds)) printf("rfds set");
                                retval  = select(sizeof(rfds)*8,&rfds,NULL,NULL, &tv);

                                if (retval == -1)
                                    perror("select()");
                        // if another packet come I receive it (discarding the previous out of date one) calcul the time
                        // I have to wait, wait and send the new packet
                                else if(retval)
                                {
                                    if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1)
                                    {
                                    die("recvfrom()");
                                    }
                            // save the packet into old buf
                                    oldData[1] = oldData[0];
                                    oldData[0] = data;


                                    clock_gettime(CLOCK_REALTIME, &spec);
                                    s   = spec.tv_sec;
                                    ms  = s*1000 + round(spec.tv_nsec / 1.0e6) - ms;
                                    //we assume that no more of 2 other packets can arrive early
                                    // so we wait during the time remaining and send the last packet receive (don't care about the previous one)
                                    usleep (tv.tv_usec-(ms*1000));
                                    if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                                    {
                                        die("sendto()");
                                    }
                                }
                        // No new packet has been receive so I send the packet received before
                                else
                                {
                                    if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                                    {
                                        die("sendto()");
                                    }
                                }

                            }
                            // No packet received while waiting in the first send and
                            else printf ("i wait too long\n");

                        }

                        else
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
                        }

//                if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
//                    die("recvfrom()");
//                }
//                printf("packet : %d\t%d\t",data.id,data.psize);
//                int i;
//                for (i=0; i<LEN_BUF_SENSOR-2; i++)
//                {
//                    printf("%f\t",data.values[i]);
//                }
//                // save the packet into old buf
//                oldData[1]= oldData[0];
//                oldData[0]= data;
//                // date the endof the recvfrom function qnd calcul how many time it took to receive the packet
//                clock_gettime(CLOCK_REALTIME, &spec);
//                s= spec.tv_sec;
//                ms = s*1000+round(spec.tv_nsec / 1.0e6)-ms;
//                time_to_wait = 10 - ms;
////                printf ("--------\n%ld",ms);
////                printf ("--------\n%ld",time_to_wait);
//                if (time_to_wait<0)
//                {
//                    printf("TIMEOUT\n");
//                    // if TIMEOUT is reached ze extrapolate with linear approximation from the last two values
//                    data.values[0] = (oldData[0].values[0])+1; // increment the id
//                    int i;
//                    for ( i=2; i<8; i++)
//                    {
//                    data.values[i] = oldData[0].values[i] + oldData[0].values[i] - oldData[1].values[i]; // calcul & assign the extrapolated value to the data we send
//                    }
//                    if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
//                            {
//                                die("sendto()");
//                            }
//                }


//                    //printf("i'm in mode 1\n");
//                    ms = sendAndWait(rfds,s_onBoard, data, sizeof(SensorData) , 0 , outsock, slen, tv, 0, time_to_wait);
//                    printf("ms after waiting  %ld\n", ms );
//                    if (ms!=-1){
//                        if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
//                        die("recvfrom()");
//                        }
//                        // save the packet into old buf
//                        oldData[1] = oldData[0];
//                        oldData[0] = data;
//
//                        printf("packet is : %d %d", data.id, data.psize);
//                        int i;
//                        for (i =0 ; i<8 ; i++){printf ("\t %f", data.values[i] );}
//
//
//                        tv.tv_sec   = 0;
//                        tv.tv_usec  = ms*1000;
//                        // get time so if a new packet arrive i can know how many time i have to wait before sending it
//                        clock_gettime(CLOCK_REALTIME, &spec);
//                        s   = spec.tv_sec;
//                        ms  = s*1000+ round(spec.tv_nsec / 1.0e6);
//
//                        if (FD_ISSET(s_onBoard, &rfds)) printf("rfds set");
//                        retval  = select(1,&rfds,NULL,NULL, &tv);
//
//                        if (retval == -1)
//                            perror("select()");
//                        // if another packet come I receive it (discarding the previous out of date one) calcul the time
//                        // I have to wait, wait and send the new packet
//                        else if(retval)
//                        {
//                            if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
//                                die("recvfrom()");
//                            }
//                            // save the packet into old buf
//                            oldData[1] = oldData[0];
//                            oldData[0] = data;
//
//
//                            clock_gettime(CLOCK_REALTIME, &spec);
//                            s   = spec.tv_sec;
//                            ms  = s*1000 + round(spec.tv_nsec / 1.0e6) - ms;
//                            usleep (tv.tv_usec-(ms*1000));
//                            if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
//                            {
//                                die("sendto()");
//                            }
//                        }
//                        // No new packet has been receive so I send the packet received before
//                        else
//                        {
//                         if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
//                        {
//                            die("sendto()");
//                        }
//                        }
//
//                    }
//                    else printf ("i wait too long\n");

            }

            if(mode == 2 ){
                FD_ZERO(&rfds);
                FD_SET(s_onBoard, &rfds);
                if (FD_ISSET(s_onBoard, &rfds)) printf("rfds set\n");
                // date the start of the recvfrom function
                    clock_gettime(CLOCK_REALTIME, &spec);
                    s           = spec.tv_sec;
                    ms          = s*1000+round(spec.tv_nsec / 1.0e6);
                    tv.tv_sec   = 0;
                    tv.tv_usec  = 20000;
//                  printf("msbefore %ld ...........",ms);
                    // TODO what if it receive nothing (set a TIMEOUT)
                    retval= select(sizeof(rfds)*8,&rfds,NULL,NULL, &tv);

                        if (retval == -1)
                        {
                            perror("select()");
                        }
                        else if (retval)
                        {
                            if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData),0 ,  (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                                die("recvfrom()");
                            }
                            printf("packet : %d\t%d\t",data.id,data.psize);
                            int i;
                            for (i=0; i<LEN_BUF_SENSOR-2; i++)
                            {
                                printf("%f\t",data.values[i]);
                            }
                            // save the packet into old buf
                            oldData[1]= oldData[0];
                            oldData[0]= data;
                            // date the endof the recvfrom function qnd calcul how many time it took to receive the packet
                            clock_gettime(CLOCK_REALTIME, &spec);
                            s= spec.tv_sec;
                            ms = s*1000+round(spec.tv_nsec / 1.0e6)-ms;
                            time_to_wait = 20 - ms;
//                          printf ("--------\n%ld",ms);
                       printf ("--------\n%ld",time_to_wait);
                            ms = sendAndWait(rfds,s_onBoard, data, sizeof(SensorData) , outsock, slen, tv, 0, time_to_wait);
                            printf("ms after waiting  %ld\n", ms );
                            if (ms!=-1)
                            {   printf("j'y suis");
                                if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1){
                                    die("recvfrom()");
                                }
                        // save the packet into old buf
                                oldData[1] = oldData[0];
                                oldData[0] = data;

                                printf("packet is : %d %d", data.id, data.psize);
                                int i;
                                for (i =0; i<6; i++){printf ("\t %f", data.values[i] );}

                                tv.tv_sec   = 0;
                                tv.tv_usec  = ms*1000;
                        // get time so if a new packet arrive i can know how many time i have to wait before sending it
                                clock_gettime(CLOCK_REALTIME, &spec);
                                s   = spec.tv_sec;
                                ms  = s*1000+ round(spec.tv_nsec / 1.0e6);

                                if (FD_ISSET(s_onBoard, &rfds)) printf("rfds set");
                                retval  = select(sizeof(rfds)*8,&rfds,NULL,NULL, &tv);

                                if (retval == -1)
                                    perror("select()");
                        // if another packet come I receive it (discarding the previous out of date one) calcul the time
                        // I have to wait, wait and send the new packet
                                else if(retval)
                                {
                                    if ((recv_len = recvfrom(s_onBoard, &data, sizeof(SensorData), 0, (struct sockaddr *) &outsock, (socklen_t*) &slen)) == -1)
                                    {
                                    die("recvfrom()");
                                    }
                            // save the packet into old buf
                                    oldData[1] = oldData[0];
                                    oldData[0] = data;


                                    clock_gettime(CLOCK_REALTIME, &spec);
                                    s   = spec.tv_sec;
                                    ms  = s*1000 + round(spec.tv_nsec / 1.0e6) - ms;
                                    usleep (tv.tv_usec-(ms*1000));
                                    if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                                    {
                                        die("sendto()");
                                    }
                                }
                        // No new packet has been receive so I send the packet received before
                                else
                                {
                                    if (sendto(s_sim, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
                                    {
                                        die("sendto()");
                                    }
                                }

                            }
                            else printf ("i wait too long\n");

                        }

                        else
                        {

                            printf("TIMEOUT\n");
                            // if TIMEOUT is reached ze extrapolate with linear approximation from the last two values
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
                        }
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


long sendAndWait (fd_set rfds, int s_out, struct SensorData buf, int buflen, struct sockaddr_in outsock, int slen,
                        struct timeval tv, int second, int msecond  ){
        int retval;
        //need to reset timer at every loop
        tv.tv_sec=second;//TODO put the real value
        tv.tv_usec=msecond*1000;
        // here I set the variable to see in how many time a new packet is receive
        long s, ms;
        struct timespec spec;
         // Convert nanoseconds to milliseconds

        if (sendto(s_out, &buf, buflen, 0 , (struct sockaddr *) &outsock, slen)==-1)
            {
                die("sendto()");
            }


        clock_gettime(CLOCK_REALTIME, &spec);
        s   = spec.tv_sec;
        ms  = s*1000 + round(spec.tv_nsec / 1.0e6);
        retval= select(sizeof(rfds)*8,&rfds,NULL,NULL, &tv);

        if (retval == -1){
            perror("select()");
            return -1;
            }
        else if (retval){
            clock_gettime(CLOCK_REALTIME, &spec);
            s   = spec.tv_sec;
            ms  = s*1000 + round(spec.tv_nsec / 1.0e6)-ms;
            printf ("packet received\n");
            return ms;
            }
        else
        {
            printf("I have waited %d sec %d mseconds.\n",second, msecond);
            return -1;


        }

}

