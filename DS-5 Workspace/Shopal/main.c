/*
 * main.c
 *
 *  Created on: Jan 24, 2019
 *      Author: Hyun Jun Kim
 */


#define LOWERBOUND_LATITUDE 4900.00
#define UPPERBOUND_LATITUDE 4930.00
#define LOWERBOUND_LONGITUDE 12300.00
#define UPPERBOUND_LONGITUDE 12330.00
#define OUT_OF_RANGE_COUNT 30

#include "portablewifi.h"
#include "gps.h"

int main()
{

    Init_GPS();
    setup_WiFi();

    struct Coordinate c = updateCoordinates();
    int sentFlag = 1;

    // Stay in the loop as long as DE1 is not in the acceptable range, otherwise exit and send shopal list
    while(1) {



    	c = updateCoordinates();

        if(!(c.lat > LOWERBOUND_LATITUDE && c.lat < UPPERBOUND_LATITUDE && c.lon > LOWERBOUND_LONGITUDE && c.lon < UPPERBOUND_LONGITUDE))
        {
        	int numTimes = 1;
        	int i=0;

        	while(i<OUT_OF_RANGE_COUNT && sentFlag == 0){
        		c = updateCoordinates();
                if(!(c.lat > LOWERBOUND_LATITUDE && c.lat < UPPERBOUND_LATITUDE && c.lon > LOWERBOUND_LONGITUDE && c.lon < UPPERBOUND_LONGITUDE)){
                	numTimes++;
                }
                i++;
        	}

        	if(numTimes >= OUT_OF_RANGE_COUNT) {
        		printf("out of range\n");
        		sentFlag = 1;
        	}
        }

        if(c.lat > LOWERBOUND_LATITUDE && c.lat < UPPERBOUND_LATITUDE && c.lon > LOWERBOUND_LONGITUDE && c.lon < UPPERBOUND_LONGITUDE && (sentFlag ==1)) {
        	printf("inrange\n");
        	send_shopal_list();
        	sentFlag = 0;
        }

    }


}
