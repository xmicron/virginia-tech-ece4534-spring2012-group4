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
	int i = 0, prevRange = 0, curRange = 0, count = -1;

	//timer R1timer;
	//timer R2timer;
	//timer R3timer;
	for (i = 0; i < 3; i++)
	{
		RInst[i].InstrumentID = 0;
		RInst[i].Note = 0;
		RInst[i].BPM = 0;
	}
	for (i = 0; i < 2; i++)
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
			//masterBuffer.buf[1]	 //most significant ADC value
			//masterBuffer.buf[2]	 //least significant ADC value
			//masterBuffer.buf[3]	 //timer value
			//masterBuffer.buf[4]	 //channel number

			/*
			channel 1: ADC note
			channel 2: ADC velocity
			channel 3: ADC pitch
			channel 4: ADC note #2
			channel 5: ADC velocity #2
			channel 6: ADC pitch #2
			*/

			int ADCValue = masterBuffer.buf[1] << 8;
			ADCValue |= masterBuffer.buf[2];

			if (masterBuffer.buf[4] == 0)  // Instrument 1 Note
			{
				if (ADCValue >= 200  && ADCValue < 250) // C4
				{	
					Inst[0].Note = 1; 	
				}
				else if (ADCValue >= 260 && ADCValue < 300) // D4
				{
					Inst[0].Note = 2;
				}
				else if (ADCValue >= 310 && ADCValue < 345) // E4 
				{
					Inst[0].Note = 3;
				}
				else if (ADCValue >= 360 && ADCValue < 390)	// F4
				{
					Inst[0].Note = 4;
				}
				else if (ADCValue >= 410 && ADCValue < 455)	// G4
				{
					Inst[0].Note = 5;
				}
				else if (ADCValue >= 470 && ADCValue < 540)	// A4
				{
					Inst[0].Note = 6;
				}
				else if (ADCValue >= 550 && ADCValue < 645)	// B4
				{
					Inst[0].Note = 7;
				}
				else if (ADCValue >= 660 && ADCValue < 790)	// C5
				{
					Inst[0].Note = 8;
				}
				else if (ADCValue < 180)
				{
					Inst[0].Note = 0;
				}

				FlipBit(6);
				

				i2cBuffer.length = 1;
				i2cBuffer.buf[0] = 1 << (Inst[masterBuffer.buf[4]].Note - 1);
	
				/*if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}*/
			}
			else if (masterBuffer.buf[4] == 1)	//Instrument 1 Velocity
			{
				// NOTE: If a hand is not in the range of a sensor, play nothing (do not send a MIDI msg)
				// NOTE: This is a state machine which determines if a sensor beam has been struck, and subsequently,
				// if a note should be played.  Additionally, it determines the velocity that the note should be played with

				// Check to see if a hand is in the range of a sensor.
				if (ADCValue > 200) curRange = 1;
				else curRange = 0;

				if (count >= 0) count--;

				// STATE 1: if the beam has just been struck
				if (prevRange == 0 && curRange == 1)
				{
					// Set the COUNT variable to designate the number of messages which should be ignored 
					// before reding the velocity sensor.  In this manner, you can ignore the first (few) invalid
					// sensor reads as a hand crosses its beam.
				 	count = 1; 		
					prevRange = 1;	
				}
				// STATE 2: if a hand has been removed from a beam, reset to look for a new break	B0 7B 00
				else if (prevRange == 1 && curRange == 0) 
				{
					prevRange = 0;
					/*
					i2cBuffer.buf[0] = 0x80;
					if (Inst[0].Note == 1)
						i2cBuffer.buf[1] = 60;
					else if (Inst[0].Note == 2)
						i2cBuffer.buf[1] = 62;
					else if (Inst[0].Note == 3)
						i2cBuffer.buf[1] = 64;
					else if (Inst[0].Note == 4)
						i2cBuffer.buf[1] = 65;
					else if (Inst[0].Note == 5)
						i2cBuffer.buf[1] = 67;
					else if (Inst[0].Note == 6)
						i2cBuffer.buf[1] = 69;
					else if (Inst[0].Note == 7)
						i2cBuffer.buf[1] = 71;
					else if (Inst[0].Note == 8)
						i2cBuffer.buf[1] = 72;
					i2cBuffer.buf[2] = 50;
					if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
						VT_HANDLE_FATAL_ERROR(0);
					}
					 */
				}

				// STATE 3: if count == 0, it is time to read the sensor, and play a note
				if (Inst[0].InstrumentID != 0 && count == 0)
				{
					ADCValue = ADCValue / 7; //change to properly make between 1-127 for velocity value
			 		//if (ADCValue > Inst[0].Velocity || ADCValue == 0)
					Inst[0].Velocity = ADCValue;
								

				  	i2cBuffer.length = 3;
					i2cBuffer.buf[0] = 0x90;
					if (Inst[0].Note == 1)
						i2cBuffer.buf[1] = 60;
					else if (Inst[0].Note == 2)
						i2cBuffer.buf[1] = 62;
					else if (Inst[0].Note == 3)
						i2cBuffer.buf[1] = 64;
					else if (Inst[0].Note == 4)
						i2cBuffer.buf[1] = 65;
					else if (Inst[0].Note == 5)
						i2cBuffer.buf[1] = 67;
					else if (Inst[0].Note == 6)
						i2cBuffer.buf[1] = 69;
					else if (Inst[0].Note == 7)
						i2cBuffer.buf[1] = 71;
					else if (Inst[0].Note == 8)
						i2cBuffer.buf[1] = 72;
					
					if (Inst[0].Velocity > 28) i2cBuffer.buf[2] = Inst[0].Velocity;
					else i2cBuffer.buf[2] = 0;
		
					if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
						VT_HANDLE_FATAL_ERROR(0);
					}
					FlipBit(2);
				}
				
				//prepare to send to I2C to play an instrument
			}
			else if (masterBuffer.buf[4] == 2)	//Instrument 1 Pitch
			{

			}
			else if (masterBuffer.buf[4] == 3)	//Instrument 2 Note
			{

			}
			else if (masterBuffer.buf[4] == 4)  //Instrument 2 Velocity
			{

			}
			else if (masterBuffer.buf[4] == 5)	//Instrument 2 Pitch
			{

			}

			lcdmsgBuffer.buf[0] = 0x06;
			lcdmsgBuffer.buf[1] = masterBuffer.buf[1];
			lcdmsgBuffer.buf[2] = masterBuffer.buf[2];
			lcdmsgBuffer.buf[3] = masterBuffer.buf[4];

			/*if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}*/

			//FlipBit(4);			
		}
		else if (masterBuffer.buf[0] == 0x09) //temporary for nick
		{
		  	lcdmsgBuffer.buf[0] = 0x06;
			lcdmsgBuffer.buf[1] = masterBuffer.buf[1];
			lcdmsgBuffer.buf[2] = masterBuffer.buf[2];
			lcdmsgBuffer.buf[3] = masterBuffer.buf[4];

			if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
				VT_HANDLE_FATAL_ERROR(0);
			}  
		}
		else if (masterBuffer.buf[0] == 0x0A || masterBuffer.buf[0] == 0x0B || masterBuffer.buf[0] == 0x0C) //message from LCD thread - Instrument Change
		{
			FlipBit(4);
			if (masterBuffer.buf[0] == 0x0A)
			{
			 	Inst[masterBuffer.buf[1]].InstrumentID = masterBuffer.buf[2];

				/*construct MIDI message to change instrument*/
				i2cBuffer.length = 3;
				i2cBuffer.buf[0] = 0xC0 + masterBuffer.buf[1];
				i2cBuffer.buf[2] = Inst[masterBuffer.buf[1]].InstrumentID;
				i2cBuffer.buf[1] = 0x00;
	
				if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}
			}
			else if (masterBuffer.buf[0] == 0x0B || masterBuffer.buf[0] == 0x0C)
			{
			 	RInst[masterBuffer.buf[1]].InstrumentID = masterBuffer.buf[2];
				RInst[masterBuffer.buf[1]].Note = masterBuffer.buf[3];
				RInst[masterBuffer.buf[1]].BPM = masterBuffer.buf[4];

				/*construct MIDI message to change instrument
				
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
}

