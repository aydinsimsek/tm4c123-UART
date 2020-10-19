#include "TM4C123.h"
#include <stdlib.h>
#include <stdint.h> 

void printString(char *s); 
void uartTransmit(char c); 
char uartReceive(void);
void delay(void); 

int main()
{
		SYSCTL->RCGCUART |= 0x1; // Enable the UART0 Module
    SYSCTL->RCGCGPIO |= 0x1; // Enable the clock to Port A since UART0 uses PA0 for RX and PA1 for TX
    GPIOA->DEN |= 0x3; // Enable digital functions for PA0 and PA1
    GPIOA->AFSEL |= 0x3; // Enable alternate function for PA0 and PA1
    GPIOA->PCTL |= 0x11; // Assign UART signals to PA0 and PA1
    UART0->CTL = 0x0; // Disable the UART
    UART0->IBRD = 104; // Write the integer portion for 9600 baud rate
    UART0->FBRD = 11; // Write the fractional portion for 9600 baud rate
    UART0->LCRH = 0x60; // Configure UART for data length of 8 bits, no parity bit, one stop bit, FIFOs disabled, no interrupts
    UART0->CC = 0x0; // Select the UART Baud Clock Source as system clock
    UART0->CTL = 0x301; // Enable the UART, RX and TX sections 
		SYSCTL->RCGCGPIO |= 0x20; // Enable the clock to the PORTF since LEDs are connected to PF1, PF2 and PF3
    GPIOF->DIR |= 0xE; // Configure LED pins to be an output
    GPIOF->DEN |= 0xE; // Enable digital functions for LED pins 
		GPIOF->DATA &= 0xF1; // Turn off the LEDs

		printString("Do you want to blink the LEDs? [Y/N]"); 
		char answer = uartReceive(); 
		switch(answer) 
		{
			case 'Y': 
				while(1)
				{
					GPIOF->DATA = 0x2; // turn on the red LED 
					delay();
					GPIOF->DATA = 0x4; // turn on the blue LED 
					delay();
					GPIOF->DATA = 0x8; // turn on the green LED 
					delay(); 
				}
			case 'N':
				GPIOF->DATA = 0x0; 
				break; 
			default: 
				printString("You typed an invalid character"); 
		}
}
	
void printString(char *s)
{
	while(*s != NULL) 
	{
		uartTransmit(*(s++));
	}
}	

void uartTransmit(char c) 
{
	while(UART0->FR & (1<<5)); // if TX holding register is full, wait until it becomes empty 
	UART0->DR = c; 
}

char uartReceive()
{
	while(UART0->FR & (1<<4)); // if RX holding register is empty, wait until it becomes full  
	char data = UART0->DR; 
	return data;
}

void delay()
{
	int i; 
	for(i=0; i<1000000; i++); 
} 