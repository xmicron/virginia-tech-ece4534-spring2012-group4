/*
	Milestone 1 - ECE 4534
	Last Updated: 2/9/2012

	Nick Markowski
	Joey Hutchins
	Homer Rich
	Shawn Furrow

	This program is a DIGITAL oscilloscope

	Timer0 constantly runs, and interrupts when its value overflows.  Its interrupt handler calls
	ConvertADC() to begin the conversion of a signal input into pin AN0.

	The ADC interrupt handler is called every time ConvertADC() is complete.  Its interrupt handler reads the
	converted value from the ADC, and sends it to the I2C MSG Q.

	The I2C interrupt handler is triggered every time the master provokes it.  This interrupt handler first decodes
	the signal sent by the master.  If the decoded signal is a request for ADC values (0xaa), the ADC values
	are pulled from the I2C MSG Q.  The interrupt handler then replies the ADC values back to the master.

	NOTES:
	Port A is configured for analog input (AN0-3)
	Port B is configured for output.  For debugging, drive portB pins.
	The PLL clock in enabled
	UART Baud Rate is initially 29095
	
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


void main (void)
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
	unsigned char msgbuffer[12];
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

	/*
		Initialization ------------------------------------------------------------------------
	*/
	

	// Clock initialization
	OSCCON = 0x7C; // 16 MHz	// Use for internal oscillator	
	OSCTUNEbits.PLLEN = 1; 		// 4x the clock speed in the previous line
	
	
	// UART initialization
	init_uart_recv(&uc);		// initialize my uart recv handling code
	// configure the hardware USART device
  	Open2USART( USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 31);
	Open1USART( USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 51);

	RCSTA1bits.CREN = 1;
	RCSTA1bits.SPEN = 1;
	TXSTA1bits.SYNC = 0;
	PIE1bits.RC1IE = 1;
	
	// I2C/MSG Q initialization
	init_i2c(&ic);				// initialize the i2c code
	init_queues();				// initialize message queues before enabling any interrupts
	i2c_configure_slave(0x9E);	// configure the hardware i2c device as a slave

	// Timer initialization
	init_timer1_lthread(&t1thread_data);	// init the timer1 lthread
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_8);
	OpenTimer2( TIMER_INT_ON & T2_PS_1_16 /*& T2_8BIT_RW & T2_SOURCE_INT & T2_OSC1EN_OFF & T2_SYNC_EXT_OFF*/); // Turn Off
