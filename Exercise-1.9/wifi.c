/*
 * wifi.c
 *
 *  Created on: Jan 27, 2019
 *      Author: Hyun Jun Kim
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "wifi.h"

/**************************************************************************
** Subroutine to initialise the WiFi Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the WiFi port
**
** Refer to UART data sheet for details of registers and programming
***************************************************************************/
void Init_WiFi(void)
{
 // set bit 7 of Line Control Register to 1, to gain access to the baud rate
 // set Divisor latch (LSB and MSB) with correct value for required baud rate
 // set bit 7 of Line control register back to 0 and
 // program other bits in that reg for 8 bit data, 1 stop bit, no parity etc
 // Reset the Fifo?s in the FiFo Control Reg by setting bits 1 & 2
 // Now Clear all bits in the FiFo control registers

   WiFi_LineControlReg = 0x80;	// 1000 000

   WiFi_DivisorLatchLSB = 0x46;
   WiFi_DivisorLatchMSB = 0x01;

   WiFi_LineControlReg = 0x03;	// 0000 0011

   WiFi_FifoControlReg = 0x06;	// 0000 0110
   WiFi_FifoControlReg = 0x00;	// 0000 0000

   printf("wifi initialized!");
}


int putcharWiFi(int c)
{
 // wait for Transmitter Holding Register bit (5) of line status register to be '1'
 // indicating we can write to the device
 // write character to Transmitter fifo register
 // return the character we printed

   while((WiFi_LineStatusReg & 0x20) != 0x20 ){}	// 0x20 = 0010 0000

   WiFi_TransmitterFifo = c;	// setting wifi_txdata

   return c;

}



int getcharWiFi(void)
{
 // wait for Data Ready bit (0) of line status register to be '1'
 // read new character from ReceiverFiFo register
 // return new character

	while((WiFi_LineStatusReg & 0x01) != 0x01 ){}		// 0x01 = 0000 0001

	return  WiFi_ReceiverFifo;		// returning wifi_rxdata

}


// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int WiFiTestForReceivedData(void)
{
 // if WiFi_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE

  return WiFi_LineStatusReg & 0x01;

}


//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void WiFiFlush(void)
{
 // while bit 0 of Line Status Register == ?1?
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
	char temp;

	while( WiFi_LineStatusReg & 0x01 == 0x01){
		temp = WiFi_ReceiverFifo;
	}
}

void delay(int milliseconds)
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end) {}
}

void talk_to_wifi(char message[])
{
	int i = 0;
	char c;

	for (i = 0; i < strlen(message); i++) {
		putcharWiFi(message[i]);	// push 1 char onto serial port
		printf("%c", c);
		delay(100);
	}

}
