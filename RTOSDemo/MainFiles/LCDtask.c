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

unsigned char * ReturnInstrumentLabel(int index)
{
 	if (index == 0)
	{
		return "<none selected>";
	}
	else if (index == 1)
	{
		return "Acoustic Grand Piano";
	}
	else if (index == 2)
	{
		return "Bright Acoustic Piano";
	}
	else if (index == 3)
	{
		return "Electric Grand Piano";
	}
	else if (index == 4)
	{
	  	return "Honky-tonk Piano";
	}
	else if (index == 5)
	{
	  	return "Electric Piano 1";
	}
	else if (index == 6)
	{
	  	return "Electric Piano 2";
	}
	else if (index == 7)
	{
	  	return "Harpsichord";
	}
	else if (index == 8)
	{
	  	return "Clavi";
	}
	else if (index == 9)
	{
	  	return "Celesta";
	}
	else if (index == 10)
	{
	  	return "Glockenspiel";
	}
	else if (index == 11)
	{
	  	return "Music Box";
	}
	else if (index == 12)
	{
	  	return "Vibraphone";
	}
	else if (index == 13)
	{
	  	return "Marimba";
	}
	else if (index == 14)
	{
	  	return "Xylophone";
	}
	else if (index == 15)
	{
	  	return "Tubular Bells";
	}
	else if (index == 16)
	{
	  	return "Dulcimer";
	}
	else if (index == 17)
	{
	  	return "Drawbar Organ";
	}
	else if (index == 18)
	{
	  	return "Percussive Organ";
	}
	else if (index == 19)
	{
	  	return "Rock Organ";
	}
	else if (index == 20)
	{
	  	return "Church Organ";
	}
	else if (index == 21)
	{
	  	return "Reed Organ";
	}
	else if (index == 22)
	{
	  	return "Accordion";
	}
	else if (index == 23)
	{
	  	return "Harmonica";
	}
	else if (index == 24)
	{
	  	return "Tango Accordian";
	}
	else if (index == 25)
	{
	  	return "Acoustic Guitar (nylon)";
	}
	else if (index == 26)
	{
	  	return "Acoustic Guitar (steel)";
	}
	else if (index == 27)
	{
	  	return "Electric Guitar (jazz)";
	}
	else if (index == 28)
	{
	  	return "Electric Guitar (clean)";
	}
	else if (index == 29)
	{
	  	return "Electric Guitar (muted)";
	}
	else if (index == 30)
	{
	  	return "Overdriven Guitar";
	}
	else if (index == 31)
	{
	  	return "Distortion Guitar";
	}
	else if (index == 32)
	{
	  	return "Guitar Harmonics";
	}
	else if (index == 33)
	{
	  	return "Acoustic Bass";
	}
	else if (index == 34)
	{
	  	return "Electric Bass (finger)";
	}
	else if (index == 35)
	{
	  	return "Electric Bass (pick)";
	}
	else if (index == 36)
	{
	  	return "Fretless Bass";
	}
	else if (index == 37)
	{
	  	return "Slap Bass 1";
	}
	else if (index == 38)
	{
	  	return "Slap Bass 2";
	}
	else if (index == 39)
	{
	  	return "Synth Bass 1";
	}
	else if (index == 40)
	{
	  	return "Synth Bass 2";
	}
	else if (index == 41)
	{
	  	return "Violin";
	}
	else if (index == 42)
	{
	  	return "Viola";
	}
	else if (index == 43)
	{
	  	return "Cello";
	}
	else if (index == 44)
	{
	  	return "Contrabass";
	}
	else if (index == 45)
	{
	  	return "Tremolo Strings";
	}
	else if (index == 46)
	{
	  	return "Pizzicato Strings";
	}
	else if (index == 47)
	{
	  	return "Orchestral Harp";
	}
	else if (index == 48)
	{
	  	return "Timpani";
	}
	else if (index == 49)
	{
	  	return "String Ensembles 1";
	}
	else if (index == 50)
	{
	  	return "String Ensembles 2";
	}
	else if (index == 51)
	{
	  	return "Synth Strings 1";
	}
	else if (index == 52)
	{
	  	return "Synth Strings 2";
	}
	else if (index == 53)
	{
	  	return "Choir Aahs";
	}
	else if (index == 54)
	{
	  	return "Choir Oohs";
	}
	else if (index == 55)
	{
	  	return "Synth Voice";
	}
	else if (index == 56)
	{
	  	return "Orchestra Hit";
	}
	else if (index == 57)
	{
	  	return "Trumpet";
	}
	else if (index == 58)
	{
	  	return "Trombone";
	}
	else if (index == 59)
	{
	  	return "Tuba";
	}
	else if (index == 60)
	{
	  	return "Muted Trumpet";
	}
	else if (index == 61)
	{
	  	return "French Horn";
	}
	else if (index == 62)
	{
	  	return "Brass Section";
	}
	else if (index == 63)
	{
	  	return "Synth Brass 1";
	}
	else if (index == 64)
	{
	  	return "Synth Brass 2";
	}
	else if (index == 65)
	{
	  	return "Soprano Sax";
	}
	else if (index == 66)
	{
	  	return "Alto Sax";
	}
	else if (index == 67)
	{
	  	return "Tenor Sax";
	}
	else if (index == 68)
	{
	  	return "Baritone Sax";
	}
	else if (index == 69)
	{
	  	return "Oboe";
	}
	else if (index == 70)
	{
	  	return "English Horn";
	}
	else if (index == 71)
	{
	  	return "Bassoon";
	}
	else if (index == 72)
	{
	  	return "Clarinet";
	}
	else if (index == 73)
	{
	  	return "Piccolo";
	}
	else if (index == 74)
	{
	  	return "Flute";
	}
	else if (index == 75)
	{
	  	return "Recorder";
	}
	else if (index == 76)
	{
	  	return "Pan Flute";
	}
	else if (index == 77)
	{
	  	return "Blown Bottle";
	}
	else if (index == 78)
	{
	  	return "Shakuhachi";
	}
	else if (index == 79)
	{
	  	return "Whistle";
	}
	else if (index == 80)
	{
	  	return "Ocarina";
	}
	else if (index == 81)
	{
	  	return "Square Lead (Lead 1)";
	}
	else if (index == 82)
	{
	  	return "Saw Lead (Lead 2)";
	}
	else if (index == 83)
	{
	  	return "Calliope Lead (Lead 3)";
	}
	else if (index == 84)
	{
	  	return "Chiff Lead (Lead 4)";
	}
	else if (index == 85)
	{
	  	return "Charang Lead (Lead 5)";
	}
	else if (index == 86)
	{
	  	return "Voice Lead (Lead 6)";
	}
	else if (index == 87)
	{
	  	return "Fifths Lead (Lead 7)";
	}
	else if (index == 88)
	{
	  	return "Bass + Lead (Lead 8)";
	}
	else if (index == 89)
	{
	  	return "New Age (Pad 1)";
	}
	else if (index == 90)
	{
	  	return "Warm Pad (Pad 2)";
	}
	else if (index == 91)
	{
	  	return "Polysynth (Pad 3)";
	}
	else if (index == 92)
	{
	  	return "Choir (Pad 4)";
	}
	else if (index == 93)
	{
	  	return "Bowed (Pad 5)";
	}
	else if (index == 94)
	{
	  	return "Metallic (Pad 6)";
	}
	else if (index == 95)
	{
	  	return "Halo (Pad 7)";
	}
	else if (index == 96)
	{
	  	return "Sweep (Pad 8)";
	}
	else if (index == 97)
	{
	  	return "Rain (FX 1)";
	}
	else if (index == 98)
	{
	  	return "Sound Track (FX 2)";
	}
	else if (index == 99)
	{
	  	return "Crystal (FX 3)";
	}
	else if (index == 100)
	{
	  	return "Atmosphere (FX 4)";
	}
	else if (index == 101)
	{
	  	return "Brightness (FX 5)";
	}
	else if (index == 102)
	{
	  	return "Goblins (FX 6)";
	}
	else if (index == 103)
	{
	  	return "Echoes (FX 7)";
	}
	else if (index == 104)
	{
	  	return "Sci-Fi (FX 8)";
	}
	else if (index == 105)
	{
	  	return "Sitar";
	}
	else if (index == 106)
	{
	  	return "Banjo";
	}
	else if (index == 107)
	{
	  	return "Shamisen";
	}
	else if (index == 108)
	{
	  	return "Koto";
	}
	else if (index == 109)
	{
	  	return "Kalimba";
	}
	else if (index == 110)
	{
	  	return "Bag Pipe";
	}
	else if (index == 111)
	{
	  	return "Fiddle";
	}
	else if (index == 112)
	{
	  	return "Shanai";
	}
	else if (index == 113)
	{
	  	return "Tinkle Bell";
	}
	else if (index == 114)
	{
	  	return "Agogo";
	}
	else if (index == 115)
	{
	  	return "Pitched Percussion";
	}
	else if (index == 116)
	{
	  	return "Woodblock";
	}
	else if (index == 117)
	{
	  	return "Taiko Drum";
	}
	else if (index == 118)
	{
	  	return "Melodic Tom";
	}
	else if (index == 119)
	{
	  	return "Synth Drum";
	}
	else if (index == 120)
	{
	  	return "Reverse Cymbal";
	}
	else if (index == 121)
	{
	  	return "Guitar Fret Noise";
	}
	else if (index == 122)
	{
	  	return "Breath Noise";
	}
	else if (index == 123)
	{
	  	return "Seashore";
	}
	else if (index == 124)
	{
	  	return "Bird Tweet";
	}
	else if (index == 125)
	{
	  	return "Telephone Ring";
	}
	else if (index == 126)
	{
	  	return "Helicopter";
	}
	else if (index == 127)
	{
	  	return "Applause";
	}
	else if (index == 128)
	{
	  	return "Gunshot";
	}
}
unsigned char * ReturnInstrumentCategoryLabel(int index)
{
	if (index == 0)
	{
		return "Piano";
	}
	else if (index == 1)
	{
		return "Chromatic Percussion";
	}
	else if (index == 2)
	{
	 	return "Organ";
	}
	else if (index == 3)
	{
		return "Guitar";
	}
	else if (index == 4)
	{
	   	return "Bass";
	}
	else if (index == 5)
	{
	 	return "Strings";
	}
	else if (index == 6)
	{
	 	return "Ensemble";
	}
	else if (index == 7)
	{
	 	return "Brass";
	}
	else if (index == 8)
	{
	 	return "Reed";
	}
	else if (index == 9)
	{
	 	return "Pipe";
	}
	else if (index == 10)
	{
	 	return "Synth Lead";
	}
	else if (index == 11)
	{
	 	return "Synth Pad";
	}
	else if (index == 12)
	{
	 	return "Synth Effects";
	}
	else if (index == 13)
	{
	 	return "Ethnic";
	}
	else if (index == 14)
	{
	 	return "Percussive";
	}
	else if (index == 15)
	{
	 	return "Sound Effects";
	}
}
unsigned char * ReturnRepeatingLabel(int index)
{
 	if (index == 0)
	{
	 	return "Done";
	}	
	else if (index == 1)
	{
	 	return "Change Instrument";
	}
	else if (index == 2)
	{
	 	return "Change Note";
	}
	else if (index == 3)
	{
	  	return "Change BPM";
	}
}
unsigned char * ReturnNoteLabel(int index)
{
	if (index == 0)
	{
	 	return "<none selected>";
	}
	else if (index == 1)
	{
	 	return "A";
	}
	else if (index == 2)
	{
	 	return "B";
	}
	else if (index == 3)
	{
	 	return "C";
	}
	else if (index == 4)
	{
	 	return "D";
	}
	else if (index == 5)
	{
	 	return "E";
	}
	else if (index == 6)
	{
	 	return "F";
	}
	else if (index == 7)
	{
	 	return "G";
	}
}
void ClearOldSelection(int Cur_Panel)
{
	int a = 0;
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
			GLCD_PutPixel(a, 148);
			GLCD_PutPixel(a, 149);
		}
		for (a = 62; a < 150; a++)
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
			GLCD_PutPixel(a, 152);
			GLCD_PutPixel(a, 153);
			GLCD_PutPixel(a, 238);
			GLCD_PutPixel(a, 239);
		}
		for (a = 152; a < 240; a++)
		{
			GLCD_PutPixel(182, a);
			GLCD_PutPixel(183, a);
			GLCD_PutPixel(318, a);
			GLCD_PutPixel(319, a);
		}
		GLCD_SetTextColor(Green);
	}
}
void MakeSelection(int Cur_Panel)
{
	int a = 0;
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
			GLCD_PutPixel(a, 148);
			GLCD_PutPixel(a, 149);
		}
		for (a = 62; a < 150; a++)
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
			GLCD_PutPixel(a, 152);
			GLCD_PutPixel(a, 153);
			GLCD_PutPixel(a, 238);
			GLCD_PutPixel(a, 239);
		}
		for (a = 152; a < 240; a++)
		{
			GLCD_PutPixel(182, a);
			GLCD_PutPixel(183, a);
			GLCD_PutPixel(318, a);
			GLCD_PutPixel(319, a);
		}
		GLCD_SetTextColor(Green);
	}
}
void P2SelectionClear(int Cur_Panel, int mult)
{
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
	if (Cur_Panel == 0)
		GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentLabel(0));
	else
 		GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentLabel(mult*8+Cur_Panel));
}
void P2MakeSelection(int Cur_Panel, int mult)
{
  	GLCD_SetBackColor(Yellow);
	GLCD_SetTextColor(Red);
	if (Cur_Panel == 0)
 		GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentLabel(0));
	else
		GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentLabel(mult*8+Cur_Panel));
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
}
void P1CatSelectionClear(Cur_Panel)
{
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
 	GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentCategoryLabel(Cur_Panel));
}
void P1CatMakeSelection(Cur_Panel)
{
  	GLCD_SetBackColor(Yellow);
	GLCD_SetTextColor(Red);
 	GLCD_DisplayString(Cur_Panel+3,2,0,(unsigned char *)ReturnInstrumentCategoryLabel(Cur_Panel));
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
}
void P3CatSelectionClear(Cur_Panel)
{
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
 	GLCD_DisplayString(Cur_Panel+7,2,0,(unsigned char *)ReturnRepeatingLabel(Cur_Panel));
}
void P3CatMakeSelection(Cur_Panel)
{
  	GLCD_SetBackColor(Yellow);
	GLCD_SetTextColor(Red);
 	GLCD_DisplayString(Cur_Panel+7,2,0,(unsigned char *)ReturnRepeatingLabel(Cur_Panel));
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
}
void InitPage(int pageNum, InstrumentStruct I1, InstrumentStruct I2, RepeatingInstrumentStruct R1, RepeatingInstrumentStruct R2, RepeatingInstrumentStruct R3, int Mult)
{
	if (pageNum == 0)
	{
	 	int a = 0;
		char toPr[6];
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
			GLCD_PutPixel(a, 150);
			GLCD_PutPixel(a, 151);
			//GLCD_PutPixel(a, 180);
			//GLCD_PutPixel(a, 181);
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
	
		GLCD_DisplayString(1,1,0,(unsigned char *)"Repeating Instrument 1");
		GLCD_DisplayString(11,1,0,(unsigned char *)"Repeating Instrument 2");
		GLCD_DisplayString(21,1,0,(unsigned char *)"Repeating Instrument 3");
	
		GLCD_DisplayString(2,5,0,(unsigned char *)"Inst: ");
		GLCD_DisplayString(2,11,0,(unsigned char *)ReturnInstrumentLabel(R1.InstrumentID));
		GLCD_DisplayString(3,5,0,(unsigned char *)"Note: ");
		GLCD_DisplayString(3,11,0,(unsigned char *)ReturnNoteLabel(R1.Note));
		sprintf(toPr, "BPM: %i", R1.BPM);
		GLCD_DisplayString(4,5,0,(unsigned char *)toPr);

		GLCD_DisplayString(12,5,0,(unsigned char *)"Inst: ");
		GLCD_DisplayString(12,11,0,(unsigned char *)ReturnInstrumentLabel(R2.InstrumentID));
		GLCD_DisplayString(13,5,0,(unsigned char *)"Note: ");
		GLCD_DisplayString(13,11,0,(unsigned char *)ReturnNoteLabel(R2.Note));
		sprintf(toPr, "BPM: %i", R2.BPM);
		GLCD_DisplayString(14,5,0,(unsigned char *)toPr);

		GLCD_DisplayString(22,5,0,(unsigned char *)"Inst: ");
		GLCD_DisplayString(22,11,0,(unsigned char *)ReturnInstrumentLabel(R3.InstrumentID));
		GLCD_DisplayString(23,5,0,(unsigned char *)"Note: ");
		GLCD_DisplayString(23,11,0,(unsigned char *)ReturnNoteLabel(R3.Note));
		sprintf(toPr, "BPM: %i", R3.BPM);
		GLCD_DisplayString(24,5,0,(unsigned char *)toPr);
	
		GLCD_DisplayString(1,35,0,(unsigned char *)"Master Volume");
		GLCD_DisplayString(2,42,0,(unsigned char *)"0");
	
		GLCD_DisplayString(9,31,0,(unsigned char *)"Player Instrument 1");
		GLCD_DisplayString(10,32,0,(unsigned char *)ReturnInstrumentLabel(I1.InstrumentID));
	
		GLCD_DisplayString(20,31,0,(unsigned char *)"Player Instrument 2");
		GLCD_DisplayString(21,32,0,(unsigned char *)ReturnInstrumentLabel(I2.InstrumentID));
	}
	else if (pageNum == 1)
	{
		int a = 0;
	 	GLCD_DisplayString(0,0,1,(unsigned char *)"Select Category:");
		GLCD_SetBackColor(Yellow);
		GLCD_SetTextColor(Red);
		GLCD_DisplayString(3,2,0,(unsigned char *)ReturnInstrumentCategoryLabel(0));
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(Green);

		for (a = 1; a < 16; a++)
		{
			GLCD_DisplayString(a+3,2,0,(unsigned char *)ReturnInstrumentCategoryLabel(a));
		}
	}
	else if (pageNum == 2)
	{
	 	int a = Mult * 8 + 1;
		int b = 0;
		int c = 1;
	 	GLCD_DisplayString(0,0,1,(unsigned char *)"Select Instrument:");
		GLCD_SetBackColor(Yellow);
		GLCD_SetTextColor(Red);
		GLCD_DisplayString(3,2,0,(unsigned char *)ReturnInstrumentLabel(0));
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(Green);

		for (b = a; b < a+8; b++)
		{
			GLCD_DisplayString(c+3,2,0,(unsigned char *)ReturnInstrumentLabel(b));
			c++;
		}
	}
	else if (pageNum == 3)
	{
		char toPr[7];
		int a = 0;
		GLCD_DisplayString(0,0,1,(unsigned char *)"Repeating Settings:");

		if (Mult == 0)
		{
		  	GLCD_DisplayString(3,2,0,(unsigned char *)"Inst: ");
			GLCD_DisplayString(3,8,0,(unsigned char *)ReturnInstrumentLabel(R1.InstrumentID));
			GLCD_DisplayString(4,2,0,(unsigned char *)"Note: ");
			GLCD_DisplayString(4,8,0,(unsigned char *)ReturnNoteLabel(R1.Note));
			sprintf(toPr, "BPM: %i", R1.BPM);
			GLCD_DisplayString(5,2,0,(unsigned char *)toPr);
		}
		else if (Mult == 1)
		{
		  	GLCD_DisplayString(3,2,0,(unsigned char *)"Inst: ");
			GLCD_DisplayString(3,8,0,(unsigned char *)ReturnInstrumentLabel(R2.InstrumentID));
			GLCD_DisplayString(4,2,0,(unsigned char *)"Note: ");
			GLCD_DisplayString(4,8,0,(unsigned char *)ReturnNoteLabel(R2.Note));
			sprintf(toPr, "BPM: %i", R2.BPM);
			GLCD_DisplayString(5,2,0,(unsigned char *)toPr);
		}
		else if (Mult == 2)
		{
		  	GLCD_DisplayString(3,2,0,(unsigned char *)"Inst: ");
			GLCD_DisplayString(3,8,0,(unsigned char *)ReturnInstrumentLabel(R3.InstrumentID));
			GLCD_DisplayString(4,2,0,(unsigned char *)"Note: ");
			GLCD_DisplayString(4,8,0,(unsigned char *)ReturnNoteLabel(R3.Note));
			sprintf(toPr, "BPM: %i", R3.BPM);
			GLCD_DisplayString(5,2,0,(unsigned char *)toPr);
		}

		GLCD_SetBackColor(Yellow);
		GLCD_SetTextColor(Red);
		GLCD_DisplayString(7,2,0,(unsigned char *)ReturnRepeatingLabel(0));
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(Green);

		for (a = 1; a < 4; a++)
		{
			GLCD_DisplayString(a+7,2,0,(unsigned char *)ReturnRepeatingLabel(a));
		} 	
	}
}




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
	float avg = 0;
	
	
	
	#elif LCD_EXAMPLE_OP==3
	vtLCDMsg msgBuffer;
	#if JOYSTICK_MODE==0
	int Cur_Panel = 0;
	int Cur_Page = 0;
	int Cur_Inst = 0;
	int RorP = 0;
	int P2SelectionMultiplier = 0;
	int P3Selection = 0;
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

	#elif JOYSTICK_MODE==1 //shawn's initializations go here
	cursorPos Cursor;
	Cursor.x = 0;
	Cursor.y = 0;
	#endif
	
	
	
	#endif
	lcdParamStruct * params = pvParameters;
	vtLCDMsgQueue *lcdPtr = (vtLCDMsgQueue *) params->lcdQ;
	MasterMsgQueue * masterData = params->masterQ;
	MasterMsgQueueMsg masterBuffer;

	/* Initialize the LCD */
	GLCD_Init();
	

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

