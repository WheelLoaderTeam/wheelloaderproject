#include "Socket.h"

void die(char *s)
{
    perror(s);
    exit(1);
}


int initSocket(int inport,int outport,int si, int so, char* outipaddress, struct sockaddr_in insock, struct sockaddr_in outsock){


    if ((si=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    if ((so=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &insock, 0, sizeof(insock));
    insock.sin_family = AF_INET;
    insock.sin_port = htons(inport);
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *) &outsock, 0, sizeof(outsock));
    outsock.sin_family = AF_INET;
    outsock.sin_port = htons(outport);
    if (inet_aton(outipaddress, &outsock.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    //bind socket to port
    if( bind(si , (struct sockaddr*)&insock, sizeof(insock) ) == -1)
    {
        die("bind");
    }


    return 1;

}

