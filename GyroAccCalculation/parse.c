#include <stdio.h>
#include <stdlib.h>
#include <float.h> 				//DBL_MAX
#include <math.h> 				//asin
#define ss_checkfrequency 5		//ss = StationaryState
#define ss_deltathreshold 0.50
#define ss_numrecords	  200   //At 100 hz = 2 seconds of data
#define end_rcv_buf    	  1024	//Adjust depending on size of buffer
#define begin_rcv_buf     0


typedef struct{
	double xAxis;
	double yAxis;
	double zAxis;
}gyro_bias;

typedef struct{
	double pitch;
	double roll;
}abs_pos;
	


int parseFile(char *filename);
int checkIfSS(int rcv_buf_pos);
abs_pos getAbsPos(int rcv_buf_pos);
gyro_bias getGyroBias(int rcv_buf_pos);
void sendPacket(int packet_old, int packet_new, double x_bias, double y_bias, double z_bias);
void sendAccData();

int main(int argc, char *argv[]) {
	int rcv_buf_pos;
	int counter = 0, packet_old = 0, packet_new = 0;
	abs_pos radians; // must think about the best way to initialize these structs. 
	gyro_bias bias;
    rcv_buf_pos = parseFile(argv[1]);

	while(1){
		packet_old = packet_new;
		while(packet_new = rcvPacket()); //rcvPacket to be written by Jesper
		//Create circular buffer and store packet_new here, updating buffer ptr
		while(sendPacket(packet_old, packet_new, bias.xAxis, bias.yAxis, bias.zAxis));
		counter++;
		if(counter == ss_numrecords){
			counter = 0;
			if(checkIfSS(rcv_buf_pos)){
				radians = getAbsPos(rcv_buf_pos);
				bias = getGyroBias(rcv_buf_pos);
				packet_new = radians.pitch + radians.roll;//Need to modify this to appropriately format packet. 
				sendPacket(0, packet_new, 0, 0, 0);
			}
		}
	}
}

double *x_acc_rcv, *y_acc_rcv, *z_acc_rcv, *x_head_rcv, *y_head_rcv, *z_head_rcv;


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
checkIfSS(int buf_pos)
This function checks if the tractor is in a stationary state. A preset number of latest records(ss_numrecords) are read out of the accelerometer buffer. If each accelerometer axis shows a delta smaller than ss_deltathreshold , then the tractor is assumed to be stationary, and the function returns 1. If any accelerometer delta exceeds ss_deltathreshold, the function returns 0. The algorithm for determining delta is simply maximum - minimum.
*/
int checkIfSS(int rcv_buf_pos){
	int i = ss_numrecords;
	double x_min = DBL_MAX, x_max = -1 * DBL_MAX;
	double y_min = DBL_MAX, y_max = -1 * DBL_MAX;
	double z_min = DBL_MAX, z_max = -1 * DBL_MAX;

	//printf("Buf Pos = %d\n", rcv_buf_pos);
	do{
		if(rcv_buf_pos == begin_rcv_buf){
			//wrap around to correct position
		}
		if(x_acc_rcv[rcv_buf_pos] > x_max){x_max = x_acc_rcv[rcv_buf_pos];}
		if(x_acc_rcv[rcv_buf_pos] < x_min){x_min = x_acc_rcv[rcv_buf_pos];}
		if(y_acc_rcv[rcv_buf_pos] > y_max){y_max = y_acc_rcv[rcv_buf_pos];}
		if(y_acc_rcv[rcv_buf_pos] < y_min){y_min = y_acc_rcv[rcv_buf_pos];}
		if(z_acc_rcv[rcv_buf_pos] > z_max){z_max = z_acc_rcv[rcv_buf_pos];}
		if(z_acc_rcv[rcv_buf_pos] < z_min){z_min = z_acc_rcv[rcv_buf_pos];}
		rcv_buf_pos--;
	}while(i--);
	//test
	printf("x_delta = %lf, y_delta = %lf, z_delta = %lf \n", x_max - x_min, y_max - y_min, z_max - z_min);

	if( ((x_max - x_min) < ss_deltathreshold) && 
		((y_max - y_min) < ss_deltathreshold) && 
		((z_max - z_min) < ss_deltathreshold)){
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
abs_pos getAbsPos(int rcv_buf_pos){
	int i = ss_numrecords;
	double x_acc_rcvavg = 0, y_acc_rcvavg = 0;
	abs_pos radians;

	do{
		if(rcv_buf_pos == begin_rcv_buf){
			//wrap around to correct position
		}
		x_acc_rcvavg+= x_acc_rcv[rcv_buf_pos];
		y_acc_rcvavg+= y_acc_rcv[rcv_buf_pos];
		rcv_buf_pos--;
	}while(i--);

	x_acc_rcvavg = (x_acc_rcvavg/ss_numrecords);
	y_acc_rcvavg = (y_acc_rcvavg/ss_numrecords);
	radians.pitch = asin (y_acc_rcvavg);
	radians.roll  = asin (x_acc_rcvavg);
	//Test
	printf("Angle in degrees of pitch: %lf, and roll: %lf\n x_acc_rcvavg: %lf y_acc_rcvavg: %lf\n", 
			(radians.pitch * (180/3.14159)), (radians.roll * (180/3.14159)), x_acc_rcvavg, y_acc_rcvavg);
	return radians;
}

/*
calcGyroBias()
This function calculates the bias of the gyros. Called when the system is determined to be at rest. 
*/

gyro_bias getGyroBias(int rcv_buf_pos){
	int i = ss_numrecords;
	gyro_bias bias;
	do{
		if(rcv_buf_pos == begin_rcv_buf){
			//wrap around to correct position
		}
		bias.xAxis+= x_head_rcv[rcv_buf_pos];
		bias.yAxis+= y_head_rcv[rcv_buf_pos];
		bias.zAxis+= z_head_rcv[rcv_buf_pos];
		rcv_buf_pos--;
	}while(i--);

	bias.xAxis = bias.xAxis/ss_numrecords;
	bias.yAxis = bias.yAxis/ss_numrecords;
	bias.zAxis = bias.zAxis/ss_numrecords;
	//Test
	printf("X Gyro bias: %lf, Y Gyro bias: %lf, Z Gyro bias: %lf\n", bias.xAxis, bias.yAxis, bias.zAxis);
	return bias;
}

/*
sendGyroData()
This function subtracts the appropriate gyro bias, and sends the data to the correct address.
*/

void sendGyroData(int gyro_packet_old, int gyro_packet_new, double x_bias, double y_bias, double z_bias){

	//Packet format = Acc xyz gyro xyz, 12 bytes(2 per)

}


/*
sendAccData()
This function takes the raw acceleration data, multiplies it by a constant and sends it to the simulator
*/

void sendAccData(){

}
