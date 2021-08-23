

#include <stdio.h>
#include "touch.h"

/*****************************************************************************
** Initialize touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
    // Program serial port to communicate with touchscreen
    TouchScreen_LineControlReg = 0x80;
    TouchScreen_DivisorLatchLSB = 0x45;
    TouchScreen_DivisorLatchMSB = 0x01;
    TouchScreen_LineControlReg = 0x03;
    TouchScreen_FifoControlReg = 0x06;
    TouchScreen_FifoControlReg = 0x00;

    // send touchscreen controller an "enable touch" command
    putcharTouchScreen(0x55);
    putcharTouchScreen(0x01);
    putcharTouchScreen(0x12);
}

/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched(void)
{
    // return TRUE if any data received from serial port connected to
    // touchscreen or FALSE otherwise
    return ((TouchScreen_ReceiverFifo & 0x81) == 0x81);
}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch()
{
    while (!ScreenTouched())
        ;
}

/*****************************************************************************
** test if screen released
*****************************************************************************/
/*int ScreenReleased(void)
{
    // return TRUE if any data received from serial port connected to
    // touch screen or FALSE otherwise
    return (((TouchScreen_ReceiverFifo & 0x81) != 0x81) && ((TouchScreen_ReceiverFifo & 0x80) == 0x80));
}
*/
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
/*void WaitForRelease()
{
    while (!ScreenReleased())
        ;
}
*/
/* a data type to hold a point/coord */
typedef struct
{
    int x, y;
} Point;

struct Point getPoint()
{

    struct Point p1;

    // calibrated correctly so that it maps to a pixel on screen

    //x-coordinate
    p1.x = getcharTouchScreen();
    p1.x += ((int)getcharTouchScreen()) << 7;

    //y-coordinate
    p1.y = getcharTouchScreen();
    p1.y += ((int)getcharTouchScreen()) << 7;

    //Re-adjusting according ot screen coordinates
    p1.x = p1.x * 479 / 4095;
    p1.y = p1.y * 799 / 4095;

    return p1;
}
/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
struct Point GetPress(void)
{
    // wait for a pen down command then return the X,Y coord of the point
    WaitForTouch();

   // printf("out of GetPress\n");

    return getPoint();
}


/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
struct Point GetRelease(void)
{

    // wait for a pen up command then return the X,Y coord of the point
    WaitForRelease();

    return getPoint();
}

/* Helper Methods */

int putcharTouchScreen(int c)
{
    // wait for Transmitter Holding Register bit (5) of line status register to be '1'
    // indicating we can write to the device
    // write character to Transmitter fifo register
    // return the character we printed

    while ((TouchScreen_LineStatusReg & 0x20) != 0x20)
    {
    }

    TouchScreen_TransmitterFifo = c;

    return c;
}
int getcharTouchScreen(void)
{
    // wait for Data Ready bit (0) of line status register to be '1'
    // read new character from ReceiverFiFo register
    // return new character

    while ((TouchScreen_LineStatusReg & 0x01) != 0x01)
    {
    }

    return TouchScreen_ReceiverFifo;


}

//stuff added 

int ScreenReleased(void) {
	int first = getcharTouch();
	return ((first>>7)%2 == 1 && (first%2) == 0); //pen up event
}

void WaitForRelease() {
    printf("enterWait\n");
	while(ScreenReleased() == 0){
    }
    printf("outOfWait\n");
//	printf("released\n");
}

int getcharTouch( void ) {
 // wait for Data Ready bit (0) of line status register to be '1'
	while((TouchScreen_LineStatusReg & 1) != 1);
 // read new character from ReceiverFiFo register
//	printf("received...\n");
	return TouchScreen_ReceiverFifo;
 // return new character
}



/*
int main()
{
    Point p;

    Init_Touch();

    while (1)
    {

        p = GetPress();
        printf("Pressed at: %d, %d\n", p.x, p.y);

        p = GetRelease();
        printf("Release at: %d, %d\n", p.x, p.y);
    }

    return 0;
}*/
