/*
 * main.c
 *
 *  Created on: Jan 24, 2019
 *      Author: Hyun Jun Kim
 */
#include <stdio.h>
#include <stdlib.h>
//#include "GPSPort.h"


#define GPS_ReceiverFifo (*(volatile unsigned char *)(0xFF210210))
#define GPS_TransmitterFifo (*(volatile unsigned char *)(0xFF210210))
#define GPS_InterruptEnableReg (*(volatile unsigned char *)(0xFF210212))
#define GPS_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210214))
#define GPS_FifoControlReg (*(volatile unsigned char *)(0xFF210214))
#define GPS_LineControlReg (*(volatile unsigned char *)(0xFF210216))
#define GPS_ModemControlReg (*(volatile unsigned char *)(0xFF210218))
#define GPS_LineStatusReg (*(volatile unsigned char *)(0xFF21021A))
#define GPS_ModemStatusReg (*(volatile unsigned char *)(0xFF21021C))
#define GPS_ScratchReg (*(volatile unsigned char *)(0xFF21021E))
#define GPS_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210210))
#define GPS_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210212))

/*
#define GPS_ReceiverFifo (*(volatile unsigned char *)(0xFF210200))
#define GPS_TransmitterFifo (*(volatile unsigned char *)(0xFF210200))
#define GPS_InterruptEnableReg (*(volatile unsigned char *)(0xFF210202))
#define GPS_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210204))
#define GPS_FifoControlReg (*(volatile unsigned char *)(0xFF210204))
#define GPS_LineControlReg (*(volatile unsigned char *)(0xFF210206))
#define GPS_ModemControlReg (*(volatile unsigned char *)(0xFF210208))
#define GPS_LineStatusReg (*(volatile unsigned char *)(0xFF21020A))
#define GPS_ModemStatusReg (*(volatile unsigned char *)(0xFF21020C))
#define GPS_ScratchReg (*(volatile unsigned char *)(0xFF21020E))
#define GPS_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210200))
#define GPS_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210202))
*/

/**************************************************************************
** Subroutine to initialise the GPS Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the GPS port
**
** Refer to UART data sheet for details of registers and programming
***************************************************************************/
void Init_GPS(void)
{
 // set bit 7 of Line Control Register to 1, to gain access to the baud rate
    // set Divisor latch (LSB and MSB) with correct value for required baud rate
 // set bit 7 of Line control register back to 0 and
 // program other bits in that reg for 8 bit data, 1 stop bit, no parity etc
 // Reset the Fifos in the FiFo Control Reg by setting bits 1 & 2
 // Now Clear all bits in the FiFo control registers

   GPS_LineControlReg = 0x80;

   GPS_DivisorLatchLSB = 0x45;
   GPS_DivisorLatchMSB = 0x01;

   GPS_LineControlReg = 0x03;

   GPS_FifoControlReg = 0x06;
   GPS_FifoControlReg = 0x00;

}
int putcharGPS(int c)
{
 // wait for Transmitter Holding Register bit (5) of line status register to be '1'
 // indicating we can write to the device
 // write character to Transmitter fifo register
 // return the character we printed

   while((GPS_LineStatusReg & 0x20) != 0x20 ){}

   GPS_TransmitterFifo = c;

   return c;


}
int getcharGPS( void )
{
 // wait for Data Ready bit (0) of line status register to be '1'
 // read new character from ReceiverFiFo register
 // return new character

 while((GPS_LineStatusReg & 0x01) != 0x01 ){}

 return  GPS_ReceiverFifo;

}
// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int GPSTestForReceivedData(void)
{
 // if GPS_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE

  return (GPS_LineStatusReg & 0x1) == 1;

}
//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void GPSFlush(void)
{
 // while bit 0 of Line Status Register == 1
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
	char temp;

	while( GPS_LineStatusReg & 0x01 == 0x01){

		temp = GPS_ReceiverFifo;

	}
}



void GPSOutMessage (char * message) {
//	int i = 0;
	while (*message)
	{
		//putchar(message[i]);
		putcharGPS(*message++);
	}
}



struct Coordinate {
    int time;
    double lat;
    double lon;
};


struct Coordinate updateCoordinates(void)
{
    char field[50];
    char incoming[500];
    char time[7];
    char lat[15];
    char lon[15];
    int counter;
    int counter2;
    int i;
    int j;
    struct Coordinate curCoordinate;

    counter = 0;
    counter2 = 0;
    j = 0;
    curCoordinate.lat = 0;
    curCoordinate.lon = 0;
    curCoordinate.time = 0;

    for (i = 0; i < 2000000 && j < 500; i++)
    {
        if (GPSTestForReceivedData() == 1 && j < 500)
        {
            incoming[j] = (char)(getcharGPS());
            i = 0; // reset timer if we got something back
            j++;
        }
    }

    while (counter < 500)
    {

        while (incoming[counter] != '$')
        {
            counter++;
            if (counter >= 500)
                return curCoordinate;
        }

        counter++;

        for (i = 0; i <= 5; i++)
        {
            field[i] = incoming[counter];
            counter++;
            if (counter >= 500)
                return curCoordinate;
        }

        if (field[0] == 'G' && field[1] == 'P' && field[2] == 'G' && field[3] == 'G' && field[4] == 'A')
        {

            for (i = 6; i < 50; i++)
            {
                field[i] = incoming[counter];
                counter++;
                if (counter >= 500)
                    return curCoordinate;
            }

            counter2 = 6;

            for (i = 0; i < 6 && field[counter2] != ','; i++)
            {
               // printf("kalle shiri!\n");
               // printf("%c", field[counter2]);
              //  printf("\n");
                time[i] = field[counter2];
                counter2++;
            }
            time[i]= '\0';

            while (field[counter2] != ',')
            {
                counter2++;
            }

            counter2++;

            for (i = 0; i < 15 && field[counter2] != ','; i++)
            {
                lat[i] = field[counter2];
                counter2++;
            }
            lat[i] = '\0';

            counter2++;

            while (field[counter2] != ',')
            {
                counter2++;
            }

            counter2++;

            for (i = 0; i < 15 && field[counter2] != ','; i++)
            {
                lon[i] = field[counter2];
                counter2++;
            }
            lon[i] = '\0';

           /* printf("\ntime: ");
            printf(time);

            printf("\nLat: ");
            printf(lat);

            printf("\nLong: ");
            printf(lon);

            printf("\n");*/

            curCoordinate.time = atoi(time);
            if(curCoordinate.time < 80000)
                curCoordinate.time += 160000;
            else 
                curCoordinate.time -= 80000;

            curCoordinate.lat = strtod(lat, NULL);
            curCoordinate.lon = strtod(lon, NULL);


            return curCoordinate;
        }
    }

    return curCoordinate;
}



int main()
{

    Init_GPS();
    struct Coordinate curCoordinate = updateCoordinates();

    for(int i=0; i<1000; i++){
        curCoordinate = updateCoordinates();
        printf("%d", curCoordinate.time);
    	printf("\n");
    }

	return 0;
}
