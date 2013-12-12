#include "stats.h"

void printStats(stats* s){
	system("clear");
	
	printf("No. of packets recived: %d\n", s->packetsReceived);
	printf("No. of packets lost: %d\n", s->packetsLost);
	printf("No. of packets arriving out of order: %d\n", s->packetsOutOfOrder);
	
	printf("No. of packets sent: %d\n", s->packetsSent);
	
	
	printf("Received lift: %f\n", s->lastReceivedLift);
	printf("Received tilt: %f\n", s->lastReceivedTilt);
	
	
	printf("Sent lift (A9): %f\n", s->lastSentA9);
	printf("Sent lift (A10): %f\n", s->lastSentA10);
	printf("Sent lift (A11): %f\n", s->lastSentA11);
	printf("Sent lift (A12): %f\n", s->lastSentA12);
	
	
	printf("Minimum transmission time: %lld.%.9ld\n", (long long)s->minTransmissionTime.tv_sec, s->minTransmissionTime.tv_nsec);
	printf("Maximum transmission time: %lld.%.9ld\n", (long long)s->maxTransmissionTime.tv_sec, s->maxTransmissionTime.tv_nsec);
	printf("Mean transmission time: %lld.%.9ld\n", (long long)s->meanTransmissionTime.tv_sec, s->meanTransmissionTime.tv_nsec);

}

void newTransmissionTime(stats* s, struct timespec transmissionTime){
	s->totalTransmissionTime = tsAdd(s->totalTransmissionTime, transmissionTime);
	s->meanTransmissionTime = tsDiv(s->totalTransmissionTime, s->packetsReceived);
	
	if(tsComp(s->minTransmissionTime, transmissionTime) == 1){ //minTransmissionTime > transmissionTime
		s->minTransmissionTime = transmissionTime;
	}
	if(tsComp(s->maxTransmissionTime, transmissionTime) == -1){ //maxTransmissionTime < transmissionTime
		s->maxTransmissionTime = transmissionTime;
	}
}

