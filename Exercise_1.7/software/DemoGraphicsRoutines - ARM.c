#include <stdio.h>

// #defined constants representing values we write to the graphics 'command' register to get
// it to do something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine		1
#define DrawVLine		2
#define DrawLine		3
#define PutAPixel		0xA
#define GetAPixel		0xB
#define ProgramPaletteColour   	0x10

/*******************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
*******************************************************************************************/

#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

//Predefined Colour Values
//Use the symbolic constants below as the values to write to the Colour Register
//When you ask the graphics chip to draw something. These values are also returned
//By the read pixel command

// the header file "Colours.h" contains symbolic names for all 64colours e.g. RED
// while the source file ColourPaletteData.c contains the 24 bit RGB data
// that is pre-programmed into each of the 64 palettes

#define	BLACK		0
#define	WHITE		1
#define	RED			2
#define	LIME		3
#define	BLUE		4
#define	YELLOW		5
#define	CYAN		6
#define	MAGENTA		7


#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsX1Reg   		(*(volatile unsigned short int *)(0xFF210002))
#define GraphicsY1Reg   		(*(volatile unsigned short int *)(0xFF210004))
#define GraphicsX2Reg   		(*(volatile unsigned short int *)(0xFF210006))
#define GraphicsY2Reg			(*(volatile unsigned short int *)(0xFF210008))
#define GraphicsColourReg		(*(volatile unsigned short int *)(0xFF21000E))
#define GraphicsBackGroundColourReg   		(*(volatile unsigned short int *)(0xFF210010))

/**********************************************************************
* This function writes a single pixel to the x,y coords specified in the specified colour
* Note colour is a palette number (0-63) not a 24 bit RGB value
**********************************************************************/
void WriteAPixel (int x, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;		// set pixel colour with a palette number
	GraphicsCommandReg = PutAPixel;		// give graphics a "write pixel" command
}

/*****************************************************************************************
* This function read a single pixel from x,y coords specified and returns its colour
* Note returned colour is a palette number (0-63) not a 24 bit RGB value
******************************************************************************************/
int ReadAPixel (int x, int y)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;			// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;		// return the palette number (colour)
}

/****************************************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
****************************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;          // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                	 // program green and blue into 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour;	// issue command
}


int main()
{
	int i ;
	printf("Writing Pixels!\n");

	// draw a line across the screen in RED at y coord 100 and from x = 0 to 799
	for(i = 0; i < 800; i ++)
		WriteAPixel(i, 100, RED);

	// read the pixels back and make sure we read 2 (RED) to prove it's working
	for(i = 0; i < 800; i ++)
		printf("Colour value (i.e. pallette number) = %d at [%d, 100]\n", ReadAPixel(i, 100), i);

	return 0;
}