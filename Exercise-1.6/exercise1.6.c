/*
 * main.c
 *
 *  Created on: Jan 29, 2019
 *      Author: Andrea
 */

#include <stdio.h>

#define BT_ReceiverFifo (*(volatile unsigned char *)(0xFF210220))
#define BT_TransmitterFifo (*(volatile unsigned char *)(0xFF210220))
#define BT_InterruptEnableReg (*(volatile unsigned char *)(0xFF210222))
#define BT_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210224))
#define BT_FifoControlReg (*(volatile unsigned char *)(0xFF210224))
#define BT_LineControlReg (*(volatile unsigned char *)(0xFF210226))
#define BT_ModemControlReg (*(volatile unsigned char *)(0xFF210228))
#define BT_LineStatusReg (*(volatile unsigned char *)(0xFF21022A))
#define BT_ModemStatusReg (*(volatile unsigned char *)(0xFF21022C))
#define BT_ScratchReg (*(volatile unsigned char *)(0xFF21022E))
#define BT_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210220))
#define BT_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210222))

void Init_BT( void )
{
	// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
	// set Divisor latch (LSB and MSB) with correct value for required baud rate
	// set bit 7 of Line control register (LCR) back to 0 and
	// program other bits in (LCR) for 8 bit data, 1 stop bit, no parity etc
	// Reset the Fifo’s in the FIFO Control Reg by setting bits 1 & 2
	// Now Clear all bits in the FIFO control registers}

	BT_LineControlReg = 0x80;

	BT_DivisorLatchLSB = 0x1B; // Need to change to work with 115Kbps - Not sure if right
	BT_DivisorLatchMSB = 0x00;

	BT_LineControlReg = 0x03;

	BT_FifoControlReg = 0x06;
	BT_FifoControlReg = 0x00;

}

int putcharBT( int c )
{
	// wait for Transmitter Holding Register bit (5) of line status register to be '1‘
	// indicating we can write to the device
	// write character to Transmitter fifo register
	// return the character we printed

	while((BT_LineStatusReg & 0x20) != 0x20 ){}

	BT_TransmitterFifo = c;

	return c;
}

int getcharBT( void )
{
	// wait for Data Ready bit (0) of line status register to be '1'
	// read new character from ReceiverFiFo register
	// return new character

	while((BT_LineStatusReg & 0x01) != 0x01 ){}

	return  BT_ReceiverFifo;

}

// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int BT_TestForReceivedData(void)
{
 // if RS232_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE

	if((BT_LineStatusReg & 0x01) == 0x01){
		return 1;
	}
	else
	{
		return 0;
	}

}
//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void BT_Flush(void)
{
 // while bit 0 of Line Status Register == '1'
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
	char temp;

	while( BT_LineStatusReg & 0x01 == 0x01){

		temp = BT_ReceiverFifo;

	}
}

void BTOutMessage (char * message) {
//	int i = 0;
	while (*message)
	{
		//putchar(message[i]);
		putcharBT(*message++);
	}
}

void BTFactoryReset(void) {
	// wait for 1 second between command
	// enter these commands in upper case
	// $$$ enter command mode
	// SF,1 factory reset
	// SN,Device1 set device name to “Device1”
	// SP,1234 set 4 digit pin to “1234”
	// R,1<CR> reboot BT controller

	char c, Message[100] ;
	int i;

	while(1){

	 printf("\r\nEnter Message for Bluetooth Controller:") ;
	 gets(Message); // get command string from user keyboard
	 BTOutMessage(Message) ; // write string to BT device

	 // if the command string was NOT "$$$" send \r\n
	 if(strcmp(Message, "$$$") != 0) { // $$$ puts BT module into command mode
		putcharBT('\r') ;
		putcharBT('\n') ;
	 }

	 // now read back acknowledge string from device and display on console,
	 // will timeout after no communication for about 2 seconds
	 for(i = 0; i < 2000000; i ++) {
		 if(BT_TestForReceivedData() == 1) {
			c = getcharBT() ;
			putchar(c) ;
			i=0 ; // reset timer if we got something back
		 }
	 }
	}
}

int main (void) {

	Init_BT();
	BTFactoryReset();

	return 0;
}
