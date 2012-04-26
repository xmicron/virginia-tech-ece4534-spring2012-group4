#include "joystickAPI.h"

static cursorPos Cursor;

void setCursorPos(int x,int y)
{
	Cursor.x = x;
	Cursor.y = y;
}

void setCursorYPos(int x){Cursor.x = x;}

void setCursorYPos(int y){Cursor.y = y;}

int getCursorXPos(){return Cursor.x}

int getCursorYPos(){return Cursor.y}

unsigned short invertColor(unsigned short origColor)
{
	return (origColor ^ 0xFFFF);
}		  

void invertPixel(int x, int y)
{
	unsigned short temp = GLCD_GetTextColor();
	unsigned short color = GLCD_ReadPixelColor(x,y);
	color = invertColor(color);
	GLCD_SetTextColor(color);
	GLCD_PutPixel(x,y);
	GLCD_SetTextColor(temp);
}
uint8_t getJoystickPin()
{
		uint8_t PIN_CONFIG = LPC_GPIO1->FIOPIN >>20;
	
		if (PIN_CONFIG && 0x01 == 0)
			return 0;
		else if (PIN_CONFIG && 0x8)
			return 1;
		else if (PIN_CONFIG && 0x10)
			return 2;
		else if (PIN_CONFIG && 0x20)
			return 3;
		else if (PIN_CONFIG && 0x40)
			return 4;
		else
			return 5;
}

void paintCursor()
{
	invertPixel(Cursor.x, Cursor.y-2); 
	invertPixel(Cursor.x, Cursor.y-1);
	invertPixel(Cursor.x, Cursor.y);
	invertPixel(Cursor.x, Cursor.y+1);
	invertPixel(Cursor.x, Cursor.y+2);
	invertPixel(Cursor.x-2, Cursor.y);
	invertPixel(Cursor.x-1, Cursor.y);
	invertPixel(Cursor.x+1, Cursor.y);
	invertPixel(Cursor.x+2, Cursor.y);
}