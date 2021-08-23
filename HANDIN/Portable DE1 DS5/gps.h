/*
 * gps.h
 *
 */

#ifndef GPS_H_
#define GPS_H_

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

void Init_GPS(void);
int putcharGPS(int);
int getcharGPS(void);
int GPSTestForReceivedData(void);
void GPSFlush(void);
void GPSOutMessage(char *);
struct Coordinate updateCoordinates(void);

struct Coordinate {
    double lat;
    double lon;
};

#endif /* WIFI_H_ */
