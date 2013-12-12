#ifndef __RECEIVESENSORDATA_H_
#define __RECEIVESENSORDATA_H_
///***Data Types***///
// static acceleration in meters per second^2 and rotation in radians
typedef struct{
    float accX;
    float accY;
    float accZ;
    float rotX;
    float rotY;
    float rotZ;
}sensor_data;

///***Function Prototypes***///
sensor_data receiveSensorData();
int open_serialport(const char *device, int rate);


#endif
