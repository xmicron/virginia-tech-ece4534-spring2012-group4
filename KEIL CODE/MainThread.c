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
	portTickType xUpdateRate, xLastUpdateTime;
	xUpdateRate = 1 / portTICK_RATE_MS;

	MasterParamStruct *temp = pvParameters;
	MasterMsgQueue *masterQ = temp->masterQ;
	MasterMsgQueueMsg masterBuffer;

	vtLCDMsgQueue *lcdQ = temp->lcdQ;
	vtLCDMsg lcdmsgBuffer;

	I2CMsgQueue *i2cQ = temp->i2cQ;
	I2CMsgQueueMsg i2cBuffer;

	InstrumentStruct Inst[2];
	RepeatingInstrumentStruct RInst[3];
	int RTimer = 0;
	xLastUpdateTime = xTaskGetTickCount();

	int i = 0;
	//channel 0
	int curRange0 = 0, prevRange0 = 0, count0 = -1; 
	unsigned int TimerCh0_1 = 0, TimerCh0_2, TimerCh0_3 = 0, TimerCh0_4 = 0, TimerCh0_5 = 0;
	int ADCArray[5];
	//channel 1
	int prevRange1 = 0, curRange1 = 0;
	unsigned int TimerDiff1_1 = 0, TimerDiff1_2 = 0, TimerDiff1_3 = 0, TimerDiff1_4 = 0, TimerDiff1_5 = 0; 
	//channel 2
	int curRange2 = 0, prevRange2 = 0, count2 = -1;
	unsigned int curADCValue2 = 0, initADCValue2 = 0, ADCDiff2 = 0;

	//channel 3
	int curRange3 = 0, prevRange3 = 0, count3 = -1; 
	unsigned int TimerCh3_1 = 0, TimerCh3_2, TimerCh3_3 = 0, TimerCh3_4 = 0, TimerCh3_5 = 0;
	//channel 4
	int prevRange4 = 0, curRange4 = 0;
	unsigned int TimerDiff4_1 = 0, TimerDiff4_2 = 0, TimerDiff4_3 = 0, TimerDiff4_4 = 0, TimerDiff4_5 = 0; 
	//channel 5
	int curRange5 = 0, prevRange5 = 0, count5 = -1;
	unsigned int curADCValue5 = 0, initADCValue5 = 0, ADCDiff5 = 0;
	
	for (i = 0; i < 3; i++)
	{
		RInst[i].InstrumentID = 0;
		RInst[i].Note = 0;
		RInst[i].BPM = 0;
		RInst[i].lastTimer = 0;
	}
	for (i = 0; i < 2; i++)
	{
		Inst[i].InstrumentID = 0;
		Inst[i].lastTimer = 0;
	}

	for(;;)
	{
		if (uxQueueMessagesWaiting(masterQ->inQ) > 0)
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
	
				if (masterBuffer.buf[11] == 0)  // Instrument 1 Note
				{
				
	
					if (ADCValue > 200) curRange0 = 1;
					else curRange0 = 0;
	
					if (count0 >= 0) count0--;
					
					if (prevRange0 == 0 && curRange0 == 1)
					{
						count0 = 4;
						prevRange0 = 1;
	
						lcdmsgBuffer.length = 6;
						lcdmsgBuffer.buf[0] = 0x10;
						lcdmsgBuffer.buf[1] = masterBuffer.buf[4];
						lcdmsgBuffer.buf[2] = masterBuffer.buf[5];
						lcdmsgBuffer.buf[3] = masterBuffer.buf[6];
						lcdmsgBuffer.buf[4] = masterBuffer.buf[7];
						lcdmsgBuffer.buf[5] = masterBuffer.buf[3];
						FlipBit(0);
	
						TimerCh0_1 = masterBuffer.buf[3];
						TimerCh0_2 = masterBuffer.buf[4];
						TimerCh0_3 = masterBuffer.buf[5];
						TimerCh0_4 = masterBuffer.buf[6];
						TimerCh0_5 = masterBuffer.buf[7];
	
	
						
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}  
					}
	
					else if (prevRange0 == 1 && curRange0 == 0) 
					{
						prevRange0 = 0;
					}
	
					if (count0 == 0)
					{
						if (ADCValue >= 200  && ADCValue < 260) // C4
						{	
							Inst[0].Note = 1; 	
						}
						else if (ADCValue >= 260 && ADCValue < 310) // D4
						{
							Inst[0].Note = 2;
						}
						else if (ADCValue >= 310 && ADCValue < 360) // E4 
						{
							Inst[0].Note = 3;
						}
						else if (ADCValue >= 360 && ADCValue < 410)	// F4
						{
							Inst[0].Note = 4;
						}
						else if (ADCValue >= 410 && ADCValue < 470)	// G4
						{
							Inst[0].Note = 5;
						}
						else if (ADCValue >= 470 && ADCValue < 550)	// A4
						{
							Inst[0].Note = 6;
						}
						else if (ADCValue >= 550 && ADCValue < 660)	// B4
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
				   		printf("\n\nThis is the number chosen: %i\n", Inst[0].Note);
						lcdmsgBuffer.length = 1;
						lcdmsgBuffer.buf[0] = 0xFD;
						lcdmsgBuffer.buf[1] = Inst[0].Note;
	
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
	
					}
	
					
				}
				else if (masterBuffer.buf[11] == 1)	//Instrument 1 Velocity
				{
					// NOTE: If a hand is not in the range of a sensor, play nothing (do not send a MIDI msg)
					// NOTE: This is a state machine which determines if a sensor beam has been struck, and subsequently,
					// if a note should be played.  Additionally, it determines the velocity that the note should be played with
	
					// Check to see if a hand is in the range of a sensor.
					if (ADCValue > 200) curRange1 = 1;
					else curRange1 = 0;
	
					
	
					// STATE 1: if the beam has just been struck
					if (prevRange1 == 0 && curRange1 == 1)
					{
						FlipBit(1);
						//in case the user tries to play the instrument WAAAAAYY too fast
						if ((RTimer - Inst[0].lastTimer) < 200 && Inst[0].InstrumentID != 0)
						{
						 	i2cBuffer.buf[0] = 0x4;
							i2cBuffer.buf[1] = 0x80;
						 	
							if (Inst[0].lastNote == 1)
								i2cBuffer.buf[2] = 60;
							else if (Inst[0].lastNote == 2)
								i2cBuffer.buf[2] = 62;
							else if (Inst[0].lastNote == 3)
								i2cBuffer.buf[2] = 64;
							else if (Inst[0].lastNote == 4)
								i2cBuffer.buf[2] = 65;
							else if (Inst[0].lastNote == 5)
								i2cBuffer.buf[2] = 67;
							else if (Inst[0].lastNote == 6)
								i2cBuffer.buf[2] = 69;
							else if (Inst[0].lastNote == 7)
								i2cBuffer.buf[2] = 71;
							else if (Inst[0].lastNote == 8)
								i2cBuffer.buf[2] = 72;
						 	
							i2cBuffer.buf[3] = Inst[0].Velocity;
							
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
								VT_HANDLE_FATAL_ERROR(0);
							}
							printf ("Main Thread: Told Player instrument %i to turn off. Note: %i, Velocity: %i\n", 0, Inst[0].lastNote, Inst[0].Velocity);
						}
						
						
						// Set the COUNT variable to designate the number of messages which should be ignored 
						// before reding the velocity sensor.  In this manner, you can ignore the first (few) invalid
						// sensor reads as a hand crosses its beam.
					 	 		
						prevRange1 = 1;
						
						// Store a timing value
						
						lcdmsgBuffer.length = 11;
						lcdmsgBuffer.buf[0] = 0x09;
						lcdmsgBuffer.buf[1] = masterBuffer.buf[4];
						lcdmsgBuffer.buf[2] = masterBuffer.buf[5];
						lcdmsgBuffer.buf[3] = masterBuffer.buf[6];
						lcdmsgBuffer.buf[4] = masterBuffer.buf[7];
						lcdmsgBuffer.buf[10] = masterBuffer.buf[3];
	
						//FlipBit(2);
	
						if (TimerCh0_1 > masterBuffer.buf[3])
						{
						 	masterBuffer.buf[7] --;
							masterBuffer.buf[3] += 255;
						}
						if (TimerCh0_5 > masterBuffer.buf[7])
						{
						 	masterBuffer.buf[6] --;		// was [6]
							masterBuffer.buf[7] += 255;
						}
						/*if (TimerCh0_3 > masterBuffer.buf[6])
						{
						 	masterBuffer.buf[5] --;
							masterBuffer.buf[6] += 255;
						}
						if (TimerCh0_4 > masterBuffer.buf[5])
						{
						 	masterBuffer.buf[4] --;
							masterBuffer.buf[5] += 255;
						} */
						TimerDiff1_1 = masterBuffer.buf[3] - TimerCh0_1;
						TimerDiff1_2 = masterBuffer.buf[4] - TimerCh0_2;
						TimerDiff1_3 = masterBuffer.buf[5] - TimerCh0_3;
						TimerDiff1_4 = masterBuffer.buf[6] - TimerCh0_4;
						TimerDiff1_5 = masterBuffer.buf[7] - TimerCh0_5;
					   	printf("----------------------%i\n", TimerDiff1_4);
						lcdmsgBuffer.buf[5] = TimerDiff1_1;
						lcdmsgBuffer.buf[6] = TimerDiff1_2;
						lcdmsgBuffer.buf[7] = TimerDiff1_3;
						lcdmsgBuffer.buf[8] = TimerDiff1_4;
						lcdmsgBuffer.buf[9] = TimerDiff1_5;	 
						  			
			
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}			
					    i2cBuffer.length = 4;
							printf("+++++++++++++++++%i\n", TimerDiff1_5);
						if (TimerDiff1_5 > 100) Inst[0].Velocity = 25;
						else if (TimerDiff1_5 <= 100 && TimerDiff1_5 > 80) Inst[0].Velocity = 45;
						else if (TimerDiff1_5 <= 80 && TimerDiff1_5 > 60) Inst[0].Velocity = 45;
						else if (TimerDiff1_5 <= 60 && TimerDiff1_5 > 50) Inst[0].Velocity = 45;
						else if (TimerDiff1_5 <= 50 && TimerDiff1_5 > 40) Inst[0].Velocity = 88;
						else if (TimerDiff1_5 <= 40 && TimerDiff1_5 > 30) Inst[0].Velocity = 88;
						else if (TimerDiff1_5 <= 30 && TimerDiff1_5 > 20) Inst[0].Velocity = 88;
						else if (TimerDiff1_5 <= 20 && TimerDiff1_5 > 10) Inst[0].Velocity = 125;
						else if (TimerDiff1_5 <= 10) Inst[0].Velocity = 125;
	
						i2cBuffer.buf[3] = Inst[0].Velocity;
	
					  	
						i2cBuffer.buf[0] = 0x4;
						i2cBuffer.buf[1] = 0x90;
						if (Inst[0].Note == 1)
							i2cBuffer.buf[2] = 60;
						else if (Inst[0].Note == 2)
							i2cBuffer.buf[2] = 62;
						else if (Inst[0].Note == 3)
							i2cBuffer.buf[2] = 64;
						else if (Inst[0].Note == 4)
							i2cBuffer.buf[2] = 65;
						else if (Inst[0].Note == 5)
							i2cBuffer.buf[2] = 67;
						else if (Inst[0].Note == 6)
							i2cBuffer.buf[2] = 69;
						else if (Inst[0].Note == 7)
							i2cBuffer.buf[2] = 71;
						else if (Inst[0].Note == 8)
							i2cBuffer.buf[2] = 72;
						
						if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
						printf("Main Thread: Sent a Player instrument %i with note %i and velocity %i\n", 0, i2cBuffer.buf[2], i2cBuffer.buf[3]);
						Inst[0].lastTimer = RTimer;
						Inst[0].lastNote = Inst[0].Note;
						FlipBit(2);
	
					}
					// STATE 2: if a hand has been removed from a beam, reset to look for a new break	B0 7B 00
					else if (prevRange1 == 1 && curRange1 == 0) 
					{
						prevRange1 = 0;
	
					}
	
					// STATE 3: if count == 0, it is time to read the sensor, and play a note
				//	if (/*Inst[0].InstrumentID != 0 &&*/ count == 0)
			//		{
						//ADCValue = ADCValue / 7; //change to properly make between 1-127 for velocity value
				 		//if (ADCValue > Inst[0].Velocity || ADCValue == 0)
						//Inst[0].Velocity = ADCValue;
						
						
			//		}
					
					//prepare to send to I2C to play an instrument
				}
				else if (masterBuffer.buf[11] == 2)	//Instrument 1 Pitch
				{	  
					// Check to see if a hand is in the range of a sensor.
					if (ADCValue > 200) curRange2 = 1;
					else curRange2 = 0;
	
					if (count2 >= 0) count2--;
	
					if (prevRange2 == 0 && curRange2 == 1)
					{
						prevRange2 = 1;
						//FlipBit(4);
						count2 = 5;
					}
	
					else if (prevRange2 == 1 && curRange2 == 1)	
					{
						if (ADCValue >= 200  && ADCValue < 250)	curADCValue2 = 1; 
						else if (ADCValue >= 260 && ADCValue < 300)	curADCValue2 = 2;
						else if (ADCValue >= 310 && ADCValue < 345)	curADCValue2 = 3;
						else if (ADCValue >= 360 && ADCValue < 390)	curADCValue2 = 4;
						else if (ADCValue >= 410 && ADCValue < 455)	curADCValue2 = 5;
						else if (ADCValue >= 470 && ADCValue < 540)	curADCValue2 = 6;
						else if (ADCValue >= 550 && ADCValue < 645)	curADCValue2 = 7;
						else if (ADCValue >= 660 && ADCValue < 790) curADCValue2 = 8;
						else if (ADCValue < 180)	curADCValue2 = 0;
	
						ADCDiff2 = initADCValue2 - curADCValue2;
						if(ADCDiff2 < 0)	lcdmsgBuffer.buf[1] = 0;		 	
						else if(ADCDiff2 > 0)	lcdmsgBuffer.buf[1] = 1;
						else	lcdmsgBuffer.buf[1] = 2; 	
						
						lcdmsgBuffer.buf[0] = 0x5F;
						lcdmsgBuffer.length = 2;
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}
	
					else if (prevRange2 == 1 && curRange2 == 0) 
					{
						prevRange2 = 0;
	
					}
					if(count2 == 0)	
					{
						if (ADCValue >= 200  && ADCValue < 250)	initADCValue2 = 1; 
						else if (ADCValue >= 260 && ADCValue < 300)	initADCValue2 = 2;
						else if (ADCValue >= 310 && ADCValue < 345)	initADCValue2 = 3;
						else if (ADCValue >= 360 && ADCValue < 390)	initADCValue2 = 4;
						else if (ADCValue >= 410 && ADCValue < 455)	initADCValue2 = 5;
						else if (ADCValue >= 470 && ADCValue < 540)	initADCValue2 = 6;
						else if (ADCValue >= 550 && ADCValue < 645)	initADCValue2 = 7;
						else if (ADCValue >= 660 && ADCValue < 790) initADCValue2 = 8;
						else if (ADCValue < 180)	initADCValue2 = 0;
						//initADCValue2 = ADCValue;
						lcdmsgBuffer.buf[0] = 0x5F;
						lcdmsgBuffer.buf[1] = initADCValue2;
						lcdmsgBuffer.length = 2;
						
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}	
				}
				else if (masterBuffer.buf[11] == 3)	//Instrument 2 Note
				{
			   		if (ADCValue > 200) curRange3 = 1;
					else curRange3 = 0;
	
					if (count3 >= 0) count3--;
					
					if (prevRange3 == 0 && curRange3 == 1)
					{
						count3 = 5;
						prevRange3 = 1;
	
						lcdmsgBuffer.length = 6;
						lcdmsgBuffer.buf[0] = 0x10;
						lcdmsgBuffer.buf[1] = masterBuffer.buf[4];
						lcdmsgBuffer.buf[2] = masterBuffer.buf[5];
						lcdmsgBuffer.buf[3] = masterBuffer.buf[6];
						lcdmsgBuffer.buf[4] = masterBuffer.buf[7];
						lcdmsgBuffer.buf[5] = masterBuffer.buf[3];
						
	
						TimerCh3_1 = masterBuffer.buf[3];
						TimerCh3_2 = masterBuffer.buf[4];
						TimerCh3_3 = masterBuffer.buf[5];
						TimerCh3_4 = masterBuffer.buf[6];
						TimerCh3_5 = masterBuffer.buf[7];
	
	
						
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}  
					}
	
					else if (prevRange3 == 1 && curRange3 == 0) 
					{
						prevRange3 = 0;
					}
	
					if (count3 == 0)
					{
						if (ADCValue >= 200  && ADCValue < 250) // C4
						{	
							Inst[1].Note = 1; 	
						}
						else if (ADCValue >= 260 && ADCValue < 300) // D4
						{
							Inst[1].Note = 2;
						}
						else if (ADCValue >= 310 && ADCValue < 345) // E4 
						{
							Inst[1].Note = 3;
						}
						else if (ADCValue >= 360 && ADCValue < 390)	// F4
						{
							Inst[1].Note = 4;
						}
						else if (ADCValue >= 410 && ADCValue < 455)	// G4
						{
							Inst[1].Note = 5;
						}
						else if (ADCValue >= 470 && ADCValue < 540)	// A4
						{
							Inst[1].Note = 6;
						}
						else if (ADCValue >= 550 && ADCValue < 645)	// B4
						{
							Inst[1].Note = 7;
						}
						else if (ADCValue >= 660 && ADCValue < 790)	// C5
						{
							Inst[1].Note = 8;
						}
						else if (ADCValue < 180)
						{
							Inst[1].Note = 0;
						}
	
						lcdmsgBuffer.length = 1;
						lcdmsgBuffer.buf[0] = 0xFD;
						lcdmsgBuffer.buf[1] = Inst[1].Note;
					   	FlipBit(3);
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
	
					}
	
				}
				else if (masterBuffer.buf[11] == 4)  //Instrument 2 Velocity
				{
			   		// NOTE: If a hand is not in the range of a sensor, play nothing (do not send a MIDI msg)
					// NOTE: This is a state machine which determines if a sensor beam has been struck, and subsequently,
					// if a note should be played.  Additionally, it determines the velocity that the note should be played with
	
					// Check to see if a hand is in the range of a sensor.
					if (ADCValue > 200) curRange4 = 1;
					else curRange4 = 0;
	
					
	
					// STATE 1: if the beam has just been struck
					if (prevRange4 == 0 && curRange4 == 1)
					{
						FlipBit(4);
						//in case the user tries to play the instrument WAAAAAYY too fast
						if ((RTimer - Inst[1].lastTimer) < 200 && Inst[1].InstrumentID != 0)
						{
						 	i2cBuffer.buf[0] = 0x4;
							i2cBuffer.buf[1] = 0x81;
			
							if (Inst[1].lastNote == 1)
								i2cBuffer.buf[2] = 60;
							else if (Inst[1].lastNote == 2)
								i2cBuffer.buf[2] = 62;
							else if (Inst[1].lastNote == 3)
								i2cBuffer.buf[2] = 64;
							else if (Inst[1].lastNote == 4)
								i2cBuffer.buf[2] = 65;
							else if (Inst[1].lastNote == 5)
								i2cBuffer.buf[2] = 67;
							else if (Inst[1].lastNote == 6)
								i2cBuffer.buf[2] = 69;
							else if (Inst[1].lastNote == 7)
								i2cBuffer.buf[2] = 71;
							else if (Inst[1].lastNote == 8)
								i2cBuffer.buf[2] = 72;
			
							i2cBuffer.buf[3] = Inst[1].Velocity;
							
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
								VT_HANDLE_FATAL_ERROR(0);
							}
							printf ("Main Thread: Told Player instrument %i to turn off. Note: %i, Velocity: %i\n", 1, Inst[1].lastNote, Inst[1].Velocity);
						}
						
						
						// Set the COUNT variable to designate the number of messages which should be ignored 
						// before reding the velocity sensor.  In this manner, you can ignore the first (few) invalid
						// sensor reads as a hand crosses its beam.
					 	 		
						prevRange4 = 1;
						
						// Store a timing value
						
						lcdmsgBuffer.length = 11;
						lcdmsgBuffer.buf[0] = 0x09;
						lcdmsgBuffer.buf[1] = masterBuffer.buf[4];
						lcdmsgBuffer.buf[2] = masterBuffer.buf[5];
						lcdmsgBuffer.buf[3] = masterBuffer.buf[6];
						lcdmsgBuffer.buf[4] = masterBuffer.buf[7];
						lcdmsgBuffer.buf[10] = masterBuffer.buf[3];
	
						FlipBit(4);
	
						if (TimerCh3_1 > masterBuffer.buf[3])
						{
						 	masterBuffer.buf[7] --;
							masterBuffer.buf[3] += 255;
						}
						if (TimerCh3_5 > masterBuffer.buf[7])
						{
						 	masterBuffer.buf[6] --;		// was [6]
							masterBuffer.buf[7] += 255;
						}
						/*if (TimerCh0_3 > masterBuffer.buf[6])
						{
						 	masterBuffer.buf[5] --;
							masterBuffer.buf[6] += 255;
						}
						if (TimerCh0_4 > masterBuffer.buf[5])
						{
						 	masterBuffer.buf[4] --;
							masterBuffer.buf[5] += 255;
						} */
						TimerDiff4_1 = masterBuffer.buf[3] - TimerCh3_1;
						TimerDiff4_2 = masterBuffer.buf[4] - TimerCh3_2;
						TimerDiff4_3 = masterBuffer.buf[5] - TimerCh3_3;
						TimerDiff4_4 = masterBuffer.buf[6] - TimerCh3_4;
						TimerDiff4_5 = masterBuffer.buf[7] - TimerCh3_5;
	
						lcdmsgBuffer.buf[5] = TimerDiff4_1;
						lcdmsgBuffer.buf[6] = TimerDiff4_2;
						lcdmsgBuffer.buf[7] = TimerDiff4_3;
						lcdmsgBuffer.buf[8] = TimerDiff4_4;
						lcdmsgBuffer.buf[9] = TimerDiff4_5;	 
						  			
			
						if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}			
					    i2cBuffer.length = 4;
						if (TimerDiff4_5 > 100) Inst[1].Velocity = 25;
						else if (TimerDiff4_5 <= 100 && TimerDiff4_5 > 80) Inst[1].Velocity = 45;
						else if (TimerDiff4_5 <= 80 && TimerDiff4_5 > 60) Inst[1].Velocity = 45;
						else if (TimerDiff4_5 <= 60 && TimerDiff4_5 > 50) Inst[1].Velocity = 45;
						else if (TimerDiff4_5 <= 50 && TimerDiff4_5 > 40) Inst[1].Velocity = 88;
						else if (TimerDiff4_5 <= 40 && TimerDiff4_5 > 30) Inst[1].Velocity = 88;
						else if (TimerDiff4_5 <= 30 && TimerDiff4_5 > 20) Inst[1].Velocity = 88;
						else if (TimerDiff4_5 <= 20 && TimerDiff4_5 > 10) Inst[1].Velocity = 125;
						else if (TimerDiff4_5 <= 10) Inst[1].Velocity = 125;
	
						i2cBuffer.buf[3] = Inst[1].Velocity;
	
					  	
						i2cBuffer.buf[0] = 0x4;
						i2cBuffer.buf[1] = 0x91;
						if (Inst[1].Note == 1)
							i2cBuffer.buf[2] = 60;
						else if (Inst[1].Note == 2)
							i2cBuffer.buf[2] = 62;
						else if (Inst[1].Note == 3)
							i2cBuffer.buf[2] = 64;
						else if (Inst[1].Note == 4)
							i2cBuffer.buf[2] = 65;
						else if (Inst[1].Note == 5)
							i2cBuffer.buf[2] = 67;
						else if (Inst[1].Note == 6)
							i2cBuffer.buf[2] = 69;
						else if (Inst[1].Note == 7)
							i2cBuffer.buf[2] = 71;
						else if (Inst[1].Note == 8)
							i2cBuffer.buf[2] = 72;
						
			
						if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
						printf("Main Thread: Sent a Player instrument %i with note %i and velocity %i\n", 1, i2cBuffer.buf[2], i2cBuffer.buf[3]);
						Inst[1].lastTimer = RTimer;
						Inst[1].lastNote = Inst[1].Note;
						FlipBit(5);
	
					}
					// STATE 2: if a hand has been removed from a beam, reset to look for a new break	B0 7B 00
					else if (prevRange4 == 1 && curRange4 == 0) 
					{
						prevRange4 = 0;
	
					}
	
					// STATE 3: if count == 0, it is time to read the sensor, and play a note
				//	if (/*Inst[0].InstrumentID != 0 &&*/ count == 0)
			//		{
						//ADCValue = ADCValue / 7; //change to properly make between 1-127 for velocity value
				 		//if (ADCValue > Inst[0].Velocity || ADCValue == 0)
						//Inst[0].Velocity = ADCValue;
						
						
			//		}
					
					//prepare to send to I2C to play an instrument
				}
				else if (masterBuffer.buf[11] == 5)	//Instrument 2 Pitch
				{
	
				}
	
			//	lcdmsgBuffer.buf[0] = 0x06;
		//		lcdmsgBuffer.buf[1] = masterBuffer.buf[1];
		//		lcdmsgBuffer.buf[2] = masterBuffer.buf[2];
		//		lcdmsgBuffer.buf[3] = masterBuffer.buf[4];
	
				/*if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}*/
	
				//FlipBit(4);			
			}
		/*	else if (masterBuffer.buf[0] == 0x09) //temporary for nick
			{
			  	lcdmsgBuffer.buf[0] = 0x06;
				lcdmsgBuffer.buf[1] = masterBuffer.buf[1];
				lcdmsgBuffer.buf[2] = masterBuffer.buf[2];
				lcdmsgBuffer.buf[3] = masterBuffer.buf[4];
	
				if (xQueueSend(lcdQ->inQ,(void *) (&lcdmsgBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}  
			}	   */
			else if (masterBuffer.buf[0] == 0x0A || masterBuffer.buf[0] == 0x0B || masterBuffer.buf[0] == 0x0C) //message from LCD thread - Instrument Change
			{
				//FlipBit(4);
				if (masterBuffer.buf[0] == 0x0A)
				{
				 	Inst[masterBuffer.buf[1]].InstrumentID = masterBuffer.buf[2];

					printf("MainThread. Received Message to update Player Instrument %i to %i.\n", masterBuffer.buf[1], masterBuffer.buf[2]);
	
					/*construct MIDI message to change instrument*/
					i2cBuffer.length = 4;
					i2cBuffer.buf[0] = 0x4;
					i2cBuffer.buf[1] = 0xC0 + masterBuffer.buf[1];
					i2cBuffer.buf[3] = Inst[masterBuffer.buf[1]].InstrumentID;
					i2cBuffer.buf[2] = 0x00;
		
					if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
						VT_HANDLE_FATAL_ERROR(0);
					}
				}
				else if (masterBuffer.buf[0] == 0x0B || masterBuffer.buf[0] == 0x0C)
				{
					if (masterBuffer.buf[2] == 0 || masterBuffer.buf[3] == 0 || masterBuffer.buf[4] == 0)
					{
					 	i2cBuffer.buf[0] = 0x4;
						i2cBuffer.buf[1] = 0x80 + masterBuffer.buf[1] + 2;
		
						if (RInst[masterBuffer.buf[1]].Note == 1)
							i2cBuffer.buf[2] = 60;
						else if (RInst[masterBuffer.buf[1]].Note == 2)
							i2cBuffer.buf[2] = 62;
						else if (RInst[masterBuffer.buf[1]].Note == 3)
							i2cBuffer.buf[2] = 64;
						else if (RInst[masterBuffer.buf[1]].Note == 4)
							i2cBuffer.buf[2] = 65;
						else if (RInst[masterBuffer.buf[1]].Note == 5)
							i2cBuffer.buf[2] = 67;
						else if (RInst[masterBuffer.buf[1]].Note == 6)
							i2cBuffer.buf[2] = 69;
						else if (RInst[masterBuffer.buf[1]].Note == 7)
							i2cBuffer.buf[2] = 71;
						else if (RInst[masterBuffer.buf[1]].Note == 8)
							i2cBuffer.buf[2] = 72;
		
						i2cBuffer.buf[3] = 100;
						
						if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}
				 	RInst[masterBuffer.buf[1]].InstrumentID = masterBuffer.buf[2];
					RInst[masterBuffer.buf[1]].Note = masterBuffer.buf[3];
					RInst[masterBuffer.buf[1]].BPM = masterBuffer.buf[4];

					printf("MainThread. Received Message to update Repeating Instrument %i to instrument %i, note %i, BPM %i.\n", masterBuffer.buf[1], masterBuffer.buf[2], masterBuffer.buf[3], masterBuffer.buf[4]);

					if (RInst[masterBuffer.buf[1]].InstrumentID != 0 && RInst[masterBuffer.buf[1]].Note != 0)
					{
					 	RInst[masterBuffer.buf[1]].lastTimer = RTimer;
					}
					
					/*construct MIDI message to change instrument*/
					
					//uint8_t MidiSendMsg[3];
					i2cBuffer.length = 4;
					i2cBuffer.buf[0] = 0x4;
					i2cBuffer.buf[1] = 0xC0 + masterBuffer.buf[1] + 2;
					i2cBuffer.buf[3] = RInst[masterBuffer.buf[1]].InstrumentID;
					i2cBuffer.buf[2] = 0x00;
		
					if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
						VT_HANDLE_FATAL_ERROR(0);
					}
				}
			}
		}

		vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );
		//xLastUpdateTime = xTaskGetTickCount();

		int p;
		
		for (p = 0; p < 3; p++)
		{
		 	if ( RTimer >= RInst[p].lastTimer + (60000 / RInst[p].BPM) && RInst[p].InstrumentID != 0 && RInst[p].BPM != 0 && RInst[p].Note != 0)
			{
			 	RInst[p].lastTimer = RTimer;

				//printf("MainThread. Constructing MIDI message for Repeating Instrument %i at time %i.\n", p, RTimer * 10);

				i2cBuffer.buf[0] = 0x4;
				i2cBuffer.buf[1] = 0x90 + p + 2;

				if (RInst[p].Note == 1)
					i2cBuffer.buf[2] = 60;
				else if (RInst[p].Note == 2)
					i2cBuffer.buf[2] = 62;
				else if (RInst[p].Note == 3)
					i2cBuffer.buf[2] = 64;
				else if (RInst[p].Note == 4)
					i2cBuffer.buf[2] = 65;
				else if (RInst[p].Note == 5)
					i2cBuffer.buf[2] = 67;
				else if (RInst[p].Note == 6)
					i2cBuffer.buf[2] = 69;
				else if (RInst[p].Note == 7)
					i2cBuffer.buf[2] = 71;
				else if (RInst[p].Note == 8)
					i2cBuffer.buf[2] = 72;

				i2cBuffer.buf[3] = 100;
				
				if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}
			}
			if ((RTimer - RInst[p].lastTimer) == 200 && RInst[p].InstrumentID != 0 && RInst[p].BPM != 0 && RInst[p].Note != 0)
			{
			 	i2cBuffer.buf[0] = 0x4;
				i2cBuffer.buf[1] = 0x80 + p + 2;

				if (RInst[p].Note == 1)
					i2cBuffer.buf[2] = 60;
				else if (RInst[p].Note == 2)
					i2cBuffer.buf[2] = 62;
				else if (RInst[p].Note == 3)
					i2cBuffer.buf[2] = 64;
				else if (RInst[p].Note == 4)
					i2cBuffer.buf[2] = 65;
				else if (RInst[p].Note == 5)
					i2cBuffer.buf[2] = 67;
				else if (RInst[p].Note == 6)
					i2cBuffer.buf[2] = 69;
				else if (RInst[p].Note == 7)
					i2cBuffer.buf[2] = 71;
				else if (RInst[p].Note == 8)
					i2cBuffer.buf[2] = 72;

				i2cBuffer.buf[3] = 100;
				
				if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}
			}
		}
		for (p = 0; p < 2; p++)
		{
		 	if ((RTimer - Inst[p].lastTimer) == 200 && Inst[p].InstrumentID != 0)
			{
			 	i2cBuffer.buf[0] = 0x4;
				i2cBuffer.buf[1] = 0x80 + p;

				if (Inst[p].lastNote == 1)
					i2cBuffer.buf[2] = 60;
				else if (Inst[p].lastNote == 2)
					i2cBuffer.buf[2] = 62;
				else if (Inst[p].lastNote == 3)
					i2cBuffer.buf[2] = 64;
				else if (Inst[p].lastNote == 4)
					i2cBuffer.buf[2] = 65;
				else if (Inst[p].lastNote == 5)
					i2cBuffer.buf[2] = 67;
				else if (Inst[p].lastNote == 6)
					i2cBuffer.buf[2] = 69;
				else if (Inst[p].lastNote == 7)
					i2cBuffer.buf[2] = 71;
				else if (Inst[p].lastNote == 8)
					i2cBuffer.buf[2] = 72;

				i2cBuffer.buf[3] = Inst[p].Velocity;
				
				if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
					VT_HANDLE_FATAL_ERROR(0);
				}
				printf ("Main Thread: Told Player instrument %i to turn off. Note: %i, Velocity: %i\n", p, i2cBuffer.buf[2], Inst[p].Velocity);
			}
		}
		
		if (RTimer > 999999)
		{
		  	RTimer = 250;

			for (p = 0; p < 3; p++)
			{
			 	RInst[p].lastTimer = 250 - (1000000 - RInst[p].lastTimer);
			}
		}
		else
			RTimer++;	
	}
}

