#include "maindefs.h"
#include <i2c.h>
#include "my_i2c.h"

static i2c_comm *ic_ptr;

// Configure for I2C Master mode -- the variable "slave_addr" should be stored in
//   i2c_comm (as pointed to by ic_ptr) for later use.
void i2c_configure_master(unsigned char slave_addr)
{
	// Your code goes here
}

// Sending in I2C Master mode [slave write]
// 		returns -1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the sending of an i2c message -- interrupt handler will take care of
//   completing the message send.  When the i2c message is sent (or the send has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_SEND_COMPLETE if
//   the send was successful and an internal_message of type MSGT_MASTER_SEND_FAILED if the
//   send failed (e.g., if the slave did not acknowledge).  Both of these internal_messages
//   will have a length of 0.
// The subroutine must copy the msg to be sent from the "msg" parameter below into
//   the structure to which ic_ptr points [there is already a suitable buffer there].
unsigned char	i2c_master_send(unsigned char length,unsigned char *msg) 
{
	// Your code goes here
}

// Receiving in I2C Master mode [slave read]
// 		returns -1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the receiving of an i2c message -- interrupt handler will take care of
//   completing the i2c message receive.  When the receive is complete (or has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_RECV_COMPLETE if
//   the receive was successful and an internal_message of type MSGT_MASTER_RECV_FAILED if the
//   receive failed (e.g., if the slave did not acknowledge).  In the failure case
//   the internal_message will be of length 0.  In the successful case, the
//   internal_message will contain the message that was received [where the length
//   is determined by the parameter passed to i2c_master_recv()].
// The interrupt handler will be responsible for copying the message received into
unsigned char	i2c_master_recv(unsigned char length) 
{
	// Your code goes here
}

void	start_i2c_slave_reply(unsigned char length,unsigned char *msg)
{
	for (ic_ptr->outbuflen=0;ic_ptr->outbuflen<length;ic_ptr->outbuflen++) {
		ic_ptr->outbuffer[ic_ptr->outbuflen] = msg[ic_ptr->outbuflen];
	}
	ic_ptr->outbuflen = length;
	ic_ptr->outbufind = 1; // point to the second byte to be sent

	// put the first byte into the I2C peripheral
	SSP1BUF = ic_ptr->outbuffer[0];
	// we must be ready to go at this point, because we'll be releasing the I2C
	// peripheral which will soon trigger an interrupt
	SSP1CON1bits.CKP = 1;
	
}

// an internal subroutine used in the slave version of the i2c_int_handler
void	handle_start(unsigned char data_read)
{
	ic_ptr->event_count = 1;
	ic_ptr->buflen = 0;
	// check to see if we also got the address	
	if (data_read) {
		if (SSP1STATbits.D_A == 1) {
			// this is bad because we got data and
			// we wanted an address
			ic_ptr->status = I2C_IDLE;
			ic_ptr->error_count++;
			ic_ptr->error_code = I2C_ERR_NOADDR;
		} else {
			if (SSP1STATbits.R_W == 1) {
				ic_ptr->status = I2C_SLAVE_SEND;
			} else {
				ic_ptr->status = I2C_RCV_DATA;
			}
		}
	} else {
		ic_ptr->status = I2C_STARTED;
	}
}

