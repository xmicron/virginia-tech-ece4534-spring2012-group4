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
#include "queue.h"
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

#define testNUM 0

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( I2CTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartI2CTask( unsigned portBASE_TYPE uxPriority, i2cParamStruct *params)
{
	if ((params->i2cQ->inQ = xQueueCreate(I2CMsgQLen,sizeof(I2CMsgQueueMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}	

	/* Start the task */
	portBASE_TYPE retval;

	if ((retval = xTaskCreate( I2CTask, ( signed char * ) "i2cThread", i2cSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// This is the actual task that is run
static portTASK_FUNCTION( I2CTask, pvParameters )
{
	portTickType xUpdateRate, xLastUpdateTime;
	const uint8_t ReturnADCValue = 0xAA;
	
	//const uint8_t InsteonReserved;	 //reserved for Insteon if we are only requesting insteon data
	//uint8_t InsteonSendValue[12]; //reserved for Insteon send
	
	uint8_t MidiSendCount = 1;
	uint8_t MidiSendValue[5] = {0xAF, 0x80, 0x64, 0x64, 0x00};
	uint8_t InstSendValue[6] = {0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
	int msgcount = 0;
	
	uint8_t temp1, rxLen, status;
	uint8_t ADCValueReceived[12];
	uint8_t MidiReceived[2];


	// Get the parameters
	i2cParamStruct *param = (i2cParamStruct *) pvParameters;
	
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->i2cDev;
	
	// Get the LCD information pointer
	vtLCDMsgQueue *lcdData = param->lcdQ;
	vtLCDMsg lcdBuffer;

	MasterMsgQueue * masterData = param->masterQ;
	MasterMsgQueueMsg masterBuffer;

	I2CMsgQueue * i2cQ = param->i2cQ;
	I2CMsgQueueMsg i2cBuffer;

	vTaskDelay(10/portTICK_RATE_MS);

	xUpdateRate = i2cREAD_RATE_BASE / portTICK_RATE_MS;

	/* We need to initialise xLastUpdateTime prior to the first call to vTaskDelayUntil(). */
	xLastUpdateTime = xTaskGetTickCount();
	for(;;)
	{
		//delay for some amount of time before looping again
		//vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );
		
		//Send a request to the PIC for ADC values
		if (vtI2CEnQ(devPtr,0x01,0x4F,1,&ReturnADCValue,12) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		//wait for message from I2C
		//bit0 is first portion of ADC value
		//bit1 is last portion of ADC value
		//bit2 is timer value
		//bit3 thru bit7 are garbage (all 0's)
		//bit8 is the size of the Midi buffer on the PIC
		//bit9 is the count
		//bit10 is the ADC channel number
		//bit11 is the OPCode we sent (0xAA)
		
		if (vtI2CDeQ(devPtr,12,&ADCValueReceived[0],&rxLen,&status) != pdTRUE) {
			//VT_HANDLE_FATAL_ERROR(0);
		}
	  
#if testNUM==1 // Valid I2C message from PIC - expected to send result to Main Thread
	ADCValueReceived[0] = 2;
	ADCValueReceived[1] = 0xC2;
	ADCValueReceived[2] = 0;
	ADCValueReceived[8] = 0;
	ADCValueReceived[9] = 0;
	ADCValueReceived[10] = 0;
	ADCValueReceived[11] = 0xAA;
#elif testNUM==2 //Invalid I2C message from PIC (wrong Op-Code)
	ADCValueReceived[0] = 2;
	ADCValueReceived[1] = 0xC2;
	ADCValueReceived[2] = 0;
	ADCValueReceived[8] = 0;
	ADCValueReceived[9] = 0;
	ADCValueReceived[10] = 0;
	ADCValueReceived[11] = 0xAB;  //not the op-code
#elif testNUM==3 // Invalid I2C message from PIC (invalid channel number)
	ADCValueReceived[0] = 2;
	ADCValueReceived[1] = 0xC2;
	ADCValueReceived[2] = 0;
	ADCValueReceived[8] = 0;
	ADCValueReceived[9] = 0;
	ADCValueReceived[10] = 6; //1 channel too high
	ADCValueReceived[11] = 0xAA;
#elif testNUM==4 // Improper I2C message format from PIC (message 0 and message 1 are in the wrong order
	ADCValueReceived[0] = 0xC2;
	ADCValueReceived[1] = 0x02;
	ADCValueReceived[2] = 0;
	ADCValueReceived[8] = 0;
	ADCValueReceived[9] = 0;
	ADCValueReceived[10] = 0;
	ADCValueReceived[11] = 0xAA;
#elif testNUM==5 // Midi buffer on the PIC is full - skip next message send
	ADCValueReceived[0] = 2;
	ADCValueReceived[1] = 0xC2;
	ADCValueReceived[2] = 0;
	ADCValueReceived[8] = 4;
	ADCValueReceived[9] = 0;
	ADCValueReceived[10] = 0;
	ADCValueReceived[11] = 0xAA;
#endif
	 	if (ADCValueReceived[11] != 170) //check the message returned make sure it is 0xAA signifying it's the correct op-code
		{
			//FlipBit(6);
		}
		if (ADCValueReceived[0] > 3)
		{
		 	//FlipBit(1);
		}
		if (ADCValueReceived[10] < 0 || ADCValueReceived[10] > 5)
		{
			//FlipBit(2);
		}

		//if (ADCValueReceived[8] < 4) //for nick's code
		if (ADCValueReceived[8] < 4 && uxQueueMessagesWaiting(i2cQ->inQ) > 0) // for everything else
		{
			if (xQueueReceive(i2cQ->inQ,(void *) &i2cBuffer,portMAX_DELAY) != pdTRUE) //receive message from message queue
			{
				VT_HANDLE_FATAL_ERROR(0);
			}

			if (i2cBuffer.buf[0] == 0x11)
			{
			 	InstSendValue[0] = i2cBuffer.buf[0];
				InstSendValue[1] = i2cBuffer.buf[1];
				InstSendValue[2] = i2cBuffer.buf[2];
				InstSendValue[3] = i2cBuffer.buf[3];
				InstSendValue[4] = i2cBuffer.buf[4];
				InstSendValue[5] = i2cBuffer.buf[5];

				FlipBit(2);
				FlipBit(3);
				FlipBit(4);

				if (vtI2CEnQ(devPtr,0x00,0x4F,6,InstSendValue,0) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
		
				//wait for message from I2C
				if (vtI2CDeQ(devPtr,0,&MidiReceived[0],&rxLen,&status) != pdTRUE) {
					//VT_HANDLE_FATAL_ERROR(0);
				}
			}
			else if (i2cBuffer.buf[0] == 0x4)
			{
				MidiSendValue[1] = i2cBuffer.buf[1];
				MidiSendValue[2] = i2cBuffer.buf[2];
				MidiSendValue[3] = i2cBuffer.buf[3];
				
				//prepare to send Midi message to I2Cto the PIC
				if (MidiSendCount > 100)
					MidiSendCount = 1;
				MidiSendValue[4] = MidiSendCount;
			 	if (vtI2CEnQ(devPtr,0x00,0x4F,5,MidiSendValue,0) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
		
				//wait for message from I2C
				if (vtI2CDeQ(devPtr,0,&MidiReceived[0],&rxLen,&status) != pdTRUE) {
					//VT_HANDLE_FATAL_ERROR(0);
				}
				MidiSendCount++;
	
				FlipBit(1);
			}
		}
		  
		FlipBit(7);	  


		

		if (lcdData != NULL && ADCValueReceived[0] != 0xFF) 
		{
			// Send a message to the LCD task for it to print (and the LCD task must be configured to receive this message)
			/*lcdBuffer.length = 4;
			lcdBuffer.buf[0] = 0x06;
			lcdBuffer.buf[1] = ADCValueReceived[0];
			lcdBuffer.buf[2] = ADCValueReceived[1];
			lcdBuffer.buf[3] = ADCValueReceived[10];

			if (xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}*/
			

			//message sent to the master message queue
			masterBuffer.length = 13;
			masterBuffer.buf[0] = 0x08; //means the message is from I2C	- change to 0x09 for Nick's program
			masterBuffer.buf[1] = ADCValueReceived[0];
			masterBuffer.buf[2] = ADCValueReceived[1];
			masterBuffer.buf[3] = ADCValueReceived[2];
			masterBuffer.buf[4] = ADCValueReceived[3];
			masterBuffer.buf[5] = ADCValueReceived[4];
			masterBuffer.buf[6] = ADCValueReceived[5];
			masterBuffer.buf[7] = ADCValueReceived[6];
			masterBuffer.buf[8] = ADCValueReceived[7];
			masterBuffer.buf[9] = ADCValueReceived[8];
			masterBuffer.buf[10] = ADCValueReceived[9];
			masterBuffer.buf[11] = ADCValueReceived[10];
			masterBuffer.buf[12] = ADCValueReceived[11];
			
			if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			} 
		}  
	}
}

