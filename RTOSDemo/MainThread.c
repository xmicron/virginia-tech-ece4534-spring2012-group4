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
#include "MainThread.h"



// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the LCD operations
#if PRINTF_VERSION==1
#define mainthreadSTACK_SIZE		8*configMINIMAL_STACK_SIZE
#else
#define mainthreadSTACK_SIZE		4*configMINIMAL_STACK_SIZE
#endif

// Set the task up to run every second
//#define i2cREAD_RATE_BASE	( ( portTickType ) 1000 )

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( MainThread, pvParameters );

/*-----------------------------------------------------------*/

void vStartMainThread( unsigned portBASE_TYPE uxPriority, MasterParamStruct *params)
{
	if ((params->masterQ->inQ = xQueueCreate(MasterMsgQLen,sizeof(MasterMsgQueueMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}	

	/* Start the task */
	portBASE_TYPE retval;

	if ((retval = xTaskCreate( MainThread, ( signed char * ) "MainThread", mainthreadSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// This is the actual task that is run
static portTASK_FUNCTION( MainThread, pvParameters )
{
	MasterParamStruct *temp = pvParameters;
	MasterMsgQueue *masterQ = temp->masterQ;
	MasterMsgQueueMsg masterBuffer;

	vtLCDMsgQueue *lcdQ = temp->lcdQ;
	vtLCDMsg lcdmsgBuffer;

	for(;;)
	{
		if (xQueueReceive(masterQ->inQ,(void *) &masterBuffer,portMAX_DELAY) != pdTRUE) //receive message from message queue
		{
			VT_HANDLE_FATAL_ERROR(0);
		}
	   	uint8_t ulCurrentState = GPIO2->FIOPIN;
		if( ulCurrentState & 0x08 )
		{
			GPIO2->FIOCLR = 0x08;
		}
		else
		{
			GPIO2->FIOSET = 0x08;
		}
		lcdmsgBuffer.buf[0] = masterBuffer.buf[0];
		if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
			VT_HANDLE_FATAL_ERROR(0);
		}
	}
}

