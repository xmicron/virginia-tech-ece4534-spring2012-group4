#ifndef I2CTEMP_TASK_H
#define I2CTEMP_TASK_H
#include "vtI2C.h"
#include "lcdTask.h"
typedef struct __I2CMsgQueue {
	xQueueHandle inQ;					   	// Queue used to send messages from other tasks to the I2C task to send
} I2CMsgQueue;

#define I2CMsgQLen 10
// Structure used to define the messages that are sent to the LCD thread
//   the maximum length of a message to be printed is the size of the "buf" field below
#define I2CMLen 5
typedef struct __I2CMsgQueueMsg {
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[I2CMLen]; // On the way in, message to be sent, on the way out, message received (if any)
} I2CMsgQueueMsg;

// Structure used to pass parameters to the task
typedef struct __i2cTempStruct {
	vtI2CStruct *dev;
	vtLCDStruct *lcdData;
	I2CMsgQueue *msgQ;
} i2cTempStruct;



void vStarti2cTempTask( unsigned portBASE_TYPE uxPriority, i2cTempStruct *);

#endif