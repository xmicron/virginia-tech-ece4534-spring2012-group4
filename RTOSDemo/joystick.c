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

#define test 0

//hello world



#define ADC_POWERON (1 << 12) 

#define PCLK_ADC 24
#define PCLK_ADC_MASK (3 << 24)

// AD0.0 - P0.23, PINSEL1 [15:14] = 01
#define SELECT_ADC0 (0x1<<14)

// ADOCR constants
#define START_ADC (1<<24)
#define OPERATIONAL_ADC (1 << 21)
#define SEL_AD0 (1 <<0)

#define ADC_DONE_BIT	(1 << 31)

// Function to provide short delay
void main_delay (int n) __attribute__((noinline));
void main_delay(int n)
{
   volatile int d;
   for (d=0; d<n*3000; d++){}
}

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
	int count = 0;
	printf("Start of Joystick thread. Begin for loop.\n");

	vTaskDelay(5000);

	for(;;)
	{
		PIN_CONFIG = LPC_GPIO1->FIOPIN >>20;
		//test = test * -1;
#if test==1
	//Volume change test
	printf("Volume change test\n");
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 2;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);

	vTaskDelay(10000);

	printf("Brightness change test\n");
	//Brightness Change test
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 2;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 4;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);

	vTaskDelay(10000);

	//Player 1 Instrument change test
	printf("Player 1 instrument change test\n");
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 2;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);	
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);

	vTaskDelay(10000);

	//Repeating Instrument Note Change
	printf("Repeating Instrument Note change test\n");
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);

	vTaskDelay(10000);

	//Repeating Instrument BPM change dependent upon above test
	printf("Repeating Instrument BPM change test\n");
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);

	vTaskDelay(10000);

	//Final test to demonstrate working Repeating Instrument generation
	printf("Repeating Instrument instrument change test\n");
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 3;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 1;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	lcdBuffer.length = 2;
	lcdBuffer.buf[0] = 0x11;
	lcdBuffer.buf[1] = 0;
	if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
		VT_HANDLE_FATAL_ERROR(0);
	}
	vTaskDelay(50);
	
#else		 
		if ((PIN_CONFIG & 0x1) == 0 && (PIN_CONFIG != PrevPIN_CONFIG))	 //select BIT
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
		else if ((PIN_CONFIG & 0x8) == 0 && ((PIN_CONFIG != PrevPIN_CONFIG) || count > 20))	  //up joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 1;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x10) == 0 && ((PIN_CONFIG != PrevPIN_CONFIG) || count > 20))  //right joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 2;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x20) == 0 && ((PIN_CONFIG != PrevPIN_CONFIG) || count > 20))  //down joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 3;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		else if ((PIN_CONFIG & 0x40) == 0 && ((PIN_CONFIG != PrevPIN_CONFIG) || count > 20)) //left joystick
		{
			lcdBuffer.length = 2;
			lcdBuffer.buf[0] = 0x11;
			lcdBuffer.buf[1] = 4;
			if (xQueueSend(lcdQ->inQ,(void *) (&lcdBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
			//FlipBit(5);
		}
		//count is configured to repeatedly send button messages if the user holds it down for a long amount of time
		//currently configured to around 600ms
		if (PIN_CONFIG == PrevPIN_CONFIG && count < 50)
			count ++;
		else if (PIN_CONFIG != PrevPIN_CONFIG)
			count = 0;
			
		PrevPIN_CONFIG = PIN_CONFIG;
		vTaskDelay(30);		//delay 30ms
#endif
	}
}

