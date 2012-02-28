#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "semphr.h"

/* include files. */
#include "vtUtilities.h"
#include "vtI2C.h"
#include "LCDtask.h"
#include "i2cTemp.h"



// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the LCD operations
#if PRINTF_VERSION==1
#define i2cSTACK_SIZE		8*configMINIMAL_STACK_SIZE
#else
#define i2cSTACK_SIZE		4*configMINIMAL_STACK_SIZE
#endif

// Set the task up to run every second
#define i2cREAD_RATE_BASE	( ( portTickType ) 1 )

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( vi2cTempUpdateTask, pvParameters );

/*-----------------------------------------------------------*/

void vStarti2cTempTask( unsigned portBASE_TYPE uxPriority, i2cParamStruct *params)
{
	/* Start the task */
	portBASE_TYPE retval;

	if ((retval = xTaskCreate( vi2cTempUpdateTask, ( signed char * ) "i2cTemp", i2cSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// This is the actual task that is run
static portTASK_FUNCTION( vi2cTempUpdateTask, pvParameters )
{
	portTickType xUpdateRate, xLastUpdateTime;
	const uint8_t i2cCmdInit[]= {0xAC,0x00};
	const uint8_t i2cCmdStartConvert[]= {0xEE};
	const uint8_t i2cCmdStopConvert[]= {0x22};
	const uint8_t i2cCmdReadVals[]= {0xAA};
	const uint8_t i2cCmdReadCnt[]= {0xA8};
	const uint8_t i2cCmdReadSlope[]= {0xA9};
	float temperature;
	float countPerC, countRemain;
	uint8_t messageReceived[2];
	uint8_t temp1, rxLen, status;
	// Get the parameters
	i2cParamStruct *param = (i2cParamStruct *) pvParameters;
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->i2cDev;
	// Get the LCD information pointer
	vtLCDMsgQueue *lcdData = param->lcdQ;
	vtLCDMsg lcdBuffer;

	// Assumes that the I2C device (and thread) have already been initialized
	// 0x4F address of the temperature sensor (DS1621)
	// Should do something more comprehensive with the msgType for debugging...

	// Temperature sensor configuration sequence (DS1621)
	if (vtI2CEnQ(devPtr,0x01,0x4F,sizeof(i2cCmdInit),i2cCmdInit,0) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	// wait on the result
	if (vtI2CDeQ(devPtr,0,NULL,&rxLen,&status) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	// Must wait 10ms after writing to the temperature sensor's configuration registers(per sensor data sheet)
	vTaskDelay(10/portTICK_RATE_MS);
	// Tell it to start converting
	if (vtI2CEnQ(devPtr,0x02,0x4F,sizeof(i2cCmdStartConvert),i2cCmdStartConvert,0) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	// wait on the result
	if (vtI2CDeQ(devPtr,0,NULL,&rxLen,&status) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}

	// Scale the update rate to ensure it really is in ms
	xUpdateRate = i2cREAD_RATE_BASE / portTICK_RATE_MS;

	/* We need to initialise xLastUpdateTime prior to the first call to vTaskDelayUntil(). */
	xLastUpdateTime = xTaskGetTickCount();
	
	// Like all good tasks, this should never exit
	for(;;)
	{
		/* Ask the RTOS to delay reschduling this task for the specified time */
		vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );
		
		// Read in the values from the temperature sensor
		// We have three transactions on i2c to read the full temperature 
		//   we send all three requests to the I2C thread (via a Queue) and *then* we wait on all three responses
		// Temperature read -- use a convenient routine defined above
		
		
		if (vtI2CEnQ(devPtr,0x01,0x4F,sizeof(i2cCmdReadVals),i2cCmdReadVals,2) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}


		// Read in the read counter
		if (vtI2CEnQ(devPtr,0x04,0x4F,sizeof(i2cCmdReadCnt),i2cCmdReadCnt,1) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		// Read in the slope;
		if (vtI2CEnQ(devPtr,0x05,0x4F,sizeof(i2cCmdReadSlope),i2cCmdReadSlope,1) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		// wait on the results for the first read (ignore the second byte because we are doing a special computation -- see the sensor datasheet)
		
		
		if (vtI2CDeQ(devPtr,2,&messageReceived[0],&rxLen,&status) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		

		temperature = messageReceived[0];//<<8;


		//temperature = temperature + messageReceived[1];
		//temperature = temp1;
			
		vtITMu8(vtITMPortTempVals,rxLen); // Log the length received
		// wait on the results of the second read
		if (vtI2CDeQ(devPtr,1,&temp1,&rxLen,&status) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		countRemain = temp1;
		vtITMu8(vtITMPortTempVals,rxLen); // Log the length received;
		// wait on the results of the third read
		if (vtI2CDeQ(devPtr,1,&temp1,&rxLen,&status) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		countPerC = temp1;	
		vtITMu8(vtITMPortTempVals,rxLen); // Log the length received;
		// end of i2c temperature read

		// Do the accurate temperature calculation
		temperature += -0.25 + ((countPerC-countRemain)/countPerC);
		
		//#if PRINTF_VERSION == 1
		printf("Temp %f F (%f C)\n",(32.0 + ((9.0/5.0)*temperature)), (temperature));
		
		/*My Changes*/
		lcdBuffer.buf[0] = messageReceived[0];
		lcdBuffer.buf[1] = messageReceived[1];
		//sprintf((char*)(lcdBuffer.buf),"%i",messageReceived[0]);
	   	


		/*End My Changes*/
		//#else
		// we do not have full printf (so no %f) and therefore need to print out integers
		printf("Temp %d F (%d C)\n",lrint(32.0 + ((9.0/5.0)*temperature)), lrint(temperature));
		sprintf((char*)(lcdBuffer.buf),"(%d C)",lrint(temperature));
		//#endif
			 
		if (lcdData != NULL) {
			// Send a message to the LCD task for it to print (and the LCD task must be configured to receive this message)
			lcdBuffer.length = strlen((char*)(lcdBuffer.buf))+1;
			

			if (xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
		}
	}
}

