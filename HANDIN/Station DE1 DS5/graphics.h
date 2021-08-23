/*
 * graphics.h
 *
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#define XRES 800
#define YRES 480

// graphics register addresses
#define GraphicsCommandReg (*(volatile unsigned short int *)(0xFF210000))
#define GraphicsStatusReg (*(volatile unsigned short int *)(0xFF210000))
#define GraphicsX1Reg (*(volatile unsigned short int *)(0xFF210002))
#define GraphicsY1Reg (*(volatile unsigned short int *)(0xFF210004))
#define GraphicsX2Reg (*(volatile unsigned short int *)(0xFF210006))
#define GraphicsY2Reg (*(volatile unsigned short int *)(0xFF210008))
#define GraphicsColourReg (*(volatile unsigned short int *)(0xFF21000E))
#define GraphicsBackGroundColourReg (*(volatile unsigned short int *)(0xFF210010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS                          \
    while ((GraphicsStatusReg & 0x0001) != 0x0001) \
        ;

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine 1
#define DrawVLine 2
#define DrawLine 3
#define PutAPixel 0xA
#define GetAPixel 0xB
#define ProgramPaletteColour 0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file "ColourPaletteData.c" contains the 24 bit RGB data
// that is pre-programmed into the palette

#define BLACK 0
#define WHITE 1
#define RED 2
#define LIME 3
#define BLUE 4
#define YELLOW 5
#define CYAN 6
#define MAGENTA 7

//Division
#define DIV_SHOPPING_LIST 500
#define DIV_VOICE 300

//Shopping List Constants
#define X_LIST_START 25
#define Y_LIST_START 75
#define MARGIN_SHOPPING_LIST 25
#define X_LIST_OFFSET 375
#define Y_LIST_OFFSET 100
#define MAX_LIST_SIZE 3

//Scroll Bar Constants
#define SCROLL_BUTTON_SIZE 50

//Normal Button
#define MARGIN_BUTTON_25 25
#define MARGIN_BUTTON_50 50

#define BUTTON_SIZE_HEIGHT 50
#define BUTTON_SIZE_WIDTH 100

//Loading Dialog
#define X_LOADING 300
#define Y_LOADING 150


void WriteAPixel(int, int, int);
int ReadAPixel(int, int);
void ProgramPalette(int , int );
void HLine(int, int, int, int);
void VLine(int, int, int, int);
int abs(int);
int sign(int);
void Line(int, int, int, int, int);
void Rectangle(int, int, int, int, int, int);
void FilledBorderedRectangle(int, int, int, int, int, int, int);
void FilledRectangle(int, int, int, int, int);
void Circle(int, int, int, int, int);
void FilledCircle(int, int, int, int);
void FilledCircleWithBorder(int, int, int, int, int, int);
void RandLines(int);
void OutGraphicsCharFont1(int, int, int, int, int, int);
void OutGraphicsCharFont2(int, int, int, int, int, int);
void print(char *, int, int, int, int, int);
void Button(char *, int, int, int, int, int, int, int, int, int);
void ButtonCircle(char *, int, int, int, int, int, int, int, int);
void displayRefreshButton();
void drawShoppingItem(char *, int);
void clearScreen();
void displayTitle();
void displayScrollElementAndSendAndHelpButton();
void displayMainScreen();
void displayHelpScreen();
void displayLoadingDialog(char *);


#endif /* GRAPHICS_H_ */