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

#define SERVER "192.168.2.97"
#define PACKETLEN 8 //Max length of buffer
//#define PORT    //The port on which to send data

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    float increment = 0.001, increment2 = 0.0005;
    /////////////////////// declare what you want to send//////////////////////////////
    SensorData data;
    data.id = 2001;
    data.psize = 32;
    data.values[0]=0;
    data.values[1]=0;
    data.values[2]=0;
    data.values[3]=0;
    data.values[4]=0;
    data.values[5]=0;
    ///////////////////////////////////////////////////////////////////////////////////
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(MOV_PORT);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    while(1)
    {
        //data.values[3] += increment;
        //if (data.values[3] > 0.15 || data.values[3] < -0.15) {
        //    increment *= -1;
        //}
        data.values[2] += increment2;
        if (data.values[2] > 0.2 || data.values[2] < -0.2) {
            increment2 *= -1;
        }

        //send the message
        if (sendto(s, &data, sizeof(SensorData) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        usleep(10000);



    }

    close(s);
    return 0;
}
