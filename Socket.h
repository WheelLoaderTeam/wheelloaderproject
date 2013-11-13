#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h> //exit(0);
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>


/**
*\def IMU_PORT Port number for sending the movement packets to OryxPC
*/
#define IMU_PORT 65100


/**
*\def MOV_PORT Port number for sending the movements packets to Simulqtor
*/
#define MOV_PORT 65101

/**
*\def EBU_PORT Port number receiving from EBU Com.
*/
#define EBU_PORT 65200

/**
*\def EBUOUT_PORT Port number sending EBUs data
*/
#define EBUOUT_PORT 65201


/**
*\def VIDEOIN_PORT Port number receiving video
*/
#define VIDEOIN_PORT 65300

/**
*\def VIDEOOUT_PORT Port number sending video
*/
#define VIDEOOUT_PORT 65301

/**
*\def ĈMDI_PORT Port number receiving commands from Oryx Simulator
*/
#define CMDI_PORT 65400

/**
*\def ĈMDO_PORT Port number sending commands to Wheel Loader
*/
#define CMDO_PORT 65401

/**
*\def WL_IP Ip address of wheel loader
*/

#define WL_IP "10.0.0.1"

/**
*\def OPC_IP Ip address of the PC next to the Oryx Simulator
*/

#define OPC_IP "10.0.0.4"

/**
*\def EBU1_IP Ip address of EBU1
*/

#define EBU1_IP "10.0.0.2"

/**
*\def EBU2_IP Ip address of EBU2
*/

#define EBU2_IP "10.0.0.3"

/**
*\def SIM_IP Ip address of the Oryx simulator
*/

#define SIM_IP "10.0.0.5" //TODO change with the good IP

/** \brief PROTOTYPES
 */
 void die(char *s);
int initSocket(int inport,int outport, char* outipaddress, struct sockaddr_in insock, struct sockaddr_in outsock);




#endif // SOCKET_H_INCLUDED