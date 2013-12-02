/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include"../OryxSim_PC/sensorData.h"

#define SERVER "127.0.0.1"
#define PACKETLEN 8 //Max length of buffer
#define PORT 8888   //The port on which to send data

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);

    /////////////////////// declare what you want to send//////////////////////////////
    SensorData data;
    data.id = 0;
    data.psize = PACKETLEN;
    data.values[0]=9.57;
    data.values[1]=50.69;
    data.values[2]=24.56;
    data.values[3]=42.32;
    data.values[4]=74.96;
    data.values[5]=38.06;
    ///////////////////////////////////////////////////////////////////////////////////
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    while(1)
    {


        //send the message
        if (sendto(s, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        data.id +=1;
        usleep(11000);



    }

    close(s);
    return 0;
}
