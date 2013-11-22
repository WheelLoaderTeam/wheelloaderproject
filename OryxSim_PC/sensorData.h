#ifndef SENSORDATA_H_INCLUDED
#define SENSORDATA_H_INCLUDED
#include "../net/socket.h"

typedef struct SensorData {
    uint32_t id;
    uint32_t psize;
    float values[LEN_BUF_SENSOR-2];
}SensorData;

#endif // SENSORDATA_H_INCLUDED