// this is the interrupt handler for i2c -- it is currently built for slave mode
// -- to add master mode, you should determine (at the top of the interrupt handler)
//    which mode you are in and call the appropriate subroutine.  The existing code
//    below should be moved into its own "i2c_slave_handler()" routine and the new
//    master code should be in a subroutine called "i2c_master_handler()"
void i2c_int_handler()
{
	unsigned char	i2c_data;
	unsigned char	data_read = 0;
	unsigned char	data_written = 0;
	unsigned char	msg_ready = 0;
	unsigned char	msg_to_send = 0;
	unsigned char	overrun_error = 0;
	unsigned char	error_buf[3];

	// clear SSP1OV
	if (SSP1CON1bits.SSPOV == 1) {
		SSP1CON1bits.SSPOV = 0;
		// we failed to read the buffer in time, so we know we
		// can't properly receive this message, just put us in the
		// a state where we are looking for a new message
		ic_ptr->status = I2C_IDLE;
		overrun_error = 1;
		ic_ptr->error_count++;
		ic_ptr->error_code = I2C_ERR_OVERRUN;
	}
	// read something if it is there
	if (SSP1STATbits.BF == 1) {
		i2c_data = SSP1BUF;
		data_read = 1;
	}



	if (!overrun_error) {
		switch(ic_ptr->status) {
			case	I2C_IDLE: {
				// ignore anything except a start
				if (SSP1STATbits.S == 1) {
					handle_start(data_read);	
					// if we see a slave read, then we need to handle it here
					if (ic_ptr->status == I2C_SLAVE_SEND) {
						data_read = 0;
						msg_to_send = 1;
					}
				}	
				break;
			}
			case	I2C_STARTED: {
				// in this case, we expect either an address or a stop bit
				if (SSP1STATbits.P == 1) {
					// we need to check to see if we also read an
					// address (a message of length 0)
					ic_ptr->event_count++;
					if (data_read) {
						if (SSP1STATbits.D_A == 0) {
							msg_ready = 1;
						} else {
							ic_ptr->error_count++;
							ic_ptr->error_code = I2C_ERR_NODATA;
						}
					}
					ic_ptr->status = I2C_IDLE;
				} else if (data_read) {
					ic_ptr->event_count++;
					if (SSP1STATbits.D_A == 0) {
						if (SSP1STATbits.R_W == 0) { // slave write
							ic_ptr->status = I2C_RCV_DATA;
						} else { // slave read
							ic_ptr->status = I2C_SLAVE_SEND;
							msg_to_send = 1;
							// don't let the clock stretching bit be let go
							data_read = 0;
						}
					} else {
						ic_ptr->error_count++;
						ic_ptr->status = I2C_IDLE;
						ic_ptr->error_code = I2C_ERR_NODATA;
					}
				}
				break;
			}
			case I2C_SLAVE_SEND: {
				if (ic_ptr->outbufind < ic_ptr->outbuflen) {
					SSP1BUF = ic_ptr->outbuffer[ic_ptr->outbufind];
					ic_ptr->outbufind++;
					data_written = 1;
				} else {
					// we have nothing left to send
					ic_ptr->status = I2C_IDLE;
				}
				break;
			}
			case I2C_RCV_DATA: {
				// we expect either data or a stop bit or a (if a restart, an addr)
				 if (SSP1STATbits.P == 1) {
					// we need to check to see if we also read data
					ic_ptr->event_count++;
					if (data_read) {
						if (SSP1STATbits.D_A == 1) {
							ic_ptr->buffer[ic_ptr->buflen] = i2c_data;
							ic_ptr->buflen++;
							msg_ready = 1;
						} else {
							ic_ptr->error_count++;
							ic_ptr->error_code = I2C_ERR_NODATA;
							ic_ptr->status = I2C_IDLE;
						}
					} else {
						msg_ready = 1;
					}
					ic_ptr->status = I2C_IDLE;
				} else if (data_read) {
					ic_ptr->event_count++;
					if (SSP1STATbits.D_A == 1) {
						ic_ptr->buffer[ic_ptr->buflen] = i2c_data;
						ic_ptr->buflen++;
					} else /* a restart */ {
						if (SSP1STATbits.R_W == 1) {
							ic_ptr->status = I2C_SLAVE_SEND;
							msg_ready = 1;
							msg_to_send = 1;
							// don't let the clock stretching bit be let go
							data_read = 0;
						} else { /* bad to recv an address again, we aren't ready */
							ic_ptr->error_count++;
							ic_ptr->error_code = I2C_ERR_NODATA;
							ic_ptr->status = I2C_IDLE;
						}	
					}
				}
				break;
			}
		}
	}

	// release the clock stretching bit (if we should)
	if (data_read || data_written) {
		// release the clock
		if (SSP1CON1bits.CKP == 0) {
			SSP1CON1bits.CKP = 1;
		}
	}

	// must check if the message is too long, if 
	if ((ic_ptr->buflen > MAXI2CBUF-2) && (!msg_ready)) {
		ic_ptr->status = I2C_IDLE;
		ic_ptr->error_count++;
		ic_ptr->error_code = I2C_ERR_MSGTOOLONG;
	}


	// Any time data is sent from the Keil (Master), execute this code
	if (msg_ready) {
		LATB++;
		// Regardless of message type (request, or send from Keil), send buffer data to main
		ic_ptr->buffer[ic_ptr->buflen] = ic_ptr->event_count;
		ToMainHigh_sendmsg(ic_ptr->buflen+1,MSGT_I2C_DATA,(void *) ic_ptr->buffer);
		ic_ptr->buflen = 0;
	} else if (ic_ptr->error_count >= I2C_ERR_THRESHOLD) {
		error_buf[0] = ic_ptr->error_count;
		error_buf[1] = ic_ptr->error_code;
		error_buf[2] = ic_ptr->event_count;
		ToMainHigh_sendmsg(sizeof(unsigned char)*3,MSGT_I2C_DBG,(void *) error_buf);
		ic_ptr->error_count = 0;
	}

	// This is inside of the I2C interrupt handler fxn.  
	if (msg_to_send) {
		unsigned char TX_MSG[12];
		unsigned char type;
		int t = 0;
		
		// If data request from Keil (PIC -> Keil)
		if(ic_ptr->buffer[0] == 0xaa)	{						// Check to make sure master sent 0xaa
			int temp;
			int error;
			//LATBbits.LATB1 = !LATBbits.LATB1;
			// Use block_on_FromMainHigh to see if master is requesting data from an empty queue...if so,
			// send 0xFF.  If not, send the ADC data received from the I2C message queue to the master.
			if(block_on_FromMainHigh())	{						// Defined in messages.c	
				error = FromMainHigh_recvmsg(12, &type, (void *) TX_MSG);	// receive ADC value from FromMainHighMQ
				start_i2c_slave_reply(12,TX_MSG);							// send ADC value to Master via I2C
				// CHECK ERROR
				
			}	
			else	{
				// FF, FF is recognized by Keil as empty message queue, and are discarded
				for(t = 0; t < 12; t++)	{
					TX_MSG[t] = 0xFF;
				}
				start_i2c_slave_reply(12,TX_MSG);
			}
		}
		
		msg_to_send = 0;
		//ToMainHigh_sendmsg(0,MSGT_I2C_RQST,(void *)ic_ptr->buffer);
	}
}

