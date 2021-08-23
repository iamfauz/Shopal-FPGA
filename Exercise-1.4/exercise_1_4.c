

#include <stdio.h>

#define TouchScreen_ReceiverFifo (*(volatile unsigned char *)(0xFF210230))
#define TouchScreen_TransmitterFifo (*(volatile unsigned char *)(0xFF210230))
#define TouchScreen_InterruptEnableReg (*(volatile unsigned char *)(0xFF210232))
#define TouchScreen_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210234))
#define TouchScreen_FifoControlReg (*(volatile unsigned char *)(0xFF210234))
#define TouchScreen_LineControlReg (*(volatile unsigned char *)(0xFF210236))
#define TouchScreen_ModemControlReg (*(volatile unsigned char *)(0xFF210238))
#define TouchScreen_LineStatusReg (*(volatile unsigned char *)(0xFF21023A))
#define TouchScreen_ModemStatusReg (*(volatile unsigned char *)(0xFF21023C))
#define TouchScreen_ScratchReg (*(volatile unsigned char *)(0xFF21023E))
#define TouchScreen_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210230))
#define TouchScreen_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210232))

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
int ScreenReleased(void)
{
    // return TRUE if any data received from serial port connected to
    // touch screen or FALSE otherwise
    return (((TouchScreen_ReceiverFifo & 0x81) != 0x81) && ((TouchScreen_ReceiverFifo & 0x80) == 0x80));
}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForRelease()
{
    while (!ScreenReleased())
        ;
}

/* a data type to hold a point/coord */
typedef struct
{
    int x, y;
} Point;

Point getPoint()
{

    Point p1;

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
Point GetPress(void)
{
    // wait for a pen down command then return the X,Y coord of the point
    WaitForTouch();

    return getPoint();
}

/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void)
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
}
