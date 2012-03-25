#ifndef MESSAGEQUEUES_H
#define MESSAGEQUEUES_H

#include "queue.h"
#include "vtI2C.h"
//#include "vtI2C.h"
//#include "lcdTask.h"

typedef struct __MasterMsgQueue {
	xQueueHandle inQ;					   	// Queue used to send messages from other tasks to the I2C task to send
} MasterMsgQueue;

typedef struct __vtLCDMsgQueue {
	xQueueHandle inQ;					   	// Queue used to send messages from other tasks to the LCD task to print
} vtLCDMsgQueue;

typedef struct __I2CMsgQueue {
	xQueueHandle inQ;					   	// Queue used to send messages from other tasks to the I2C task to send
} I2CMsgQueue;



#define MasterMsgQLen 20
#define MasterMLen 12
typedef struct __MasterMsgQueueMsg {
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[MasterMLen]; // On the way in, message to be sent, on the way out, message received (if any)
} MasterMsgQueueMsg;

#define I2CMsgQLen 10
#define I2CMLen 5
typedef struct __I2CMsgQueueMsg {
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[I2CMLen]; // On the way in, message to be sent, on the way out, message received (if any)
} I2CMsgQueueMsg;

#define vtLCDQLen 100
#define vtLCDMLen 20 
typedef struct __vtLCDMsg {
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[vtLCDMLen]; // On the way in, message to be sent, on the way out, message received (if any)
} vtLCDMsg;


// Structure used to pass parameters to the task
typedef struct __MasterParamStruct {
	MasterMsgQueue *masterQ;
	vtLCDMsgQueue *lcdQ;
	I2CMsgQueue *i2cQ;
} MasterParamStruct;

// Structure used to pass parameters to the task
typedef struct __i2cParamStruct {
	vtI2CStruct *i2cDev;
	vtLCDMsgQueue *lcdQ;//soon to go
	I2CMsgQueue *i2cQ;
	MasterMsgQueue *masterQ;
} i2cParamStruct;
typedef struct __lcdParamStruct {
 	vtLCDMsgQueue *lcdQ;
	MasterMsgQueue * masterQ;
} lcdParamStruct;

#endif