#define _XOPEN_SOURCE 600

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include <sys/select.h>

#include "EBUrelays.h"
#include "EBUanalogOut.h"

#include "commandPacket.h"

#include "socket.h"

#include "stats.h"


int commandPacket2EBUpacket(commandPacket* command, EBUanalogOut* analogEBUpacket);

struct timespec tsAdd(struct timespec ts1, struct timespec ts2);
struct timespec tsSub(struct timespec ts1, struct timespec ts2);
int tsComp(struct timespec ts1, struct timespec ts2);
