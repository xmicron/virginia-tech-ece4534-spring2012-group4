/*
	Shawn's Milestone 4 - PCB test code
	4/15/2012
*/

/* Compile options:  -ml (Large code model) */
#include "maindefs.h"
#include <stdio.h>
#include <usart.h>
#include <i2c.h>
#include <timers.h>
#include "interrupts.h"
#include "messages.h"
#include "my_uart.h"
#include "my_i2c.h"
#include "uart_thread.h"
#include "timer1_thread.h"
#include "timer0_thread.h"
#include "newADC.h"
#include <adc.h>
#include <portb.h>
#include <delays.h>

// NOTE: Be sure to enable control bits in code in MPLAB
// Set clocks to make UART and I2C work
#pragma config WDTEN = OFF		// Watchdog Timers
//#pragma config FOSC = HSPLL		// Enable the PLL
//#pragma config FOSC2 = ON
#pragma config XINST = ON


int main (void)
{
	/*
		Define Variables ---------------------------------------------------------------------
	*/
	// I2C/MSG Q variables
	char c;		// Is this used?
	signed char	length;
	unsigned char	msgtype;
	unsigned char last_reg_recvd;
	i2c_comm ic;
	//unsigned char msgbuffer[MSGLEN+1];
	unsigned char msgbuffer[3];
	unsigned char i;
	int temp_array[2];
	int I2C_buffer[];
	int index = 0;
	int ITR = 0;
	int I2C_RX_MSG_COUNT = 0;
	int I2C_RX_MSG_PRECOUNT = 0;
	int I2C_TX_MSG_COUNT = 1;


	// Timer variables
	timer1_thread_struct t1thread_data; 	// info for timer1_lthread
	timer0_thread_struct t0thread_data; 	// info for timer0_lthread
	int timer_on = 1;
	int timer2Count0 = 0, timer2Count1 = 0;

	// UART variables
	uart_comm uc;
	uart_thread_struct	uthread_data; 		// info for uart_lthread

	// ADC variables
	int ADCVALUE = 0;
	int adc_counter = 0;
	int adc_chan_num = 0;
	int adcValue = 0;

	// MIDI variable
	char notePlayed;

	// Test variable
	int test = 2;

	/*
		Initialization ------------------------------------------------------------------------
	*/
	

	// Clock initialization
	OSCCON = 0x7C; // 16 MHz	// Use for internal oscillator	
	OSCTUNEbits.PLLEN = 1; 		// 4x the clock speed in the previous line
	
	
	// UART initialization
//	init_uart_recv(&uc);		// initialize my uart recv handling code
	// configure the hardware USART device
  	Open2USART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 31);
	Open1USART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 51);
	
	// I2C/MSG Q initialization
	init_i2c(&ic);				// initialize the i2c code
	init_queues();				// initialize message queues before enabling any interrupts
	i2c_configure_slave(0x9E);	// configure the hardware i2c device as a slave

	// Timer initialization
	//init_timer1_lthread(&t1thread_data);	// init the timer1 lthread
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_1);
	//OpenTimer2( TIMER_INT_ON & T2_PS_1_16 /*& T2_8BIT_RW & T2_SOURCE_INT & T2_OSC1EN_OFF & T2_SYNC_EXT_OFF*/); // Turn Off
	
	// ADC initialization
	// set up PORTA for input
	PORTA = 0x0;	// clear the port
	LATA = 0x0;		// clear the output latch
	TRISA = 0x0F;	// set RA3-RA0 to inputs
	ANSELA = 0xFF;	
	initADC();

	// Interrupt initialization
	// Peripheral interrupts can have their priority set to high or low
	// enable high-priority interrupts and low-priority interrupts
	enable_interrupts();
	// Decide on the priority of the enabled peripheral interrupts, 0 is low 1 is high

	IPR1bits.TMR1IP = 0;		// Timer1 interrupt
	IPR1bits.RCIP = 0;			// USART RX interrupt
	IPR1bits.SSP1IP = 1;			// I2C interrupt
	PIE1bits.SSP1IE = 1;			// must specifically enable the I2C interrupts
	IPR1bits.ADIP = 1;			// ADC interrupt WE ADDED THIS

	
	// set direction for PORTB to output
	TRISB = 0x0;
	LATB = 0x0;
	ANSELC = 0x00;

	Delay10KTCYx(200);
	Delay10KTCYx(200);

	switch(test)
	{
		case 0:
		{
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x30);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x32);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);	
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x34);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x35);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x37);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x39);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x3B);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x3C);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
			Delay10KTCYx(200);
			Delay10KTCYx(200);
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x3B);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x39);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x37);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x35);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x34);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x32);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		
			putc2USART(0x90);
			Delay1KTCYx(8);
			putc2USART(0x30);
			Delay1KTCYx(8);
		    putc2USART(0x65);
		
			Delay10KTCYx(200);
		};	
		case 1:
		{
			
			putc1USART(0x02);
			Delay1KTCYx(8);
			putc1USART(0x60);
		};
		case 2:
		{
			
		


			// Hand off messages to subroutines -----------------------------------------------------------
		
			// This loop is responsible for "handing off" messages to the subroutines
			// that should get them.  Although the subroutines are not threads, but
			// they can be equated with the tasks in your task diagram if you 
			// structure them properly
		
		  	while (1) {
				// Call a routine that blocks until either on the incoming
				// messages queues has a message (this may put the processor into
				// an idle mode
				block_on_To_msgqueues();
				
				// High Priority MSGQ ----------------------------------------------------------------------
				
				// At this point, one or both of the queues has a message.  It 
				// makes sense to check the high-priority messages first -- in fact,
				// you may only want to check the low-priority messages when there
				// is not a high priority message.  That is a design decision and
				// I haven't done it here.
				
				length = ToMainHigh_recvmsg(MSGLEN,&msgtype,(void *) msgbuffer);
				if (length < 0) {
					// no message, check the error code to see if it is concern
					if (length != MSGQUEUE_EMPTY) {
						//printf("Error: Bad high priority receive, code = %x\r\n", length);
					}
				} else {
					switch (msgtype) {
						
						case MSGT_ADC:	{
							
							
							//msgbuffer[0] = 0x11;
							//msgbuffer[1] = 0x22;
							//msgbuffer[2] = 0x33;
						
							
							// Send I2C msg
							FromMainHigh_sendmsg(3, msgtype, msgbuffer);	// Send ADC msg to FromMainHigh MQ, which I2C
																			// int hdlr later Reads
						};
					};
				}
			}
		};	
	};	
}