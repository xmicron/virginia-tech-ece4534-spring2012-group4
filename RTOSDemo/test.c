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

void vStartI2CTask( unsigned portBASE_TYPE uxPriority, i2cTempStruct *params)
{
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
	const uint8_t SendValue[4] = {0xAF, 0xFF, 0x00, 0x0A};
	const uint8_t Gimmesomething = 0xBB;
	
	uint8_t temp1, rxLen, status;
	uint8_t ADCValueReceived[8];
	uint8_t SecondaryReceived[2];


	// Get the parameters
	i2cTempStruct *param = (i2cTempStruct *) pvParameters;
	
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->dev;
	
	// Get the LCD information pointer
	vtLCDStruct *lcdData = param->lcdData;
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

		//int i = 0;

		//toggle the 2.4 LED
		
		//vTaskDelay(10);
		//Ask for message from I2C
		if (vtI2CEnQ(devPtr,0x01,0x4F,1,&ReturnADCValue,8) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		//wait for message from I2C
		if (vtI2CDeQ(devPtr,8,&ADCValueReceived[0],&rxLen,&status) != pdTRUE) {
			//VT_HANDLE_FATAL_ERROR(0);
		} 

	   
	   	//vTaskDelayUntil( &xLastUpdateTime, xUpdateRate ); 
		/*if (vtI2CEnQ(devPtr,0x00,0x4F,4,SendValue,0) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		//wait for message from I2C
		if (vtI2CDeQ(devPtr,0,&SecondaryReceived[0],&rxLen,&status) != pdTRUE) {
			//VT_HANDLE_FATAL_ERROR(0);
		}*/  

		//uint8_t testint = 0;
		
		//load the read message from I2C into the lcd Buffer
		//lcdBuffer.buf[0] = ADCValueReceived[0];
		//lcdBuffer.buf[1] = ADCValueReceived[1];
		  
		/*if (ADCValueReceived[6] == 0x01)  //channel 1 2.5 LED
		{
			uint8_t ulCurrentState = GPIO2->FIOPIN;
			if( ulCurrentState & 0x20 )
			{
				GPIO2->FIOCLR = 0x20;
			}
			else
			{
				GPIO2->FIOSET = 0x20;
			}
		}*/
		//if (ADCValueReceived[6] == 0x00)  //channel 0 bottom LED
		//{
			uint8_t ulCurrentState = GPIO2->FIOPIN;
			if( ulCurrentState & 0x40 )
			{
				GPIO2->FIOCLR = 0x40;
			}
			else
			{
				GPIO2->FIOSET = 0x40;
			}
		//}
		/*if (ADCValueReceived[3] == 0xFF)  //Junk 2.4 LED
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
		//if (lcdData != NULL && ADCValueReceived[0] == 0xFF) 
		//{
			// Send a message to the LCD task for it to print (and the LCD task must be configured to receive this message)
			//lcdBuffer.length = strlen((char*)(lcdBuffer.buf))+1;
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
			/*if (xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			} */
		//}*/
	}
}

