#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* include files. */
#include "GLCD.h"
#include "vtUtilities.h"
#include "LCDtask.h"

// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the LCD operations
#if PRINTF_VERSION==1
#define lcdSTACK_SIZE		8*configMINIMAL_STACK_SIZE
#else
#define lcdSTACK_SIZE		4*configMINIMAL_STACK_SIZE
#endif

// If LCD_EXAMPLE_OP=0, then repeatedly write text
// If LCD_EXAMPLE_OP=1, then do a rotating ARM bitmap display
// If LCD_EXAMPLE_OP=2, then receive from a message queue and print the contents to the screen
#define LCD_EXAMPLE_OP 3
// If JOYSTICK_MODE ==0, no crosshair joystick, but instead a selection joystick
// If JOYSTICK_MODE ==1, crosshair mode for the main page (under construction)
#define JOYSTICK_MODE 1
// If JOYSTICK_MODE == 0, Insteon disabled
// If JOYSTICK_MODE == 1, Insteon Enabled
#define INSTEON_MODE 0

// Set the task up to run every 200 ms
#define lcdWRITE_RATE_BASE	( ( portTickType ) 10 )

static portTASK_FUNCTION_PROTO( vLCDUpdateTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartLCDTask( unsigned portBASE_TYPE uxPriority,lcdParamStruct *ptr )
{

	// Create the queue that will be used to talk to the LCD
	if ((ptr->lcdQ->inQ = xQueueCreate(vtLCDQLen,sizeof(vtLCDMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	/* Start the task */
	portBASE_TYPE retval;
	if ((retval = xTaskCreate( vLCDUpdateTask, ( signed char * ) "LCD", lcdSTACK_SIZE, (void*)ptr, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

typedef struct __cursorPos {
	uint8_t x;
	uint8_t y;
} cursorPos;

// This is the actual task that is run
static portTASK_FUNCTION( vLCDUpdateTask, pvParameters )
{
	//initial variable declarations
	portTickType xUpdateRate, xLastUpdateTime;
	int i = 0;
	char toPr[20];
	
#if LCD_EXAMPLE_OP==2
	int pixel_buffer[80];
	float max = 0;
	float min = 990;
	unsigned short screenColor;
	unsigned char curLine = 0;
	float avg = 0;
#elif LCD_EXAMPLE_OP==3
	int Cur_Panel = 0;
	int Cur_Page = 0;
	int Cur_Inst = 0;
	int RorP = 0;
	int P2SelectionMultiplier = 0;
	int P1Selection = 0;
	int P3Selection = 0;

	int VOLUME = 100;
	int SLIDER = 10;
	InstrumentStruct Inst[2];
	RepeatingInstrumentStruct RInst[3];
	for (Cur_Page = 0; Cur_Page < 3; Cur_Page++)
	{
		Inst[Cur_Page].InstrumentID = 0;
		RInst[Cur_Page].InstrumentID = 0;
		RInst[Cur_Page].Note = 0;
		RInst[Cur_Page].BPM = 0;
	}
	for (Cur_Page = 0; Cur_Page < 3; Cur_Page++)
	{
		Inst[Cur_Page].InstrumentID = 0;
	}
	Cur_Page = 0;
	cursorPos Cursor;
	Cursor.x = 0;
	Cursor.y = 0;
#endif
	lcdParamStruct * params = pvParameters;
	vtLCDMsgQueue *lcdPtr = (vtLCDMsgQueue *) params->lcdQ;
	MasterMsgQueue * masterData = params->masterQ;
	I2CMsgQueue * i2cQ = params->i2cQ;
	MasterMsgQueueMsg masterBuffer;
	I2CMsgQueueMsg i2cBuffer;
	vtLCDMsg msgBuffer;



	
	//LCD Screen Initializations
	GLCD_Init();
	xUpdateRate = lcdWRITE_RATE_BASE / portTICK_RATE_MS;// Scale the update rate to ensure it really is in ms
	xLastUpdateTime = xTaskGetTickCount();//We need to initialise xLastUpdateTime prior to the first call to vTaskDelayUntil().
	// Note that srand() & rand() require the use of malloc() and should not be used unless you are using
	//   MALLOC_VERSION==1
	#if MALLOC_VERSION==1
	srand((unsigned) 55); // initialize the random number generator to the same seed for repeatability
	#endif
#if LCD_EXAMPLE_OP==3
	GLCD_SetTextColor(Green);
	GLCD_SetBackColor(Black);
	GLCD_Clear(Black);
	InitPage(0, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
#elif LCD_EXAMPLE_OP==2
	GLCD_Clear(Yellow);
	GLCD_SetTextColor(Black);
#endif
	for(;;)
	{
#if	LCD_EXAMPLE_OP==3					//this will be our UI mode OP
		
		
		if (xQueueReceive(lcdPtr->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) //receive message from message queue
		{
			VT_HANDLE_FATAL_ERROR(0);
		}
		

   #if JOYSTICK_MODE==0
		
		if (msgBuffer.buf[0] = 0x11)
		{
			FlipBit(0);
			if (Cur_Page == 0)
			{
				if (P1Selection == 0)
				{
					if (msgBuffer.buf[1] == 0) //select bit hit
					{
						if (INSTEON_MODE == 1)
						{
	
						}
						if (Cur_Panel > 3)
						{
							GLCD_Clear(Black);
							Cur_Page = 1;
							Cur_Inst = Cur_Panel-4;
							RorP = 0;
							InitPage(1, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
							Cur_Panel = 0;
						}
						else if (Cur_Panel == 3)
						{
							P1Selection = 1;
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							Cur_Panel = 0;
							GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume");
						}
						else if (Cur_Panel < 3)
						{
							GLCD_Clear(Black);
							Cur_Page = 3;
							Cur_Inst = Cur_Panel;
							P3Selection = 0;
							RorP = 1;
							InitPage(3, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], Cur_Inst);
							Cur_Panel = 0;
						}
					}
				 	if (msgBuffer.buf[1] == 1) //move crosshair up
					{
						if (INSTEON_MODE == 1)
						{
							  //02 61 01 11 00
							i2cBuffer.buf[0] = 0x11;
							i2cBuffer.buf[1] = 0x02;
							i2cBuffer.buf[2] = 0x61;
							i2cBuffer.buf[3] = 0x01;
							i2cBuffer.buf[4] = 0x11;
							i2cBuffer.buf[5] = 0x00;
	
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
							}
						}
						if (Cur_Panel > 0)
						{
							ClearOldSelection(Cur_Panel);
						 	Cur_Panel--;
							MakeSelection(Cur_Panel);
						}
					}
					if (msgBuffer.buf[1] == 2) //move crosshair right
					{
						if (INSTEON_MODE == 1)
						{
	
						}
						if (Cur_Panel < 3)
						{
							ClearOldSelection(Cur_Panel);
							Cur_Panel = Cur_Panel + 3;
							MakeSelection(Cur_Panel);
						}
					} 
					if (msgBuffer.buf[1] == 3) //move crosshair down
					{
						if (INSTEON_MODE == 1)
						{
							//02 61 01 13 00
							i2cBuffer.buf[0] = 0x11;
							i2cBuffer.buf[1] = 0x02;
							i2cBuffer.buf[2] = 0x61;
							i2cBuffer.buf[3] = 0x01;
							i2cBuffer.buf[4] = 0x13;
							i2cBuffer.buf[5] = 0x00;
	
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
							}
						}
						if (Cur_Panel < 5)
						{
							ClearOldSelection(Cur_Panel);
						 	Cur_Panel++;
							MakeSelection(Cur_Panel);
						}
					}
					if (msgBuffer.buf[1] == 4) //move crosshair left
					{
						if (INSTEON_MODE == 1)
						{
							//02 64 01 01
							i2cBuffer.buf[0] = 0x11;
							i2cBuffer.buf[1] = 0x02;
							i2cBuffer.buf[2] = 0x64;
							i2cBuffer.buf[3] = 0x01;
							i2cBuffer.buf[4] = 0x01;
							FlipBit(6);
	
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
							}
						}
						if (Cur_Panel > 3)
						{
							ClearOldSelection(Cur_Panel);
							Cur_Panel = Cur_Panel - 3;
							MakeSelection(Cur_Panel);
						}
						else if (Cur_Panel == 3)
						{
							ClearOldSelection(Cur_Panel);
							Cur_Panel = 0;
							MakeSelection(Cur_Panel);
						}
					}
			 	}
				else if (P1Selection == 1)
				{
					if (msgBuffer.buf[1] == 0)
					{
					  	if (Cur_Panel == 0)
						{
							GLCD_SetTextColor(Green);
							GLCD_SetBackColor(Black);
							GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume");
							P1Selection = 2;
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							sprintf(toPr, "%i", VOLUME);
							GLCD_DisplayString(2,40,0,(unsigned char *)toPr);	
						}
						else if (Cur_Panel == 1)
						{
						  	GLCD_SetTextColor(Blue);
							GLCD_SetBackColor(Black);
						    GLCD_DisplayString(4, 34, 0, (unsigned char *)"Ambient Lighting");
							P1Selection = 3;
						}
					}
					else if (msgBuffer.buf[1] == 1)
					{
					 	if (Cur_Panel > 0)
						{
							Cur_Panel--;
							GLCD_SetTextColor(Green);
							GLCD_SetBackColor(Black);
						    GLCD_DisplayString(4, 34, 0, (unsigned char *)"Ambient Lighting");
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume"); 
						}
					}
					else if (msgBuffer.buf[1] == 3)
					{
						if (Cur_Panel < 1)
						{
							Cur_Panel++;
							GLCD_SetTextColor(Green);
							GLCD_SetBackColor(Black);
							GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume");
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							GLCD_DisplayString(4, 34, 0, (unsigned char *)"Ambient Lighting");
						}
					}
				}
				else if (P1Selection == 2)
				{
				 	if (msgBuffer.buf[1] == 0)
					{
					  	GLCD_SetTextColor(Green);
						GLCD_SetBackColor(Black);
						sprintf(toPr, "%i", VOLUME);
						GLCD_DisplayString(2,40,0,(unsigned char *)toPr);
					  	P1Selection = 0;
						Cur_Panel = 3;
					}
					else if (msgBuffer.buf[1] == 1)
					{
						if (VOLUME < 100)
						{
							GLCD_SetTextColor(Black);
							GLCD_SetBackColor(Black);
							sprintf(toPr, "%i", VOLUME);
							GLCD_DisplayString(2,40,0,(unsigned char *)toPr);
						  	VOLUME++;
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							sprintf(toPr, "%i", VOLUME);
							GLCD_DisplayString(2,40,0,(unsigned char *)toPr);
						}
					}
					else if (msgBuffer.buf[1] == 3)
					{
						if (VOLUME > 0)
						{
						   	GLCD_SetTextColor(Black);
							GLCD_SetBackColor(Black);
							sprintf(toPr, "%i", VOLUME);
							GLCD_DisplayString(2,40,0,(unsigned char *)toPr);
						  	VOLUME--;
							GLCD_SetTextColor(Red);
							GLCD_SetBackColor(Yellow);
							sprintf(toPr, "%i", VOLUME);
							GLCD_DisplayString(2,40,0,(unsigned char *)toPr);
						}
					}
				}
				else if (P1Selection == 3)
				{
				  	if (msgBuffer.buf[1] == 0)
					{
					 	P1Selection = 0;
						Cur_Panel = 3;
						GLCD_SetTextColor(Green);
						GLCD_SetBackColor(Black);
					    GLCD_DisplayString(4, 34, 0, (unsigned char *)"Ambient Lighting");
					}
					else if (msgBuffer.buf[1] == 2)
					{ 
					  	if (SLIDER < 10)
						{
							SLIDER++;
							Set_Slider(SLIDER-1, SLIDER);
						}
					}
					else if (msgBuffer.buf[1] == 4)
					{
					 	if (SLIDER > 0)
						{
							SLIDER--;
							Set_Slider(SLIDER+1, SLIDER);
						}
					}
				}
			}
			else if (Cur_Page == 1)
			{
				if (msgBuffer.buf[1] == 0) //select bit hit
				{
					GLCD_Clear(Black);
					Cur_Page = 2;
					P2SelectionMultiplier = Cur_Panel;
					InitPage(2, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
					Cur_Panel = 0;
				}
			 	if (msgBuffer.buf[1] == 1) //move crosshair up
				{
					if (Cur_Panel > 0)
					{
						P1CatSelectionClear(Cur_Panel);
					 	Cur_Panel--;
						P1CatMakeSelection(Cur_Panel);
					}
				}
				if (msgBuffer.buf[1] == 2) //move crosshair right
				{				
				} 
				if (msgBuffer.buf[1] == 3) //move crosshair down
				{
					if (Cur_Panel < 15)
					{
						P1CatSelectionClear(Cur_Panel);
					 	Cur_Panel++;
						P1CatMakeSelection(Cur_Panel);
					}
				}
				if (msgBuffer.buf[1] == 4) //move crosshair left
				{
				}
			}
			else if (Cur_Page == 2)
			{
			 	if (msgBuffer.buf[1] == 0) //select bit hit
				{
					
					if (RorP == 0)
					{GLCD_Clear(Black);
						Cur_Page = 0;
						if (Cur_Panel == 0)
							Inst[Cur_Inst].InstrumentID = 0;
						else
							Inst[Cur_Inst].InstrumentID = P2SelectionMultiplier*8+Cur_Panel;
						P2SelectionMultiplier = 0;
						InitPage(0, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
						Cur_Panel = 0;

						masterBuffer.length = 3;
						masterBuffer.buf[0] = 0x0A; //signifies comes from lcd thread - instrument change
						masterBuffer.buf[1] = Cur_Inst;
						masterBuffer.buf[2] = Inst[Cur_Inst].InstrumentID;
	
						if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}
					else if (RorP == 1)
					{
						masterBuffer.length = 5;
						masterBuffer.buf[0] = 0x0A; //signifies comes from lcd thread Instrument change
						masterBuffer.buf[1] = Cur_Inst+2;
						masterBuffer.buf[2] = RInst[Cur_Inst].InstrumentID;
						masterBuffer.buf[3] = RInst[Cur_Inst].Note;
						masterBuffer.buf[4] = RInst[Cur_Inst].BPM;
	
						if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}

						GLCD_Clear(Black);
						Cur_Page = 3;
						if (Cur_Panel == 0)
							RInst[Cur_Inst].InstrumentID = 0;
						else
							RInst[Cur_Inst].InstrumentID = P2SelectionMultiplier*8+Cur_Panel;
						P3Selection = 0;
						RorP = 1;
						InitPage(3, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], Cur_Inst);
						Cur_Panel = 0;
					}
				}
			 	if (msgBuffer.buf[1] == 1) //move crosshair up
				{
					if (Cur_Panel > 0)
					{
						P2SelectionClear(Cur_Panel, P2SelectionMultiplier);
					 	Cur_Panel--;
						P2MakeSelection(Cur_Panel, P2SelectionMultiplier);
					}
				}
				if (msgBuffer.buf[1] == 2) //move crosshair right
				{
				} 
				if (msgBuffer.buf[1] == 3) //move crosshair down
				{
					if (Cur_Panel < 8)
					{
						P2SelectionClear(Cur_Panel, P2SelectionMultiplier);
					 	Cur_Panel++;
						P2MakeSelection(Cur_Panel, P2SelectionMultiplier);
					}
				}
				if (msgBuffer.buf[1] == 4) //move crosshair left
				{
				}
			}
			else if (Cur_Page == 3)
			{
				if (P3Selection == 0)
				{
				   	if (msgBuffer.buf[1] == 0) //select bit hit
					{
						if (Cur_Panel == 0)
						{
						   	GLCD_Clear(Black);
							Cur_Page = 0;
							//P2SelectionMultiplier = Cur_Panel;
							InitPage(0, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
							Cur_Panel = 0;
						}
						else if (Cur_Panel == 1)
						{
						  	GLCD_Clear(Black);
							Cur_Page = 1;
							RorP = 1;
							InitPage(1, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
							Cur_Panel = 0;
						}
						else if (Cur_Panel == 2)
						{
							P3Selection = 1;
							GLCD_DisplayString(12,5,0,(unsigned char *)"Select Note: ");
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green);  	
						}
						else if (Cur_Panel == 3)
						{
							char toPr[2];
						 	P3Selection = 2;
							GLCD_DisplayString(12,5,0,(unsigned char *)"Select BPM: ");
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							RInst[Cur_Inst].BPM = 60;
							sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
							GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green); 
						}
					}
				 	if (msgBuffer.buf[1] == 1) //move crosshair up
					{
						if (Cur_Panel > 0)
						{
							P3CatSelectionClear(Cur_Panel);
						 	Cur_Panel--;
							P3CatMakeSelection(Cur_Panel);
						}
					}
					if (msgBuffer.buf[1] == 2) //move crosshair right
					{
					} 
					if (msgBuffer.buf[1] == 3) //move crosshair down
					{
						if (Cur_Panel < 3)
						{
							P3CatSelectionClear(Cur_Panel);
						 	Cur_Panel++;
							P3CatMakeSelection(Cur_Panel);
						}
					}
					if (msgBuffer.buf[1] == 4) //move crosshair left
					{
					}
				}
				else if (P3Selection == 1)
				{
				   	if (msgBuffer.buf[1] == 0) //select bit hit
					{
						P3Selection = 0;
						GLCD_SetBackColor(Black);
						GLCD_SetTextColor(Black);
						GLCD_DisplayString(4,8,0,(unsigned char *)ReturnNoteLabel(0));
						GLCD_DisplayString(12,5,0,(unsigned char *)"Select Note: ");
						GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
						GLCD_SetBackColor(Black);
						GLCD_SetTextColor(Green);
	
						GLCD_DisplayString(4,2,0,(unsigned char *)"Note: ");
						GLCD_DisplayString(4,8,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
	
						masterBuffer.length = 5;
						masterBuffer.buf[0] = 0x0B; //signifies comes from lcd thread Note Change
						masterBuffer.buf[1] = Cur_Inst+2;
						masterBuffer.buf[2] = RInst[Cur_Inst].InstrumentID;
						masterBuffer.buf[3] = RInst[Cur_Inst].Note;
						masterBuffer.buf[4] = RInst[Cur_Inst].BPM;
	
						if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}
				 	if (msgBuffer.buf[1] == 1) //move crosshair up
					{
						if (RInst[Cur_Inst].Note < 7)
						{
							GLCD_SetTextColor(Black);
							GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
							RInst[Cur_Inst].Note++;
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green);
						}
					}
					if (msgBuffer.buf[1] == 2) //move crosshair right
					{
					} 
					if (msgBuffer.buf[1] == 3) //move crosshair down
					{
						if (RInst[Cur_Inst].Note > 0)
						{
							GLCD_SetTextColor(Black);
							GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
							RInst[Cur_Inst].Note--;
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							GLCD_DisplayString(12,18,0,(unsigned char *)ReturnNoteLabel(RInst[Cur_Inst].Note));
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green);
						}
					}
					if (msgBuffer.buf[1] == 4) //move crosshair left
					{
					}
				}
				else if (P3Selection == 2)
				{
				   	if (msgBuffer.buf[1] == 0) //select bit hit
					{
						char toPr[7];
					 	P3Selection = 0;
						GLCD_SetBackColor(Black);
						GLCD_SetTextColor(Black);
						sprintf(toPr, "BPM: XX");
						GLCD_DisplayString(5,2,0,(unsigned char *)toPr);
						GLCD_DisplayString(12,5,0,(unsigned char *)"Select BPM: ");
						sprintf(toPr, "%2i", RInst[Cur_Inst].BPM);
						GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
						GLCD_SetBackColor(Black);
						GLCD_SetTextColor(Green);
	
						sprintf(toPr, "BPM: %i", RInst[Cur_Inst].BPM);
						GLCD_DisplayString(5,2,0,(unsigned char *)toPr);
	
						masterBuffer.length = 5;
						masterBuffer.buf[0] = 0x0C; //signifies comes from lcd thread BPM change
						masterBuffer.buf[1] = Cur_Inst+2;
						masterBuffer.buf[2] = RInst[Cur_Inst].InstrumentID;
						masterBuffer.buf[3] = RInst[Cur_Inst].Note;
						masterBuffer.buf[4] = RInst[Cur_Inst].BPM;
	
						if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
						}
					}
				 	if (msgBuffer.buf[1] == 1) //move crosshair up
					{
						if (RInst[Cur_Inst].BPM < 240)
						{
							char toPr[2];
							GLCD_SetTextColor(Black);
							sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
							GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
						 	RInst[Cur_Inst].BPM++;
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
							GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green);
						}
					}
					if (msgBuffer.buf[1] == 2) //move crosshair right
					{
					} 
					if (msgBuffer.buf[1] == 3) //move crosshair down
					{
						if (RInst[Cur_Inst].BPM > 0)
						{
							char toPr[2];
							GLCD_SetTextColor(Black);
							sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
							GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
						 	RInst[Cur_Inst].BPM--;
							GLCD_SetBackColor(Yellow);
							GLCD_SetTextColor(Red);
							sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
							GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
							GLCD_SetBackColor(Black);
							GLCD_SetTextColor(Green);
						}
					}
					if (msgBuffer.buf[1] == 4) //move crosshair left
					{
					}
				}
			}
		}	

   #elif JOYSTICK_MODE==1 //shawn's code goes here
		
		GLCD_SetTextColor(Black);
		//Starting code for handling the crosshair.
		if (msgBuffer.buf[1] == 0) //select bit hit
		{

		}
		if (msgBuffer.buf[1] == 1) //move crosshair up
		{
			GLCD_PutPixel(Cursor.x, Cursor.y-2);
			GLCD_PutPixel(Cursor.x, Cursor.y-1);
			GLCD_PutPixel(Cursor.x, Cursor.y);
			GLCD_PutPixel(Cursor.x, Cursor.y+1);
			GLCD_PutPixel(Cursor.x, Cursor.y+2);
			GLCD_PutPixel(Cursor.x-2, Cursor.y);
			GLCD_PutPixel(Cursor.x-1, Cursor.y);
			GLCD_PutPixel(Cursor.x+1, Cursor.y);
			GLCD_PutPixel(Cursor.x+2, Cursor.y);
			if (Cursor.y > 0)
				Cursor.y -= 4;
		}
		if (msgBuffer.buf[1] == 2) //move crosshair right
		{
			GLCD_PutPixel(Cursor.x, Cursor.y-2);
			GLCD_PutPixel(Cursor.x, Cursor.y-1);
			GLCD_PutPixel(Cursor.x, Cursor.y);
			GLCD_PutPixel(Cursor.x, Cursor.y+1);
			GLCD_PutPixel(Cursor.x, Cursor.y+2);
			GLCD_PutPixel(Cursor.x-2, Cursor.y);
			GLCD_PutPixel(Cursor.x-1, Cursor.y);
			GLCD_PutPixel(Cursor.x+1, Cursor.y);
			GLCD_PutPixel(Cursor.x+2, Cursor.y);
			if (Cursor.x < 320)
				Cursor.x += 4;
		}
		if (msgBuffer.buf[1] == 3) //move crosshair down
		{
			GLCD_PutPixel(Cursor.x, Cursor.y-2);
			GLCD_PutPixel(Cursor.x, Cursor.y-1);
			GLCD_PutPixel(Cursor.x, Cursor.y);
			GLCD_PutPixel(Cursor.x, Cursor.y+1);
			GLCD_PutPixel(Cursor.x, Cursor.y+2);
			GLCD_PutPixel(Cursor.x-2, Cursor.y);
			GLCD_PutPixel(Cursor.x-1, Cursor.y);
			GLCD_PutPixel(Cursor.x+1, Cursor.y);
			GLCD_PutPixel(Cursor.x+2, Cursor.y);
			if (Cursor.y < 240)
				Cursor.y += 4;
		}
		if (msgBuffer.buf[1] == 4) //move crosshair left
		{
			GLCD_PutPixel(Cursor.x, Cursor.y-2);
			GLCD_PutPixel(Cursor.x, Cursor.y-1);
			GLCD_PutPixel(Cursor.x, Cursor.y);
			GLCD_PutPixel(Cursor.x, Cursor.y+1);
			GLCD_PutPixel(Cursor.x, Cursor.y+2);
			GLCD_PutPixel(Cursor.x-2, Cursor.y);
			GLCD_PutPixel(Cursor.x-1, Cursor.y);
			GLCD_PutPixel(Cursor.x+1, Cursor.y);
			GLCD_PutPixel(Cursor.x+2, Cursor.y);
			if (Cursor.x > 0)
				Cursor.x -= 4;
		}
		GLCD_SetTextColor(Green);
		//handle crosshair
		GLCD_PutPixel(Cursor.x, Cursor.y-2);
		GLCD_PutPixel(Cursor.x, Cursor.y-1);
		GLCD_PutPixel(Cursor.x, Cursor.y);
		GLCD_PutPixel(Cursor.x, Cursor.y+1);
		GLCD_PutPixel(Cursor.x, Cursor.y+2);
		GLCD_PutPixel(Cursor.x-2, Cursor.y);
		GLCD_PutPixel(Cursor.x-1, Cursor.y);
		GLCD_PutPixel(Cursor.x+1, Cursor.y);
		GLCD_PutPixel(Cursor.x+2, Cursor.y);
	   	
		GLCD_Clear(Blue);
		unsigned short test = GLCD_ReadPixelColor(0,0, 0x00);
		unsigned char b[16];
		b[0] = (test & 0x8000) >> 15;
		b[1] = (test & 0x4000) >> 14;
		b[2] = (test & 0x2000) >> 13;
		b[3] = (test & 0x1000) >> 12;
		b[4] = (test & 0x0800) >> 11;
		b[5] = (test & 0x0400) >> 10;
		b[6] = (test & 0x0200) >> 9;
		b[7] = (test & 0x0100) >> 8;
		b[8] = (test & 0x0080) >> 7;
		b[9] = (test & 0x0040) >> 6;
		b[10] = (test & 0x0020) >> 5;
		b[11] = (test & 0x0010) >> 4;
		b[12] = (test & 0x0008) >> 3;
		b[13] = (test & 0x0004) >> 2;
		b[14] = (test & 0x0002) >> 1;
		b[15] = (test & 0x0001) >> 0;
		int a;
		for(a=0;a<16;a++)
		{
			if(b[a] == 1) GLCD_DisplayChar(0,a+1,0,'1');
			else GLCD_DisplayChar(0,a+1,0,'0');
		}

		int p = 0;
		for (p = 30; p < 35; p++)
		{

			unsigned short test = GLCD_ReadPixelColor(0,0, p);
			unsigned char b[16];
			b[0] = (test & 0x8000) >> 15;
			b[1] = (test & 0x4000) >> 14;
			b[2] = (test & 0x2000) >> 13;
			b[3] = (test & 0x1000) >> 12;
			b[4] = (test & 0x0800) >> 11;
			b[5] = (test & 0x0400) >> 10;
			b[6] = (test & 0x0200) >> 9;
			b[7] = (test & 0x0100) >> 8;
			b[8] = (test & 0x0080) >> 7;
			b[9] = (test & 0x0040) >> 6;
			b[10] = (test & 0x0020) >> 5;
			b[11] = (test & 0x0010) >> 4;
			b[12] = (test & 0x0008) >> 3;
			b[13] = (test & 0x0004) >> 2;
			b[14] = (test & 0x0002) >> 1;
			b[15] = (test & 0x0001) >> 0;
			int a;
			for(a=0;a<16;a++)
			{
				if(b[a] == 1) GLCD_DisplayChar(p-29,a+1,0,'1');
				else GLCD_DisplayChar(p-29,a+1,0,'0');
			}
		}
		   
		//GLCD_DisplayChar(7,5,0,b[1]);
		
	   /*
		GLCD_Clear(White);
		unsigned short temp = GLCD_ReadPixelColor(50, 50);
		GLCD_Clear(Black);
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(temp);
		GLCD_DisplayString(6,5,0,(unsigned char *)"Select Note: ");
		printf("Hello");  */

		/*
		int y = 0;
		for (y = 0; y < 240; y++) 
		{
		 	unsigned short temp = GLCD_ReadPixelColor(y, 1);
			if (temp == 0xFFE0)
				FlipBit(2);
			GLCD_SetTextColor(temp);
			//GLCD_PutPixel(10, 10);
			//GLCD_SetTextColor(Yellow);
			int x = 0;
			for (x = 2; x < 320; x++)
			{
			 	GLCD_PutPixel(x, y);
			}
		}
		
		//GLCD_SetTextColor(Blue);
		//GLCD_PutPixel(1,1);

		FlipBit(0);		*/
	#endif


#elif 	LCD_EXAMPLE_OP==2
		// wait for a message from another task telling us to send/recv over i2c
		
		
		if (xQueueReceive(lcdPtr->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		//FlipBit(0);
		
		//get the data from the message queue and reconvert it into a 10-bit value
		if (msgBuffer.buf[0] ==	0x09)
		{
			//unsigned int pixel = (msgBuffer.buf[1] << 24) | (msgBuffer.buf[2] << 16) | (msgBuffer.buf[3] << 8) | (msgBuffer.buf[4]);
			//int pixel = msgBuffer.buf[1];
			GLCD_SetBackColor(Yellow);
			GLCD_SetTextColor(Black);
			//if (msgBuffer.buf[4] > 255)
			//	FlipBit(5);

			char toprint[20];
			sprintf((char *)toprint, "%3.0i, %3.0i, %3.0i, %3.0i, %3.0i", msgBuffer.buf[1], msgBuffer.buf[2], msgBuffer.buf[3], msgBuffer.buf[4], msgBuffer.buf[10]);
			GLCD_DisplayString(1,2,0, (unsigned char*)&toprint);
			sprintf((char *)toprint, "%3.0i, %3.0i, %3.0i, %3.0i, %3.0i", msgBuffer.buf[6], msgBuffer.buf[7], msgBuffer.buf[8], msgBuffer.buf[9], msgBuffer.buf[5]);
			GLCD_DisplayString(3,2,0, (unsigned char*)&toprint);

		}

		if (msgBuffer.buf[0] ==	0x10)
		{
			//unsigned int pixel = (msgBuffer.buf[1] << 24) | (msgBuffer.buf[2] << 16) | (msgBuffer.buf[3] << 8) | (msgBuffer.buf[4]);
			//int pixel = msgBuffer.buf[1];
			GLCD_SetBackColor(Yellow);
			GLCD_SetTextColor(Black);
			//if (msgBuffer.buf[4] > 255)
			//	FlipBit(5);

			char toprint[20];
			sprintf((char *)toprint, "%3.0i, %3.0i, %3.0i, %3.0i, %3.0i", msgBuffer.buf[1], msgBuffer.buf[2], msgBuffer.buf[3], msgBuffer.buf[4], msgBuffer.buf[5]);
			GLCD_DisplayString(2,2,0, (unsigned char*)&toprint);
		}

		else if (msgBuffer.buf[0] == 0x06)
		{
		 	int pixel = msgBuffer.buf[1]<<8;
			pixel |= msgBuffer.buf[2];
			float temp = pixel;
			avg = /*avg + */temp / 300;
	
			GLCD_SetBackColor(Yellow);
			GLCD_SetTextColor(Black);
			if (msgBuffer.buf[3] == 0)
			{
				char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(0,2,0, (unsigned char*)&toprint);
			}
			else if (msgBuffer.buf[3] == 1)
			{
			 	char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(1,2,0, (unsigned char*)&toprint);
			} 
			else if (msgBuffer.buf[3] == 2)
			{
			  	char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(2,2,0, (unsigned char*)&toprint);
			} 
			else if (msgBuffer.buf[3] == 3)
			{
				char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(3,2,0, (unsigned char*)&toprint);
			} 
			else if (msgBuffer.buf[3] == 4)
			{
				char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(4,2,0, (unsigned char*)&toprint);
			} 
			else if (msgBuffer.buf[3] == 5)
			{
			 	char toprint[20];
				sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
				GLCD_DisplayString(5,2,0, (unsigned char*)&toprint);
			}
			
	
			//set minimum and maximum values should they change across the 80 data points in buffer
			if (pixel > max)
				max = pixel;
			if (pixel < min)
				min = pixel;
			//trim pixel data value to fit it on screen
			pixel = pixel / 5;
	
			//add pixel to a data buffer so we can clear the screen and keep accurate data
			if (msgBuffer.buf[3] < 6 && msgBuffer.buf[3] > 0)
			{
				GLCD_SetTextColor(Yellow);
	
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]);
	
				pixel_buffer[i] = pixel;
	
				GLCD_SetTextColor(Black);
	
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]);
	
				i++;
			}
	
			//i++;
	
			//once we have a filled buffer of data points, print the screen
			if (i > 79)
			{
				//GLCD_Clear(Yellow);
	
				//each data point is 4 pixels by 4 pixels
				//for (i = 0; i < 80; i++)
				//{
						/*GLCD_PutPixel(i*4, 240-pixel_buffer[i]);
						GLCD_PutPixel(i*4, 240-pixel_buffer[i]+1);
						GLCD_PutPixel(i*4, 240-pixel_buffer[i]+2);
						GLCD_PutPixel(i*4, 240-pixel_buffer[i]+3);*/
				
						/*GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]);
						GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+1);
						GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+2);
						GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+3);*/
				
						/*GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]);
						GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+1);
						GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+2);
						GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+3);*/
				
						/*GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]);
						GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+1);
						GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+2);
						GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+3);*/
				//}
				//GLCD_SetBackColor(Yellow);
				//GLCD_SetTextColor(Black);
				//convert max and mins to voltages
				max = max / 300;
				min = min / 300;
				avg = avg / 80;
	
				//display the max and min voltages on the screen
				//char toprint[20];
				//sprintf((char *)toprint, "val: %7.1f", avg);
				//GLCD_DisplayString(2,9,0, (unsigned char*)&toprint);
				//sprintf((char *)toprint, "%1.1fV", min);
				//GLCD_DisplayString(9,9,1, (unsigned char*)&toprint);
	
				//display the scalar on the screen
				//GLCD_DisplayString(9,0,1, (unsigned char *)"0V");
				//GLCD_DisplayString(7,0,1, (unsigned char *)"1V");
				//GLCD_DisplayString(5,0,1, (unsigned char *)"2V");
				//GLCD_DisplayString(3,0,1, (unsigned char *)"3V");
	
				//reset values for the next buffer screen
				i = 0;
				max = 0;
				min = 990;
				avg = 0;
			}
		}
#endif	
	}
}

