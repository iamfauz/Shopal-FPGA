/*
 * touch.h
 *
 */

#ifndef TOUCH_H_
#define TOUCH_H_

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

void Init_Touch(void);
int ScreenTouched(void);
void WaitForTouch();
//int ScreenReleased(void);
//void WaitForRelease();
int putcharTouchScreen(int);
int getcharTouchScreen(void);
struct Point getPoint();
struct Point GetPress(void);
struct Point GetRelease(void);
int getcharTouch(void);
int ScreenReleased(void);
void WaitForRelease();

/* a data type to hold a point/coord */
struct Point
{
    int x, y;
};




#endif /* TOUCH_H_ */
