#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>


/**
*\def IMU_PORT Port number for IMU Com.
*/
#define IMU_PORT 65100


/**
*\def MOV_PORT Port number for sending the force feedback
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

/** \brief PROTOTYPES
 */
 int initializeSockets();




#endif // SOCKET_H_INCLUDED
