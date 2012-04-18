// This is where the "user" interrupts handlers should go
// The *must* be declared in "user_interrupts.h"

#include "maindefs.h"
#include <timers.h>
#include "user_interrupts.h"
#include "messages.h"

// A function called by the interrupt handler
void timer0_int_handler()
{
	unsigned int val;
	int	length, msgtype;
 
	// reset the timer
	//WriteTimer0(0x9777);
	WriteTimer0(0xEFFF);
	//WriteTimer0(0x00);

	ConvertADC();	// Call convert ADC() to start ADC conversion

	// try to receive a message and, if we get one, echo it back
	//length = FromMainHigh_recvmsg(sizeof(val),&msgtype,(void *)&val);
	//if (length == sizeof(val)) {
		//ToMainHigh_sendmsg(sizeof(val),MSGT_TIMER0,(void *) &val);
	//}
}

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer1 interrupt
void timer1_int_handler()
{
	//unsigned int result;

	// read the timer and then send an empty message to main()
	//result = ReadTimer1();
	//ToMainLow_sendmsg(0,MSGT_TIMER1,(void *) 0);
	
	// reset the timer
	WriteTimer1(0);
}

// Timer 2 interrupt handler
// Used to record when ADC reads happen in time
void timer2_int_handler()
{
	unsigned char val[1];
	ToMainHigh_sendmsg(1, MSGT_TIMER2, (void *) val);
}

// ADC interrupt hdlr
// This interrupt is drivin high every time the ADC conversion is complete
void adc_int_handler()	{
	unsigned char val[3];

	// Read ADC, then store the values into message buffer
	//ReadADC();
	val[0] = 0x11;
	val[1] = 0x22;	
	
	// Read the timer to determine when the ADC was read
	val[2] = 0x33;

	//LATB = val[1];

	ToMainHigh_sendmsg(3, MSGT_ADC, (void *) val);	// Send ADC value to ToMainHigh MSGQ
	PIR1bits.ADIF = 0;								// Reset the ADC interrupt

}