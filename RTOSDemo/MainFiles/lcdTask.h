#ifndef LCD_TASK_H
#define LCD_TASK_H

#include "messagequeues.h"
#include "GLCD.h"

void vStartLCDTask( unsigned portBASE_TYPE uxPriority, lcdParamStruct *);

typedef struct __InstrumentStruct {
	int InstrumentID;
	int Note;
	int Velocity;
	int Pitch;					 
} InstrumentStruct;
typedef struct __RepeatingInstrumentStruct {
	int InstrumentID;
	int Note;
	int BPM;					 
} RepeatingInstrumentStruct;



//Large User Interface functions
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
	 	return "C4";
	}
	else if (index == 2)
	{
	 	return "D4";
	}
	else if (index == 3)
	{
	 	return "E4";
	}
	else if (index == 4)
	{
	 	return "F4";
	}
	else if (index == 5)
	{
	 	return "G4";
	}
	else if (index == 6)
	{
	 	return "A4";
	}
	else if (index == 7)
	{
	 	return "B4";
	}
	else if (index == 8)
	{
	 	return "C5";
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

#endif