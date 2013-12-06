#include <stdio.h>
#include <stdlib.h>

typedef struct stats {
	int packetsReceived;
	int packetsLost;
	int packetsOutOfOrder;
	
	int packetsSent;
	
	float lastReceivedLift;
	float lastReceivedTilt;

	float lastSentA9;
	float lastSentA10;
	float lastSentA11;
	float lastSentA12;
}stats;

#define new_stats {0,0,0,0,0,0,0,0,0,0}

void printStats(stats* s);
