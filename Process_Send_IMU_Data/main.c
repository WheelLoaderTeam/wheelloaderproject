#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h> 				//Do we need?
#include <math.h> 				//asin
#include "processdata.h"
#include "../OryxSim_PC/sensorData.h"
#include "receiveSensorData.h"

#define SENSOR_FREQ             100     // expected sensor frequency in Hz
#define BUF_SIZE                1024	// size of circular buffer

#define SS_CHECK_FREQ           0.2     // in Hz
#define SS_DELTA_THRESHOLD      0.50
#define SS_NUM                  200     // at 100 hz = 2 seconds of data

#define K                       -10     // position = (acceleration - bias)*K

/*** GLOBAL VARIABLES ***/

circular_buffer savebuffer;
float *x_acc, *y_acc, *z_acc, *x_head, *y_head, *z_head;

// pitch and roll determined when stationary
abs_pos radians;

// current pitch and roll
abs_pos radians_curr;

// bias determined when stationary
bias gyro_bias;
bias acc_bias;
int COM1;
/*** FUNCTION PROTOTYPES ***/

int processData(sensor_data *data);
//sensor_data receiveSensorData(); // declared in receiveSensorData.h, delete upon succsessful compile
void sendSensorData(sensor_data *data, int s_out_sensordata, struct sockaddr_in outsock, int slen);

int checkIfSS();
abs_pos getAbsPos();
bias getGyroBias();
bias getAccBias();

void initBuffer();
void writeToBuffer(sensor_data *data);

/*** FUNCTION CODE ***/
int main(int argc, char *argv[]) {
    COM1 = open_serialport("/dev/ttyUSB0",500000); //Open USB port
    struct sockaddr_in outsock;
    int s_out_sensordata, slen = sizeof(struct sockaddr_in);
//    initClientSocket(IMU_PORT, &s_out_sensordata, OPC_IP, &outsock);
    initClientSocket(6666, &s_out_sensordata, "127.0.0.1", &outsock); //fakeclient
    sensor_data data;
    initBuffer();
    while(1) {
        data = receiveSensorData();
        writeToBuffer(&data);
        if (processData(&data))
            sendSensorData(&data, s_out_sensordata, outsock, slen);
    }
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
            radians = getAbsPos();
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
            radians = getAbsPos();
            gyro_bias = getGyroBias();
            acc_bias = getAccBias();
            ss_flag = true;
        }
    }

    // update current tilt
    if (!ss_flag) {
        radians_curr.roll += data->rotX;
        radians_curr.pitch += data->rotY;
    } else {
        radians_curr = radians;
        ss_flag = false;
    }

    counter++;
    return 1;
}

void sendSensorData(sensor_data *data, int s_out_sensordata, struct sockaddr_in outsock, int slen) {
    printf("HI, we're in sendSensorData\n");
    static int id = 0;
    packet_header header = {id++,6}; // size = 6 OR 6*4 OR 8 OR 8*4 ???
    packet_load load;

  //  header.id;

    load.posX = (data->accX - acc_bias.xAxis) * K;
    load.posY = (data->accY - acc_bias.yAxis) * K;
    load.posZ = (data->accZ - acc_bias.zAxis) * K;

    load.rotX = radians_curr.roll;
    load.rotY = radians_curr.pitch;
    load.rotZ = 0;

    //send a packet over the network
    SensorData send_data;
    send_data.id = header.id;
    send_data.psize = 32;
    send_data.values[0] = load.posX;
    send_data.values[1] = load.posY;
    send_data.values[2] = load.posZ;
    send_data.values[3] = load.rotX;
    send_data.values[4] = load.rotY;
    send_data.values[5] = load.rotZ;
    if (sendto(s_out_sensordata, &send_data, sizeof(SensorData) , 0 , (struct sockaddr *) &outsock, slen)==-1)
    {
        die("sendto(), processdata.c line 139");
    }
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
 checkIfSS(int rcv_buf_pos)
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
	radians.pitch = asin(y_accavg);
	radians.roll  = asin(x_accavg);
	//Test
	printf("Angle in degrees of pitch: %lf, and roll: %lf\n x_accavg: %lf y_accavg: %lf\n",
           (radians.pitch * (180/3.14159)), (radians.roll * (180/3.14159)), x_accavg, y_accavg);
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
 getGyroBias()
 This function calculates the bias of the gyros.
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
