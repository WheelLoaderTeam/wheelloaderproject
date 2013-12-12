/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include <sys/time.h>
#include <time.h>
#include "../OryxSim_PC/sensorData.h"

#define PACKETLEN 8  //Max length of buffer
#define PORT 6666  //The port on which to listen for incoming data

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_me, si_other;

    int s, slen = sizeof(si_other) , recv_len;
    SensorData data;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
    long currentTime;
    struct timespec spec;
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
        clock_gettime(CLOCK_REALTIME, &spec);
        currentTime = spec.tv_sec*1000000 + spec.tv_nsec / 1000;

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, &data, sizeof(SensorData), 0, (struct sockaddr *) &si_other, (socklen_t *)&slen)) == -1)
        {
            die("recvfrom()");
        }
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("packet : %d\t%d\t",data.id,data.psize);
                int i;
                for (i=0; i<LEN_BUF_SENSOR-2; i++)
                {
                    printf("%f\t",data.values[i]);
                }
        clock_gettime(CLOCK_REALTIME, &spec);
        currentTime = spec.tv_sec*1000000 + spec.tv_nsec / 1000 - currentTime;
        printf("time between received packet is %ld ms\n",currentTime/1000);

    }

    close(s);
    return 0;
}