#if LCD_EXAMPLE_OP==3
	#if JOYSTICK_MODE==0
	GLCD_Clear(Black);
	InitPage(0, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
	

	//GLCD_DisplayString(8,30,0,(unsigned char *)"Repeating Instrument 1");
	#elif JOYSTICK_MODE==1//shawn's LCD screen initializations
	#endif
#elif LCD_EXAMPLE_OP==2
	GLCD_Clear(Yellow);
	GLCD_SetTextColor(Black);
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
		
		
		if (Cur_Page == 0)
		{
			if (msgBuffer.buf[0] == 0) //select bit hit
			{
				if (Cur_Panel > 3)
				{
					GLCD_Clear(Black);
					Cur_Page = 1;
					Cur_Inst = Cur_Panel-4;
					RorP = 0;
					InitPage(1, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
					Cur_Panel = 0;
				}
				else if (Cur_Panel == 3)
				{
					//Cur_Page = 2;
				}
				else if (Cur_Panel < 3)
				{
					GLCD_Clear(Black);
					Cur_Page = 3;
					Cur_Inst = Cur_Panel;
					P3Selection = 0;
					RorP = 1;
					InitPage(3, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], Cur_Inst);
					Cur_Panel = 0;
				}
			}
		 	if (msgBuffer.buf[0] == 1) //move crosshair up
			{
				if (Cur_Panel > 0)
				{
					ClearOldSelection(Cur_Panel);
				 	Cur_Panel--;
					MakeSelection(Cur_Panel);
				}
			}
			if (msgBuffer.buf[0] == 2) //move crosshair right
			{
				if (Cur_Panel < 3)
				{
					ClearOldSelection(Cur_Panel);
					Cur_Panel = Cur_Panel + 3;
					MakeSelection(Cur_Panel);
				}
			} 
			if (msgBuffer.buf[0] == 3) //move crosshair down
			{
				if (Cur_Panel < 5)
				{
					ClearOldSelection(Cur_Panel);
				 	Cur_Panel++;
					MakeSelection(Cur_Panel);
				}
			}
			if (msgBuffer.buf[0] == 4) //move crosshair left
			{
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
		else if (Cur_Page == 1)
		{
			if (msgBuffer.buf[0] == 0) //select bit hit
			{
				GLCD_Clear(Black);
				Cur_Page = 2;
				P2SelectionMultiplier = Cur_Panel;
				InitPage(2, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
				Cur_Panel = 0;
			}
		 	if (msgBuffer.buf[0] == 1) //move crosshair up
			{
				if (Cur_Panel > 0)
				{
					P1CatSelectionClear(Cur_Panel);
				 	Cur_Panel--;
					P1CatMakeSelection(Cur_Panel);
				}
			}
			if (msgBuffer.buf[0] == 2) //move crosshair right
			{
			} 
			if (msgBuffer.buf[0] == 3) //move crosshair down
			{
				if (Cur_Panel < 15)
				{
					P1CatSelectionClear(Cur_Panel);
				 	Cur_Panel++;
					P1CatMakeSelection(Cur_Panel);
				}
			}
			if (msgBuffer.buf[0] == 4) //move crosshair left
			{
			}
		}
		else if (Cur_Page == 2)
		{
		 	if (msgBuffer.buf[0] == 0) //select bit hit
			{
				
				if (RorP == 0)
				{
					GLCD_Clear(Black);
					Cur_Page = 0;
					if (Cur_Panel == 0)
						Inst[Cur_Inst].InstrumentID = 0;
					else
						Inst[Cur_Inst].InstrumentID = P2SelectionMultiplier*8+Cur_Panel;
					P2SelectionMultiplier = 0;
					InitPage(0, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
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
					GLCD_Clear(Black);
					Cur_Page = 3;
					if (Cur_Panel == 0)
						RInst[Cur_Inst].InstrumentID = 0;
					else
						RInst[Cur_Inst].InstrumentID = P2SelectionMultiplier*8+Cur_Panel;
					P3Selection = 0;
					RorP = 1;
					InitPage(3, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], Cur_Inst);
					Cur_Panel = 0;

					masterBuffer.length = 5;
					masterBuffer.buf[0] = 0x0A; //signifies comes from lcd thread Instrument change
					masterBuffer.buf[1] = Cur_Inst+2;
					masterBuffer.buf[2] = RInst[Cur_Inst].InstrumentID;
					masterBuffer.buf[3] = RInst[Cur_Inst].Note;
					masterBuffer.buf[4] = RInst[Cur_Inst].BPM;

					if (xQueueSend(masterData->inQ,(void *) (&masterBuffer),portMAX_DELAY) != pdTRUE) {  
						VT_HANDLE_FATAL_ERROR(0);
					}
				}
			}
		 	if (msgBuffer.buf[0] == 1) //move crosshair up
			{
				if (Cur_Panel > 0)
				{
					P2SelectionClear(Cur_Panel, P2SelectionMultiplier);
				 	Cur_Panel--;
					P2MakeSelection(Cur_Panel, P2SelectionMultiplier);
				}
			}
			if (msgBuffer.buf[0] == 2) //move crosshair right
			{
			} 
			if (msgBuffer.buf[0] == 3) //move crosshair down
			{
				if (Cur_Panel < 8)
				{
					P2SelectionClear(Cur_Panel, P2SelectionMultiplier);
				 	Cur_Panel++;
					P2MakeSelection(Cur_Panel, P2SelectionMultiplier);
				}
			}
			if (msgBuffer.buf[0] == 4) //move crosshair left
			{
			}
		}
		else if (Cur_Page == 3)
		{
			if (P3Selection == 0)
			{
			   	if (msgBuffer.buf[0] == 0) //select bit hit
				{
					if (Cur_Panel == 0)
					{
					   	GLCD_Clear(Black);
						Cur_Page = 0;
						//P2SelectionMultiplier = Cur_Panel;
						InitPage(0, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
						Cur_Panel = 0;
					}
					else if (Cur_Panel == 1)
					{
					  	GLCD_Clear(Black);
						Cur_Page = 1;
						RorP = 1;
						InitPage(1, Inst[0], Inst[1], RInst[0], RInst[1], RInst[2], P2SelectionMultiplier);
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
						sprintf(toPr, "%i", RInst[Cur_Inst].BPM);
						GLCD_DisplayString(12,18,0,(unsigned char *)toPr);
						GLCD_SetBackColor(Black);
						GLCD_SetTextColor(Green); 
					}
				}
			 	if (msgBuffer.buf[0] == 1) //move crosshair up
				{
					if (Cur_Panel > 0)
					{
						P3CatSelectionClear(Cur_Panel);
					 	Cur_Panel--;
						P3CatMakeSelection(Cur_Panel);
					}
				}
				if (msgBuffer.buf[0] == 2) //move crosshair right
				{
				} 
				if (msgBuffer.buf[0] == 3) //move crosshair down
				{
					if (Cur_Panel < 3)
					{
						P3CatSelectionClear(Cur_Panel);
					 	Cur_Panel++;
						P3CatMakeSelection(Cur_Panel);
					}
				}
				if (msgBuffer.buf[0] == 4) //move crosshair left
				{
				}
			}
			else if (P3Selection == 1)
			{
			   	if (msgBuffer.buf[0] == 0) //select bit hit
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
			 	if (msgBuffer.buf[0] == 1) //move crosshair up
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
				if (msgBuffer.buf[0] == 2) //move crosshair right
				{
				} 
				if (msgBuffer.buf[0] == 3) //move crosshair down
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
				if (msgBuffer.buf[0] == 4) //move crosshair left
				{
				}
			}
			else if (P3Selection == 2)
			{
			   	if (msgBuffer.buf[0] == 0) //select bit hit
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
			 	if (msgBuffer.buf[0] == 1) //move crosshair up
				{
					if (RInst[Cur_Inst].BPM < 100)
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
				if (msgBuffer.buf[0] == 2) //move crosshair right
				{
				} 
				if (msgBuffer.buf[0] == 3) //move crosshair down
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
				if (msgBuffer.buf[0] == 4) //move crosshair left
				{
				}
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
		int pixel = msgBuffer.buf[0]<<8;
		pixel |= msgBuffer.buf[1];
		float temp = pixel;
		avg = /*avg + */temp / 300;

		GLCD_SetBackColor(Yellow);
		GLCD_SetTextColor(Black);
		if (msgBuffer.buf[2] == 0)
		{
			char toprint[20];
			sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
			GLCD_DisplayString(0,2,0, (unsigned char*)&toprint);
		}
		else if (msgBuffer.buf[2] == 1)
		{
		 	char toprint[20];
			sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
			GLCD_DisplayString(1,2,0, (unsigned char*)&toprint);
		} 
		else if (msgBuffer.buf[2] == 2)
		{
		  	char toprint[20];
			sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
			GLCD_DisplayString(2,2,0, (unsigned char*)&toprint);
		} 
		else if (msgBuffer.buf[2] == 3)
		{
			char toprint[20];
			sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
			GLCD_DisplayString(3,2,0, (unsigned char*)&toprint);
		} 
		else if (msgBuffer.buf[2] == 4)
		{
			char toprint[20];
			sprintf((char *)toprint, "%3.0i-%1.2f", pixel, avg);
			GLCD_DisplayString(4,2,0, (unsigned char*)&toprint);
		} 
		else if (msgBuffer.buf[2] == 5)
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
		if (msgBuffer.buf[2] < 6 && msgBuffer.buf[2] > 0)
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
#else
		Bad setting
#endif	
	}
}

