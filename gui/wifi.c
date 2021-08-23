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
 // Reset the Fifo�s in the FiFo Control Reg by setting bits 1 & 2
 // Now Clear all bits in the FiFo control registers

   WiFi_LineControlReg = 0x80;	// 1000 000

   WiFi_DivisorLatchLSB = 0x1B;		//0x0146 for 9600, 0x001B for 115200
   WiFi_DivisorLatchMSB = 0x00;

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
 // while bit 0 of Line Status Register == �1�
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
	char temp;

	while( WiFi_LineStatusReg & 0x01 == 1){
		temp = WiFi_ReceiverFifo;
	}
}



void talk_to_wifi(char message[])
{
	int i = 0;
	int j;
	char c;

	for (i = 0; i < strlen(message); i++) {
		putcharWiFi(message[i]);	// push 1 char onto serial port
		// c = getcharWiFi();
		//printf("%c", c);

	}
}

void receive_from_wifi(char c[]) {
	int i = 0;

//	for (int k = 0; k < 500; k++) {
//		c[k] = NULL;
//	}

	for(int j = 0; j < 2000000; j++) {
		if (i >= 499) break;
		while (WiFiTestForReceivedData()) {
			c[i++] = (char) getcharWiFi();
			j = 0;
		}
	}
	c[i] = '\0';
}

void dofile() {
	char dofilefirebase[] = "dofile(\"stationtextfirebase.lua\")\r\n";
	//char dofiletxt[] = "dofile(\"send_text_message.lua\")\r\n"; // { 'd', 'o', 'f',... }
	talk_to_wifi(dofilefirebase);
	//talk_to_wifi(dofiletxt);
}

int pull_database(char * db[]) {
	// lua function names
	char get_db[] = "get_db()\r\n";		// prints the database pulled from the wifi chip
	char print_db[] = "print_db()\r\n";
	// our string array
	char c[500] = NULL;

	printf("testing database: \n");
	//dofile

	talk_to_wifi(get_db);	//get_db & make_db
	delay(3000);
	talk_to_wifi(print_db);
	receive_from_wifi(c);

	char *p = strtok(c, ",");
	int size = 0;
	int j = 0;
	p = strtok(NULL, ",");

	while (p != NULL) {
		db[size++] = p;
		p = strtok(NULL, ",");
	}

	
	char * lastItem = db[size-1];
  lastItem[strlen(lastItem) - 2] = '\0';
	return size;
}

void delete_database() {
	char delete_db[] =  "delete_db()\r\n";
	
	printf("deleting database: \n");
	talk_to_wifi(delete_db);
}

void fifo_reset() {

	WiFi_FifoControlReg = 0x06;	// 0000 0110
	WiFi_FifoControlReg = 0x00;	// 0000 0000
}

void delay(int milliseconds)
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end) {}
}

void setup_WiFi(void)
{

 Init_WiFi();
	// allow time for dongle to lock onto baud rate
	talk_to_wifi("\r\n");
	talk_to_wifi("\r\n");
	talk_to_wifi("\r\n");
	talk_to_wifi("\r\n");
	talk_to_wifi("\r\n");

	dofile();
}

void send_shopal_list()
{
	//char checkwifi[] = "check_wifi()\r\n";

	// printf("I'm in f\n");
	// printf(message);
	// printf("\n");
	// char shopallist[1000];
	// strcpy(shopallist, "send_sms(\"(778) 949-5030\",\"(778) 999-9041\",\"");
	// strcat(shopallist, message);
	// strcat(shopallist, "\")\r\n");

	// printf("shopping list:\n");
	// printf(shopallist);
	// printf("\n");

	char database_twilio[] = "database_twilio()\r\n";
	char get_db[] = "get_db()\r\n";		// prints the database pulled from the wifi chip

	talk_to_wifi(get_db);
	delay(3000);
	talk_to_wifi(database_twilio);

	//char shopallist[] = "send_sms(\"(778) 949-5030\",\"(778) 999-9041\",\"Here's your Shopal list: Apple, Banana, Bread\")\r\n";

	//"Here's your Shopal list: Apple, Banana, Bread
	//talk_to_wifi(checkwifi);
	//delay(5000);
	// talk_to_wifi(shopallist);
}