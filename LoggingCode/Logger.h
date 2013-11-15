#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

//Function Declarations
char * currentDateTime();
int  logRealTime(int timestamp, int packetID, char *event, int data1, int data2, int data3);
int logToFile(int timestamp, int packetID, char *event, int data1, int data2, int data3);

#endif // LOGGER_H_INCLUDED
