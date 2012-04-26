#include "maindefs.h"
#include <usart.h>
#include "my_uart.h"

static uart_comm *uc_ptr;

void uart_recv_int_handler()
{
	if (DataRdy1USART()) {
	
		uc_ptr->buffer[uc_ptr->buflen] = Read1USART();
		uc_ptr->buflen++;

		// check if a message should be sent
		if (uc_ptr->buflen == MAXUARTBUF) 
		{
			
			ToMainLow_sendmsg(uc_ptr->buflen,MSGT_UART_DATA,(void *) uc_ptr->buffer);
			uc_ptr->buflen = 0;
		}
	}
	if (RCSTA1bits.OERR == 1) {
		// we've overrun the USART and must reset
		// send an error message for this
		//LATB = 0xFF;
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
		//ToMainLow_sendmsg(0,MSGT_OVERRUN,(void *) 0);
	}
}

void init_uart_recv(uart_comm *uc)
{	
	uc_ptr = uc;
	uc_ptr->buflen = 0;
}