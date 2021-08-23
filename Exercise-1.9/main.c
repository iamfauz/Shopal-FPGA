/*
 * main.c
 *
 *  Created on: Jan 24, 2019
 *      Author: Hyun Jun Kim
 */

#include <stdio.h>
#include <string.h>
#include "wifi.h"

int main()
{

	Init_WiFi();
	// allow time for dongle to lock onto baud rate
	talk_to_wifi("\r\n");
	delay(300);
	talk_to_wifi("\r\n");
	delay(300);
	talk_to_wifi("\r\n");
	delay(300);
	talk_to_wifi("\r\n");
	delay(300);
	talk_to_wifi("\r\n");
	delay(300);

	printf("starting up...\n");

	char sendtxt[] = "dofile(\"send_text_message.lua\")\r\n"; // { 'd', 'o', 'f',... }
	char checkwifi[] = "check_wifi()\r\n";

	printf("testing:\n");

	talk_to_wifi(sendtxt);
	delay(2000);

	printf("sent text!\n");

	talk_to_wifi(checkwifi);
	delay(5000);

	printf("wifi accessed!\n");

	return 0;
}
