#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <termios.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#include "EBUrelays.h"
#include "EBUanalogIn.h"
#include "EBUanalogOut.h"
#include "socket.h"

struct termios saved_attributes;

void
reset_input_mode (void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void
set_input_mode (void)
{
	struct termios tattr;
	char *name;
	
	/* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO))
	{
		fprintf (stderr, "Not a terminal.\n");
		exit (EXIT_FAILURE);
	}
	
	/* Save the terminal attributes so we can restore them later. */
	tcgetattr (STDIN_FILENO, &saved_attributes);
	atexit (reset_input_mode);
	
	/* Set the funny terminal modes. */
	tcgetattr (STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 0;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

int main(void){
	struct timespec t, a;
	t.tv_sec = 0;
	t.tv_nsec = 30000000;
	
	a.tv_sec = 1;
	a.tv_nsec = 0;

	char c;
	set_input_mode ();
	
	//setup sockets
	struct sockaddr_in relays_socket, analog_in_socket, analog_out_socket, digital_in_socket, digital_out_socket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_relays, s_analog_in, s_analog_out, s_digital_in, s_digital_out;
	
	initServerSocket(25101, &s_analog_in, &analog_in_socket);
	initServerSocket(25301, &s_digital_in, &digital_in_socket);
	
	initClientSocket(25400, &s_relays, "10.0.0.2", &relays_socket);
	initClientSocket(25200, &s_analog_out, "10.0.0.2", &analog_out_socket);
	
	
	//building relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	
	
	//wait before sending. this is a workaround to a bug in the EBU
	//TODO: Remove nanosleep and timespec t when the EBU bug is fixed.
	nanosleep(&t, NULL);
	//send relay packet
	sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
	
	c = ' ';
	while(1){
		char buf[255];
		EBUanalogIn analogIn = new_EBUanalogIn;
		EBUanalogOut analogOut = new_EBUanalogOut;
		
		//wait for analog in packet
		recvfrom(s_analog_in, buf, 255, 0, (struct sockaddr*) &analog_in_socket, &slen);
		memcpy(&analogIn, buf, sizeof(EBUanalogIn)); //copy data to type

		int r = read(STDIN_FILENO, &c, 1);
		if(r){
			if (c == 'w'){
				setAnalogOut(&analogOut, AO_9, 4);
				setAnalogOut(&analogOut, AO_10, 1);
				setAnalogOut(&analogOut, AO_11, 2.5);
				setAnalogOut(&analogOut, AO_12, 2.5);
				printf("W");
			}
			else if (c == 's'){
				setAnalogOut(&analogOut, AO_9, 1);
				setAnalogOut(&analogOut, AO_10, 4);
				setAnalogOut(&analogOut, AO_11, 2.5);
				setAnalogOut(&analogOut, AO_12, 2.5);
				printf("S");
			}
			else if (c == 'a'){
				setAnalogOut(&analogOut, AO_9, 2.5);
				setAnalogOut(&analogOut, AO_10, 2.5);
				setAnalogOut(&analogOut, AO_11, 4);
				setAnalogOut(&analogOut, AO_12, 1);
			}
			else if (c == 'd'){
				setAnalogOut(&analogOut, AO_9, 2.5);
				setAnalogOut(&analogOut, AO_10, 2.5);
				setAnalogOut(&analogOut, AO_11, 1);
				setAnalogOut(&analogOut, AO_12, 4);
			}
			else{
				setAnalogOut(&analogOut, AO_9, 2.5);
				setAnalogOut(&analogOut, AO_10, 2.5);
				setAnalogOut(&analogOut, AO_11, 2.5);
				setAnalogOut(&analogOut, AO_12, 2.5);
			}
			
			//wait before sending. this is a workaround to a bug in the EBU
			//TODO: Remove nanosleep and timespec t when the EBU bug is fixed.
			nanosleep(&t, NULL);
			//send analog out packet
			sendto(s_analog_out, (char*)&analogOut, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
		}
		
		system("clear");
		float value;
		for(int i=0; i<24; i++){
			value = getAnalogIn(&analogIn, i);
			printf("A%d= %f\n", (i+1), value);
		}
		printf("%c\n",c);
//		nanosleep(&a, NULL);
	}
}
