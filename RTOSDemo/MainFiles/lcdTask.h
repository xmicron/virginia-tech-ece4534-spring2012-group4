#ifndef LCD_TASK_H
#define LCD_TASK_H

#include "messagequeues.h"

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

#endif