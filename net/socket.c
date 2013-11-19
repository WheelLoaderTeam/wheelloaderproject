#include "socket.h"

int initServerSocket(int port, int* s, struct sockaddr_in* sock){
	initSocket(port, s, sock);
//	bind socket to port
	if( bind(*s , (struct sockaddr*)sock, sizeof(struct sockaddr_in) ) == -1){
		die("bind");
	}
	return 1;
}

int initClientSocket(int port, int* s, char* ipaddress, struct sockaddr_in* sock){
	initSocket(port, s, sock);
	if (inet_pton(AF_INET, ipaddress, &(sock->sin_addr)) == 0){
		fprintf(stderr, "inet_pton() failed\n");
		exit(1);
	}
	return 1;
}

int initSocket(int port, int* s, struct sockaddr_in* sock){
	if ((*s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		die("socket");
	}
	memset((char *) sock, 0, sizeof(struct sockaddr_in));
	sock->sin_family = AF_INET;
	sock->sin_port = htons(port);
	sock->sin_addr.s_addr = htonl(INADDR_ANY);
	return 1;
}

void die(char *s)
{
	perror(s);
	exit(1);
}