// set up the data structures for this i2c code
// should be called once before any i2c routines are called
void init_i2c(i2c_comm *ic)
{	ic_ptr = ic;
	ic_ptr->buflen = 0;
	ic_ptr->event_count = 0;
	ic_ptr->status = I2C_IDLE;
	ic_ptr->error_count = 0;
}

// setup the PIC to operate as a slave
// the address must include the R/W bit
void i2c_configure_slave(unsigned char addr) {

	//ADCON0 = 0b001111100;
	//ADCON0 = 0b010000000;
	// ensure the two lines are set for input (we are a slave)
	TRISCbits.TRISC3=1;
	TRISCbits.TRISC4=1;
	TRISC = 0xFF;
	// set the address
	SSP1ADD = addr;
	//OpenI2C1(SLAVE_7,SLEW_OFF); // replaced w/ code below
	SSP1STAT = 0x0;
	SSP1CON1 = 0x0;
	SSP1CON2 = 0x0;
	SSP1CON1 |= 0x0E;  // enable Slave 7-bit w/ start/stop interrupts
	SSP1STAT |= SLEW_OFF;
	//SCL1 = 1;
	//SDA1 = 1;
	// enable clock-stretching 
	SSP1CON2bits.SEN = 1;
	SSP1CON1 |= SSPENB;
	// end of i2c configure
}