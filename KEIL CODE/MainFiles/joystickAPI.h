/*
This API was designed to make it easier for a developer to get the status of the joystick and to paint a cursor onto the screen
*/

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

//declaration of cursor
typedef struct __cursorPos
{
	int x;
	int y;
}cursorPos;

//get and set cursor positions
void setCursorPos(int x,int y);
void setCursorYPos(int y);
void setCursorXPos(int x);
int getCursorXPos();
int getCursorYPos();

unsigned short invertColor(unsigned short origColor); //function that returns the inverted form of the given color
void invertPixel(int x, int y);  //invert the pixel specified by x and y

/*returns the status of the joystick
return 0=select button pressed
return 1=up button pressed
return 2=right button pressed
return 3=down button pressed
return 4=left button pressed
return 5=nothing pressed
*/
uint8_t getJoystickPin(); 

void paintCursor(); //paints a cursor onto the screen at the location specified by the current value of Cursor.x and Cursor.y

