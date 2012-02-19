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

	LATBbits.LATB0 = !LATBbits.LATB0;
 
	// reset the timer
	//WriteTimer0(0x000F);
	WriteTimer0(0);

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

// ADC interrupt hdlr
// This interrupt is drivin high every time the ADC conversion is complete
void adc_int_handler()	{
	unsigned int val[3];
	val[0] = ReadADC();			// Read Converted ADC value
	val[1] = val[0] & 0xFF;
	//val[0] = 0;
	val[2] = ReadTimer1();

	
	ToMainHigh_sendmsg(2, MSGT_ADC, (void *) val);	// Send ADC value to ToMainHigh MSGQ
	PIR1bits.ADIF = 0;								// Reset the ADC interrupt

}