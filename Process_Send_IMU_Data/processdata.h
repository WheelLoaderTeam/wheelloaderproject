#ifndef __PROCESSDATA_H_
#define __PROCESSDATA_H_

// used for managemnet of a circular buffer
typedef struct{
    int last_write_pos;
    int num_valid_rec;
}circular_buffer;

// gyro & acceleration bias
typedef struct{
	float xAxis;
	float yAxis;
	float zAxis;
}bias;

// pitch and roll in radians
typedef struct{
	float pitch;
    float roll;
}abs_pos;

typedef struct{
    uint32_t id;
    uint32_t size;
}packet_header;

// position in meters and rotation in radians
typedef struct{
    float posX;
    float posY;
    float posZ;
    float rotX;
    float rotY;
    float rotZ;
}packet_load;



#endif