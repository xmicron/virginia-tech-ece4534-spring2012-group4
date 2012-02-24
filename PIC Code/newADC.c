/////////////////////////////////////////////
// Program to test the A/Ds on the 4534 demo
// board.  Channel 0 is connected to the pot.
// Channel 1 is connected to a header where a
// sensor can be hooked up.
// The LEDs display is based on the value read
// from the A/D.
//////////////////////////////////////////////


//#include <p18f45j10.h>
#include "newADC.h"
#include <portb.h>
#include <delays.h>
#include <adc.h>

void initADC()
{
	OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_12_TAD,
		ADC_CH0 & ADC_CH1 &
		ADC_INT_ON & ADC_VREFPLUS_VDD & 
		ADC_VREFMINUS_VSS, 0b1011);	// WAS binary 11
	// Use SetChanADC(ADC_CH1) to look at sensor channel
	SetChanADC(ADC_CH0);
	//Delay10TCYx(50);	// WAS 50
}

void readADC(int *value)
{
	ConvertADC(); // Start conversion
	//while( BusyADC() ); // Wait for ADC conversion
	//(*value) = ReadADC(); // Read result and put in temp
	//Delay1KTCYx(1);  // wait a bit...
}

void stopADC()
{
	CloseADC(); // Disable A/D converter
}
