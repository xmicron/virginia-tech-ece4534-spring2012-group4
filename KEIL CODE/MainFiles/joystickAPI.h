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
#include "GLCD.h"

#define joystickSTACK_SIZE		4*configMINIMAL_STACK_SIZE

typedef struct __cursorPos
{
	int x;
	int y;
}cursorPos;

void setCursorPos(int x,int y);
void setCursorYPos(int y);
void setCursorXPos(int x);
int getCursorXPos();
int getCursorYPos();
unsigned short invertColor(unsigned short origColor);
void invertPixel(int x, int y);
void paintCursor();

