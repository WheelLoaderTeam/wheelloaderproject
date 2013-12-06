#include "stats.h"

void printStats(stats* s){
	system("clear");
	printf("No. of packets recived: %d\n", s->packetsReceived);
	printf("No. of packets lost: %d\n", s->packetsLost);
	printf("No. of packets arriving out of order: %d\n", s->packetsOutOfOrder);
	
	printf("No. of packets sent: %d\n", s->packetsSent);

///*
	printf("Received lift: %f\n", s->lastReceivedLift);
	printf("Received tilt: %f\n", s->lastReceivedTilt);
//*/
///*
	printf("Sent lift (A9): %f\n", s->lastSentA9);
	printf("Sent lift (A10): %f\n", s->lastSentA10);
	printf("Sent lift (A11): %f\n", s->lastSentA11);
	printf("Sent lift (A12): %f\n", s->lastSentA12);
//*/
}