// ADC initialization
	// set up PORTA for input
	PORTA = 0x0;	// clear the port
	LATA = 0x0;		// clear the output latch
	TRISA = 0xFF;	// set RA3-RA0 to inputs
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
	TRISD = 0xFF;
	LATB = 0x0;
	ANSELC = 0x00;


	/*
		Hand off messages to subroutines -----------------------------------------------------------
	*/
	// This loop is responsible for "handing off" messages to the subroutines
	// that should get them.  Although the subroutines are not threads, but
	// they can be equated with the tasks in your task diagram if you 
	// structure them properly.
  	while (1) {
		// Call a routine that blocks until either on the incoming
		// messages queues has a message (this may put the processor into
		// an idle mode
		block_on_To_msgqueues();
		/*
			High Priority MSGQ ----------------------------------------------------------------------
		*/
		
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
					// Format I2C msg
					msgbuffer[6] = (timer2Count0 & 0x00FF);
					msgbuffer[5] = (timer2Count0 & 0xFF00) >> 8;
					msgbuffer[4] = (timer2Count1 & 0x00FF);
					msgbuffer[3] = (timer2Count1 & 0xFF00) >> 8;

					msgbuffer[8] = 0x00;
					msgbuffer[10] = adc_chan_num;
					msgbuffer[11] = 0xaa;			// ADC MSG opcode
					
					// Send I2C msg
					FromMainHigh_sendmsg(12, msgtype, msgbuffer);	// Send ADC msg to FromMainHigh MQ, which I2C
																	// int hdlr later Reads

					
					// Increment I2C message count from 1 to 100
					if(I2C_TX_MSG_COUNT < 100)	{
						I2C_TX_MSG_COUNT = I2C_TX_MSG_COUNT + 1;
					}
					else	{
						I2C_TX_MSG_COUNT = 1;
					}

					// Increment the channel number
					if(adc_chan_num <= 4)	adc_chan_num++;
					else	adc_chan_num = 0;
					// Set ADC channel based off of channel number
					if(adc_chan_num == 0)	SetChanADC(ADC_CH0);
					else if(adc_chan_num == 1)	SetChanADC(ADC_CH1);
					else if(adc_chan_num == 2)	SetChanADC(ADC_CH2);
					else if(adc_chan_num == 3)	SetChanADC(ADC_CH3);
					else if(adc_chan_num == 4)	SetChanADC(ADC_CH4);
					else	SetChanADC(ADC_CH5);
				};
				case MSGT_TIMER0: {
					timer0_lthread(&t0thread_data,msgtype,length,msgbuffer);

					break;
				};
				case MSGT_TIMER2:	{
					timer2Count0++;
					if(timer2Count0 >= 0xFFFF)	{	
						timer2Count1++;
						timer2Count0 = 0;
					}
//					LATB = timer2Count1;
					break;
				}

				case MSGT_I2C_DATA: { //this data still needs to be put in a buffer
						//TXSTA1bits.TXEN = 1;
						
						//LATB = !LATB;

						if(msgbuffer[0] == 0xaf)	{
						//FromMainLow_sendmsg(5, msgtype, msgbuffer);
						// The code below checks message 'counts' to see if any I2C messages were dropped
						//I2C_RX_MSG_COUNT = msgbuffer[4];
						
							FromMainLow_sendmsg(9, msgtype, msgbuffer);						
							TXSTA2bits.TXEN = 1;
	/*
							// Send note data to the MIDI device
							//while(Busy2USART());
							putc2USART(msgbuffer[1]);
							//while(Busy2USART());
							Delay1KTCYx(8);
							putc2USART(msgbuffer[2]);
							//while(Busy2USART());
							Delay1KTCYx(8);		
							putc2USART(msgbuffer[3]);
	*/
	
							if(I2C_RX_MSG_COUNT - I2C_RX_MSG_PRECOUNT == 1)	{
								if(I2C_RX_MSG_PRECOUNT < 99)	{
									I2C_RX_MSG_PRECOUNT++;
								}
								else	{
									I2C_RX_MSG_PRECOUNT = 0;
								}
							}
							else	{
								I2C_RX_MSG_PRECOUNT = I2C_RX_MSG_COUNT;
							}
						}
						

						if (msgbuffer[0] == 0x13)
						{
						//	FromMainLow_sendmsg(9, msgtype, msgbuffer);						
							
								
						}
				};
				
	`			case MSGT_UART_DATA: 
				{
					msgbuffer[1] = 0xBB;
					msgbuffer[2] = 0x01;
					msgbuffer[3] = 0x01;
					msgbuffer[4] = 0x01;
					msgbuffer[5] = 0x01;
					msgbuffer[6] = 0x01;
					msgbuffer[7] = 0x01;
					msgbuffer[8] = 0x01;
					msgbuffer[9] = 0x01;
					msgbuffer[10] = 0x01;
					msgbuffer[11] = 0x01;
				//	FromMainHigh_sendmsg(12, msgtype, msgbuffer);
					break;
				};
				case MSGT_I2C_DBG: {
					//printf("I2C Interrupt received %x: ",msgtype);
					for (i=0;i<length;i++) {
						//printf(" %x",msgbuffer[i]);
					}					//printf("\r\n");
					// keep track of the first byte received for later use
					last_reg_recvd = msgbuffer[0];
					break;
				};
				case MSGT_I2C_RQST: {
					//printf("I2C Slave Req\r\n");
					// The last byte received is the "register" that is trying to be read
					// The response is dependent on the register.
					switch (last_reg_recvd) {
						case 0xaa: {
							break;
						}
						/*
						case 0xa8: {
							length = 1;
							msgbuffer[0] = 0x3A;
							break;
						}					
						case 0xa9: {
							length = 1;
							msgbuffer[0] = 0xA3;
							break;
						}*/
					};
					//start_i2c_slave_reply(length,msgbuffer);
					break;
				};
				default: {
					//printf("Error: Unexpected msg in queue, type = %x\r\n", msgtype);
					break;
				};
			};
		}

		/*
			Low Priority MSGQ -----------------------------------------------------------------------
		*/
		
		length = ToMainLow_recvmsg(MSGLEN,&msgtype,(void *) msgbuffer);
		if (length < 0) {
			// no message, check the error code to see if it is concern
			if (length != MSGQUEUE_EMPTY) {
			
			}
		} else {
			switch (msgtype) {
				/*
				case MSGT_TIMER1: {
					timer1_lthread(&t1thread_data,msgtype,length,msgbuffer);
					break;
				};
				case MSGT_OVERRUN:
				case MSGT_UART_DATA: 
				{
					msgbuffer[1] = 0xBB;
					msgbuffer[2] = 0x01;
					msgbuffer[3] = 0x01;
					msgbuffer[4] = 0x01;
					msgbuffer[5] = 0x01;
					msgbuffer[6] = 0x01;
					msgbuffer[7] = 0x01;
					msgbuffer[8] = 0x01;
					msgbuffer[9] = 0x01;
					msgbuffer[10] = 0x01;
					msgbuffer[11] = 0x01;
					FromMainHigh_sendmsg(12, msgtype, msgbuffer);
					break;
				};*/
				default: {
					
					break;
				};
			};
		}
 	 }

}