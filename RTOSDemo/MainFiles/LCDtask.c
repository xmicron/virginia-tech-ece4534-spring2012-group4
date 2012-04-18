/*this section is reserved for user interface function call references

NOTE: These functions are not fail-safe. Send them parameters OTHER than what's defined in them and the results 
may vary. Be careful with how you use the functions.

ReturnInstrumentLabel(int instrumentID) - send it a number 0 through 128 and it returns a character string of the instrument name
			anything other than 0 through 128 not guaranteed to work

ReturnInstrumentCategoryLabel(int categoryID) - send it a number 0 through 15 to return a category string label
		0 - Piano	1 - Chromatic Percussion	2 - Organ	3 - Guitar	4 - Bass	5 - Strings		6 - Ensemble
		7 - Brass	8 - Reed		9 - Pipe	10 - Synth Lead		11 - Synth Pad		12 - Synth Effects
		13 - Ethnic		14 - Percussive		15 - Sound Effects

ReturnRepeatingLabel(int labelID) - returns a string for the repeating instrument page. 
	0 returns "Done"
	1 returns "Change Instrument"
	2 returns "Change Note"
	3 returns "Change BPM"

ReturnNoteLabel (int index) - returns a string for the note. values range from 0 to 8

ClearOldSelection(int panel) - erases highlight on the home page based on the panel number. values range from 0 to 5 for 
each of the 6 panelson that page

MakeSelection (int panel) - highlights a panel based on the panel number passed.

P2SelectionClear(int Cur_Panel, int mult) - unselects an instrument on page 2 based on it's index (Cur_Panel) and the multiplier.
		The multiplier is used to index through the 128 instruments to be displayed. 

void P2MakeSelection(int Cur_Panel, int mult) - selects the new instrument to highlight on page two based on its index (Cur_Panel)
	and the multiplier. The multiplier is used to index through the 128 instruments properly

void P1CatSelectionClear(int Cur_Panel) - clears the category selection on page 1

void P1CatMakeSelection(int Cur_Panel) - sets a new selection on page 1

void P3CatSelectionClear(Cur_Panel) - clears the category selection on page 3

void P3CatMakeSelection(Cur_Panel) - sets new category selection on page 3

void Set_Slider( int old_Pos, int SLIDER ) - used to change the slider value on the main page of the user interface.
	the first parameter is the old position and the second is the new position. Values can only be between 0 and 10
	
	You must set the first parameter to properly erase the sliders old position otherwise you'll get a lot of sliders 
	appearing on the screen.

Panel_3_Highlight(int highlight) - highlight a selection on panel three of the main page. must be 0 or 1
	automatically unhighlights the other selection.

Panel_3_Select(int selection, int VOLUME) - moves the highlight to the appropriate section
	0 makes the volume highlight 
	1 makes the brightness text turn blue to show that it is selected

Panel_3_Finish(int selection, int VOLUME) - used to finish panel 3 editing by resetting all of the text to green and black
	and displaying the final volume paramter.
	
InitPage(int pageNum, int VOLUME, int SLIDER, InstrumentStruct I1, 
	InstrumentStruct I2, RepeatingInstrumentStruct R1, 
	RepeatingInstrumentStruct R2, RepeatingInstrumentStruct R3, 
	int index) - used to initialize a page. In order to properly display all information, you must send all of the instrument 
		data structures, the volume, and the slider value. The very last parameter is the index. It's only used on page 2 and 3

			On page two it is used as a multiplier to display the appropriate instruments based on the category selected. 
				it should be 0 for piano, 1 for chromatic percussion, etc.
			On page three it is used to index which repeating instrument is being edited.
			For any other page, for safety keep this parameter as 0.
*/
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
#define JOYSTICK_MODE 0
// If INSTEON_MODE == 0, Insteon disabled
// If INSTEON_MODE == 1, Insteon Enabled
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
	//user-interface variables follow

	//state machine variables
	int Cur_Panel = 0; 	//currently highlighted selection of the LCD screen. 
	int Cur_Page = 0;	//currently viewed page of the LCD screen
	int Cur_Inst = 0; 	//current instrument being modified when on another page. Used to index the proper instrument in the 
						//instrument arrays.
	int RorP = 0; 		//boolean to identify whether the current instrument from above is a part of the repeating or player instruments

	int P2SelectionMultiplier = 0;//multiplier to ensure page two shows the right instruments to select. 
	
	int P1Selection = 0;			//used to move from panel 3 modes. If 0, then not in panel 3 mode. If 1, then in panel 3 mode.
									//panel 3 mode locks the joystick to select either brightness or volume on panel 3
	int P3Selection = 0; 			//works similarly to P1 Selection except it locks page 3 to either note or BPM selecting. 
	//end state machine variables


	//local variables stored follow
	int VOLUME = 100;				//local volume variable.
	int SLIDER = 10;				//local brightness slider variable. 

	InstrumentStruct Inst[2];
	RepeatingInstrumentStruct RInst[3];
	//initialize all of the instruments on start-up
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
	
	//initialize crosshair values
	cursorPos Cursor;
	Cursor.x = 0;
	Cursor.y = 0;

