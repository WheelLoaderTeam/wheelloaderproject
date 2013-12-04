#ifndef __RECEIVESENSORDATA_H_
#define __RECEIVESENSORDATA_H_

// static acceleration in meters per second^2 and rotation in radians
typedef struct{
    float accX;
    float accY;
    float accZ;
    float rotX;
    float rotY;
    float rotZ;
}sensor_data;


#endif