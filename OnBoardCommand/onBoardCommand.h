#define _XOPEN_SOURCE 600

#include <signal.h>

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
#include "tsmod.h"
#include "stats.h"


int commandPacket2EBUpacket(commandPacket* command, EBUanalogOut* analogEBUpacket);

void INT_handler(int dummy);
void resetRelays();

