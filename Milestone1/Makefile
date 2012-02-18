# MPLAB IDE generated this makefile for use with GNU make.
# Project: Proj1.mcp
# Date: Thu Feb 09 15:04:04 2012

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

Proj1.cof : main.o messages.o interrupts.o user_interrupts.o my_uart.o uart_thread.o timer1_thread.o timer0_thread.o my_i2c.o newADC.o
	$(LD) /p18F45J10 /l"C:\MCC18\lib" "main.o" "messages.o" "interrupts.o" "user_interrupts.o" "my_uart.o" "uart_thread.o" "timer1_thread.o" "timer0_thread.o" "my_i2c.o" "newADC.o" /u_CRUNTIME /u_EXTENDEDMODE /z__MPLAB_BUILD=1 /m"Proj1.map" /w /o"Proj1.cof"

main.o : main.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stddef.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/usart.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/pconfig.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/i2c.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/timers.h interrupts.h messages.h my_uart.h my_i2c.h uart_thread.h timer1_thread.h timer0_thread.h newADC.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/adc.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/portb.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/delays.h
	$(CC) -p=18F45J10 "main.c" -fo="main.o" --extended -Opa-

messages.o : messages.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h interrupts.h messages.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/string.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stddef.h
	$(CC) -p=18F45J10 "messages.c" -fo="messages.o" --extended -Opa-

interrupts.o : interrupts.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h interrupts.h user_interrupts.h my_uart.h messages.h
	$(CC) -p=18F45J10 "interrupts.c" -fo="interrupts.o" --extended -Opa-

user_interrupts.o : user_interrupts.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/timers.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/pconfig.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18cxxx.h user_interrupts.h my_uart.h messages.h
	$(CC) -p=18F45J10 "user_interrupts.c" -fo="user_interrupts.o" --extended -Opa-

my_uart.o : my_uart.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/usart.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/pconfig.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18cxxx.h my_uart.h messages.h
	$(CC) -p=18F45J10 "my_uart.c" -fo="my_uart.o" --extended -Opa-

uart_thread.o : uart_thread.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stddef.h uart_thread.h
	$(CC) -p=18F45J10 "uart_thread.c" -fo="uart_thread.o" --extended -Opa-

timer1_thread.o : timer1_thread.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stddef.h messages.h timer1_thread.h
	$(CC) -p=18F45J10 "timer1_thread.c" -fo="timer1_thread.o" --extended -Opa-

timer0_thread.o : timer0_thread.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/stddef.h timer0_thread.h
	$(CC) -p=18F45J10 "timer0_thread.c" -fo="timer0_thread.o" --extended -Opa-

my_i2c.o : my_i2c.c maindefs.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/i2c.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/pconfig.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18cxxx.h my_i2c.h messages.h
	$(CC) -p=18F45J10 "my_i2c.c" -fo="my_i2c.o" --extended -Opa-

newADC.o : newADC.c newADC.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/portb.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/pconfig.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/p18f45j10.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/delays.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.40/h/adc.h
	$(CC) -p=18F45J10 "newADC.c" -fo="newADC.o" --extended -Opa-

clean : 
	$(RM) "main.o" "messages.o" "interrupts.o" "user_interrupts.o" "my_uart.o" "uart_thread.o" "timer1_thread.o" "timer0_thread.o" "my_i2c.o" "newADC.o" "Proj1.cof" "Proj1.hex" "Proj1.map"