#endif
	//initialize pointers to message queues and data structures for message passing
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
		

   #if JOYSTICK_MODE==0 // Joey's mode for the hightlight joystick function.
		
		if (msgBuffer.buf[0] = 0x11) //joystick message from the joystick thread
		{
			FlipBit(0);
			if (Cur_Page == 0)
			{
				if (P1Selection == 0) //state machine variable - if we are not currently in the volume/brightness mode - free to highlight 
								//panels on main page mode.
				{
					if (msgBuffer.buf[1] == 0) //select bit hit
					{
						if (Cur_Panel > 3)
						{
							Cur_Page = 1;	//set new current page variable
							Cur_Inst = Cur_Panel-4;	//index it to 0 or 1 depending on which is selected
							RorP = 0;//0 means it's a player instrument
							InitPage(1, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);//initialize page 1
							Cur_Panel = 0;//sets the current selection to index 0 on page 1
						}
						else if (Cur_Panel == 3)
						{
							P1Selection = 1;//enter volume/brightness selection mode. 
							Cur_Panel = 0;
							Panel_3_Highlight(Cur_Panel);//send 0 to highlight volume, 1 to highlight the brightness setting
														//automatically un highlights the other selection
						}
						else if (Cur_Panel < 3)
						{
							Cur_Page = 3;//page 3 is the repeating instrument page
							Cur_Inst = Cur_Panel;//0, 1, or 2
							P3Selection = 0; //not in note or BPM selection mode
							RorP = 1;	 //1 for repeating instrument, so the functions know

							//initialize page 3. Send all the instrument variables. Final parameter tells it which
							//instrument we are actually displaying. 
							InitPage(3, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], Cur_Inst);

							Cur_Panel = 0;//set the current selection on this page to 0 (Which will be the "done" text)
						}
					}
				 	if (msgBuffer.buf[1] == 1) //move crosshair up
					{
						
						if (Cur_Panel > 0)
						{
							ClearOldSelection(Cur_Panel);//unhighlight 
						 	Cur_Panel--;			 //increment current panel
							MakeSelection(Cur_Panel); //highlight this new panel
						}
					}
					if (msgBuffer.buf[1] == 2) //move crosshair right
					{
						if (INSTEON_MODE == 1)
						{
	
						}
						if (Cur_Panel < 3)
						{
							ClearOldSelection(Cur_Panel); //unhighlight
							Cur_Panel = Cur_Panel + 3; //increment by 3 because of the UI layout. incrementing by 3 means move right
							MakeSelection(Cur_Panel); //highlight the new selection
						}
					} 
					if (msgBuffer.buf[1] == 3) //move crosshair down
					{
						if (Cur_Panel < 5)
						{
							ClearOldSelection(Cur_Panel); //unhighlight
						 	Cur_Panel++;			   //increment
							MakeSelection(Cur_Panel);	  //make new panel selection
						}
					}
					if (msgBuffer.buf[1] == 4) //move crosshair left
					{
						if (Cur_Panel > 3)
						{
							ClearOldSelection(Cur_Panel); //unhighlight
							Cur_Panel = Cur_Panel - 3;	  //decrement by 3 to move panel left
							MakeSelection(Cur_Panel);	  //make new highlight
						}
						else if (Cur_Panel == 3)
						{
							ClearOldSelection(Cur_Panel); //same as above
							Cur_Panel = 0;
							MakeSelection(Cur_Panel);
						}
					}
			 	}
				else if (P1Selection == 1)
				{
					if (msgBuffer.buf[1] == 0)
					{
						Panel_3_Select(Cur_Panel, VOLUME);//makes a selection of either volume or brightness depending on the panel - must be 0 or 1
												//0 for volume, 1 for brightness
					  	if (Cur_Panel == 0)
						{
							P1Selection = 2;//set state machine to volume select 	
						}
						else if (Cur_Panel == 1)
						{
							P1Selection = 3;//set state machine to brightness select
						}
					}
					else if (msgBuffer.buf[1] == 1)
					{
					 	if (Cur_Panel > 0)//if brightness currently hightlighted (Cur_Panel == 1)
						{
							Cur_Panel--; //make Cur_Panel =0
							Panel_3_Highlight(Cur_Panel);//send 0 to highlight volume, 1 to highlight the brightness setting
														//automatically un highlights the other selection 
						}
					}
					else if (msgBuffer.buf[1] == 3)	//if volume currently highlighted   (Cur_Panel == 0)
					{
						if (Cur_Panel < 1)
						{
							Cur_Panel++;//make Cur_Panel =1
							Panel_3_Highlight(Cur_Panel);//send 0 to highlight volume, 1 to highlight the brightness setting
														//automatically un highlights the other selection 
						}
					}
				}
				else if (P1Selection == 2)//we are currently in volume altering mode
				{
				 	if (msgBuffer.buf[1] == 0)
					{
						Panel_3_Finish(Cur_Panel, VOLUME); //returns current panel to green and black text

					  	P1Selection = 0;//return state machine to normal
						Cur_Panel = 3;	 //keep current panel selection
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
//						Panel_3_Finish(Cur_Panel); //reset text color to green and black
					 	P1Selection = 0;//return state machine to non-volume/brightness mode
						Cur_Panel = 3;//keep current panel to 3
					}
					else if (msgBuffer.buf[1] == 2)
					{ 
					  	if (SLIDER < 10)
						{
							SLIDER++;
							Set_Slider(SLIDER-1, SLIDER);//move slider. first parameter is old position, second is new position

							i2cBuffer.buf[0] = 0x13;
							i2cBuffer.buf[1] = (SLIDER*25) + 5;
	
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
							}
						}
					}
					else if (msgBuffer.buf[1] == 4)
					{
					 	if (SLIDER > 0)
						{
							SLIDER--;
							Set_Slider(SLIDER+1, SLIDER);//move slider. first parameter is old position, second is new position

							i2cBuffer.buf[0] = 0x13;
							i2cBuffer.buf[1] = (SLIDER*25) + 5;
	
							if (xQueueSend(i2cQ->inQ,(void *) (&i2cBuffer),portMAX_DELAY) != pdTRUE) {  
							VT_HANDLE_FATAL_ERROR(0);
							}
						}
					}

				}
			}
			else if (Cur_Page == 1)//we are currently changing an instrument
			{
				if (msgBuffer.buf[1] == 0) //select bit hit
				{
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
					{
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
							Cur_Page = 0;
							//P2SelectionMultiplier = Cur_Panel;
							InitPage(0, VOLUME, SLIDER, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
							Cur_Panel = 0;
						}
						else if (Cur_Panel == 1)
						{
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

