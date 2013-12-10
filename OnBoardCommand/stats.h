#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include "tsmod.h"

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
	
	struct timespec totalTransmissionTime;
	struct timespec minTransmissionTime;
	struct timespec maxTransmissionTime;
	struct timespec meanTransmissionTime;
}stats;

#define new_stats {0,0,0,0,0,0,0,0,0,0, {0, 0}, {9999999999, 999999999}, {0, 0}, {0, 0} }

void printStats(stats* s);
void newTransmissionTime(stats* s, struct timespec transmissionTime);

