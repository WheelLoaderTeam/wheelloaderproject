#include <stdio.h>
#include <stdlib.h>
#include <float.h> 				//DBL_MAX
#include <math.h> 				//asin
#define SS_CHECKFREQUENCY 5		//ss = StationaryState
#define SS_DELTATHRESHOLD 0.50
#define NUM_OF_SS_RECORDS 200   //At 100 hz = 2 seconds of data NOTE!!! circular_buffer.num_valid_rec must cap here!!!
#define DELAY_LENGTH      250   //Length measured in # of records i.e 250 records = 2.5 sec
#define END_RCV_BUF    	  1023	//Adjust depending on size of buffer
#define BEGIN_RCV_BUF     0
#define GYRO_POINTS_PER_PCKT 2   //Number of data points stored in each Gyro packet
#define TRUE              1
#define FALSE             0
#define BYTES_PER_PACKET  32   

typedef struct{
    int last_write_pos;
    int num_valid_rec;
}circular_buffer;

typedef struct{
	float xAxis;
	float yAxis;
	float zAxis;
}gyro_bias;

typedef struct{
	float pitch;
    float roll;
}abs_pos;

typedef struct{
    float id;
    float size;
}packet_header;

//This struct and function decleration are to be declared in recvData.c. They are declared here for testing purposes.  
typedef struct{
    float posX;
    float posY;
    float posZ;
    float rotX;
    float rotY;
    float rotZ;
}packet_load;
packet_load recvData();


int parseFile(char *filename);
int checkIfSS(circular_buffer savebuffer);
abs_pos getAbsPos(circular_buffer savebuffer);
gyro_bias getGyroBias(circular_buffer savebuffer);
void sendData(packet_load packet_old, packet_load packet_new, gyro_bias bias, packet_header header, char ss_flag);
void sendAccData();

int main(int argc, char *argv[]) {
	int rcv_buf_pos;
	int counter = 0;
    char ss_flag = FALSE;
    circular_buffer savebuffer;
    savebuffer.last_write_pos = -1;
    savebuffer.num_valid_rec = 0;
    packet_header header;
    header.id = 0;
    header.size = BYTES_PER_PACKET;
    packet_load packet_old;
    packet_load packet_new;
	abs_pos radians; // must think about the best way to initialize these structs. 
	gyro_bias bias;
    rcv_buf_pos = parseFile(argv[1]);

	while(1){
		packet_old = packet_new;
		packet_new = recvData(); //rcvPacket to be written by Jesper
        writeToBuffer(savebuffer, packet_new);
		sendData(packet_old, packet_new, bias, header, ss_flag);
		counter++;
        header.id ++;
		if(counter == DELAY_LENGTH){
			counter = 0;
			if(checkIfSS(savebuffer)){
                ss_flag = TRUE;
				radians = getAbsPos(savebuffer);
				bias = getGyroBias(savebuffer);
				packet_new.rotY = radians.pitch;
                packet_new.rotX = radians.roll;//Need to modify this to appropriately format packet.
				sendData(packet_old, packet_new, bias, header, ss_flag);
                ss_flag = FALSE;
                header.id ++;
			}
		}
	}
}

double *x_acc_rcv, *y_acc_rcv, *z_acc_rcv, *x_head_rcv, *y_head_rcv, *z_head_rcv;

int writeToBuffer(circular_buffer savebuffer, packet_load packet_new){
    //savebuffer.num_valid_rec must have a cap at NUM_OF_SS_RECORDS - 1!
}

int parseFile(char *filename) {
    char buf[1024];
    char *rcv_ptr;
    int count, i, n = 0;
    FILE *f = fopen(filename,"r");
    
    //find out the number of lines
    while (fgets(buf,1024,f)) {
        n++;
    }
    rewind(f);
    
    //allocate memory
    x_acc_rcv = calloc(n-1,sizeof(double));
    y_acc_rcv = calloc(n-1,sizeof(double));
    z_acc_rcv = calloc(n-1,sizeof(double));
    x_head_rcv = calloc(n-1,sizeof(double));
    y_head_rcv = calloc(n-1,sizeof(double));
    z_head_rcv = calloc(n-1,sizeof(double));
    
    //ignore first line
    fgets(buf,1024,f);
    
    //read the data
    for (i = 0; i<n-1; i++) {
        fgets(buf,1024,f);
        rcv_ptr = buf;
        count = 0;
        while (count < 11) {
            if (*rcv_ptr == ',')
                count++;
            rcv_ptr++;
        }
        sscanf(rcv_ptr,"%lf,%lf,%lf,%lf,%lf,%lf,",&x_acc_rcv[i],&y_acc_rcv[i],&z_acc_rcv[i],
               &x_head_rcv[i],&y_head_rcv[i],&z_head_rcv[i]);
    }
    
    /*//test
    for (i = 0; i<n-1; i++) {
        printf("%lf,%lf,%lf,%lf,%lf,%lf\n",x_acc_rcv[i],y_acc_rcv[i],z_acc_rcv[i],
               x_head_rcv[i],y_head_rcv[i],z_head_rcv[i]);
    }*/
	return n-2;
}

