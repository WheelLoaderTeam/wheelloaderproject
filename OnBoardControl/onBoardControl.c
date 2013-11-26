
#include "onBoardControl.h"

const struct timespec LONG_TIMEOUT = {2, 0};
const struct timespec SHORT_TIMEOUT = {0, 100000000};

int main(void){
	int bufferEmpty = 1;
	EBUanalogOut buffer;
	uint32_t lastPacketID;
	struct timespec timeOflastPacketArrival;
	struct timespec timeOflastPacketSent;
	struct timespec now;
	
	struct timespec timeout = LONG_TIMEOUT;
	
	//setup sockets to send packets to the EBU
	//send stop packet
	//send relay packet
	
	//initServerSocket to receve packets from simulator
	//setup filedescriptors for select to wait for.
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(0, &fs);
	//set long timeout
	
	while(0){
		//wait for packet from simulator
		//peslect(NULL, NULL, NULL, NULL, NULL, NULL)
		//if //timeout
			if(bufferEmpty){
				//send stop packet
				//set long timeout
			} else{ //buffer not empty
				//send packet in buffer
				//clear buffer
				//set long timeout
			}
		//else //incomming packet
			if(bufferEmpty){
				if(0){ //(time since last packet sent > short timeout)
					//send packet imediatly
					//set long timeout
				}else //(time since last packet sent < short timeout)
					//put packet in buffer
					//set remaining time of short timeout
					clock_gettime(CLOCK_REALTIME, &now);
					timeout = tsSub(now, tsAdd(timeOflastPacketSent, SHORT_TIMEOUT));
				}
			} else{ //buffer not empty
				//determine witch packet should remain in buffer
				//set remaining time of short timeout
				clock_gettime(CLOCK_REALTIME, &now);
				timeout = tsSub(now, tsAdd(timeOflastPacketSent, SHORT_TIMEOUT));
			}
	}
	struct timespec t1;
	struct timespec t2;
	struct timespec r;
	t1.tv_sec = 30;
	t1.tv_nsec = 600000000;
	t2.tv_sec = 30;
	t2.tv_nsec = 700000000;

	r = tsAdd(t1, t2);

	if (r.tv_nsec < 0){
		printf("less than zero\n");
	}
	printf("%lu\n", r.tv_sec);
	printf("%lu\n", r.tv_nsec);
	
	return 0;
}

struct timespec tsAdd(struct timespec ts1, struct timespec ts2){
	struct timespec result;
	result.tv_sec = ts1.tv_sec + ts2.tv_sec;
	result.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
	
	if(result.tv_nsec > 1000000000){
		result.tv_sec++;
		result.tv_nsec -= 1000000000;
	}
	return result;
}

struct timespec tsSub(struct timespec ts1, struct timespec ts2){
	struct timespec result;
	result.tv_sec = ts1.tv_sec - ts2.tv_sec;
	result.tv_nsec = ts1.tv_nsec - ts2.tv_nsec;
	
	if(result.tv_nsec < 0){
		result.tv_sec--;
		result.tv_nsec += 1000000000;
	}
	return result;
}

int tsComp(struct timespec ts1, struct timespec ts2){
	if(ts1.tv_sec < ts2.tv_sec){
		return -1;
	}else if(ts1.tv_sec > ts2.tv_sec){
		return 1;
	}else{
		if(ts1.tv_nsec < ts2.tv_nsec){
			return -1;
		}else if(ts1.tv_nsec > ts2.tv_nsec){
			return 1;
		}else{
			return 0;
		}		
	}
	
}
//int commandPacket2EBUpacket(CommandPacket* commandPacket, EBUanalogOut* analogEBUpacket){
//	
//	
//	
//	return 0;
//}
