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


/*int main(void) {

	volatile static int i = 0 ;
	int adval;
	
	// Set up board/processor
	TargetResetInit();

	// Turn on power to ADC block 
	PCONP |=  ADC_POWERON;

	// Turn on ADC peripheral clock
	PCLKSEL0 = PCLKSEL0 & ~(PCLK_ADC_MASK);
	PCLKSEL0 |=  (3 << PCLK_ADC);
		
	// Set P0.23 to AD0.0 in PINSEL1
	PINSEL1	|= SELECT_ADC0; 
	
	// Enter an infinite loop, just checking ADC pot and incrementing a counter
	while(1) {

		// Start A/D conversion for on AD0.0
		AD0CR = START_ADC | OPERATIONAL_ADC | SEL_AD0 ;

		do {
			adval = AD0GDR;                      // Read A/D Data Register
		} while ((adval & ADC_DONE_BIT) == 0);   // Wait for end of A/D Conversion
		
		AD0CR &= ~(START_ADC | OPERATIONAL_ADC | SEL_AD0 );   // Stop A/D Conversion

		 // Extract AD0.0 value - 12 bit result in bits [15:4]
		adval = (adval >> 4) & 0x0FFF ;            

		printf ("%d - Pot val = %d\n", i, adval);
		main_delay(5000);	//Short delay before doing another ADC read
		i++ ;
	}
	return 0 ;
}*/





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

