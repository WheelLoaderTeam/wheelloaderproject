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
#include<stdint.h>

#define SERVER "127.0.0.1"
#define PORT 65401   //The port on which to send data

void die(char *s)
{
    perror(s);
    exit(1);
}
struct packetFormat{
    uint32_t packetId;
    uint32_t packetSize;
    float analog[9];
    uint32_t digital;
};
int main(void)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    struct packetFormat packet = {0, sizeof(struct packetFormat), 0.1, -0.1, 0.5, -0.5, 0.4, 0.8, 0.3, 0.7, 0.6, -0.2};

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_pton() failed\n");
        exit(1);
    }

    while(1)
    {
        packet.packetId++;

        //send the message
        if (sendto(s, &packet, sizeof(struct packetFormat) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        else{
	    fprintf(stderr, "Packet %d sent\t", packet.packetId);
            usleep(80000);
	}

    }

    close(s);
    return 0;
}
