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

#define JOYSTICK_MODE 0

// Set the task up to run every 200 ms
#define lcdWRITE_RATE_BASE	( ( portTickType ) 10 )

/* The LCD task. */
static portTASK_FUNCTION_PROTO( vLCDUpdateTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartLCDTask( unsigned portBASE_TYPE uxPriority,vtLCDStruct *ptr )
{

	// Create the queue that will be used to talk to the LCD
	if ((ptr->inQ = xQueueCreate(vtLCDQLen,sizeof(vtLCDMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	/* Start the task */
	portBASE_TYPE retval;
	if ((retval = xTaskCreate( vLCDUpdateTask, ( signed char * ) "LCD", lcdSTACK_SIZE, (void*)ptr, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

// If LCD_EXAMPLE_OP=0, then repeatedly write text
// If LCD_EXAMPLE_OP=1, then do a rotating ARM bitmap display
// If LCD_EXAMPLE_OP=2, then receive from a message queue and print the contents to the screen
#define LCD_EXAMPLE_OP 3
#if LCD_EXAMPLE_OP==1
// This include the file with the definition of the ARM bitmap
#include "ARM_Ani_16bpp.c"
#elif LCD_EXAMPLE_OP==3
typedef struct __cursorPos {
	uint8_t x;
	uint8_t y;
} cursorPos;
#endif

// Convert from HSL colormap to RGB values in this weird colormap
// H: 0 to 360
// S: 0 to 1
// L: 0 to 1
// The LCD has a funky bitmap.  Each pixel is 16 bits (a "short unsigned int")
//   Red is the most significant 5 bits
//   Blue is the least significant 5 bits
//   Green is the middle 6 bits
static unsigned short hsl2rgb(float H,float S,float L)
{
	float C = (1.0 - fabs(2.0*L-1.0))*S;
	float Hprime = H / 60;
	unsigned short t = Hprime / 2.0;
	t *= 2;
	float X = C * (1-abs((Hprime - t) - 1));
	unsigned short truncHprime = Hprime;
	float R1, G1, B1;

	switch(truncHprime) {
		case 0: {
			R1 = C; G1 = X; B1 = 0;
			break;
		}
		case 1: {
			R1 = X; G1 = C; B1 = 0;
			break;
		}
		case 2: {
			R1 = 0; G1 = C; B1 = X;
			break;
		}
		case 3: {
			R1 = 0; G1 = X; B1 = C;
			break;
		}
		case 4: {
			R1 = X; G1 = 0; B1 = C;
			break;
		}
		case 5: {
			R1 = C; G1 = 0; B1 = X;
			break;
		}
		default: {
			// make the compiler stop generating warnings
			R1 = 0; G1 = 0; B1 = 0;
			VT_HANDLE_FATAL_ERROR(Hprime);
			break;
		}
	}
	float m = L - 0.5*C;
	R1 += m; G1 += m; B1 += m;
	unsigned short red = R1*32; if (red > 31) red = 31;
	unsigned short green = G1*64; if (green > 63) green = 63;
	unsigned short blue = B1*32; if (blue > 31) blue = 31;
	unsigned short color = (red << 11) | (green << 5) | blue;
	return(color); 
}

// This is the actual task that is run
static portTASK_FUNCTION( vLCDUpdateTask, pvParameters )
{
	portTickType xUpdateRate, xLastUpdateTime;
	int pixel_buffer[80];
	float max = 0;
	float min = 990;
	unsigned short screenColor;
	#if LCD_EXAMPLE_OP==0
	unsigned short tscr;
	static char scrString[40];
	unsigned char curLine = 0;
	float hue=0, sat=0.2, light=0.2;
	
	
	
	#elif LCD_EXAMPLE_OP==1
	unsigned char picIndex = 0;
	
	
	
	#elif LCD_EXAMPLE_OP==2
	vtLCDMsg msgBuffer;
	unsigned char curLine = 0;
	
	
	
	#elif LCD_EXAMPLE_OP==3
	vtLCDMsg msgBuffer;
	#if JOYSTICK_MODE==0
	int Cur_Panel = 0;

	#elif JOYSTICK_MODE==1 //shawn's initializations go here
	cursorPos Cursor;
	Cursor.x = 0;
	Cursor.y = 0;
	#endif
	
	
	
	#endif
	vtLCDStruct *lcdPtr = (vtLCDStruct *) pvParameters;

	/* Initialize the LCD */
	GLCD_Init();
	GLCD_Clear(Black);

	// Scale the update rate to ensure it really is in ms
	xUpdateRate = lcdWRITE_RATE_BASE / portTICK_RATE_MS;

	/* We need to initialise xLastUpdateTime prior to the first call to 
	vTaskDelayUntil(). */
	xLastUpdateTime = xTaskGetTickCount();
	// Note that srand() & rand() require the use of malloc() and should not be used unless you are using
	//   MALLOC_VERSION==1
	#if MALLOC_VERSION==1
	srand((unsigned) 55); // initialize the random number generator to the same seed for repeatability
	#endif
	// Like all good tasks, this should never exit

	GLCD_SetTextColor(Green);
	GLCD_SetBackColor(Black);

	#if JOYSTICK_MODE==0
	int a = 0;
	for (a = 0; a < 180; a++)
	{
		GLCD_PutPixel(a, 80);
		GLCD_PutPixel(a, 81);
		GLCD_PutPixel(a, 160);
		GLCD_PutPixel(a, 161);
	}
	for (a = 0; a < 240; a++)
	{
		GLCD_PutPixel(180, a);
		GLCD_PutPixel(181, a);
	}
	for (a = 180; a < 320; a++)
	{
		GLCD_PutPixel(a, 60);
		GLCD_PutPixel(a, 61);
		GLCD_PutPixel(a, 120);
		GLCD_PutPixel(a, 121);
		GLCD_PutPixel(a, 180);
		GLCD_PutPixel(a, 181);
	}
	GLCD_SetTextColor(Yellow);
	for (a = 0; a < 180; a++)
	{
		GLCD_PutPixel(a, 0);
		GLCD_PutPixel(a, 1);
		GLCD_PutPixel(a, 78);
		GLCD_PutPixel(a, 79);
	}
	for (a = 0; a < 80; a++)
	{
		GLCD_PutPixel(0, a);
		GLCD_PutPixel(1, a);
		GLCD_PutPixel(178, a);
		GLCD_PutPixel(179, a);
	}
	GLCD_SetTextColor(Green);

	GLCD_DisplayString(1,1,0,(unsigned char *)"Player Instrument 1");
	GLCD_DisplayString(11,1,0,(unsigned char *)"Player Instrument 2");
	GLCD_DisplayString(21,1,0,(unsigned char *)"Player Instrument 3");

	GLCD_DisplayString(2,5,0,(unsigned char *)"<none selected>");
	GLCD_DisplayString(12,5,0,(unsigned char *)"<none selected>");
	GLCD_DisplayString(22,5,0,(unsigned char *)"<none selected>");

	GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume");
	GLCD_DisplayString(2,42,0,(unsigned char *)"0");

	GLCD_DisplayString(9,31,0,(unsigned char *)"Repeating Instrument 1");
	GLCD_DisplayString(10,32,0,(unsigned char *)"Inst: <none selected>");
	GLCD_DisplayString(11,32,0,(unsigned char *)"Note: <none selected>");
	GLCD_DisplayString(12,32,0,(unsigned char *)"BPM: 0");

	GLCD_DisplayString(16,31,0,(unsigned char *)"Repeating Instrument 1");
	GLCD_DisplayString(17,32,0,(unsigned char *)"Inst: <none selected>");
	GLCD_DisplayString(18,32,0,(unsigned char *)"Note: <none selected>");
	GLCD_DisplayString(19,32,0,(unsigned char *)"BPM: 0");

	GLCD_DisplayString(24,31,0,(unsigned char *)"Repeating Instrument 1");
	GLCD_DisplayString(25,32,0,(unsigned char *)"Inst: <none selected>");
	GLCD_DisplayString(26,32,0,(unsigned char *)"Note: <none selected>");
	GLCD_DisplayString(27,32,0,(unsigned char *)"BPM: 0");

	//GLCD_DisplayString(8,30,0,(unsigned char *)"Repeating Instrument 1");
	#elif JOYSTICK_MODE==1//shawn's LCD screen initializations
	#endif
	int i = 0;

	


	for(;;)
	{	
#if LCD_EXAMPLE_OP==0
		/* Ask the RTOS to delay reschduling this task for the specified time */
		vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );

		// Find a new color for the screen by randomly (within limits) selecting HSL values
		#if MALLOC_VERSION==1
		hue = rand() % 360;
		sat = (rand() % 1024) / 1023.0; sat = sat * 0.5; sat += 0.5;
		light = (rand() % 1024) / 1023.0;	light = light * 0.8; light += 0.10;
		#else
		hue = (hue + 1); if (hue >= 360) hue = 0;
		sat+=0.01; if (sat > 1.0) sat = 0.20;
		light+=0.03; if (light > 1.0) light = 0.20;
		#endif
		screenColor = hsl2rgb(hue,sat,light);
		// Now choose a complementary value for the text color
		hue += 180;
		if (hue >= 360) hue -= 360;
		tscr = hsl2rgb(hue,sat,light);
		GLCD_SetTextColor(tscr);
		GLCD_SetBackColor(screenColor);
		/* create a string for printing to the LCD */
		sprintf(scrString,"LCD: %d %d %d",screenColor,(int)xUpdateRate,(int)xLastUpdateTime);
		GLCD_ClearLn(curLine,1);
		GLCD_DisplayString(curLine,0,1,(unsigned char *)scrString);
		curLine++;
		if (curLine == 10) {
			/* clear the LCD at the end of the screen */
			GLCD_Clear(screenColor);
			curLine = 0;
		}

		printf("BackColor (RGB): %d %d %d - (HSL) %3.0f %3.2f %3.2f \n",
			(screenColor & 0xF800) >> 11,(screenColor & 0x07E0) >> 5,screenColor & 0x001F,
			hue,sat,light);
		printf("TextColor (RGB): %d %d %d\n",
			(tscr & 0xF800) >> 11,(tscr & 0x07E0) >> 5,tscr & 0x001F);

		// Here is a way to do debugging output via the built-in hardware -- it requires the ULINK cable and the
		//   debugger in the Keil tools to be connected.  You can view PORT0 output in the "Debug(printf) Viewer"
		//   under "View->Serial Windows".  You have to enable "Trace" and "Port0" in the Debug setup options.  This
		//   should not be used if you are using Port0 for printf()
		// There are 31 other ports and their output (and port 0's) can be seen in the "View->Trace->Records"
		//   windows.  You have to enable the prots in the Debug setup options.  Note that unlike ITM_SendChar()
		//   this "raw" port write is not blocking.  That means it can overrun the capability of the system to record
		//   the trace events if you go too quickly; that won't hurt anything or change the program execution and
		//   you can tell if it happens because the "View->Trace->Records" window will show there was an overrun.
		vtITMu16(vtITMPortLCD,screenColor);

#elif 	LCD_EXAMPLE_OP==1
		/* Ask the RTOS to delay reschduling this task for the specified time */
		vTaskDelayUntil( &xLastUpdateTime, xUpdateRate );
  		/* go through a  bitmap that is really a series of bitmaps */
		picIndex = (picIndex + 1) % 9;
		GLCD_Bmp(99,99,120,45,(unsigned char *) &ARM_Ani_16bpp[picIndex*(120*45*2)]);*/



#elif	LCD_EXAMPLE_OP==3					//this will be our UI mode OP
		
		
		if (xQueueReceive(lcdPtr->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) //receive message from message queue
		{
			VT_HANDLE_FATAL_ERROR(0);
		}
		if (msgBuffer.buf[0] == 2)
		{
		   	uint8_t ulCurrentState = GPIO2->FIOPIN;
			if( ulCurrentState & 0x08 )
			{
				GPIO2->FIOCLR = 0x08;
			}
			else
			{
				GPIO2->FIOSET = 0x08;
			}
		}
		

   #if JOYSTICK_MODE==0
		
		
   		
		//clear cur selection
		if (msgBuffer.buf[0] == 0 || msgBuffer.buf[0] == 1 || msgBuffer.buf[0] == 2 || msgBuffer.buf[0] == 3 || msgBuffer.buf[0] == 4 )
		{
			if (Cur_Panel == 0)
			{
				GLCD_SetTextColor(Black);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 0);
					GLCD_PutPixel(a, 1);
					GLCD_PutPixel(a, 78);
					GLCD_PutPixel(a, 79);
				}
				for (a = 0; a < 80; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);	
			}
			else if (Cur_Panel == 1)
			{
			 	GLCD_SetTextColor(Black);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 82);
					GLCD_PutPixel(a, 83);
					GLCD_PutPixel(a, 158);
					GLCD_PutPixel(a, 159);
				}
				for (a = 82; a < 160; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 2)
			{
				GLCD_SetTextColor(Black);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 162);
					GLCD_PutPixel(a, 163);
					GLCD_PutPixel(a, 238);
					GLCD_PutPixel(a, 239);
				}
				for (a = 162; a < 240; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 3)
			{
			 	GLCD_SetTextColor(Black);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 0);
					GLCD_PutPixel(a, 1);
					GLCD_PutPixel(a, 58);
					GLCD_PutPixel(a, 59);
				}
				for (a = 0; a < 60; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 4)
			{
			 	GLCD_SetTextColor(Black);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 62);
					GLCD_PutPixel(a, 63);
					GLCD_PutPixel(a, 118);
					GLCD_PutPixel(a, 119);
				}
				for (a = 62; a < 120; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 5)
			{
			  	GLCD_SetTextColor(Black);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 122);
					GLCD_PutPixel(a, 123);
					GLCD_PutPixel(a, 178);
					GLCD_PutPixel(a, 179);
				}
				for (a = 122; a < 180; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 6)
			{
			  	GLCD_SetTextColor(Black);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 182);
					GLCD_PutPixel(a, 183);
					GLCD_PutPixel(a, 238);
					GLCD_PutPixel(a, 239);
				}
				for (a = 182; a < 240; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
		}



		if (msgBuffer.buf[0] == 0) //select bit hit
		{

		}
		if (msgBuffer.buf[0] == 1) //move crosshair up
		{
			if (Cur_Panel > 0)
			{
			 	Cur_Panel--;
			}
		}
		if (msgBuffer.buf[0] == 2) //move crosshair right
		{
			if (Cur_Panel < 3)
				Cur_Panel = Cur_Panel + 4;
		} 
		if (msgBuffer.buf[0] == 3) //move crosshair down
		{
			if (Cur_Panel < 7)
			{
			 	Cur_Panel++;
			}
		}
		if (msgBuffer.buf[0] == 4) //move crosshair left
		{
			if (Cur_Panel > 3)
				Cur_Panel = Cur_Panel - 4;
		}
		
		if (msgBuffer.buf[0] == 0 || msgBuffer.buf[0] == 1 || msgBuffer.buf[0] == 2 || msgBuffer.buf[0] == 3 || msgBuffer.buf[0] == 4 )
		{
			if (Cur_Panel == 0)
			{
				GLCD_SetTextColor(Yellow);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 0);
					GLCD_PutPixel(a, 1);
					GLCD_PutPixel(a, 78);
					GLCD_PutPixel(a, 79);
				}
				for (a = 0; a < 80; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);	
			}
			else if (Cur_Panel == 1)
			{
			 	GLCD_SetTextColor(Yellow);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 82);
					GLCD_PutPixel(a, 83);
					GLCD_PutPixel(a, 158);
					GLCD_PutPixel(a, 159);
				}
				for (a = 82; a < 160; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 2)
			{
				GLCD_SetTextColor(Yellow);
				for (a = 0; a < 180; a++)
				{
					GLCD_PutPixel(a, 162);
					GLCD_PutPixel(a, 163);
					GLCD_PutPixel(a, 238);
					GLCD_PutPixel(a, 239);
				}
				for (a = 162; a < 240; a++)
				{
					GLCD_PutPixel(0, a);
					GLCD_PutPixel(1, a);
					GLCD_PutPixel(178, a);
					GLCD_PutPixel(179, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 3)
			{
			 	GLCD_SetTextColor(Yellow);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 0);
					GLCD_PutPixel(a, 1);
					GLCD_PutPixel(a, 58);
					GLCD_PutPixel(a, 59);
				}
				for (a = 0; a < 60; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 4)
			{
			 	GLCD_SetTextColor(Yellow);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 62);
					GLCD_PutPixel(a, 63);
					GLCD_PutPixel(a, 118);
					GLCD_PutPixel(a, 119);
				}
				for (a = 62; a < 120; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 5)
			{
			  	GLCD_SetTextColor(Yellow);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 122);
					GLCD_PutPixel(a, 123);
					GLCD_PutPixel(a, 178);
					GLCD_PutPixel(a, 179);
				}
				for (a = 122; a < 180; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
			else if (Cur_Panel == 6)
			{
			  	GLCD_SetTextColor(Yellow);
				for (a = 182; a < 320; a++)
				{
					GLCD_PutPixel(a, 182);
					GLCD_PutPixel(a, 183);
					GLCD_PutPixel(a, 238);
					GLCD_PutPixel(a, 239);
				}
				for (a = 182; a < 240; a++)
				{
					GLCD_PutPixel(182, a);
					GLCD_PutPixel(183, a);
					GLCD_PutPixel(318, a);
					GLCD_PutPixel(319, a);
				}
				GLCD_SetTextColor(Green);
			}
		}

   #elif JOYSTICK_MODE==1 //shawn's code goes here
		

		//Starting code for handling the crosshair. Pretty much works
		if (msgBuffer.buf[0] == 0) //select bit hit
		{

		}
		if (msgBuffer.buf[0] == 1) //move crosshair up
		{
			if (Cursor.y > 0)
				Cursor.y--;
		}
		if (msgBuffer.buf[0] == 2) //move crosshair right
		{
			if (Cursor.x < 320)
				Cursor.x++;
		}
		if (msgBuffer.buf[0] == 3) //move crosshair down
		{
			if (Cursor.y < 240)
				Cursor.y++;
		}
		if (msgBuffer.buf[0] == 4) //move crosshair left
		{
			if (Cursor.x > 0)
				Cursor.x--;
		}

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
	#endif


#elif 	LCD_EXAMPLE_OP==2
		// wait for a message from another task telling us to send/recv over i2c
		
		if (xQueueReceive(lcdPtr->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		
		//get the data from the message queue and reconvert it into a 10-bit value
		int pixel = msgBuffer.buf[1]<<8;
		pixel = pixel + msgBuffer.buf[0];

		//set minimum and maximum values should they change across the 80 data points in buffer
		if (pixel > max)
			max = pixel;
		if (pixel < min)
			min = pixel;
		//trim pixel data value to fit it on screen
		pixel = pixel / 5;

		//add pixel to a data buffer so we can clear the screen and keep accurate data
		pixel_buffer[i] = pixel;
		
		//increment for next position in the buffer
		i++;

		//once we have a filled buffer of data points, print the screen
		if (i > 79)
		{
			GLCD_Clear(Yellow);

			//each data point is 4 pixels by 4 pixels
			for (i = 0; i < 80; i++)
			{
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]+1);
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]+2);
				GLCD_PutPixel(i*4, 240-pixel_buffer[i]+3);
		
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+1);
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+2);
				GLCD_PutPixel(i*4+1, 240-pixel_buffer[i]+3);
		
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+1);
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+2);
				GLCD_PutPixel(i*4+2, 240-pixel_buffer[i]+3);
		
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]);
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+1);
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+2);
				GLCD_PutPixel(i*4+3, 240-pixel_buffer[i]+3);
			}

			//convert max and mins to voltages
			/*max = max / 300;
			min = min / 300;

			//display the max and min voltages on the screen
			uint8_t toprint[4];
			sprintf((char *)toprint, "%1.1fV", max);
			GLCD_DisplayString(0,9,1, (unsigned char*)&toprint);
			sprintf((char *)toprint, "%1.1fV", min);
			GLCD_DisplayString(9,9,1, (unsigned char*)&toprint);

			//display the scalar on the screen
			GLCD_DisplayString(9,0,1, (unsigned char *)"0V");
			GLCD_DisplayString(7,0,1, (unsigned char *)"1V");
			GLCD_DisplayString(5,0,1, (unsigned char *)"2V");
			GLCD_DisplayString(3,0,1, (unsigned char *)"3V");

			//reset values for the next buffer screen*/
			i = 0;
			max = 0;
			min = 990;
		}
#else
		Bad setting
#endif	
	}
}

