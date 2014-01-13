#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h> 				//Do we need?
#include <math.h> 				//asin
#include "processdataIMU.h"
#include "../OryxSim_PC/SensorPacketHandling.h"
#include "receiveSensorDataIMU.h"

#define SENSOR_FREQ             100     // expected sensor frequency in Hz
#define BUF_SIZE                200 	// size of circular buffer

#define SS_CHECK_FREQ           0.5     // in Hz
#define SS_DELTA_THRESHOLD      0.50
#define SS_NUM                  200     // at 100 hz = 2 seconds of data

#define K                       0     // position = (acceleration - bias)*K

/*** GLOBAL VARIABLES ***/

//Variable used for shut-down routine
int running =1;

//
circular_buffer savebuffer;
float *x_acc, *y_acc, *z_acc, *x_head, *y_head, *z_head;

// pitch and roll determined when stationary
abs_pos radians;

// current pitch and roll
abs_pos radians_curr;

// bias determined when stationary
bias gyro_bias;
bias acc_bias;


// communication port
int COM1;

//Filed used for logging
FILE *fp;

/*** FUNCTION PROTOTYPES ***/
int processData(sensor_data *data);
void logSensorData(sensor_data *data);
int checkIfSS();
abs_pos getAbsPos();
bias getGyroBias();
bias getAccBias();
void initBuffer();
void writeToBuffer(sensor_data *data);

/*** FUNCTION CODE ***/
int main(int argc, char *argv[]) {
    //Setup Signal handler and atexit functions
	signal(SIGINT, INThandler);                     //Interrupts (calls INThandler) when Ctrl+c (?)
    COM1 = open_serialport("/dev/ttyUSB0",500000);  //Open USB port
    Time_struct Curr_time;                          //Create time structure
    Curr_time = get_time();                         //Fill it with current time
    char fname[26];                                 //Create space for filename
    sprintf(fname, "%d-%d-%d-%d-%d-%d-%d.csv", Curr_time.year, Curr_time.month, Curr_time.day, Curr_time.hour, Curr_time.minute, Curr_time.second, Curr_time.msecond); //Create filename (date, time)
    fp = fopen(fname,"w");                          //Open file
    sensor_data data;                               //Create in-data struct
    initBuffer();
    while(running) {
        data = receiveSensorDataIMU();              //Fetch data
        writeToBuffer(&data);
        if (processData(&data))
            logSensorData(&data);
    }
    //At end by Ctrl+c
    printf("Fin\n");
    fclose(fp);                                     //Close file
    return 0;
}

int processData(sensor_data *data) {
    printf("HI, we're in processData\n"); //This is broken, serves as a delay
    static int  start = SS_NUM;
    static int  counter = 0;
    static bool biasSet = false;
    bool        ss_flag = false;

    // initial data collection
    if (start) {
        start--;
        return 0;
    }

    // need to establish bias first
    if(!biasSet) {
        if (checkIfSS()) {
            radians_curr = getAbsPos();
            gyro_bias = getGyroBias();
            acc_bias = getAccBias();
            ss_flag = true;
            biasSet = true;
        } else {
            return 0;
        }
    }

    // periodically check if we are stationary
    if  (counter > SENSOR_FREQ/SS_CHECK_FREQ) {
        counter = 0;
        if(checkIfSS()){
            radians_curr = getAbsPos();
            gyro_bias = getGyroBias();
            acc_bias = getAccBias();
            ss_flag = true;
        }
    }

    // update current tilt
    if (!ss_flag) {
        radians_curr.roll  += (data->rotX)/SENSOR_FREQ;
        radians_curr.pitch += (data->rotY)/SENSOR_FREQ;
    }

    counter++;
    return 1;
}

