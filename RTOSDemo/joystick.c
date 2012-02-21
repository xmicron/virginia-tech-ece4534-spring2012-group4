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

#define JOYSTICK_MODE 0

// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the LCD operations

#define joystickSTACK_SIZE		4*configMINIMAL_STACK_SIZE

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( JoystickTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartJoystickTask( unsigned portBASE_TYPE uxPriority, i2cTempStruct *params)
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
	uint8_t test;
	for(;;)
	{
#if JOYSTICK_MODE==0
		test = LPC_GPIO1->FIOPIN >>20;
		//test = test * -1;
		 
		if ((test & 0x1) == 0)	 //select BIT
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
		else if ((test & 0x8) == 0)	  //up joystick
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
		else if ((test & 0x10) == 0)  //right joystick
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
		else if ((test & 0x20) == 0)  //down joystick
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
		else if ((test & 0x40) == 0) //left joystick
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
#elif JOYSTICK_MODE==1
	//shawn's code goes here
#endif
	}
}

