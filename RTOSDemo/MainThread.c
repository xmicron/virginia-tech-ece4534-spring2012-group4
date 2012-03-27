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
#include "timer.h"



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

	I2CMsgQueue *i2cQ = temp->i2cQ;
	I2CMsgQueueMsg i2cBuffer;

	InstrumentStruct Inst[2];
	RepeatingInstrumentStruct RInst[3];
	int i = 0;

	//timer R1timer;
	//timer R2timer;
	//timer R3timer;
	for (i = 0; i < 3; i++)
	{
		Inst[i].InstrumentID = 0;
		RInst[i].InstrumentID = 0;
		RInst[i].Note = 0;
		RInst[i].BPM = 0;
	}
	for (i = 0; i < 3; i++)
	{
		Inst[i].InstrumentID = 0;
	}

	for(;;)
	{
		if (xQueueReceive(masterQ->inQ,(void *) &masterBuffer,portMAX_DELAY) != pdTRUE) //receive message from message queue
		{
			VT_HANDLE_FATAL_ERROR(0);
		}
		//FlipBit(5);
		
		if (masterBuffer.buf[0] == 0x08) //message from I2C
		{
			/*int calculate;
			calculate = masterBuffer.buf[0] & 0x03;
			calculate = calculate << 8;
			calculate |= masterBuffer.buf[1];

			i2cBuffer.length = 3;

		 	if (calculate >= 180  && calculate < 220)
				i2cBuffer.buf[1] = 0x80;
			else if (calculate >= 220  && calculate < 260)
				i2cBuffer.buf[1] = 0x40;
			else if (calculate >= 260  && calculate < 290)
				i2cBuffer.buf[1] = 0x20;
			else if (calculate >= 290  && calculate < 320)
				i2cBuffer.buf[1] = 0x10;
			else if (calculate >= 320 && calculate < 380)
				i2cBuffer.buf[1] = 0x08;
			else if (calculate >= 380 && calculate < 460)
				i2cBuffer.buf[1] = 0x04;
			else if (calculate >= 460 && calculate < 530)
				i2cBuffer.buf[1] = 0x02;
			else if (calculate >= 530 && calculate < 780)
				i2cBuffer.buf[1] = 0x01;
			
			if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}*/
			FlipBit(3);
			
		}
		else if (masterBuffer.buf[0] == 0x09) //temporary for nick
		{
		  	lcdmsgBuffer.buf[0] = 0x06;
			lcdmsgBuffer.buf[1] = masterBuffer.buf[1];
			lcdmsgBuffer.buf[2] = masterBuffer.buf[2];
			lcdmsgBuffer.buf[3] = masterBuffer.buf[4];

			if (xQueueSend(lcdQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}
		}
		else if (masterBuffer.buf[0] == 0x0A || masterBuffer.buf[0] == 0x0B || masterBuffer.buf[0] == 0x0C) //message from LCD thread - Instrument Change
		{
			FlipBit(4);

		 	/*Inst[masterBuffer.buf[1]].InstrumentID = masterBuffer.buf[2];

			//uint8_t MidiSendMsg[3];
			i2cBuffer.length = 3;
			i2cBuffer.buf[0] = 0xC0 + masterBuffer.buf[1];
			i2cBuffer.buf[1] = Inst[masterBuffer.buf[1]].InstrumentID;
			i2cBuffer.buf[2] = 0x00;

			if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}*/
		}	
	}
}