/*
checkIfSS(int rcv_buf_pos)
This function checks if the tractor is in a stationary state. A preset number of latest records(NUM_OF_SS_RECORDS) are read out of the accelerometer buffer. If each accelerometer axis shows a delta smaller than SS_DELTATHRESHOLD , then the tractor is assumed to be stationary, and the function returns 1. If any accelerometer delta exceeds SS_DELTATHRESHOLD, the function returns 0. The algorithm for determining delta is simply maximum - minimum.
*/
int checkIfSS(circular_buffer savebuffer){
    int num_records = savebuffer.num_valid_rec;
    int readindice = savebuffer.last_write_pos;
	double x_min = DBL_MAX, x_max = -1 * DBL_MAX;
	double y_min = DBL_MAX, y_max = -1 * DBL_MAX;
	double z_min = DBL_MAX, z_max = -1 * DBL_MAX;
    
	while( num_records != -1){
		if(readindice == BEGIN_RCV_BUF - 1){
			readindice = END_RCV_BUF;
		}
		if(x_acc_rcv[readindice] > x_max){x_max = x_acc_rcv[readindice];}
		if(x_acc_rcv[readindice] < x_min){x_min = x_acc_rcv[readindice];}
		if(y_acc_rcv[readindice] > y_max){y_max = y_acc_rcv[readindice];}
		if(y_acc_rcv[readindice] < y_min){y_min = y_acc_rcv[readindice];}
		if(z_acc_rcv[readindice] > z_max){z_max = z_acc_rcv[readindice];}
		if(z_acc_rcv[readindice] < z_min){z_min = z_acc_rcv[readindice];}
		readindice--;
	}
	//test
	printf("x_delta = %lf, y_delta = %lf, z_delta = %lf \n", x_max - x_min, y_max - y_min, z_max - z_min);

	if( ((x_max - x_min) < SS_DELTATHRESHOLD) && 
		((y_max - y_min) < SS_DELTATHRESHOLD) && 
		((z_max - z_min) < SS_DELTATHRESHOLD)){
		printf("System is at rest\n");
		return 1;
	}
	else{
		printf("System not at rest\n");
		return 0;
	}
}

/*
getAbsPos(int rcv_buf_pos)
This function calculates pitch(y-rotation) and roll(x-rotation) based on the static acceleration.
These values are used to "reset" the simulator, and erase any accumulated drift.
The gyro data sent is always relative to the latest pitch/roll data, again to prevent drift error.
Called when the system is determined to be at rest. 
*/
abs_pos getAbsPos(circular_buffer savebuffer){
    int num_records = savebuffer.num_valid_rec;
    int readindice = savebuffer.last_write_pos;
	double x_acc_rcvavg = 0, y_acc_rcvavg = 0;
	abs_pos radians;

	while( num_records != -1){
		if(readindice == BEGIN_RCV_BUF - 1){
			readindice = END_RCV_BUF;
		}
		x_acc_rcvavg+= x_acc_rcv[readindice];
		y_acc_rcvavg+= y_acc_rcv[readindice];
		readindice--;
        num_records--;
	}

	x_acc_rcvavg = (x_acc_rcvavg/savebuffer.num_valid_rec);
	y_acc_rcvavg = (y_acc_rcvavg/savebuffer.num_valid_rec);
	radians.pitch = asin (y_acc_rcvavg);
	radians.roll  = asin (x_acc_rcvavg);
	//Test
	printf("Angle in degrees of pitch: %lf, and roll: %lf\n x_acc_rcvavg: %lf y_acc_rcvavg: %lf\n", 
			(radians.pitch * (180/3.14159)), (radians.roll * (180/3.14159)), x_acc_rcvavg, y_acc_rcvavg);
	return radians;
}

/*
getGyroBias()
This function calculates the bias of the gyros. Called when the system is determined to be at rest. 
*/

gyro_bias getGyroBias(circular_buffer savebuffer){
	int num_records = savebuffer.num_valid_rec;
    int readindice = savebuffer.last_write_pos;
	gyro_bias bias;

    
	while( num_records != -1){
		if(readindice == BEGIN_RCV_BUF - 1){
			readindice = END_RCV_BUF;
		}
		bias.xAxis+= x_head_rcv[readindice];
		bias.yAxis+= y_head_rcv[readindice];
		bias.zAxis+= z_head_rcv[readindice];
		readindice--;
        num_records--;
	};

	bias.xAxis = bias.xAxis/savebuffer.num_valid_rec;
	bias.yAxis = bias.yAxis/savebuffer.num_valid_rec;
	bias.zAxis = bias.zAxis/savebuffer.num_valid_rec;
	//Test
	printf("X Gyro bias: %lf, Y Gyro bias: %lf, Z Gyro bias: %lf\n", bias.xAxis, bias.yAxis, bias.zAxis);
	return bias;
}

/*
sendGyroData()
This function subtracts the appropriate gyro bias, and sends the data to the correct address.
*/

void sendData(packet_load packet_old, packet_load packet_new, gyro_bias bias, packet_header header, char ss_flag){
    packet_load packet_to_send;

    if (ss_flag) {
        packet_to_send.rotX = packet_new.rotX;
        packet_to_send.rotY = packet_new.rotY;
    }
    else{
        packet_to_send.rotX = (packet_old.rotX + (packet_new.rotX - (bias.xAxis * GYRO_POINTS_PER_PCKT)));
        packet_to_send.rotY = (packet_old.rotY + (packet_new.rotY - (bias.yAxis * GYRO_POINTS_PER_PCKT)));
    }

}


/*
sendAccData()
This function takes the raw acceleration data, multiplies it by a constant and sends it to the simulator
*/

void sendAccData(){

}

/*
 recvData()
 Temporary function for written for compilation purposes. Function to be used is written by Jesper. 
 */
 
packet_load recvData(){
    
    
}
