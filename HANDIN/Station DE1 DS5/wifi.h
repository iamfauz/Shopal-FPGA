/*
 * wifi.h
 *
 *  Created on: Jan 27, 2019
 *      Author: Hyun Jun Kim
 */

#ifndef WIFI_H_
#define WIFI_H_


// WiFi Registers located between addresses 0xFF21 0240 - 0xFF21 024F
#define WiFi_ReceiverFifo (*(volatile unsigned char *)(0xFF210240))
#define WiFi_TransmitterFifo (*(volatile unsigned char *)(0xFF210240))
#define WiFi_InterruptEnableReg (*(volatile unsigned char *)(0xFF210242))
#define WiFi_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210244))
#define WiFi_FifoControlReg (*(volatile unsigned char *)(0xFF210244))
#define WiFi_LineControlReg (*(volatile unsigned char *)(0xFF210246))
#define WiFi_ModemControlReg (*(volatile unsigned char *)(0xFF210248))
#define WiFi_LineStatusReg (*(volatile unsigned char *)(0xFF21024A))
#define WiFi_ModemStatusReg (*(volatile unsigned char *)(0xFF21024C))
#define WiFi_ScratchReg (*(volatile unsigned char *)(0xFF21024E))
#define WiFi_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210240))
#define WiFi_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210242))

void Init_WiFi(void);
int putcharWiFi(int);
int getcharWiFi(void);
int WiFiTestForReceivedData(void);
void WiFiFlush(void);

void delay(int);
void talk_to_wifi(char[]);
void receive_from_wifi(char[]);
int pull_database(char* []);
void delete_database();
void fifo_reset(void);

void setup_WiFi(void);
void send_shopal_list();

#endif /* WIFI_H_ */
