#ifndef SENSORDATA_H_INCLUDED
#define SENSORDATA_H_INCLUDED
#include "../net/socket.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>


//variables that calibrate the smooth motion
//CHANGE THOSE CAREFULLY NEVER MORE 0.01 ON THE ORYX SIMULATOR
#define MAX_TRANS_MOV 0.0005
#define MAX_ROT_MOV 0.001

//struct used to handle the data from the sensors (IMU)
typedef struct SensorData {
    uint32_t id;
    uint32_t psize;
    float values[LEN_BUF_SENSOR-2];
}SensorData;

typedef struct SensorDataTime {
    uint32_t id;
    uint32_t psize;
    float values[LEN_BUF_SENSOR-2];
    struct timespec timestamp;
}SensorDataTime;

// two simple
float verifMaxMovement(float p, float n, float maxi);
SensorData smoothMotion(SensorData previous, SensorData next );

#endif // SENSORDATA_H_INCLUDED
