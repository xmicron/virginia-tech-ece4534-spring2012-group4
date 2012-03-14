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
#define i2cREAD_RATE_BASE	( ( portTickType ) 100 )

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

	if ((retval = xTaskCreate( I2CTask, ( signed char * ) "i2cTemp", i2cSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// This is the actual task that is run
static portTASK_FUNCTION( I2CTask, pvParameters )
{
	portTickType xUpdateRate, xLastUpdateTime;
	const uint8_t ReturnADCValue = 0xAA;
	
	uint8_t SendCount = 1;
	uint8_t SendValue[5] = {0xAF, 0x80, 0x64, 0x64, 0x00};
	const uint8_t Gimmesomething = 0xBB;
	
	uint8_t temp1, rxLen, status;
	uint8_t ADCValueReceived[12];
	uint8_t SecondaryReceived[2];


	// Get the parameters
	i2cParamStruct *param = (i2cParamStruct *) pvParameters;
	
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->i2cDev;
	
	// Get the LCD information pointer
	vtLCDMsgQueue *lcdData = param->lcdQ;
	vtLCDMsg lcdBuffer;

	vTaskDelay(10/portTICK_RATE_MS);

	xUpdateRate = i2cREAD_RATE_BASE / portTICK_RATE_MS;

	/* We need to initialise xLastUpdateTime prior to the first call to vTaskDelayUntil(). */
	xLastUpdateTime = xTaskGetTickCount();

	int i = 0;
	for(;;)
	{
		//delay for some amount of time before looping again
		vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );
		
		//vTaskDelay(10);
		//Send a request to the PIC for ADC values
		if (vtI2CEnQ(devPtr,0x01,0x4F,1,&ReturnADCValue,12) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		//wait for message from I2C
		//bit0 is first portion of ADC value
		//bit1 is last portion of ADC value
		//bit2 is timer value
		//bit3 thru bit7 are garbage (all 0's)
		//bit8 is the size of the Midi buffer on the PIc
		//bit9 is the count
		//bit10 is the ADC channel number
		//bit11 is the OPCode we sent (0xAA)
		
		if (vtI2CDeQ(devPtr,12,&ADCValueReceived[0],&rxLen,&status) != pdTRUE) {
			//VT_HANDLE_FATAL_ERROR(0);
		} 
	 	if (ADCValueReceived[11] != 170) //check the message returned
		{
			uint8_t ulCurrentState = GPIO2->FIOPIN;
			if( ulCurrentState & 0x10 )
			{
				GPIO2->FIOCLR = 0x10;
			}
			else
			{
				GPIO2->FIOSET = 0x10;
			}
		}
		int calculate;
		calculate = ADCValueReceived[0] & 0x03;
		calculate = calculate << 8;
		calculate = calculate | ADCValueReceived[1];

		if (calculate > 192 && calculate < 232)
			SendValue[1] = 0x01;
		else if (calculate >= 232 && calculate < 272)
			SendValue[1] = 0x02;
		else if (calculate >= 272 && calculate < 312)
			SendValue[1] = 0x04;
		else if (calculate >= 312 && calculate < 342)
			SendValue[1] = 0x08;
		else if (calculate >= 342 && calculate < 372)
			SendValue[1] = 0x10;
		else if (calculate >= 372 && calculate < 412)
			SendValue[1] = 0x20;
		else if (calculate >= 412 && calculate < 442)
			SendValue[1] = 0x40;
		else if (calculate >= 442 && calculate < 472)
			SendValue[1] = 0x80;
		else SendValue[1] = 0xFF;
		
		//prepare to send Midi message to I2Cto the PIC
		if (SendCount > 100)
			SendCount = 1;
		SendValue[4] = SendCount;
	   	//vTaskDelayUntil( &xLastUpdateTime, xUpdateRate ); 
		if (vtI2CEnQ(devPtr,0x00,0x4F,5,SendValue,0) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		//wait for message from I2C
		if (vtI2CDeQ(devPtr,0,&SecondaryReceived[0],&rxLen,&status) != pdTRUE) {
			//VT_HANDLE_FATAL_ERROR(0);
		}
		SendCount++;  

		
		
		
		
		uint8_t ulCurrentState = GPIO2->FIOPIN;
		if( ulCurrentState & 0x40 )
		{
			GPIO2->FIOCLR = 0x40;
		}
		else
		{
			GPIO2->FIOSET = 0x40;
		}
		

		/*if (lcdData != NULL && ADCValueReceived[0] == 0xFF) 
		{
			// Send a message to the LCD task for it to print (and the LCD task must be configured to receive this message)
			lcdBuffer.length = strlen((char*)(lcdBuffer.buf))+1;
			i++;
			if (i>20)
			{
				uint8_t ulCurrentState = GPIO2->FIOPIN;
				if( ulCurrentState & 0x08 )
				{
					GPIO2->FIOCLR = 0x08;
				}
				else
				{
					GPIO2->FIOSET = 0x08;
				}
				i = 0;
			}
			if (xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			} 
		}*/
	}
}

