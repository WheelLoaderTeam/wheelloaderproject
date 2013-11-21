#include <stdio.h>
#include <stdlib.h>
#include <float.h> 				//DBL_MAX
#include <math.h> 				//asin
#define ss_checkfrequency 5		//ss = StationaryState
#define ss_deltathreshold 0.50
#define ss_numrecords	  200   //At 100 hz = 2 seconds of data

int parseFile(char *filename);
int checkIfSS(int buf_pos);
void accPitchRoll(int buf_pos);
void gyroBias(int buf_pos);

int main(int argc, char *argv[]) {
	int buffer_position;
    buffer_position = parseFile(argv[1]);
	//checkIfSS(buffer_position);
	//accPitchRoll(buffer_position);
	gyroBias(buffer_position);

	return 0;
}

double *x_acc, *y_acc, *z_acc, *x_head, *y_head, *z_head;

int parseFile(char *filename) {
    char buf[1024];
    char *ptr;
    int count, i, n = 0;
    FILE *f = fopen(filename,"r");
    
    //find out the number of lines
    while (fgets(buf,1024,f)) {
        n++;
    }
    rewind(f);
    
    //allocate memory
    x_acc = calloc(n-1,sizeof(double));
    y_acc = calloc(n-1,sizeof(double));
    z_acc = calloc(n-1,sizeof(double));
    x_head = calloc(n-1,sizeof(double));
    y_head = calloc(n-1,sizeof(double));
    z_head = calloc(n-1,sizeof(double));
    
    //ignore first line
    fgets(buf,1024,f);
    
    //read the data
    for (i = 0; i<n-1; i++) {
        fgets(buf,1024,f);
        ptr = buf;
        count = 0;
        while (count < 11) {
            if (*ptr == ',')
                count++;
            ptr++;
        }
        sscanf(ptr,"%lf,%lf,%lf,%lf,%lf,%lf,",&x_acc[i],&y_acc[i],&z_acc[i],
               &x_head[i],&y_head[i],&z_head[i]);
    }
    
    /*//test
    for (i = 0; i<n-1; i++) {
        printf("%lf,%lf,%lf,%lf,%lf,%lf\n",x_acc[i],y_acc[i],z_acc[i],
               x_head[i],y_head[i],z_head[i]);
    }*/
	return n-2;
}

/*
checkIfSS(int buf_pos)
This function checks if the tractor is in a stationary state. A preset number of latest records(ss_numrecords) are read out of the accelerometer buffer. If each accelerometer axis shows a delta smaller than ss_deltathreshold , then the tractor is assumed to be stationary, and the function returns 1. If any accelerometer delta exceeds ss_deltathreshold, the function returns 0. The algorithm for determining delta is simply maximum - minimum.
*/
int checkIfSS(int buf_pos){
	int i = ss_numrecords;
	double x_min = DBL_MAX, x_max = -1 * DBL_MAX;
	double y_min = DBL_MAX, y_max = -1 * DBL_MAX;
	double z_min = DBL_MAX, z_max = -1 * DBL_MAX;

	//printf("Buf Pos = %d\n", buffer_position);
	do{
		if(buf_pos == 0){
			//wrap around to correct position
		}
		if(x_acc[buf_pos] > x_max){x_max = x_acc[buf_pos];}
		if(x_acc[buf_pos] < x_min){x_min = x_acc[buf_pos];}
		if(y_acc[buf_pos] > y_max){y_max = y_acc[buf_pos];}
		if(y_acc[buf_pos] < y_min){y_min = y_acc[buf_pos];}
		if(z_acc[buf_pos] > z_max){z_max = z_acc[buf_pos];}
		if(z_acc[buf_pos] < z_min){z_min = z_acc[buf_pos];}
		buf_pos--;
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
accPitchRoll(int buf_pos)
This function calculates pitch(y-rotation) and roll(x-rotation) based on the static acceleration.
These values are used to "reset" the simulator, and erase any accumulated drift.
The gyro data sent is always relative to the latest pitch/roll data, again to prevent drift error.
Called when the system is determined to be at rest. 
*/
void accPitchRoll(int buf_pos){
	int i = ss_numrecords;
	double x_accavg = 0, y_accavg = 0;
	double pitch_rad = 0, roll_rad = 0;

	do{
		if(buf_pos == 0){
			//wrap around to correct position
		}
		x_accavg+= x_acc[buf_pos];
		y_accavg+= y_acc[buf_pos];
		buf_pos--;
	}while(i--);

	x_accavg = (x_accavg/ss_numrecords);
	y_accavg = (y_accavg/ss_numrecords);
	pitch_rad = asin (y_accavg);
	roll_rad  = asin (x_accavg);
	//Test
	printf("Angle in degrees of pitch: %lf, and roll: %lf\n x_accavg: %lf y_accavg: %lf\n", 
			(pitch_rad * (180/3.14159)), (roll_rad * (180/3.14159)), x_accavg, y_accavg);
}

/*
gyroBias()
This function calculates the bias of the gyros. Called if the system is determined to be at rest. 
*/

void gyroBias(int buf_pos){
	int i = ss_numrecords;
	double x_gyrbias = 0, y_gyrbias = 0, z_gyrbias = 0;

	do{
		if(buf_pos == 0){
			//wrap around to correct position
		}
		x_gyrbias+= x_head[buf_pos];
		y_gyrbias+= y_head[buf_pos];
		z_gyrbias+= z_head[buf_pos];
		buf_pos--;
	}while(i--);

	x_gyrbias = x_gyrbias/ss_numrecords;
	y_gyrbias = y_gyrbias/ss_numrecords;
	z_gyrbias = z_gyrbias/ss_numrecords;
	//Test
	printf("X Gyro bias: %lf, Y Gyro bias: %lf, Z Gyro bias: %lf\n", x_gyrbias, y_gyrbias, z_gyrbias);

}

/*
sendGyroData()
This function subtracts the gyro bias, and sends the data with respect to the last calibrated pitch and roll.
*/

void sendGyroData(){

}


/*
sendAccData()
This function takes the raw acceleration data, multiplies it by a constant and sends it to the simulator
*/

void sendAccData(){

}
