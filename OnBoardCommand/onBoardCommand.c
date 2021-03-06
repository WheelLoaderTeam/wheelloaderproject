
#include "onBoardCommand.h"

const struct timespec LONG_TIMEOUT = {2, 0};
const struct timespec SHORT_TIMEOUT = {0, 100000000};
const struct timespec TIMESPECZERO = {0, 0};

int main(void){
	//Setup Signal handler and atexit functions
	signal(SIGINT, INT_handler);
	atexit(resetRelays);

	stats s = new_stats;
	char rcvBuf[255];
	
	int noPacketsReceived = 1;
	int firstPacket = 0;
	
	int bufferEmpty = 1;
	EBUanalogOut buffer;
	uint32_t lastPacketID = 0;
	struct timespec timeOflastPacketSent;
	struct timespec now;
	
	commandPacket comPacket;
	
	struct timespec timeout = LONG_TIMEOUT;
	
	
	//setup sockets to send packets to the EBU and receve from commandPC
	struct sockaddr_in relays_socket, analog_out_socket, commandSocket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_relays, s_analog_out, s_commandSocket;
	
	initClientSocket(EBU_RELAYS_PORT, &s_relays, EBU2_IP, &relays_socket);
	initClientSocket(EBU_ANALOG_OUT_PORT, &s_analog_out, EBU2_IP, &analog_out_socket);
	
	//prepare and send stop packet
	EBUanalogOut analogStop = new_EBUanalogOut;
	setAnalogOut(&analogStop, AO_9, 2.5);
	setAnalogOut(&analogStop, AO_10, 2.5);
	setAnalogOut(&analogStop, AO_11, 2.5);
	setAnalogOut(&analogStop, AO_12, 2.5);
	sendto(s_relays, (char*)&analogStop, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
	
	//prepare and send relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
	
	clock_gettime(CLOCK_REALTIME, &timeOflastPacketSent);
	
	//setup socket to receve packets from simulator
	initServerSocket(CMDO_PORT, &s_commandSocket, &commandSocket);
	
	while(1){
		//setup filedescriptors for select to wait for.
		fd_set fs;
		FD_ZERO(&fs);
		FD_SET(s_commandSocket, &fs);
		
		//wait for packet from simulator
		int rVal;
		printf("Timeout set to: %lld.%.9ld\n", (long long)timeout.tv_sec, timeout.tv_nsec);

		rVal = pselect(s_commandSocket+1, &fs, NULL, NULL, &timeout, NULL);
		clock_gettime(CLOCK_REALTIME, &now);
		if(rVal == 0){ //timeout
			if(bufferEmpty){
				//send stop packet
				s.packetsSent++;
				s.lastSentA9 = getAnalogOut(&analogStop, AO_9);
				s.lastSentA10 = getAnalogOut(&analogStop, AO_10);
				s.lastSentA11 = getAnalogOut(&analogStop, AO_11);
				s.lastSentA12 = getAnalogOut(&analogStop, AO_12);

				sendto(s_relays, (char*)&analogStop, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
			} else { //buffer not empty
				//send packet in buffer
				s.packetsSent++;
				s.lastSentA9 = getAnalogOut(&buffer, AO_9);
				s.lastSentA10 = getAnalogOut(&buffer, AO_10);
				s.lastSentA11 = getAnalogOut(&buffer, AO_11);
				s.lastSentA12 = getAnalogOut(&buffer, AO_12);

				sendto(s_relays, (char*)&buffer, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
				bufferEmpty = 1;//clear buffer
			}
			timeout = LONG_TIMEOUT;
			timeOflastPacketSent = now;
		}else{ //incoming packet
			recvfrom(s_commandSocket, rcvBuf, 255, 0, (struct sockaddr*) &commandSocket, &slen);
			memcpy(&comPacket, rcvBuf, sizeof(commandPacket));
			firstPacket = 0;
			if(noPacketsReceived){
				lastPacketID = comPacket.packetId;
				noPacketsReceived = 0;
				firstPacket = 1;
			}
			
			s.packetsReceived++;
			s.lastReceivedLift = comPacket.analog[LEVER_LIFT];
			s.lastReceivedTilt = comPacket.analog[LEVER_TILT];
			newTransmissionTime(&s, tsSub(now, comPacket.timeSent) );

			if(bufferEmpty){
				if(comPacket.packetId <= lastPacketID && !firstPacket){ //ignore out-of-order packets
					s.packetsLost--;	//If a packet arrives out of order it will preveously been assumed lost.
					s.packetsOutOfOrder++;
					
					//set remaining time of long timeout
					timeout = tsSub(tsAdd(timeOflastPacketSent, LONG_TIMEOUT), now);
				}else if(tsComp(tsSub(now, timeOflastPacketSent), SHORT_TIMEOUT) == 1){ //(time since last packet sent > short timeout)
					//send packet imediatly
					EBUanalogOut tempPacket;
					commandPacket2EBUpacket(&comPacket, &tempPacket);
					
					if(!firstPacket){
						s.packetsLost += comPacket.packetId - lastPacketID - 1;
					}
					s.packetsSent++;
					s.lastSentA9 = getAnalogOut(&tempPacket, AO_9);
					s.lastSentA10 = getAnalogOut(&tempPacket, AO_10);
					s.lastSentA11 = getAnalogOut(&tempPacket, AO_11);
					s.lastSentA12 = getAnalogOut(&tempPacket, AO_12);
					
					sendto(s_relays, (char*)&tempPacket, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
					timeout = LONG_TIMEOUT;
					timeOflastPacketSent = now;
					lastPacketID = comPacket.packetId; // save packetID
				}else{ //(time since last packet sent < short timeout)
					//put packet in buffer
					s.packetsLost += comPacket.packetId - lastPacketID - 1;
					
					commandPacket2EBUpacket(&comPacket, &buffer);
					lastPacketID = comPacket.packetId;
					bufferEmpty = 0;
					//set remaining time of short timeout
					timeout = tsSub(tsAdd(timeOflastPacketSent, SHORT_TIMEOUT), now);
				}
			}else{ //buffer not empty
				if(comPacket.packetId > lastPacketID){//determine witch packet should remain in buffer
					//put packet in buffer
					s.packetsLost += comPacket.packetId - lastPacketID - 1;
					
					commandPacket2EBUpacket(&comPacket, &buffer);
					lastPacketID = comPacket.packetId;
					bufferEmpty = 0;
				}else{
					s.packetsLost--;
					s.packetsOutOfOrder++;
				}
				//set remaining time of short timeout
				timeout = tsSub(tsAdd(timeOflastPacketSent, SHORT_TIMEOUT), now);
			}
		}
		if(tsComp(timeout, TIMESPECZERO) == -1){
			timeout = TIMESPECZERO;
		}
		printStats(&s);
		printf("Last packet ID: %d\n", lastPacketID);
	}
	return 0;
}

int commandPacket2EBUpacket(commandPacket* command, EBUanalogOut* analogEBUpacket){
	float lift = command->analog[LEVER_LIFT] * 2 + 2.5;
	
	setAnalogOut(analogEBUpacket, AO_9, 5-lift);
	setAnalogOut(analogEBUpacket, AO_10, lift);
	
	float tilt = command->analog[LEVER_TILT] * 2 + 2.5;
	setAnalogOut(analogEBUpacket, AO_11, 5-tilt);
	setAnalogOut(analogEBUpacket, AO_12, tilt);
	
	return 0;
}

void INT_handler(int dummy){
	exit(EXIT_SUCCESS);
}

void resetRelays(){
	struct sockaddr_in relays_socket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_relays;
	
	initClientSocket(EBU_RELAYS_PORT, &s_relays, EBU2_IP, &relays_socket);

	EBUrelays relays = newEBUrelays();
	sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
}

