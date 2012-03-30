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

#define joystickSTACK_SIZE		4*configMINIMAL_STACK_SIZE

//#define joyDebounce	( ( portTickType ) 1 )

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( JoystickTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartJoystickTask( unsigned portBASE_TYPE uxPriority, i2cParamStruct *params)
{
	/* Start the task */
	portBASE_TYPE retval;

	if ((retval = xTaskCreate( JoystickTask, ( signed char * ) "joystickTask", joystickSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// This is the actual task that is run
static portTASK_FUNCTION( JoystickTask, pvParameters )
{
	uint8_t PIN_CONFIG;
	uint8_t PrevPIN_CONFIG;

	vtLCDMsgQueue *lcdQ = pvParameters;
	vtLCDMsg lcdBuffer;

	uint8_t i = 0;

	for(;;)
	{
		PIN_CONFIG = LPC_GPIO1->FIOPIN >>20;
		//test = test * -1;
		 
		if ((PIN_CONFIG & 0x1) == 0 && /*makes sure only sends one message per button press -->*/(PIN_CONFIG != PrevPIN_CONFIG))	 //select BIT
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 0;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			uint8_t ulCurrentState = GPIO2->FIOPIN;
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x8) == 0 && (PIN_CONFIG != PrevPIN_CONFIG))	  //up joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 1;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x10) == 0 && (PIN_CONFIG != PrevPIN_CONFIG))  //right joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 2;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x20) == 0 && (PIN_CONFIG != PrevPIN_CONFIG))  //down joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 3;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x40) == 0 && (PIN_CONFIG != PrevPIN_CONFIG)) //left joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 4;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		PrevPIN_CONFIG = PIN_CONFIG;
		//vTaskDelay(100);
	}
}

