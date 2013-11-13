#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int main();
void initCOM1();
void keyboardInput();
void* COM1Input();
void convert();

char COM1;
char POSx;
char POSy;
char POSz;
char ROTy;
char ROTz;

int main(){

	sem_init(&sem[0], 0, 0);
	sem_init(&sem[1], 0, 0);

	pthread_t threads;
	pthread_create(&threads, NULL, COM1Input, NULL);

	return 1;
}

void initCOM1(){
	COM1=open("/dev/ttyS0", O_RDWR); // open comport
	struct termios options;

    tcgetattr(COM1, &options);//Get the current options for the port

    cfsetispeed(&options, B9600);//Set the baud rates to 9600
    cfsetospeed(&options, B9600);

	options.c_cflag &= ~PARENB;	//no parity checking
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cflag &= ~CSIZE; //Character Size 5 bits
    options.c_cflag |= CS5;

    tcsetattr(COM1, TCSANOW, &options);//Set the new options for the port
}

void* COM1Input(){

	initCOM1();

	fd_set rfds;
	FD_ZERO(&rfds); // emtpy set
	FD_SET(COM1, &rfds); // include com1:

	char input[1];
	while(1){

        select(4,&rfds,NULL,NULL,NULL); // wait
		read(COM1, input,  1); // read data from COM1 and put it in input 1 ligne so 1 packet

		}

	}

void convert(){



}