void logSensorData(sensor_data *data) {
    printf("HI, we're in logSensorData\n");
    packet_load load;

    load.posX = (data->accX - acc_bias.xAxis) * K;
    load.posY = (data->accY - acc_bias.yAxis) * K;
    load.posZ = (data->accZ - acc_bias.zAxis) * K;

    load.rotX = radians_curr.roll;
    load.rotY = radians_curr.pitch;
    load.rotZ = 0;
    Time_struct Curr_time;
    Curr_time = get_time();
    fprintf(fp, "%d-%d-%d-%d-%d-%d-%d %f %f %f %f %f %f %f %f %f %f %f %f\n", Curr_time.year, Curr_time.month, Curr_time.day, Curr_time.hour, Curr_time.minute, Curr_time.second, Curr_time.msecond, data->accX, data->accY, data->accZ, data->rotX, data->rotY, data->rotZ, load.posX,load.posY,load.posZ, load.rotX, load.rotY, load.rotZ);
}


void initBuffer(){
    savebuffer.last_write_pos = -1;
    savebuffer.num_valid_rec  = 0;
    //allocate memory
    x_acc = calloc(BUF_SIZE,sizeof(float));
    y_acc = calloc(BUF_SIZE,sizeof(float));
    z_acc = calloc(BUF_SIZE,sizeof(float));
    x_head = calloc(BUF_SIZE,sizeof(float));
    y_head = calloc(BUF_SIZE,sizeof(float));
    z_head = calloc(BUF_SIZE,sizeof(float));
}

void writeToBuffer(sensor_data *data){
    x_acc[savebuffer.last_write_pos + 1] = data->accX;
    y_acc[savebuffer.last_write_pos + 1] = data->accY;
    z_acc[savebuffer.last_write_pos + 1] = data->accZ;
    x_head[savebuffer.last_write_pos + 1] = data->rotX;
    y_head[savebuffer.last_write_pos + 1] = data->rotY;
    z_head[savebuffer.last_write_pos + 1] = data->rotZ;
    savebuffer.last_write_pos++;
    if(savebuffer.last_write_pos == BUF_SIZE){
        savebuffer.last_write_pos = -1;
    }
    if (savebuffer.num_valid_rec < BUF_SIZE) {
        savebuffer.num_valid_rec++;
    }
}

/*
 checkIfSS(circular_buffer savebuffer)
 This function checks if the tractor is in a stationary state. A preset number
 of latest records(NUM_OF_SS_RECORDS) are read out of the accelerometer buffer.
 If each accelerometer axis shows a delta smaller than SS_DELTATHRESHOLD , then
 the tractor is assumed to be stationary, and the function returns 1. If any
 accelerometer delta exceeds SS_DELTATHRESHOLD, the function returns 0. The
 algorithm for determining delta is simply maximum - minimum.
 */
int checkIfSS(circular_buffer savebuffer){
    int num_records = savebuffer.num_valid_rec;
    int index = savebuffer.last_write_pos;
	float x_min, x_max, y_min, y_max, z_min, z_max;
    x_min = x_max = x_acc[index];
    y_min = y_max = y_acc[index];
	z_min = z_max = z_acc[index];
	while(num_records > 0){
		if(index == -1){
			index = BUF_SIZE-1;
		}
		if(x_acc[index] > x_max){x_max = x_acc[index];}
		if(x_acc[index] < x_min){x_min = x_acc[index];}
		if(y_acc[index] > y_max){y_max = y_acc[index];}
		if(y_acc[index] < y_min){y_min = y_acc[index];}
		if(z_acc[index] > z_max){z_max = z_acc[index];}
		if(z_acc[index] < z_min){z_min = z_acc[index];}
		index--;
	}
	//test
	printf("x_delta = %lf, y_delta = %lf, z_delta = %lf \n", x_max - x_min, y_max - y_min, z_max - z_min);

	if(((x_max - x_min) < SS_DELTA_THRESHOLD) &&
       ((y_max - y_min) < SS_DELTA_THRESHOLD) &&
       ((z_max - z_min) < SS_DELTA_THRESHOLD)){
		printf("System is at rest\n");
		return 1;
	}
	else{
		printf("System not at rest\n");
		return 0;
	}
}

/*
 getAbsPos(circular_buffer savebuffer)
 This function calculates pitch(y-rotation) and roll(x-rotation) based on the
 static acceleration. These values are used to "reset" the simulator, and erase
 any accumulated drift. The gyro data sent is always relative to the latest
 pitch/roll data, again to prevent drift error.
 Called when the system is determined to be at rest.
 */
abs_pos getAbsPos(){
    int num_records = savebuffer.num_valid_rec;
    int index = savebuffer.last_write_pos;
	float x_accavg = 0, y_accavg = 0;
	abs_pos radians;

	while(num_records > 0){
		if(index == -1){
			index = BUF_SIZE-1;
		}
		x_accavg += x_acc[index];
		y_accavg += y_acc[index];
		index--;
        num_records--;
	}

	x_accavg = (x_accavg/savebuffer.num_valid_rec);
	y_accavg = (y_accavg/savebuffer.num_valid_rec);
	radians.pitch = -asin(x_accavg);
	radians.roll  = asin(y_accavg);
	//Test
	printf("Angle in radians of pitch: %lf, and roll: %lf\n x_accavg: %lf y_accavg: %lf\n",
           radians.pitch, radians.roll, x_accavg, y_accavg);
	return radians;
}

/*
 getGyroBias()
 This function calculates the bias of the gyros.
 Called when the system is determined to be at rest.
 */
bias getGyroBias(){
	int num_records = savebuffer.num_valid_rec;
    int index = savebuffer.last_write_pos;
	bias gyro_bias = {0.0, 0.0, 0.0};


	while( num_records > 0){
		if(index == -1){
			index = BUF_SIZE-1;
		}
		gyro_bias.xAxis+= x_head[index];
		gyro_bias.yAxis+= y_head[index];
		gyro_bias.zAxis+= z_head[index];
		index--;
        num_records--;
	};

	gyro_bias.xAxis = gyro_bias.xAxis/savebuffer.num_valid_rec;
	gyro_bias.yAxis = gyro_bias.yAxis/savebuffer.num_valid_rec;
	gyro_bias.zAxis = gyro_bias.zAxis/savebuffer.num_valid_rec;
	//Test
	printf("X Gyro bias: %lf, Y Gyro bias: %lf, Z Gyro bias: %lf\n",
           gyro_bias.xAxis, gyro_bias.yAxis, gyro_bias.zAxis);
	return gyro_bias;
}

/*
 getAccBias()
 This function calculates the bias of the accelerometers.
 Called when the system is determined to be at rest.
 */
bias getAccBias(){
	int num_records = savebuffer.num_valid_rec;
    int index = savebuffer.last_write_pos;
	bias acc_bias = {0.0, 0.0, 0.0};

	while( num_records > 0){
		if(index == -1){
			index = BUF_SIZE-1;
		}
		acc_bias.xAxis+= x_acc[index];
		acc_bias.yAxis+= y_acc[index];
		acc_bias.zAxis+= z_acc[index];
		index--;
        num_records--;
	};

	acc_bias.xAxis = acc_bias.xAxis/savebuffer.num_valid_rec;
	acc_bias.yAxis = acc_bias.yAxis/savebuffer.num_valid_rec;
	acc_bias.zAxis = acc_bias.zAxis/savebuffer.num_valid_rec;
	//Test
	printf("X Acc bias: %lf, Y Acc bias: %lf, Z Acc bias: %lf\n",
           acc_bias.xAxis, acc_bias.yAxis, acc_bias.zAxis);
	return acc_bias;
}
// To handle interrupt by keypress
void INThandler (){
    running=0; //Stop the infinite loop
}
//Workaround to get timestamping to work with ms.
Time_struct get_time(){
    Time_struct Time_Val;
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv,&tz);
    tm=localtime(&tv.tv_sec);
    Time_Val.year = tm->tm_year+1900;
    Time_Val.month = tm->tm_mon+1;
    Time_Val.day = tm->tm_mday;
    Time_Val.hour = tm->tm_hour;
    Time_Val.minute = tm->tm_min;
    Time_Val.second = tm->tm_sec;
    Time_Val.msecond = tv.tv_usec;
return Time_Val;
}
